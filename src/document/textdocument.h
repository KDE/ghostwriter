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

#include "io/textio.h"
#include "util/kerrorcode.h"
#include "util/kresult.h"

namespace ghostwriter
{

class TextDocument;

/**
 * Text document.
 */
class TextDocumentPrivate;
class TextDocument final : public QTextDocument
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(TextDocument)

public:
    using IOResult = KResult<KSuccess, TextIOError>;
    using TextIOFactoryFunction = std::function<std::unique_ptr<TextIO>(const QUrl &, QStringConverter::Encoding)>;

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
     * @param draftName Name to use for the draft document's display name.
     * @param draftUrl Untitled draft URL for use as temporary storage and display name.
     * @param encoding Encoding to use when reading or writing the document.
     * @param parent Parent object, or nullptr for no parent.
     * @param textIOFactory Function to create the TextIO instance to use with the draft URL.
     *                      This parameter should not be used except for mocking in unit tests.
     *                      Its default is already set to an internal standard production function.
     *
     * @return TextDocument object if successful, TextIOError error code otherwise.
     */
    static KResult<TextDocument *, TextIOError> create(const QString &draftName,
                                                       const QUrl &draftUrl,
                                                       QStringConverter::Encoding encoding,
                                                       QObject *parent = nullptr,
                                                       TextIOFactoryFunction textIOFactory = defaultTextIOFactory);

    /**
     * Opens the file contents at the given file path.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @param url File URL to a local path from which to load the document.
     * @param encoding Encoding to use when reading or writing the document.
     * @param parent Parent object, or nullptr for no parent.
     * @param textIOFactory Function to create the TextIO instance to use with the draft URL.
     *                      This parameter should not be used except for mocking in unit tests.
     *                      Its default is already set to an internal standard production function.
     *
     * @return TextDocument object if successful, TextIOError error code otherwise.
     */
    static KResult<TextDocument *, TextIOError>
    open(const QUrl &url, QStringConverter::Encoding encoding, QObject *parent = nullptr, TextIOFactoryFunction textIOFactory = defaultTextIOFactory);

    /**
     * Opens an untitled draft backed by the given local file path.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @param draftName Name to use for the draft document's display name.
     * @param url Local file URL from which to load the draft document.
     * @param encoding Encoding to use when reading or writing the document.
     * @param parent Parent object, or nullptr for no parent.
     * @param textIOFactory Function to create the TextIO instance to use with the draft URL.
     *                      This parameter should not be used except for mocking in unit tests.
     *                      Its default is already set to an internal standard production function.
     *
     * @return Document object if successful, TextIOError error code otherwise.
     */
    static KResult<TextDocument *, TextIOError> openDraft(const QString &draftName,
                                                          const QUrl &url,
                                                          QStringConverter::Encoding encoding,
                                                          QObject *parent = nullptr,
                                                          TextIOFactoryFunction textIOFactory = defaultTextIOFactory);

    /**
     * Destructor.
     */
    ~TextDocument();

    /**
     * Closes the file, removing its contents from the buffer and completing any unsaved
     * save operation that may be in progress before returning.
     *
     * If unsuccessful, error() will return true, and errorString() will return a
     * description of the error.
     *
     * @return IOResult indicating success or failure.
     */
    IOResult close();

    /**
     * Returns the document's URL.  If the document is a untitled with no draft
     * file backing it, then QUrl's isValid() method will return false.
     *
     * @return URL to the document, or an invalid QUrl (QUrl.isValid() returns false) if
     *         the document is an untitled draft that has not yet been saved and has
     *         no backing file on disk.
     */
    constexpr QUrl url() const;

    /**
     * Returns the document's local file path.  If the document is a untitled with no draft
     * file backing it, then QString's isEmpty() method will return true.
     *
     * @return Path to the document, or an empty QString (QString.isEmpty() returns true) if
     *         the document is an untitled draft that has not yet been saved and has
     *         no backing file on disk.
     */
    constexpr QString filePath() const;

    /**
     * Returns the document's absolute file path.  If the document is a untitled
     * with no draft file backing it, then QString's isEmpty() method will return true.
     *
     * @return Path to the document, or an empty QString (QString.isEmpty() returns true) if
     *         the document is an untitled draft that has not yet been saved.
     */
    constexpr QString absoluteFilePath() const;

    /**
     * Returns the document's canonical file path.  If the document is a untitled
     * with no draft file backing it, then QString's isEmpty() method will return true.
     *
     * @return Path to the document, or an empty QString (QString.isEmpty() returns true) if
     *         the document is an untitled draft that has not yet been saved.
     */
    constexpr QString canonicalFilePath() const;

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
     * Returns whether this document has been modified without having been
     * saved to disk.
     *
     * @return @c true if the document has been modified without having been
     *            saved to disk
     * @return @c false if the file has been saved with save() or saveAs()
     */
    constexpr bool modified() const;

