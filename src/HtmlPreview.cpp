/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStatusBar>
#include <QList>
#include <QPushButton>
#include <QPrintPreviewDialog>
#include <QApplication>
#include <QClipboard>
#include <QStack>
#include <QDir>
#include <QDesktopServices>
#include <QAction>
#include <QtConcurrentRun>
#include <QFuture>
#include <QSettings>
#include <QPrinter>

// As of Qt 5.3, a new class was introduced, QPageLayout, which separated
// page layout functionality from QPrinter.  Detect whether this new
// class is supported.  Note that we have no choice but to use the API changes,
// since the old API was done away with.
//
#if QT_MAJOR_VERSION >= 5 && QT_MINOR_VERSION >= 3
#include <QMarginsF>
#include <QPageLayout>
#endif

#include "HtmlPreview.h"
#include "Exporter.h"
#include "ExporterFactory.h"
#include "ExportDialog.h"
#include "MessageBoxHelper.h"
#include "StyleSheetManagerDialog.h"

#define GW_CUSTOM_STYLE_SHEETS_KEY "Preview/customStyleSheets"
#define GW_LAST_USED_STYLE_SHEET_KEY "Preview/lastUsedStyleSheet"
#define GW_LAST_USED_EXPORTER_KEY "Preview/lastUsedExporter"

HtmlPreview::HtmlPreview
(
    TextDocument* document,
    QWidget* parent
)
    : QMainWindow(parent), document(document), handlingStyleSheetChange(false)
{
    QSettings settings;
    QString currentCssFile =
        settings.value(GW_LAST_USED_STYLE_SHEET_KEY, QString()).toString();
    QString currentExporterName =
        settings.value(GW_LAST_USED_EXPORTER_KEY).toString();
    customCssFiles =
        settings.value(GW_CUSTOM_STYLE_SHEETS_KEY, QStringList()).toStringList();

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
    headingTagExp.setMinimal(true);
    headingTagExp.setPattern("[Hh][1-6]");

    this->statusBar()->setSizeGripEnabled(false);

    QString styleSheet;
    QTextStream stream(&styleSheet);

    stream
        << "QStatusBar { margin: 0; padding: 0 } "
        << "QStatusBar::item { border: 0; margin: 0; padding: 0; border: 0px } "
        << "QPushButton { margin: 0 1px 0 1px; padding: 5px; "
        << "border-radius: 5px; background: #4183C4; color: #F2F2F2 } "
        << "QPushButton:hover { background: #5A93CC } "
        << "QPushButton:pressed, QPushButton:flat, QPushButton:checked "
        << "{ background-color: #356FA9 }"
        ;
    this->statusBar()->setStyleSheet(styleSheet);

    defaultStyleSheets.append(":/resources/github.css");

    QPushButton* copyHtmlButton = new QPushButton(tr("Copy HTML"));
    copyHtmlButton->setFocusPolicy(Qt::NoFocus);
    connect(copyHtmlButton, SIGNAL(clicked()), this, SLOT(copyHtml()));
    this->statusBar()->addPermanentWidget(copyHtmlButton);

    QPushButton* exportButton = new QPushButton(tr("Export"));
    exportButton->setFocusPolicy(Qt::NoFocus);
    connect(exportButton, SIGNAL(clicked()), this, SLOT(onExport()));
    this->statusBar()->addPermanentWidget(exportButton);

    QPushButton* printButton = new QPushButton(tr("Print"));
    printButton->setFocusPolicy(Qt::NoFocus);
    connect(printButton, SIGNAL(clicked()), this, SLOT(printPreview()));
    this->statusBar()->addPermanentWidget(printButton);

    previewerComboBox = new QComboBox(this);

    QList<Exporter*> exporters = ExporterFactory::getInstance()->getHtmlExporters();

    if (exporters.isEmpty())
    {
        setHtml(QString("<b style='color: red'>") + tr("No markdown (pandoc, multimarkdown, discount) processors are installed.  Please install or add their installation locations to your system PATH environment variable.") + QString("</b>"));
        exporter = NULL;
    }
    else
    {
        int currentExporterIndex = 0;

        for (int i = 0; i < exporters.length(); i++)
        {
            Exporter* exporter = exporters.at(i);
            previewerComboBox->addItem(exporter->getName(), qVariantFromValue((void *) exporter));

            if (exporter->getName() == currentExporterName)
            {
                currentExporterIndex = i;
            }
        }

        exporter = exporters.at(currentExporterIndex);
        previewerComboBox->setCurrentIndex(currentExporterIndex);
    }

    this->statusBar()->addWidget(previewerComboBox);
    connect(previewerComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPreviewerChanged(int)));

    styleSheetComboBox = new QComboBox(this);
    styleSheetComboBox->addItem(tr("Github (Default)"));
    styleSheetComboBox->setItemData(0, QVariant(defaultStyleSheets.at(0)));

    int customCssIndexStart = defaultStyleSheets.size();

    int cssIndex = 0;
    lastStyleSheetIndex = -1;

    for (int i = 0; i < customCssFiles.size(); i++)
    {
        QFileInfo fileInfo(customCssFiles.at(i));

        if (fileInfo.exists())
        {
            styleSheetComboBox->addItem(fileInfo.completeBaseName());
            styleSheetComboBox->setItemData(customCssIndexStart + i, QVariant(customCssFiles.at(i)));
        }
    }

    styleSheetComboBox->addItem(tr("Add/Remove Custom Style Sheets..."));

    // Find the last used style sheet, and set it as selected in the combo box.
    for (int i = 0; i < styleSheetComboBox->count() - 1; i++)
    {
        if (styleSheetComboBox->itemData(i).toString() == currentCssFile)
        {
            cssIndex = i;
            styleSheetComboBox->setCurrentIndex(i);
            break;
        }
    }

    connect(styleSheetComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeStyleSheet(int)));
    this->statusBar()->addWidget(styleSheetComboBox);

    this->setCentralWidget(htmlBrowser);

    futureWatcher = new QFutureWatcher<QString>(this);
    this->connect(futureWatcher, SIGNAL(finished()), SLOT(onHtmlReady()));
    this->changeStyleSheet(cssIndex);

    this->connect(document, SIGNAL(filePathChanged()), SLOT(updateBaseDir()));
    this->updateBaseDir();

    // Set up default page layout and page size for printing.
    printer.setPaperSize(QPrinter::Letter);

