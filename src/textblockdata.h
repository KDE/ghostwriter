/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TEXTBLOCKDATA_H
#define TEXTBLOCKDATA_H

#include <QObject>
#include <QTextBlock>
#include <QTextBlockUserData>

#include "markdowndocument.h"

namespace ghostwriter
{
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
    TextBlockData(MarkdownDocument *document, const QTextBlock &block)
        : document(document), blockRef(block)
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

    MarkdownDocument *document;

    int wordCount;
    int alphaNumericCharacterCount;
    int sentenceCount;
    int lixLongWordCount;

    /**
     * Parent text block.  For use with fetching the block's document
     * position, which can shift as text is inserted and deleted.
     */
    QTextBlock blockRef;
};
} // namespace ghostwriter

#endif // TEXTBLOCKDATA_H
