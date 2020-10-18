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

#ifndef OUTLINE_WIDGET_H
#define OUTLINE_WIDGET_H

#include <QScopedPointer>
#include <QListWidget>

#include "markdowneditor.h"

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
    OutlineWidget(MarkdownEditor *editor, QWidget *parent = 0);
    virtual ~OutlineWidget();

signals:
    /**
     * Emitted when the user selects one of the headings in the outline
     * in order to navigate to the given position in the document.
     * This signal informs connected slots which heading in the document
     * was selected by the user.  For example, if headingSequenceNumber
     * is 1, then the very first heading that appears in the document
     * was selected.  If it is 2, then the second heading tha appears
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
