/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MARKUPDOCUMENT_H
#define MARKUPDOCUMENT_H

#include <QDateTime>
#include <QScopedPointer>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>

#include "markdownast.h"

namespace ghostwriter
{
/**
 * Text document that maintains timestamp, read-only state, and new vs.
 * saved status.
 */
class MarkdownDocumentPrivate;
class MarkdownDocument : public QTextDocument
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MarkdownDocument)

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
     * Overrides base class clear() method to send cleared() signal.
     */
    void clear();

signals:
    /**
     * Emitted when the file path changes.
     */
    void filePathChanged();

    /**
     * Emitted when the contents of the document is cleared.
     */
    void cleared();

private:
    QScopedPointer<MarkdownDocumentPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // MARKUPDOCUMENT_H
