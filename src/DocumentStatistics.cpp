/***********************************************************************
 *
 * Copyright (C) 2016 wereturtle
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

#include <QtCore/qmath.h>
#include <QTextBoundaryFinder>

#include "DocumentStatistics.h"

DocumentStatistics::DocumentStatistics(TextDocument* document, QObject* parent)
    : QObject(parent), document(document)
{
    wordCount = 0;
    wordCharacterCount = 0;
    sentenceCount = 0;
    paragraphCount = 0;
    lixLongWordCount = 0;

    connect(this->document, SIGNAL(contentsChange(int,int,int)), this, SLOT(onTextChanged(int,int,int)));
    connect(this->document, SIGNAL(textBlockRemoved(const QTextBlock&)), this, SLOT(onTextBlockRemoved(const QTextBlock&)));
}

DocumentStatistics::~DocumentStatistics()
{

}

int DocumentStatistics::getWordCount() const
{
    return wordCount;
}

void DocumentStatistics::onTextSelected
(
    const QString& selectedText,
    int selectionStart,
    int selectionEnd
)
{
    int selectionWordCount;
    int selectionLixLongWordCount;
    int selectionWordCharacterCount;

    countWords
    (
        selectedText,
        selectionWordCount,
        selectionLixLongWordCount,
        selectionWordCharacterCount
    );

    int selectionSentenceCount = countSentences(selectedText);

    // Count the number of selected paragraphs.
    int selectedParagraphCount = 0;

    QTextBlock block = this->document->findBlock(selectionStart);
    QTextBlock end = this->document->findBlock(selectionEnd).next();

    while (block != end)
    {
        TextBlockData* blockData = (TextBlockData*) block.userData();

        if ((NULL != blockData) && !blockData->blankLine)
        {
            selectedParagraphCount++;
        }

        block = block.next();
    }

    emit wordCountChanged(selectionWordCount);
    emit characterCountChanged(selectedText.length());
    emit sentenceCountChanged(selectionSentenceCount);
    emit paragraphCountChanged(selectedParagraphCount);
    emit pageCountChanged(calculatePageCount(selectionWordCount));
    emit complexWordsChanged(calculateComplexWords(selectionWordCount, selectionLixLongWordCount));
    emit readingTimeChanged(calculateReadingTime(selectionWordCount));
    emit lixReadingEaseChanged(calculateLIX(selectionWordCount, selectionLixLongWordCount, selectionSentenceCount));
    emit readabilityIndexChanged(calculateCLI(selectionWordCharacterCount, selectionWordCount, selectionSentenceCount));
}

void DocumentStatistics::onTextDeselected()
{
    updateStatistics();
}

void DocumentStatistics::onTextChanged(int position, int charsRemoved, int charsAdded)
{
    Q_UNUSED(charsRemoved)

    int startIndex = position - charsRemoved;

    if (startIndex < 0)
    {
        startIndex = 0;
    }

    int endIndex = position + charsAdded;

    if ((endIndex < startIndex) || (endIndex >= document->characterCount()))
    {
        endIndex = document->characterCount() - 1;
    }

    // Update the word counts of affected blocks.  Note that there is no need to
    // check for changes to section headings, since the Highlighter class will
    // take care of this for us.
    //
    QTextBlock startBlock = document->findBlock(startIndex);
    QTextBlock endBlock = document->findBlock(endIndex);

    QTextBlock block = startBlock;

    updateBlockStatistics(block);

    while (block != endBlock)
    {
        block = block.next();
        updateBlockStatistics(block);
    }

    updateStatistics();
}

void DocumentStatistics::onTextBlockRemoved(const QTextBlock& block)
{
    TextBlockData* blockData = (TextBlockData*) block.userData();

    if (NULL != blockData)
    {
        wordCount -= blockData->wordCount;
        lixLongWordCount -= blockData->lixLongWordCount;
        wordCharacterCount -= blockData->alphaNumericCharacterCount;
        sentenceCount -= blockData->sentenceCount;

        if (!blockData->blankLine)
        {
            paragraphCount--;
        }

        updateStatistics();
    }
}

void DocumentStatistics::updateStatistics()
{
    emit wordCountChanged(wordCount);
    emit totalWordCountChanged(wordCount);
    emit characterCountChanged(document->characterCount() - 1);
    emit sentenceCountChanged(sentenceCount);
    emit paragraphCountChanged(paragraphCount);
    emit pageCountChanged(calculatePageCount(wordCount));
    emit complexWordsChanged(calculateComplexWords(wordCount, lixLongWordCount));
    emit readingTimeChanged(calculateReadingTime(wordCount));
    emit lixReadingEaseChanged(calculateLIX(wordCount, lixLongWordCount, sentenceCount));
    emit readabilityIndexChanged(calculateCLI(wordCharacterCount, wordCount, sentenceCount));
}

void DocumentStatistics::updateBlockStatistics(QTextBlock& block)
{
    TextBlockData* blockData = (TextBlockData*) block.userData();

    if (NULL == blockData)
    {
        blockData = new TextBlockData(document, block);
        block.setUserData(blockData);
    }

    int oldWordCount = blockData->wordCount;
    int oldLixLongWordCount = blockData->lixLongWordCount;
    int oldAlphaNumCharCount = blockData->alphaNumericCharacterCount;

    countWords
    (
        block.text(),
        blockData->wordCount,
        blockData->lixLongWordCount,
        blockData->alphaNumericCharacterCount
    );

    wordCount += blockData->wordCount - oldWordCount;
    lixLongWordCount += blockData->lixLongWordCount - oldLixLongWordCount;
    wordCharacterCount += blockData->alphaNumericCharacterCount - oldAlphaNumCharCount;

    int oldSentenceCount = blockData->sentenceCount;
    blockData->sentenceCount = countSentences(block.text());
    sentenceCount += blockData->sentenceCount - oldSentenceCount;

    if (blockData->blankLine)
    {
        if (block.text().trimmed().length() > 0)
        {
            blockData->blankLine = false;
            paragraphCount++;
        }
    }
    else if (block.text().trimmed().length() <= 0)
    {
        blockData->blankLine = true;
        paragraphCount--;
    }
}

void DocumentStatistics::countWords
(
    const QString& text,
    int& words,
    int& lixLongWords,
    int& alphaNumericCharacters
)
{
    bool inWord = false;
    int separatorCount = 0;
    int wordLen = 0;

    words = 0;
    lixLongWords = 0;
    alphaNumericCharacters = 0;

    for (int i = 0; i < text.length(); i++)
    {
        if (text[i].isLetterOrNumber())
        {
            inWord = true;
            separatorCount = 0;
            wordLen++;
            alphaNumericCharacters++;
        }
        else if (text[i].isSpace() && inWord)
        {
            inWord = false;
            words++;

            if (separatorCount > 0)
            {
                wordLen--;
                alphaNumericCharacters--;
            }

            separatorCount = 0;

            if (wordLen > 6)
            {
                lixLongWords++;
            }

            wordLen = 0;
        }
        else
        {
            // This is to handle things like double dashes (`--`)
            // that separate words, while still counting hyphenated
            // words as a single word.
            //
            separatorCount++;

            if (inWord)
            {
                if (separatorCount > 1)
                {
                    separatorCount = 0;
                    inWord = false;
                    words++;
                    wordLen--;
                    alphaNumericCharacters--;

                    if (wordLen > 6)
                    {
                        lixLongWords++;
                    }

                    wordLen = 0;
                }
                else
                {
                    wordLen++;
                    alphaNumericCharacters++;
                }
            }
        }
    }

    if (inWord)
    {
        words++;

        if (separatorCount > 0)
        {
            wordLen--;
            alphaNumericCharacters--;
        }

        if (wordLen > 6)
        {
            lixLongWords++;
        }
    }
}

int DocumentStatistics::countSentences(const QString& text)
{
    int count = 0;

    QString trimmedText = text.trimmed();

    if (trimmedText.length() > 0)
    {
        QTextBoundaryFinder boundaryFinder(QTextBoundaryFinder::Sentence, trimmedText);
        int nextSentencePos = 0;

        boundaryFinder.setPosition(0);

        while (nextSentencePos >= 0)
        {
            int oldPos = nextSentencePos;
            nextSentencePos = boundaryFinder.toNextBoundary();

            if
            (
                ((nextSentencePos - oldPos) > 1) ||
                 (((nextSentencePos - oldPos) > 0) &&
                !trimmedText[oldPos].isSpace())
            )
            {
                count++;
            }
        }
    }

    return count;
}

int DocumentStatistics::calculatePageCount(int words)
{
    return words / 250;
}

int DocumentStatistics::calculateCLI(int characters, int words, int sentences)
{
    int cli = 0;

    if ((sentences > 0) && (words > 0))
    {
        cli = qCeil
            (
                (5.88 * (qreal)((float)characters / (float)words))
                -
                (29.6 * ((qreal)sentences / (qreal)words))
                -
                15.8
            );

        if (cli < 0)
        {
            cli = 0;
        }
    }

    return cli;
}

int DocumentStatistics::calculateLIX(int totalWords, int longWords, int sentences)
{
    int lix = 0;

    if ((totalWords > 0) && sentences > 0)
    {
        lix = qCeil
            (
                ((qreal)totalWords / (qreal)sentences)
                +
                (((qreal)longWords / (qreal)totalWords) * 100.0)
            );
    }

    return lix;
}

int DocumentStatistics::calculateComplexWords(int totalWords, int longWords)
{
    int complexWordsPercentage = 0;

    if (totalWords > 0)
    {
        complexWordsPercentage = qCeil(((qreal)longWords / (qreal)totalWords) * 100.0);
    }

    return complexWordsPercentage;
}

int DocumentStatistics::calculateReadingTime(int words)
{
    return words / 270;
}

