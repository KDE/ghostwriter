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

#ifndef HTML_PREVIEW_H
#define HTML_PREVIEW_H

#include <QScopedPointer>
#include <QString>
#include <QWidget>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtWebEngineWidgets>
#else
#include <QWebEngineView>
#endif

#include "exporter.h"
#include "markdowndocument.h"

namespace ghostwriter
{
/**
 * Live HTML Preview window.
 */
class HtmlPreviewPrivate;
class HtmlPreview : public QWebEngineView
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(HtmlPreview)

public:
    /**
     * Constructor.  Takes text document to be rendered as HTML as
     * parameter.
     */
    HtmlPreview
    (
        MarkdownDocument *document,
        Exporter *exporter,
        QWidget *parent = 0
    );

    /**
     * Destructor.
     */
    virtual ~HtmlPreview();

    /**
     * Customize QtWebEngine context menu.
     */
    void contextMenuEvent(QContextMenuEvent *event);

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
    void setHtmlExporter(Exporter *exporter);

    /**
     * Call this method to change the CSS style sheet code.
     */
    void setStyleSheet(const QString &css);

protected:
    void closeEvent(QCloseEvent *event);

private:
    QScopedPointer<HtmlPreviewPrivate> d_ptr;

};
} // namespace ghostwriter

#endif