#if QT_MAJOR_VERSION >= 5 && QT_MINOR_VERSION >= 3
    printer.setPageMargins(QMarginsF(0.5, 0.5, 0.5, 0.5), QPageLayout::Inch);
#else
    printer.setPageMargins(0.5, 0.5, 0.5, 0.5, QPrinter::Inch);
#endif
}

HtmlPreview::~HtmlPreview()
{
    QSettings settings;

    // Store the selected exporter name.
    QString exporterName;

    if (previewerComboBox->count() > 0)
    {
        exporterName =
            previewerComboBox->itemText(previewerComboBox->currentIndex());
    }

    if (!exporterName.isNull())
    {
        settings.setValue
        (
            GW_LAST_USED_EXPORTER_KEY,
            exporterName
        );
    }
    else
    {
        // Clean up key if no exporters were even installed from which
        // to select.
        //
        settings.remove(GW_LAST_USED_EXPORTER_KEY);
    }

    // Store the custom style sheet list.
    settings.setValue(GW_CUSTOM_STYLE_SHEETS_KEY, customCssFiles);

    // Store the last used style sheet.
    // Use count > 1 to exclude the "Add/Remove Custom Style Sheets" option.
    if
    (
        (styleSheetComboBox->count() > 1) &&
        (styleSheetComboBox->currentIndex() < (styleSheetComboBox->count() - 1))
    )
    {
        QString cssPath =
            styleSheetComboBox->itemData(styleSheetComboBox->currentIndex()).toString();
        settings.setValue(GW_LAST_USED_STYLE_SHEET_KEY, cssPath);
    }

    // Wait for thread to finish if in the middle of updating the preview.
    futureWatcher->waitForFinished();
}

