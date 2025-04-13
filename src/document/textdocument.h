/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <QDateTime>
#include <QDir>
#include <QFileDevice>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QTextBlock>
#include <QTextDocument>

#include "../util/kerrorcode.h"
#include "../util/kresult.h"

namespace ghostwriter
{

class TextDocument;

/**
 * Text document.
 */
class TextDocumentPrivate;
class TextDocument : public QTextDocument
{
    Q_OBJECT

public:
    enum IOError {
        NoError = 0,
        InvalidPathError,
        DirectoryNotFoundError,
        FileNotFoundError,
        ReadError = 1,
        WriteError = 2,
        FatalError = 3,
        ResourceError = 4,
        OpenError = 5,
        AbortError = 6,
        TimeOutError = 7,
        UnspecifiedError = 8,
        RemoveError = 9,
        RenameError = 10,
        PositionError = 11,
        ResizeError = 12,
        PermissionsError = 13,
        CopyError = 14
    };

    using Result = KResult<TextDocument *, IOError>;
    using IOErrorCode = KErrorCode<IOError>;

    /**
     * Returns true if documents will automatically load changes made
     * externally to their backing files, provided their internal texts buffers
     * have not been modified with unsaved changes, false otherwise.
     *
     * @return @c true if automatically loading changes from disk is enabled
     * @return @c false otherwise
     */
    constexpr static bool autoLoadEnabled();

    /**
     * Sets whether documents should automatically load changes made
     * externally to their backing files, provided their internal text buffers
     * have not been modified with unsaved changes.
     *
     * @param enabled true if automatically loading changes from disk is
     *                to be enabled, false otherwise.
     */
    constexpr static void setAutoLoadEnabled(bool enabled);

    constexpr static TextDocument::Result setDraftLocation(const QString &path);
    constexpr static QString draftLocation();

    /**
     * Creates a new, untitled (draft) document with a backing file at the
     * draft directory given by draftLocation().
     *
     * If unsuccessful in creating a backing file for the draft, a valid document
     * object will still be returned. However, its error() method will return true,
     * and errorString() will return a description of the error. In other words,
     * the document won't have a backing draft file on disk, and will be at risk of
     * data loss until a call to saveAs() can persist its contents.
     *
     * @param draftName Untitled draft display name.
     * @param parent Parent object, or nullptr for no parent.
     *
     * @return Document object.
     */
    static Result create(const QString &draftName, QObject *parent);

    /**
     * Opens the file contents at the given file path.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @param path File path from which to load the document.
     *
     * @return Document object if successful, nullptr otherwise.
     */
    static Result open(const QString &path, QObject *parent = nullptr);

    /**
     * Opens an untitled draft backed by the given local file path.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @param path Local file path from which to load the draft document.
     *
     * @return Document object if successful, nullptr otherwise.
     */
    static Result openDraft(const QString &draftName, const QString &path, QObject *parent = nullptr);

    /**
     * Destructor.
     */
    virtual ~TextDocument();

    /**
     * Closes the file, removing its contents from the buffer and completing any unsaved
     * save operation that may be in progress before returning.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @return true if successful, false otherwise (i.e., if a save error occurred).
     */
    Result close();

    constexpr QString fileName() const;

    /**
     * Returns the document file path.  If the document is a untitled with no draft
     * file backing it, then QString's isEmpty() method will return true.
     *
     * @return Path to the document, or an empty QString (QString.isEmpty() returns true) if
     *         the document is an untitled draft that has not yet been saved and has
     *         no backing file on disk.
     */
    constexpr QString filePath() const;

    /**
     * Returns the document's absolute file path.    If the document is a untitled
     * with no draft file backing it, then QString's isEmpty() method will return true.
     *
     * @return Path to the document, or an empty QString (QString.isEmpty() returns true) if
     *         the document is an untitled draft that has not yet been saved.
     */
    constexpr QString absoluteFilePath() const;

    /**
     * Returns a name for the document suitable for display.
     *
     * @return The document's display name.
     */
    QString displayName() const;

    /**
     * Returns whether the document is an untitled draft.
     *
     * @return @c true if the document is an untitled draft
     * @return @c false otherwise
     */
    constexpr bool isDraft() const;

    /**
     * Returns whether the draft document has a file on disk backing it.
     *
     * This method only applies to an untitled draft.
     *
     * @return @c true if the draft document has a file on disk backing it
     * @return @c false otherwise
     */
    constexpr bool backed() const;

