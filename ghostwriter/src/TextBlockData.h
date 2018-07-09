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

#include "TextDocument.h"

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
        TextBlockData(TextDocument* document, const QTextBlock& block)
            : document(document), blockRef(block)
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
            document->notifyTextBlockRemoved(blockRef);
        }

        TextDocument* document;

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
};

#endif // TEXTBLOCKDATA_H
