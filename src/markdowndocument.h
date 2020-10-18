/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#ifndef MARKUPDOCUMENT_H
#define MARKUPDOCUMENT_H

#include <QTextDocument>
#include <QString>
#include <QDateTime>
#include <QTextBlock>
#include "markdownast.h"

namespace ghostwriter
{
/**
 * Text document that maintains timestamp, read-only state, and new vs.
 * saved status.
 */
class MarkdownDocument : public QTextDocument
{
    Q_OBJECT

public:
    /**
     * Constructor.
     */
    MarkdownDocument(QObject *parent = 0);

    /**
     * Constructor.
     */
    MarkdownDocument(const QString &text, QObject *parent = 0);

    /**
     * Destructor.
     */
    virtual ~MarkdownDocument();

    /**
     * Gets display name for the document for displaying in a text
     * editor's containing window or tab.
     */
    QString displayName() const;

    /**
     * Gets the document file path.
     */
    QString filePath() const;

    /**
     * Sets the document file path.
     */
    void setFilePath(const QString &path);

    /**
     * Returns true if the document is new with no file path.
     */
    bool isNew() const;

    /**
     * Returns true if the document has read only permissions.
     */
    bool isReadOnly() const;

    /**
     * Sets whether the document has read only permissions.
     */
    void setReadOnly(bool readOnly);

    /**
     * Gets the timestamp of the document, which is useful when comparing
     * the last modified time of the file represented on disk.
     */
    QDateTime timestamp() const;

    /**
     * Sets a timestamp to the document, which is useful when comparing
     * the last modified time of the file represented on disk.
     */
    void setTimestamp(const QDateTime &timestamp);

    MarkdownAST *markdownAST() const;
    void setMarkdownAST(MarkdownAST *ast);

    /**
     * For internal use only with TextBlockData class.  Emits signals
     * to notify listeners that the given text block is about to be
     * removed from the document.
     */
    void notifyTextBlockRemoved(const QTextBlock &block);

signals:
    /**
     * Emitted when the file path changes.
     */
    void filePathChanged();

    /**
     * Emitted when the QTextBlock at the given position in the document
     * is removed.
     */
    void textBlockRemoved(int position);

    /**
     * Emitted when a QTextBlock is removed from the document.  Parameter
     * is the QTextBlock that is being removed.
     */
    void textBlockRemoved(const QTextBlock &block);

private:
    QString m_displayName;
    QString m_filePath;
    bool readOnlyFlag;
    QDateTime m_timestamp;
    MarkdownAST *ast;

    /*
    * Initializes the class for an untitled document.
    */
    void initializeUntitledDocument();
};
} // namespace ghostwriter

#endif // MARKUPDOCUMENT_H