void HtmlPreview::updatePreview()
{
    if (this->isVisible())
    {
        // Some markdown processors don't handle empty text very well
        // and will error.  Thus, only pass in text from the document
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
    QString anchor = QString("livepreviewhnbr%1").arg(headingSequenceNumber);
    this->htmlBrowser->page()->mainFrame()->scrollToAnchor(anchor);
}

void HtmlPreview::onHtmlReady()
{
    QString html = futureWatcher->result();

    if (html == this->html)
    {
        // Don't bother updating if the HTML didn't change.
        return;
    }

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

    // Put any remaining new HTML data into the
    // anchored HTML string.
    //
    while (!newLine.isNull())
    {
        anchoredHtmlDoc << newLine << "\n";
        newLine = newHtmlDoc.readLine();
    }

    setHtml(anchoredHtml);
    this->html = html;

    // Traverse the DOM in the browser, and find all the H1-H6 tags.
    // Set the id attribute of each heading tag to have a unique
    // sequence number, so that when the navigateToHeading() slot
    // is triggered, we can scroll to the desired heading.
    //
    QWebFrame* frame = htmlBrowser->page()->mainFrame();
    QWebElement element = frame->documentElement();
    QStack<QWebElement> elementStack;
    int headingId = 1;

    elementStack.push(element);

    while (!elementStack.isEmpty())
    {
        element = elementStack.pop();

        // If the element is a heading tag (H1-H6), set an anchor id for it.
        if (headingTagExp.exactMatch(element.tagName()))
        {
            element.setAttribute("id", QString("livepreviewhnbr%1").arg(headingId));
            headingId++;
        }
        // else if the element is something that would have a heading tag
        // (not a paragraph, blockquote, code, etc.), then add its children
        // to traverse and look for headings.
        //
        else if
        (
            (0 != element.tagName().compare("blockquote", Qt::CaseInsensitive))
            && (0 != element.tagName().compare("code", Qt::CaseInsensitive))
            && (0 != element.tagName().compare("p", Qt::CaseInsensitive))
            && (0 != element.tagName().compare("ol", Qt::CaseInsensitive))
            && (0 != element.tagName().compare("ul", Qt::CaseInsensitive))
            && (0 != element.tagName().compare("table", Qt::CaseInsensitive))
        )
        {
            QStack<QWebElement> childStack;
            element = element.firstChild();

            while (!element.isNull())
            {
                childStack.push(element);
                element = element.nextSibling();
            }

            while (!childStack.isEmpty())
            {
                elementStack.push(childStack.pop());
            }
        }
    }
}

void HtmlPreview::onPreviewerChanged(int index)
{
    QVariant exporterVariant = previewerComboBox->itemData(index);

    exporter = (Exporter*) exporterVariant.value<void*>();
    setHtml("");
    updatePreview();
}

void HtmlPreview::changeStyleSheet(int index)
{
    // Prevent recursion, since calls to the combo box's setCurrentIndex
    // method will trigger changeStyleSheet to be called again.
    //
    if (handlingStyleSheetChange)
    {
        return;
    }

    bool previewUpdateNeeded = true;

    handlingStyleSheetChange = true;

    QString filePath;
    int selectionIndex = index;

    // If the "Add/Remove Custom Style Sheets" option was selected...
    if (styleSheetComboBox->count() == (index + 1))
    {
        // Save off the style sheet file path of the last selected item.
        QString oldSelection =
            styleSheetComboBox->itemData(lastStyleSheetIndex).toString();

        // Now make sure the last style sheet is what is selected in the
        // combo box, that way the user doesn't see the
        // "Add/Remove Custom Style Sheets" as selected.
        //
        styleSheetComboBox->setCurrentIndex(lastStyleSheetIndex);

        // Let the user add/remove style sheets via the StyleSheetManagerDialog.
        StyleSheetManagerDialog ssmDialog(customCssFiles, this);
        int result = ssmDialog.exec();

        // If changes are accepted (user clicked OK), reload the style sheets
        // into the combo box, and select the last used style sheet if it
        // wasn't removed.
        //
        if (QDialog::Accepted == result)
        {
            customCssFiles = ssmDialog.getStyleSheets();

            // Remove all the old style sheets from the combo box.
            while (styleSheetComboBox->count() > (defaultStyleSheets.size() + 1))
            {
                styleSheetComboBox->removeItem(1);
            }

            selectionIndex = 0;

            // Now put the new list of style sheets back into the combo box.
            for (int i = customCssFiles.size() - 1; i >= 0; i--)
            {
                QString styleSheet = customCssFiles[i];
                QFileInfo fileInfo(styleSheet);
                styleSheetComboBox->insertItem
                (
                    defaultStyleSheets.size(),
                    fileInfo.completeBaseName(),
                    styleSheet
                );

                if (styleSheet == oldSelection)
                {
                    previewUpdateNeeded = false;
                    selectionIndex = i + 1;
                }
            }

            // If the last selected style sheet was one of the default ones,
            // and is still currently selected, then we don't need to update
            // the preview again.
            //
            if
            (
                (lastStyleSheetIndex == selectionIndex) &&
                (selectionIndex < defaultStyleSheets.size())
            )
            {
                previewUpdateNeeded = false;
            }

            styleSheetComboBox->setCurrentIndex(selectionIndex);
            filePath = styleSheetComboBox->itemData(selectionIndex).toString();
        }
        else
        {
            // If the user canceled adding/removing custom CSS files, return.
            handlingStyleSheetChange = false;
            return;
        }
    }
    else
    {
        filePath = styleSheetComboBox->itemData(index).toString();
    }

    // Update the HTML preview with the newly selected style sheet, if needed.
    if (previewUpdateNeeded)
    {
        if (selectionIndex >= defaultStyleSheets.size())
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
                QUrl(QString("qrc") + defaultStyleSheets.at(selectionIndex))
            );
        }

        setHtml("");
        updatePreview();
    }

    lastStyleSheetIndex = selectionIndex;
    handlingStyleSheetChange = false;
}

void HtmlPreview::printPreview()
{
    QPrintPreviewDialog printPreviewDialog(&printer, this);

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

void HtmlPreview::onExport()
{
    ExportDialog exportDialog(document);

    connect(&exportDialog, SIGNAL(exportStarted(QString)), this, SIGNAL(operationStarted(QString)));
    connect(&exportDialog, SIGNAL(exportComplete()), this, SIGNAL(operationFinished()));

    exportDialog.exec();
}

void HtmlPreview::copyHtml()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(html);
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
    exporter->exportToHtml(text, html);

    return html;
}
