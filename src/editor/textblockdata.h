/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QObject>
#include <QTextBlock>
#include <QTextBlockUserData>

#include "markdown/markdownast.h"

#include "markdowndocument.h"

namespace ghostwriter
{
/**
 * User data for use with the MarkdownHighlighter and DocumentStatistics.
 */
class TextBlockData : public QTextBlockUserData
{
public:

    typedef struct MarkupRange
    {
        int start;
        int end;
        MarkdownNode::NodeType type;
    } MarkupRange;

    typedef QVector<MarkupRange> MarkupRanges;

    /**
     * Constructor.
     */
    TextBlockData()
    {
        wordCount = 0;
        alphaNumericCharacterCount = 0;
        sentenceCount = 0;
        lixLongWordCount = 0;
    }

    /**
     * Destructor.
     */
    virtual ~TextBlockData()
    {
        ;
    }

    void clearMarkup()
    {
        markup.clear();
    }

    int wordCount;
    int alphaNumericCharacterCount;
    int sentenceCount;
    int lixLongWordCount;

    MarkupRanges markup;
};
} // namespace ghostwriter

#endif // TEXTBLOCKDATA_H