    /**
     * Returns whether the document is writable.
     *
     * @return @c true if the document is writeable
     * @return @c false if the document is read-only
     */
    constexpr bool isWritable() const;

    /**
     * Returns whether the contents of the document in memory is in conflict
     * with the contents of the file on disk due to an external file IO operation.
     *
     * @return @c true the contents of the document in memory conflict with
     *                 the contents of the file on disk
     * @return false the document in memory and the file on disk are in sync
     *               with no external file IO operation having created a conflict
     */
    constexpr bool hasConflict() const;

    /**
     * Gets the last modification time of the document, which is useful when
     * comparing the last modified time of the file represented on disk.
     */
    constexpr const QDateTime &lastModified() const;

    /**
     * Renames the file to the given name (without changing its directory).
     *
     * @return IOResult indicating success or failure.
     */
    IOResult rename(const QString &name) noexcept;

    /**
     * Saves the document to disk.
     *
     * This is an asynchronous operation. If an error occurs, the saveError
     * signal will be emitted. On success, modifiedChanged will be emitted
     * with a modified value of false.
     */
    IOResult save();

    /**
     * Saves the document to disk at the given location, changing the document's
     * file path to the one given.
     *
     * This is an asynchronous operation. If an error occurs, the saveAsError
     * signal will be emitted. On success, modifiedChanged will be emitted
     * with a modified value of false.
     *
     * @param url local file path url to which to save the file.
     */
    IOResult saveAs(const QUrl &url);

    /**
     * Saves to the given location as a copy without changing the current file path
     * of the document.
     *
     * This is an asynchronous operation. If an error occurs, the saveCopyError
     * signal will be emitted. On success, modifiedChanged will be emitted
     * with a modified value of false.
     *
     * @param url local file path URL to which to save a copy of the file.
     */
    IOResult saveCopyAs(const QUrl &url) const;

    /**
     * Reverts the document's contents to the given backup file on disk that
     * was created as a snapshot of a prior version of the document.
     *
     * @param snapshotURL Local file URL of the backup file from which to restore
     *                     a prior version of the document.
     */
    IOResult revert(const QUrl &snapshotURL);

    /**
     * Reloads the document's contents to the contents on disk, provided the
     * document has not been modified in memory with unsaved changes.
     */
    IOResult reload();

    /**
     * Sets the encoding to use when reading or writing the document.
     *
     * @param encoding Encoding to use.
     */
    void setEncoding(QStringConverter::Encoding encoding);

    /**
     * @brief Returns the encoding used when reading or writing the document.
     *
     * @return QStringConverter::Encoding
     */
    QStringConverter::Encoding encoding() const;

signals:
    /**
     * Emitted when the document has been modified since the last save,
     * or else saved since the last modification in memory.
     *
     * @param modified true if the document is in a modified state in memory
     *                 and needs to be saved, false otherwise.
     */
    void modifiedChanged(bool modified);

    /**
     * Emitted when the document's file permissions have changed.
     *
     * @param modified true if the document is writeable, false otherwise.
     */
    void permissionsChanged(bool writeable);

    /**
     * Emitted when the document's encoding has changed.
     *
     * @param encoding The new encoding of the document.
     */
    void encodingChanged(QStringConverter::Encoding encoding);

    /**
     * Emitted when a conflict is detected with the file contents loaded in memory
     * versus the contents on disk.
     */
    void conflictDetected();

    /**
     * Emitted when there is an error saving the document with the save()
     * operation to disk.
     *
     * @param err A TextIOError error code with its description.
     */
    void saveError(const KErrorCode<TextIOError> &err);

    /**
     * Emitted when there is an error saving the document under another name
     * with the saveAs() operation to disk.
     *
     * @param err A TextIOError error code with its description.
     */
    void saveAsError(const KErrorCode<TextIOError> &err);

    /**
     * Emitted when there is an error saving a copy of the document
     * with the saveCopyAs() operation.
     *
     * @param err A TextIOError error code with its description.
     */
    void saveCopyAsError(const KErrorCode<TextIOError> &err);

    /**
     * Emitted when document's file URL changed during a saveAs() or rename()
     * operation.
     *
     * @param url The new URL of the document.
     */
    void fileUrlChanged(const QUrl &url);

    /**
     * Emitted when document's display name changed during a saveAs() or rename()
     * operation.
     */
    void displayNameChanged(const QString &displayName);

private:
    QScopedPointer<TextDocumentPrivate> d;

    /*
     * Creates a new TextDocument object.
     */
    TextDocument(QObject *parent);

    static std::unique_ptr<TextIO> defaultTextIOFactory(const QUrl &url, QStringConverter::Encoding encoding);
};

} // namespace ghostwriter

#endif // TEXT_DOCUMENT_H
