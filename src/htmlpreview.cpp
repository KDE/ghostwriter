/***********************************************************************
 *
 * Copyright (C) 2014-2022 wereturtle
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

#include <QFutureWatcher>
#include <QMenu>
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QWebEngineProfile>
#include <QWebEngineSettings>
#endif

#include "exporter.h"
#include "htmlpreview.h"
#include "sandboxedwebpage.h"
#include "stringobserver.h"

namespace ghostwriter
{
class HtmlPreviewPrivate
{
    Q_DECLARE_PUBLIC(HtmlPreview)

public:
    HtmlPreviewPrivate(HtmlPreview *q_ptr)
        : q_ptr(q_ptr)
    {
        ;
    }

    ~HtmlPreviewPrivate()
    {
        ;
    }

    HtmlPreview *q_ptr;

    MarkdownDocument *document;
    bool updateInProgress;
    bool updateAgain;
    StringObserver livePreviewHtml;
    StringObserver styleSheet;
    QString baseUrl;
    QRegularExpression headingTagExp;
    Exporter *exporter;
    QString wrapperHtml;
    QFutureWatcher<QString> *futureWatcher;

    void onHtmlReady();
    void onLoadFinished(bool ok);

    /**
     * Sets the base directory path for determining resource
     * paths relative to the web page being previewed.
     * This method is called whenever the file path changes.
     */
    void updateBaseDir();
    /*
    * Sets the HTML contents to display, and creates a backup of the old
    * HTML for diffing to scroll to the first difference whenever
    * updatePreview() is called.
    */
    void setHtmlContent(const QString &html);

    static QString exportToHtml(const QString &text, Exporter *exporter);
};

HtmlPreview::HtmlPreview
(
    MarkdownDocument *document,
    Exporter *exporter,
    QWidget *parent
) : QWebEngineView(parent),
    d_ptr(new HtmlPreviewPrivate(this))
{
    Q_D(HtmlPreview);
    
    d->document = document;
    d->updateInProgress = false;
    d->updateAgain = false;
    d->exporter = exporter;

    d->baseUrl = "";
    d->livePreviewHtml.setText("");
    d->styleSheet.setText("");

    this->setPage(new SandboxedWebPage(this));
    this->settings()->setDefaultTextEncoding("utf-8");
    this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls, true);
    this->settings()->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls, true);
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

    this->connect
    (
        this,
        &QWebEngineView::loadFinished,
        [d](bool ok) {
            d->onLoadFinished(ok);
        }
    );

    d->headingTagExp.setPattern("^[Hh][1-6]$");

    d->futureWatcher = new QFutureWatcher<QString>(this);
    this->connect
    (
        d->futureWatcher,
        &QFutureWatcher<QString>::finished,
        [d]() {
            d->onHtmlReady();
        }
    );

    this->connect
    (
        document,
        &MarkdownDocument::filePathChanged,
        [d]() {
            d->updateBaseDir();
        }
    );

    // Set zoom factor for Chromium browser to account for system DPI settings,
    // since Chromium assumes 96 DPI as a fixed resolution.
    //
    qreal horizontalDpi = QGuiApplication::primaryScreen()->logicalDotsPerInchX();
    this->setZoomFactor((horizontalDpi / 96.0));

    QWebChannel *channel = new QWebChannel(this);
    channel->registerObject(QStringLiteral("stylesheet"), &d->styleSheet);
    channel->registerObject(QStringLiteral("livepreviewcontent"), &d->livePreviewHtml);
    this->page()->setWebChannel(channel);

    QFile wrapperHtmlFile(":/resources/preview.html");

    if (!wrapperHtmlFile.open(QFile::ReadOnly | QFile::Text)) {
        d->wrapperHtml = tr("Error loading resources/preview.html");
    } else {
        QTextStream stream(&wrapperHtmlFile);
        d->wrapperHtml = stream.readAll();
        wrapperHtmlFile.close();
    }

    // Set the base URL and load the preview using wrapperHtml above.
    d->updateBaseDir();
}

HtmlPreview::~HtmlPreview()
{
    Q_D(HtmlPreview);
    
    // Wait for thread to finish if in the middle of updating the preview.
    d->futureWatcher->waitForFinished();
}

void HtmlPreview::contextMenuEvent(QContextMenuEvent *event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QMenu *menu = page()->createStandardContextMenu();
#else
    QMenu *menu = createStandardContextMenu();
#endif

    menu->popup(event->globalPos());
}

void HtmlPreview::updatePreview()
{
    Q_D(HtmlPreview);
    
    if (d->updateInProgress) {
        d->updateAgain = true;
        return;
    }

    if (this->isVisible()) {
        // Some markdown processors don't handle empty text very well
        // and will err.  Thus, only pass in text from the document
        // into the markdown processor if the text isn't empty or null.
        //
        if (d->document->isEmpty()) {
            d->setHtmlContent("");
        } else if (nullptr != d->exporter) {
            QString text = d->document->toPlainText();

            if (!text.isNull() && !text.isEmpty()) {
                d->updateInProgress = true;
                QFuture<QString> future =
                    QtConcurrent::run
                    (
                        &HtmlPreviewPrivate::exportToHtml,
                        d->document->toPlainText(),
                        d->exporter
                    );
                d->futureWatcher->setFuture(future);
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
            "scrollToHeading(%1);"
        ).arg(headingSequenceNumber)
    );
}

void HtmlPreview::setHtmlExporter(Exporter *exporter)
{
    Q_D(HtmlPreview);
    
    d->exporter = exporter;
    d->setHtmlContent("");
    updatePreview();
}

void HtmlPreview::setStyleSheet(const QString &css)
{
    Q_D(HtmlPreview);
    
    d->styleSheet.setText(css);
}

void HtmlPreviewPrivate::onHtmlReady()
{
    Q_Q(HtmlPreview);
    
    setHtmlContent(futureWatcher->result());
    updateInProgress = false;

    if (updateAgain) {
        updateAgain = false;
        q->updatePreview();
    }

}

void HtmlPreviewPrivate::onLoadFinished(bool ok)
{
    Q_Q(HtmlPreview);
    
    if (ok) {
        q->page()->runJavaScript("document.documentElement.contentEditable = false;");
    }
}

void HtmlPreviewPrivate::updateBaseDir()
{
    Q_Q(HtmlPreview);
    
    if (!document->filePath().isNull() && !document->filePath().isEmpty()) {
        // Note that a forward slash ("/") is appended to the path to
        // ensure it works.  If the slash isn't there, then it won't
        // recognize the base URL for some reason.
        //
        baseUrl =
            QUrl::fromLocalFile(QFileInfo(document->filePath()).dir().absolutePath()
                                + "/").toString();
    } else {
        this->baseUrl = "";
    }

    q->setHtml(wrapperHtml, baseUrl);
    q->updatePreview();
}

void HtmlPreview::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    Q_D(HtmlPreview);
    
    d->setHtmlContent("");
}

void HtmlPreviewPrivate::setHtmlContent(const QString &html)
{
    this->livePreviewHtml.setText(html);
}

QString HtmlPreviewPrivate::exportToHtml
(
    const QString &text,
    Exporter *exporter
)
{
    QString html;

    // Enable smart typography for preview, if available for the exporter.
    bool smartTypographyEnabled = exporter->smartTypographyEnabled();
    exporter->setSmartTypographyEnabled(true);

    // Export to HTML.
    exporter->exportToHtml(text, html);

    // Put smart typography setting back to the way it was before
    // so that the last setting used during document export is remembered.
    //
    exporter->setSmartTypographyEnabled(smartTypographyEnabled);

    return html;
}
} // namespace ghostwriter
