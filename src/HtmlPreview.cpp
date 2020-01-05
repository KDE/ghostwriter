/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QApplication>
#include <QStack>
#include <QDir>
#include <QDesktopServices>
#include <QtConcurrentRun>
#include <QFuture>
#include <QWebChannel>

#include "HtmlPreview.h"
#include "Exporter.h"
#include "SandboxedWebPage.h"

HtmlPreview::HtmlPreview
(
    TextDocument* document,
    Exporter* exporter,
    QWidget* parent
)
    : QWebEngineView(parent), document(document), updateInProgress(false),
        updateAgain(false), exporter(exporter)
{
    vanillaHtml = "";
    baseUrl = "";
    livePreviewHtml.setText("");
    styleSheetUrl.setText(":/resources/github.css");

    this->setPage(new SandboxedWebPage(this));
    this->settings()->setDefaultTextEncoding("utf-8");
    this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->page()->action(QWebEnginePage::Reload)->setVisible(false);
    this->page()->action(QWebEnginePage::ReloadAndBypassCache)->setVisible(false);
    this->page()->action(QWebEnginePage::OpenLinkInThisWindow)->setVisible(false);
    this->page()->action(QWebEnginePage::OpenLinkInNewWindow)->setVisible(false);
    this->page()->action(QWebEnginePage::ViewSource)->setVisible(false);
    this->page()->action(QWebEnginePage::SavePage)->setVisible(false);
    QWebEngineProfile::defaultProfile()->setHttpCacheType(QWebEngineProfile::NoCache);
    QWebEngineProfile::defaultProfile()->clearHttpCache();
    QWebEngineProfile::defaultProfile()->clearAllVisitedLinks();
    connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));
    headingTagExp.setPattern("^[Hh][1-6]$");

    futureWatcher = new QFutureWatcher<QString>(this);
    this->connect(futureWatcher, SIGNAL(finished()), SLOT(onHtmlReady()));
    this->connect(document, SIGNAL(filePathChanged()), SLOT(updateBaseDir()));

    // Set zoom factor for Chromium browser to account for system DPI settings,
    // since Chromium assumes 96 DPI as a fixed resolution.
    //
    QWidget* window = QApplication::desktop()->screen();
    int horizontalDpi = window->logicalDpiX();
    this->setZoomFactor((horizontalDpi / 96.0));

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("stylesheeturl"), &styleSheetUrl);
    channel->registerObject(QStringLiteral("livepreviewcontent"), &livePreviewHtml);
    this->page()->setWebChannel(channel);
    this->wrapperHtml =
        "<!doctype html>"
        "<html lang=\"en\">"
        "<meta charset=\"utf-8\">"
        "<head>"
        "    <script>"
        "         MathJax = {"
        "            tex: {"
        "                inlineMath: [['$', '$']]]"
        "             }"
        "         };"
        "    </script>"
        "    <script type=\"text/javascript\" id=\"MathJax-script\" src=\"https://cdn.jsdelivr.net/npm/mathjax@3/es5/tex-mml-chtml.js\"></script>"
        "    <link id='ghostwriter_css' rel='stylesheet' type='text/css' href='qrc:/resources/github.css' media='all' />"
        "    <script src=\"qrc:/qtwebchannel/qwebchannel.js\"></script>"
        "</head>"
        "<body>"
        "    <div id=\"livepreviewplaceholder\"></div>"
        "    <script src=\"qrc:/resources/gw.js\"></script>"
        "    <script>"
        "        new QWebChannel(qt.webChannelTransport,"
        "            function(channel) {"
        "                var styleSheetUrl = channel.objects.stylesheeturl;"
        "                loadStyleSheet(styleSheetUrl.text);"
        "                styleSheetUrl.textChanged.connect(loadStyleSheet);"
        ""
        "                var content = channel.objects.livepreviewcontent;"
        "                updateText(content.text);"
        "                content.textChanged.connect(updateText);"
        "            }"
        "        );"
        "    </script>"
        "</body>"
        "</html>";

    // Set the base URL and load the preview using wrapperHtml above.
    this->updateBaseDir();
}

HtmlPreview::~HtmlPreview()
{
    // Wait for thread to finish if in the middle of updating the preview.
    futureWatcher->waitForFinished();
}

void HtmlPreview::contextMenuEvent(QContextMenuEvent* event)
{
    QMenu* menu = page()->createStandardContextMenu();
    menu->popup(event->globalPos());
}

void HtmlPreview::updatePreview()
{
    if (updateInProgress)
    {
        updateAgain = true;
        return;
    }

    if (this->isVisible())
    {
        // Some markdown processors don't handle empty text very well
        // and will err.  Thus, only pass in text from the document
        // into the markdown processor if the text isn't empty or null.
        //
        if (document->isEmpty())
        {
            this->setHtmlContent("");
        }
        else if (NULL != exporter)
        {
            QString text = document->toPlainText();

            if (!text.isNull() && !text.isEmpty())
            {
                updateInProgress = true;
                QFuture<QString> future =
                    QtConcurrent::run
                    (
                        this,
                        &HtmlPreview::exportToHtml,
                        document->toPlainText(),
                        exporter
                    );
                futureWatcher->setFuture(future);
            }
        }
    }
}