    /**
     * Returns whether this document has been modified without having been
     * saved to disk.
     *
     * @return @c true if the document has been modified without having been
     *            saved to disk
     * @return @c false if the file has been saved with save() or saveAs()
     */
    constexpr bool modified() const;

    /**
     * Returns whether there is a conflict with the file contents in memory
     * vs. the contents on disk. This method should be called to see if the file
     * should be reloaded from disk or else overwritten by what is in memory.
     *
     * @return @c true if there is a conflict with the file contents in memory
     *            vs. the contents on disk
     * @return @c false otherwise
     */
    constexpr bool hasConflict() const;

    /**
     * Returns true if the document is writable (i.e., not having read only
     * permissions), false otherwise.
     */
    constexpr bool isWritable() const;

    /**
     * Gets the last modification time of the document, which is useful when
     * comparing the last modified time of the file represented on disk.
     */
    constexpr const QDateTime &lastModified() const;

    /**
     * Renames the file to the given name (without changing its directory).
     *
     * @return @c true if rename operation was successful
     * @return @c false otherwise
     */
    Result rename(const QString &name) noexcept;

    /**
     * Saves the document to disk.
     *
     * This is an asynchronous operation. If an error occurs, the saveError
     * signal will be emitted. On success, modifiedChanged will be emitted
     * with a modified value of false.
     */
    Result save();

    /**
     * Saves the document to disk at the given location, changing the document's
     * file path to the one given.
     *
     * This is an asynchronous operation. If an error occurs, the saveAsError
     * signal will be emitted. On success, modifiedChanged will be emitted
     * with a modified value of false.
     *
     * @param path file path to which to save the file.
     */
    Result saveAs(const QString &path);

    /**
     * Saves to the given location as a copy without changing the current file path
     * of the document.
     *
     * This is an asynchronous operation. If an error occurs, the saveCopyError
     * signal will be emitted. On success, modifiedChanged will be emitted
     * with a modified value of false.
     *
     * @param path file path to which to save a copy of the file.
     */
    Result saveCopyAs(const QString &path) const;

    /**
     * Reverts the document's contents to the given backup file on disk that
     * was created as a snapshot of a prior version of the document.
     *
     * @param snapshotPath Local file path of the backup file from which to restore
     *                     a prior version of the document.
     */
    Result revert(const QString &snapshotPath);

    /**
     * Reverts the document's contents to the given backup file on disk that
     * was created as a snapshot of a prior version of the document.
     */
    Result reload();

signals:
    /**
     * Emitted when a conflict with the contents on disk vs. in memory is
     * detected. Upon receiving this signal, the recipient should
     * generally prompt the user and inquire whether to overwrite the
     * contents in memory with what is on disk.
     */
    void conflictDetected();

    /**
     * Emitted when the document has been modified since the last save,
     * or else saved since the last modification in memory.
     *
     * @param modified true if the document is in a modified state in memory
     *                 and needs to be saved, false otherwise.
     */
    void modifiedChanged(bool modified);

    /**
     * Emitted when there is an error saving the document with the save()
     * operation to disk.
     *
     * @param description A description of the error that occurred.
     */
    void saveError(const IOErrorCode &err);

    /**
     * Emitted when there is an error saving the document under another name
     * with the saveAs() operation to disk.
     *
     * @param description A description of the error that occurred.
     */
    void saveAsError(const IOErrorCode &err);

    /**
     * Emitted when there is an error saving a copy of the document
     * with the saveCopyAs() operation.
     *
     * @param description A description of the error that occurred.
     */
    void saveCopyAsError(const IOErrorCode &err);

    /**
     * Emitted when document's file path changed during a saveAs() or rename()
     * operation.
     *
     * @param path The new path of the document.
     */
    void filePathChanged(const QString &path);

    /**
     * Emitted when document's display name changed during a saveAs() or rename()
     * operation.
     */
    void displayNameChanged();

private:
    QScopedPointer<TextDocumentPrivate> d;

    /*
     * Creates a new, untitled (draft) document with a backing file at the
     * draft directory given by draftLocation().
     *
     * If unsuccessful in creating a backing file for the draft, error() will
     * return true, and errorString() will return a description of the error.
     * However, the object can still be used.  It simply won't have a backing
     * draft file. As such, the document is at risk of data loss until a call
     * to saveAs() can persist its contents to disk.
     *
     * @param parent Parent object, or nullptr for no parent.
     */
    TextDocument(QObject *parent = nullptr);
};

} // namespace ghostwriter

#endif // TEXT_DOCUMENT_H
