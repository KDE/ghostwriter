/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MARKDOWNDOCUMENT_H
#define MARKDOWNDOCUMENT_H

#include <QDateTime>
#include <QList>
#include <QScopedPointer>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>
#include <QUrl>

#include "../markdown/markdownast.h"

namespace ghostwriter
{

// TODO: Update AsyncTextWriter to use QSaveFile.

/**
 * Text document.
 */
class MarkdownDocumentPrivate;
class MarkdownDocument : public QTextDocument
{
    Q_OBJECT

public:
    /**
     * Returns true if documents will automatically load changes made
     * externally to their backing files, provided their internal texts buffers
     * have not been modified with unsaved changes, false otherwise.
     */
    static bool autoLoadEnabled();

    /**
     * Sets whether documents should automatically load changes made
     * externally to their backing files, provided their internal text buffers
     * have not been modified with unsaved changes.
     */
    static void setAutoLoadEnabled(bool enabled);

    /**
     * Sets the directory where untitled draft documents should be created
     * for backup purposes.
     */
    void setDraftLocation(const QDir &path);

    /**
     * Returns the directory where untitled draft documents will be created
     * for backup purposes.
     */
    QDir draftLocation() const;

    /**
     * Creates a new, untitled (draft) document with no backing file path.
     */
    MarkdownDocument(QWidget *parent = nullptr);

    /**
     * Loads the file contents at the given URL, returning true on success.
     *
     * If an error occurs loading the file, this method will emit the
     * loadError() signal and return false,
     */
    bool openFile(const QUrl &url);

    /**
     * Loads an untitled draft at the given local file path, returning
     * true on success.
     *
     * If an error occurs loading the file, this method will emit the
     * loadError() signal and return false.
     */
    bool openDraft(const QString &path);

    /**
     * Destructor.
     */
    virtual ~MarkdownDocument();

    /**
     * Returns true if the document is an untitled draft, false otherwise.
     */
    bool isDraft() const;

    /**
     * Gets the document file URL.  If the document is untitled with no draft
     * draft file backing it, then URL's isEmpty() method will return true.
     */
    QUrl url() const;

    /**
     * Returns true if this document has been modified without having been
     * saved to disk. This method always returns false if the file has
     * been saved with save() or saveAs(), or if auto-save is enabled.
     */
    bool modified() const;

    /**s
     * Returns true if there is a conflict with the file contents in memory
     * vs. the contents on disk, false otherwise. This method should be
     * called to see if the file should be reloaded from disk or else
     * overwritten by what is in memory.
     */
    bool hasConflict() const;

    /**
     * Returns true if the document is writeable (i.e., not having read only
     * permissions), false otherwise.
     */
    bool isWritable() const;

    /**
     * Gets the last modification time of the document, which is useful when
     * comparing the last modified time of the file represented on disk.
     */
    QDateTime lastModifiedTime() const;

    /**
     * Renames the file to the given name (without changing its directory).
     */
    void rename(const QString &name);

    /**
     * Saves the document to disk.
     */
    void save();

    /**
     * Saves the document to disk at the given location, changing the document's
     * URL to the one given.
     */
    void saveAs(const QUrl &url);

    /**
     * Saves the document to disk to the given location as a copy without
     * changing the current URL of the document.
     */
    void saveCopyAs(const QUrl &url) const;

    /**
     * Reverts the document's contents to the given backup file on disk that
     * was created as a snapshot of a prior version of the document.
     */
    void revert(const QString &snapshotPath);

    /**
     * Returns the Markdown AST tree associated with this document, or null
     * if not set.
     */
    MarkdownAST *markdownAST() const;

    /**
     * Sets the document's Markdown AST tree. Note that the document will
     * take ownership of the object and is responsible for deleting it.
     */
    void setMarkdownAST(MarkdownAST *ast);

    /**
     * Returns a description of the last error that occurred, or else a null/
     * empty string if no error occurred.
     */
    QString errorString() const;

signals:
    void conflictDetected();
    void modifiedChanged(bool modified);
    void loadError(const QString &description);
    void loadProgress(int percent);
    void saveError(const QString &description);
    void urlChanged(const QUrl &url);

private:
    QScopedPointer<MarkdownDocumentPrivate> d;
};
} // namespace ghostwriter

#endif // MARKDOWNDOCUMENT_H
