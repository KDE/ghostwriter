/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef OUTLINE_WIDGET_H
#define OUTLINE_WIDGET_H

#include <QScopedPointer>
#include <QListWidget>

#include "editor/markdowneditor.h"

namespace ghostwriter
{
/**
 * Outline widget for use in navigating document headings and displaying the
 * current position in the document to the user.
 */
class OutlineWidgetPrivate;
class OutlineWidget : public QListWidget
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(OutlineWidget)

public:
    OutlineWidget(MarkdownEditor *editor, QWidget *parent = nullptr);
    virtual ~OutlineWidget();

signals:
    /**
     * Emitted when the user selects one of the headings in the outline
     * in order to navigate to the given position in the document.
     * This signal informs connected slots which heading in the document
     * was selected by the user.  For example, if headingSequenceNumber
     * is 1, then the very first heading that appears in the document
     * was selected.  If it is 2, then the second heading that appears
     * in the document was selected, and so on.
     */
    void headingNumberNavigated(int headingSequenceNumber);

public slots:
    /**
     * Using the user's current document position, updates which entry
     * in the navigation outline is highlighted/styled so that the user
     * knows in which section of the document the text cursor is located.
     */
    void updateCurrentNavigationHeading(int position);

private:
    QScopedPointer<OutlineWidgetPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // OUTLINE_WIDGET_H
