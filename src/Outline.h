/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
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

#ifndef OUTLINE_H
#define OUTLINE_H

#include <QListWidget>
#include <QString>
#include <QTextBlock>

/**
 * Outline widget for use in navigating document headings and displaying the
 * current position in the document to the user.
 */
class Outline : public QListWidget
{
    Q_OBJECT

    public:
        Outline(QWidget* parent = 0);
        virtual ~Outline();

    signals:
        /**
         * Emitted when the user selects one of the headings in the outline
         * in order to navigate to the given position in the document.
         * This signal should be connected to an editor so that the text
         * cursor position can be set to the provided value.
         */
        void documentPositionNavigated(const int position);

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

        /**
         * Inserts heading text into the outline for the given heading level
         * and QTextBlock's document position.
         */
        void insertHeadingIntoOutline
        (
            int level,
            const QString& text,
            const QTextBlock& block
        );

        /**
         * Removes heading having the given document position from the outline.
         */
        void removeHeadingFromOutline(int position);

    private slots:
        /*
         * Invoked when the user selects one of the headings in the outline
         * in order to navigate to a different position in the document.
         */
        void onOutlineHeadingSelected(QListWidgetItem* item);

    private:
        static const int TEXT_BLOCK_ROLE;

        int currentPosition;

        /*
         * Gets the document position stored in the given item.
         */
        int getDocumentPosition(QListWidgetItem* item);

        /*
         * Binary search of the outline tree.  Returns row of the matching
         * QListWidgetItem, or else -1 if the item with the given document
         * position is not found.
         *
         * If exactMatch is false and the item is not found, this method will
         * return the row number where the heading would belong if it were in
         * the tree (i.e., an ideal insertion point for a new heading).
         */
        int findHeading(int position, bool exactMatch = true);

};

#endif // OUTLINE_H
