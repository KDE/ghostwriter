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

#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QObject>
#include <QTextBlockUserData>
#include <QTextBlock>

/**
 * User data for use with the MarkdownHighlighter and DocumentStatistics.
 */
class TextBlockData : public QObject, public QTextBlockUserData
{
    Q_OBJECT

    public:
        /**
         * Constructor.
         */
        TextBlockData()
        {
            wordCount = 0;
            alphaNumericCharacterCount = 0;
            sentenceCount = 0;
            lixLongWordCount = 0;
            blankLine = true;
        }

        /**
         * Destructor.
         */
        virtual ~TextBlockData()
        {
            emit textBlockRemoved(blockRef);
            emit textBlockRemoved(blockRef.position());
        }

        int wordCount;
        int alphaNumericCharacterCount;
        int sentenceCount;
        int lixLongWordCount;
        bool blankLine;

        /**
         * Parent text block.  For use with fetching the block's document
         * position, which can shift as text is inserted and deleted.
         */
        QTextBlock blockRef;

    signals:
        /**
         * Emitted when the parent QTextBlock at the given position in the
         * document is removed.
         */
        void textBlockRemoved(int position);

        /**
         * Emitted when the given parent QTextBlock is removed from the
         * document.
         */
        void textBlockRemoved(QTextBlock& block);
};

#endif // TEXTBLOCKDATA_H
