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

#ifndef HTML_PREVIEW_H
#define HTML_PREVIEW_H

#include <QWidget>
#include <QMainWindow>
#include <QTextDocument>
#include <QComboBox>
#include <QThread>
#include <QTimer>
#include <QList>
#include <QPrinter>
#include <QPushButton>
#include <QRegExp>
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
            QWidget* parent = 0
        );

        /**
         * Destructor.
         */
        virtual ~HtmlPreview();

    signals:
        /**
         * Emitted when a lengthy operation has started, such as when the user
         * chooses to export the document to disk, so that the user can be
         * informed, perhaps via a progress bar.  The description provides
         * descriptive text as to the nature of the operation to display to
         * the user.
         */
        void operationStarted(const QString& description);

        /**
         * Emitted when a lengthy operation has finished, such as when an
         * exportation of the document to disk has completed, so that the user
         * can be informed, perhaps via the removal of a progress bar previously
         * displayed when the operation began.
         */
        void operationFinished();

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

    private slots:
        void onHtmlReady();
        void onPreviewerChanged(int index);
        void changeStyleSheet(int index);
        void printPreview();
        void printHtmlToPrinter(QPrinter* printer);
        void onExport();
        void copyHtml();
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
        QComboBox* previewerComboBox;
        QComboBox* styleSheetComboBox;
        Exporter* exporter;
        QTimer* htmlPreviewUpdateTimer;
        int minWidth;
        bool documentChanged;
        bool typingPaused;
        QString html;
        QRegExp headingTagExp;
        int lastStyleSheetIndex;
        QStringList customCssFiles;

        // flag used to prevent recursion in changeStyleSheet
        bool handlingStyleSheetChange;

        QFutureWatcher<QString>* futureWatcher;
        QStringList defaultStyleSheets;

        /*
         * Sets the HTML contents to display, and creates a backup of the old
         * HTML for diffing to scroll to the first difference whenever
         * updatePreview() is called.
         */
        void setHtml(const QString& html);

        QString exportToHtml(const QString& text, Exporter* exporter) const;
};

#endif
