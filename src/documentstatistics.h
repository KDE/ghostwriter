/***********************************************************************
 *
 * Copyright (C) 2016-2020 wereturtle
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

#ifndef DOCUMENTSTATISTICS_H
#define DOCUMENTSTATISTICS_H

#include <QObject>
#include <QScopedPointer>

#include "markdowndocument.h"
#include "textblockdata.h"

namespace ghostwriter
{
/**
 * Class to compute document statistics for a QTextDocument.
 */
class DocumentStatisticsPrivate;
class DocumentStatistics : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DocumentStatistics)

public:
    /**
     * Constructor.  Pass in the MarkdownDocument as parameter.
     */
    DocumentStatistics(MarkdownDocument *document, QObject *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~DocumentStatistics();

    /**
     * Gets the word count of the document.
     */
    int wordCount() const;

signals:
    /**
     * Emitted when word count changes.  May be word count
     * of entire document or of selected text.
     */
    void wordCountChanged(int value);

    /**
     * Emitted when word count changes.  The value is
     * always the word count of the entire document.
     */
    void totalWordCountChanged(int value);

    /**
     * Emitted when character count changes.
     */
    void characterCountChanged(int value);

    /**
     * Emitted when paragraph count changes.
     */
    void paragraphCountChanged(int value);

    /**
     * Emitted when sentence count changes.
     */
    void sentenceCountChanged(int value);

    /**
     * Emitted when page count changes.
     */
    void pageCountChanged(int value);

    /**
     * Emitted when complex word percentage changes.
     */
    void complexWordsChanged(int percentage);

    /**
     * Emitted when reading time changes.
     */
    void readingTimeChanged(int minutes);

    /**
     * Emitted when LIX reading ease value changes.
     */
    void lixReadingEaseChanged(int value);

    /**
     * Emitted when Coleman-Liau readability index (CLI) changes.
     */
    void readabilityIndexChanged(int value);

public slots:
    /**
     * Recalculates statistics text selected in the document's editor.
     */
    void onTextSelected
    (
        const QString &selectedText,
        int selectionStart,
        int selectionEnd
    );

    /**
     * Reverts statistics to be for entire document after text has been
     * deselected in the document's text editor.
     */
    void onTextDeselected();


protected slots:
    void onTextChanged(int position, int charsRemoved, int charsAdded);
    void onTextBlockRemoved(const QTextBlock &blockData);

private:
    QScopedPointer<DocumentStatisticsPrivate> d_ptr;
};

} //namespace ghostwriter

#endif // DOCUMENTSTATISTICS_H
