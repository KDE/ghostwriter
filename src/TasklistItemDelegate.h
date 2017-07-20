/***********************************************************************
 *
 * Copyright (C) 2017 hirschenberger
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

#ifndef TASKLISTITEMDELEGATE_H
#define TASKLISTITEMDELEGATE_H

#include <QItemDelegate>
#include <QListWidgetItem>
#include <QFontMetrics>
#include <QIcon>

class TasklistItemDelegate : public QItemDelegate
{
    public:
        TasklistItemDelegate(QListWidget* parent = 0);

        void drawDisplay(QPainter * painter, const QStyleOptionViewItem & option, const QString& textRectangle) const;
        void drawCheck(QPainter *painter, const QStyleOptionViewItem &option, const QRect &rect, Qt::CheckState state) const;


        QSize sizeHint ( const QStyleOptionViewItem& option, const QModelIndex&) const{
            int height = option.fontMetrics.height();

            return QSize(200, height+5);
        }
    private:
        QIcon openTask;
        QIcon closedTask;
};

#endif // TasklistItemDelegate_H