void HtmlPreview::navigateToHeading(int headingSequenceNumber)
{
    this->page()->runJavaScript
    (
        QString
        (
            "document.getElementById('livepreviewhnbr%1').scrollIntoView()"
        ).arg(headingSequenceNumber)
    );
}

void HtmlPreview::setHtmlExporter(Exporter* exporter)
{
    this->exporter = exporter;
    setHtmlContent("");
    updatePreview();
}

void HtmlPreview::setStyleSheet(const QString& filePath)
{
    // If not an internal resource file, use a URL path from the local file
    if (!filePath.startsWith(":") && !filePath.startsWith("qrc:") && !filePath.startsWith("file:"))
    {
        styleSheetUrl.setText(QUrl::fromLocalFile(filePath).toString());
    }
    // Ensure "qrc" is in front of local resource file path
    else if (filePath.startsWith(":"))
    {
        styleSheetUrl.setText("qrc" + filePath);
    }
    else
    {
        styleSheetUrl.setText(filePath);
    }
}

void HtmlPreview::onHtmlReady()
{
    QString html = futureWatcher->result();

    // Find where the change occurred since last time, and slip an
    // anchor in the location so that we can scroll there.
    //
    QString anchoredHtml = "";

    QTextStream newHtmlDoc((QString*) &html, QIODevice::ReadOnly);
    QTextStream oldHtmlDoc((QString*) &(this->vanillaHtml), QIODevice::ReadOnly);
    QTextStream anchoredHtmlDoc(&anchoredHtml, QIODevice::WriteOnly);

    bool differenceFound = false;
    QString oldLine = oldHtmlDoc.readLine();
    QString newLine = newHtmlDoc.readLine();

    while (!oldLine.isNull() && !newLine.isNull() && !differenceFound)
    {
        if (oldLine != newLine)
        {
            // Found the difference, so insert an anchor point at the
            // beginning of the line.
            //
            differenceFound = true;
            anchoredHtmlDoc << "<a id=\"livepreviewmodifypoint\"></a>";
        }
        else
        {
            anchoredHtmlDoc << newLine << "\n";
            oldLine = oldHtmlDoc.readLine();
            newLine = newHtmlDoc.readLine();
        }
    }

    // If lines were removed at the end of the new document,
    // ensure anchor point is inserted.
    //
    if (!differenceFound && !oldLine.isNull() && newLine.isNull())
    {
        differenceFound = true;
        anchoredHtmlDoc << "<a id=\"livepreviewmodifypoint\"></a>";
    }

    // Put any remaining new HTML data into the
    // anchored HTML string.
    //
    while (!newLine.isNull())
    {
        if (!differenceFound)
        {
            anchoredHtmlDoc << "<a id=\"livepreviewmodifypoint\"></a>";
        }

        differenceFound = true;
        anchoredHtmlDoc << newLine << "\n";
        newLine = newHtmlDoc.readLine();
    }

    if (differenceFound)
    {
        setHtmlContent(anchoredHtml);
        this->vanillaHtml = html;
    }

    updateInProgress = false;

    if (updateAgain)
    {
        updateAgain = false;
        updatePreview();
    }

}

void HtmlPreview::onLoadFinished(bool ok)
{
    if (ok)
    {
        this->page()->runJavaScript("document.documentElement.contentEditable = false;");
    }
}

void HtmlPreview::updateBaseDir()
{
    if (!document->getFilePath().isNull() && !document->getFilePath().isEmpty())
    {
        // Note that a forward slash ("/") is appended to the path to
        // ensure it works.  If the slash isn't there, then it won't
        // recognize the base URL for some reason.
        //
        baseUrl =
            QUrl::fromLocalFile(QFileInfo(document->getFilePath()).dir().absolutePath()
                + "/").toString();
    }
    else
    {
        this->baseUrl = "";
    }

    this->setHtml(wrapperHtml, baseUrl);
    this->updatePreview();
}

QSize HtmlPreview::sizeHint() const
{
    return QSize(500, 600);
}

void HtmlPreview::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);

    setHtmlContent("");
    vanillaHtml = "";
    livePreviewHtml.setText("");
}

void HtmlPreview::setHtmlContent(const QString& html)
{
    this->vanillaHtml = html;
    this->livePreviewHtml.setText(html);
}

QString HtmlPreview::exportToHtml
(
    const QString& text,
    Exporter* exporter
) const
{
    QString html;

    // Enable smart typography for preview, if available for the exporter.
    bool smartTypographyEnabled = exporter->getSmartTypographyEnabled();
    exporter->setSmartTypographyEnabled(true);

    // Export to HTML.
    exporter->exportToHtml(text, html);

    // Put smart typography setting back to the way it was before
    // so that the last setting used during document export is remembered.
    //
    exporter->setSmartTypographyEnabled(smartTypographyEnabled);

    return html;
}
