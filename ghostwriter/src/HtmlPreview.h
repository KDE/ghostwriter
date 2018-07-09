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

#ifndef HTML_PREVIEW_H
#define HTML_PREVIEW_H

#include <QWidget>
#include <QMainWindow>
#include <QTextDocument>
#include <QThread>
#include <QTimer>
#include <QList>
#include <QPrinter>
#include <QRegularExpression>
#include <QUrl>
#include <QFutureWatcher>
#include <QStringList>

#if QT_VERSION >= 0x050000
#include <QtWebKitWidgets>
#else
#include <QtWebKit>
#endif

#include "Exporter.h"
#include "TextDocument.h"

class QPrintPreviewDialog;
class QPrinter;

/**
 * Live HTML Preview window.
 */
class HtmlPreview : public QMainWindow
{
    Q_OBJECT
    
    public:
        /**
         * Constructor.  Takes text document to be rendered as HTML as
         * parameter.
         */
        HtmlPreview
        (
            TextDocument* document,
            Exporter* exporter,
            QWidget* parent = 0
        );

        /**
         * Destructor.
         */
        virtual ~HtmlPreview();

    public slots:
        /**
         * Call this method to re-render the HTML for the document.
         */
        void updatePreview();

        /**
         * Call this method to navigate to the HTML heading tag (h1 - h6)
         * having the given sequence number.  For example, to navigate to the
         * very first heading in the document, pass in a value of 1.  To go
         * to the second heading that appears in the document, pass in a value
         * of 2, etc.
         */
        void navigateToHeading(int headingSequenceNumber);

        /**
         * Call this method to set the HTML exporter used in
         * generating HTML from the Markdown document.
         */
        void setHtmlExporter(Exporter* exporter);

        /**
         * Call this method to change the CSS style sheet.
         */
        void setStyleSheet(const QString& filePath);

        /**
         * Call this method to display a print preview dialog of
         * the rendered HTML.
         */
        void printPreview();

    private slots:
        void onHtmlReady();
        void printHtmlToPrinter(QPrinter* printer);
        void onLinkClicked(const QUrl& url);

        /**
         * Sets the base directory path for determining resource
         * paths relative to the web page being previewed.
         * This method is called whenever the file path changes.
         */
        void updateBaseDir();

    protected:
        QSize sizeHint() const;
        void closeEvent(QCloseEvent* event);

    private:
        QWebView* htmlBrowser;
        QUrl baseUrl;
        TextDocument* document;
        bool updateInProgress;
        bool updateAgain;
        QString html;
        QRegularExpression headingTagExp;
        Exporter* exporter;

        /*
         * Used to set default page layout options for printing.  Also,
         * if the user closes the print preview dialog, the page layout and
         * page size settings are remembered in the event that the user reopens
         * the dialog during the same application session.
         */
        QPrinter* printer;

        QFutureWatcher<QString>* futureWatcher;

        /*
         * Sets the HTML contents to display, and creates a backup of the old
         * HTML for diffing to scroll to the first difference whenever
         * updatePreview() is called.
         */
        void setHtml(const QString& html);

        QString exportToHtml(const QString& text, Exporter* exporter) const;
        
        /*
         * Gets the current printer settings. Default settings are lazy loaded
         * as needed, since initializing the QPrinter class can take several
         * seconds on some systems.
         */
        QPrinter* getPrinterSettings();
};

#endif
