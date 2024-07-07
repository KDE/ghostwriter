/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MARKDOWNDOCUMENT_H
#define MARKDOWNDOCUMENT_H

#include <QDateTime>
#include <QScopedPointer>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>

#include <functional>
#include <tuple>

#include "../markdown/markdownast.h"

namespace ghostwriter
{

// TODO: Update AsyncTextWriter to use QSaveFile.

/**
 * Text document that maintains timestamp, read-only state, and new vs.
 * saved status. 
 */
class MarkdownDocumentPrivate;
class MarkdownDocument : public QTextDocument
{
    Q_OBJECT

public:
    /**
     * Creates a new, untitled document.
     */
    static MarkdownDocument *create();

    /**
     * Creates a draft document with a temporary file in the given draft
     * directory path backing its contents.
     *
     * This method will return null if an error occurs, in which case
     * errorString will be set to the relevant error message. Otherwise,
     * errorString will be empty.
     */
    static MarkdownDocument *create(const QString &draftDirPath, QString &err);

    /**
     * Loads a document at the given file path.
     *
     * This method will return null if an error occurs, in which case
     * errorString will be set to the relevant error message. Otherwise,
     * errorString will be empty.
     */
    static MarkdownDocument *load(const QString &filePath, QString &err);

    /**
     * Loads a draft document backed by the given temporary file path.
     *
     * This method will return null if an error occurs, in which case
     * errorString will be set to the relevant error message. Otherwise,
     * errorString will be empty.
     */
    static MarkdownDocument *recover(const QString &filePath, QString &err);

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
     * Returns true if the document is new with no file path or else an
     * untitled draft that is backed by a temporary file path, false otherwise.
     */
    bool isNew() const;

    /**
     * Returns true if the document is an untitled draft backed by a temporary
     * file path, false otherwise.
     */
    bool isDraft() const;

    /**
     * Returns true if the document has a backup file from before the start
     * of the session to which it can revert, false otherwise.
     */
    bool hasBackup() const;

    /**
     * Returns true if there is a conflict with the file contents in memory
     * vs. the contents on disk, false otherwise. This method should be
     * called to see if the file should be reloaded from disk or else
     * overwritten by what is in memory.
     */
    bool hasConflict() const;

    /**
     * Returns true if the document has read only permissions.
     */
    bool isReadOnly() const;

    /**
     * Gets the last modification time of the document, which is useful when
     * comparing the last modified time of the file represented on disk.
     */
    QDateTime lastModified() const;

    MarkdownAST *markdownAST() const;
    void setMarkdownAST(MarkdownAST *ast);

    bool rename();

    /**
     * Saves the document to disk.
     */
    void save();

    /**
     * Saves the document to disk at the given location.
     */
    void saveAs(const QString &filePath);

    /**
     * Revert to the backup on disk prior to opening the document and
     * editing/auto-saving it.
     */
    std::tuple<bool, QString> revert();

    QString errorString();

signals:
    void saveComplete(bool ok, const QString message = QString());

private:
    void setDraft(bool draft);
    void setFilePath(const QString &filePath);

    typedef enum { CreateDraft, ReloadDraft, OpenExisting } Mode;

    /*
     * Constructor. Creates a new, untitled document.
     */
    MarkdownDocument();

    /*
     * Constructor. Creates or Loads a document with the given path.  If
     * draft is set to true, a draft document backed by the given file
     * path will be created if it does not already exist, or else loaded
     * if it does.
     */
    MarkdownDocument(const QString &text);

private:
    QScopedPointer<MarkdownDocumentPrivate> d;
};
} // namespace ghostwriter

#endif // MARKDOWNDOCUMENT_H
