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

#ifndef TASKLIST_H
#define TASKLIST_H

#include <QListWidget>
#include <QTextBlock>

class Tasklist: public QListWidget
{
        Q_OBJECT

    public:
        Tasklist(QWidget *parent = 0);

        static const int TEXT_BLOCK_ROLE;

    signals:
        void documentPositionNavigated(const int position);
        void taskToggled();

    public slots:
        void insertTask(Qt::CheckState, QString, QTextBlock);
        void removeTask(int);
        void toggleTask(QListWidgetItem*);

    private slots:
        void onTasklistItemSelected(QListWidgetItem *item);

    private:
        int getDocumentPosition(QListWidgetItem *item);
        int findTasklist(int position, bool exactMatch);

        int currentPosition;
};

#endif // TASKLIST_H
