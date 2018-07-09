/***********************************************************************
 *
 * Copyright (C) 2014-2018 wereturtle
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
#include <QPrintPreviewDialog>
#include <QApplication>
#include <QStack>
#include <QDir>
#include <QDesktopServices>
#include <QtConcurrentRun>
#include <QFuture>
#include <QPrinter>
#include <QDesktopWidget>

#include "HtmlPreview.h"
#include "Exporter.h"

HtmlPreview::HtmlPreview
(
    TextDocument* document,
    Exporter* exporter,
    QWidget* parent
)
    : QMainWindow(parent), document(document), updateInProgress(false),
        updateAgain(false), exporter(exporter)
{
    htmlBrowser = new QWebView(this);
    htmlBrowser->settings()->setDefaultTextEncoding("utf-8");
    
    setWindowTitle(tr("HTML Preview"));
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    html = "";
    htmlBrowser->setHtml("");
    htmlBrowser->page()->setContentEditable(false);
    htmlBrowser->page()->setLinkDelegationPolicy(QWebPage::DelegateExternalLinks);
    htmlBrowser->page()->action(QWebPage::Reload)->setVisible(false);
    htmlBrowser->page()->action(QWebPage::OpenLink)->setVisible(false);
    htmlBrowser->page()->action(QWebPage::OpenLinkInNewWindow)->setVisible(false);
    connect(htmlBrowser, SIGNAL(linkClicked(QUrl)), this, SLOT(onLinkClicked(QUrl)));
    headingTagExp.setPattern("^[Hh][1-6]$");

    this->setCentralWidget(htmlBrowser);

    printer = NULL;

    futureWatcher = new QFutureWatcher<QString>(this);
    this->connect(futureWatcher, SIGNAL(finished()), SLOT(onHtmlReady()));

    this->connect(document, SIGNAL(filePathChanged()), SLOT(updateBaseDir()));
    this->updateBaseDir();

    // Set zoom factor for WebKit browser to account for system DPI settings,
    // since WebKit assumes 96 DPI as a fixed resolution.
    //
    QWidget* window = QApplication::desktop()->screen();
    int horizontalDpi = window->logicalDpiX();
    // Don't want to affect image size, only text size.
    htmlBrowser->settings()->setAttribute(QWebSettings::ZoomTextOnly, true);
    htmlBrowser->setZoomFactor((horizontalDpi / 96.0));
}

HtmlPreview::~HtmlPreview()
{
    // Wait for thread to finish if in the middle of updating the preview.
    futureWatcher->waitForFinished();
    
    if (NULL != printer)
    {
        delete printer;
        printer = NULL;
    }
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
            this->setHtml("");
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
    this->htmlBrowser->page()->mainFrame()->evaluateJavaScript
    (
        QString
        (
            "var headers = document.querySelectorAll(\"h1, h2, h3, h4, h5, h6\");\n"
            "\n"
            "if (%1 < headers.length && %1 >= 0) {\n"
            "    headers[%1].scrollIntoView();\n"
            "}\n"
        ).arg(headingSequenceNumber - 1)
    );
}

void HtmlPreview::onHtmlReady()
{
    QString html = futureWatcher->result();

    // Find where the change occurred since last time, and slip an
    // anchor in the location so that we can scroll there.
    //
    QString anchoredHtml = "";

    QTextStream newHtmlDoc((QString*) &html, QIODevice::ReadOnly);
    QTextStream oldHtmlDoc((QString*) &(this->html), QIODevice::ReadOnly);
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
            anchoredHtmlDoc << "<div id=\"livepreviewmodifypoint\" />";
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
        anchoredHtmlDoc << "<div id=\"livepreviewmodifypoint\" />";
    }

    // Put any remaining new HTML data into the
    // anchored HTML string.
    //
    while (!newLine.isNull())
    {
        if (!differenceFound)
        {
            anchoredHtmlDoc << "<div id=\"livepreviewmodifypoint\" />";
        }

        differenceFound = true;
        anchoredHtmlDoc << newLine << "\n";
        newLine = newHtmlDoc.readLine();
    }

    if (differenceFound)
    {
        setHtml(anchoredHtml);
        this->html = html;
    }

    updateInProgress = false;

    if (updateAgain)
    {
        updateAgain = false;
        updatePreview();
    }

}

void HtmlPreview::setHtmlExporter(Exporter* exporter)
{
    this->exporter = exporter;
    setHtml("");
    updatePreview();
}

void HtmlPreview::setStyleSheet(const QString& filePath)
{
    // Update the HTML preview with the newly selected style sheet, if needed.
    if (!filePath.startsWith(":"))
    {
        htmlBrowser->settings()->setUserStyleSheetUrl
        (
            QUrl::fromLocalFile(filePath)
        );
    }
    else
    {
        htmlBrowser->settings()->setUserStyleSheetUrl
        (
            QUrl(QString("qrc") + filePath)
        );
    }

    setHtml("");
    updatePreview();
}

void HtmlPreview::printPreview()
{
    QPrinter* printer = getPrinterSettings();
    QPrintPreviewDialog printPreviewDialog(printer, this);

    connect
    (
        &printPreviewDialog,
        SIGNAL(paintRequested(QPrinter*)),
        this,
        SLOT(printHtmlToPrinter(QPrinter*))
    );

    printPreviewDialog.exec();
}

void HtmlPreview::printHtmlToPrinter(QPrinter* printer)
{
    this->htmlBrowser->print(printer);
}

void HtmlPreview::onLinkClicked(const QUrl& url)
{
    QDesktopServices::openUrl(url);
}

void HtmlPreview::updateBaseDir()
{
    if (!document->getFilePath().isNull() && !document->getFilePath().isEmpty())
    {
        // Note that a forward slash ("/") is appended to the path to
        // ensure it works.  If the slash isn't there, then it won't
        // recognize the base URL for some reason.
        //
        this->baseUrl =
            QUrl::fromLocalFile(QFileInfo(document->getFilePath()).dir().absolutePath()
                + "/");
    }
    else
    {
        this->baseUrl = QUrl();
    }

    this->updatePreview();
}

QSize HtmlPreview::sizeHint() const
{
    return QSize(500, 600);
}

void HtmlPreview::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);

    setHtml("");
    html = "";
}

void HtmlPreview::setHtml(const QString& html)
{
    this->html = html;

    htmlBrowser->setContent(html.toUtf8(), "text/html", baseUrl);
    htmlBrowser->page()->mainFrame()->scrollToAnchor("livepreviewmodifypoint");
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

QPrinter* HtmlPreview::getPrinterSettings()
{
    // If the printer has not yet been initialized, then
    // set up default page layout and page size for printing.
    if (NULL == printer)
    {
        printer = new QPrinter();
        printer->setPaperSize(QPrinter::Letter);
        printer->setPageMargins(0.5, 0.5, 0.5, 0.5, QPrinter::Inch);
    }
    
    return printer;
}
