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

#ifndef OUTLINE_H
#define OUTLINE_H

#include <QListWidget>
#include <QString>

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
         * Connect to this slot for when the source document that is being
         * outlined is modified.  This slot method will take care of shifting
         * the document positions in the outline by the number of characters
         * added/removed, as well as removing from the outline deleted headings
         * that may have been in range of any deleted text.
         *
         * IMPORTANT NOTE:
         *
         * Order of connection is extremely important.  The object that
         * monitors the text document and signals this class's
         * insertHeadingIntoOutline() and removeHeadingFromOutline() slots
         * should be connected to those slots AFTER the being connected to
         * the text document.  In other words, if you are using
         * the Highlighter class to notify this class of changes to the
         * document headings, then connect the Highlighter class to the
         * text document AFTER this class has been connected to the text
         * document's contentsChange() signal.  The connections must happen
         * in this order, or else the Outline will not have a chance to update
         * the heading positions, resulting in possibly duplicate headings
         * being inserted by the Highlighter into the Outline.
         */
        void onTextChanged
        (
            int position,
            int charsRemoved,
            int charsAdded
        );

        /**
         * Inserts heading text into the outline for the given heading level
         * and document position.
         */
        void insertHeadingIntoOutline(int position, int level, const QString heading);

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
        static const int HEADING_LEVEL_ROLE;
        static const int DOCUMENT_POS_ROLE;

        int currentPosition;

        /*
         * Adds value of offset to the document positions stored in each
         * heading, beginning with the heading whose position is greater
         * than  startPosition.  This method is called when text is added
         * or deleted from text document.
         */
        void updateDocumentPositions(int startPosition, int offset);

};

#endif // OUTLINE_H
