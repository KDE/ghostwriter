/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef HTML_PREVIEW_H
#define HTML_PREVIEW_H

#include <QScopedPointer>
#include <QString>
#include <QWidget>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QtWebEngineWidgets>
#else
#include <QtWebEngineWidgets/QWebEngineView>
#endif

#include "editor/markdowndocument.h"
#include "export/exporter.h"

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
        QWidget *parent = nullptr
    );

    /**
     * Destructor.
     */
    virtual ~HtmlPreview();

    /**
     * Customize QtWebEngine context menu.
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

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

    /**
     * Call this method to enable or disable math rendering.
     */
    void setMathEnabled(bool enabled);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QScopedPointer<HtmlPreviewPrivate> d_ptr;

};
} // namespace ghostwriter

#endif
