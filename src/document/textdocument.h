/*
 * SPDX-FileCopyrightText: 2014-2026 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef TEXT_DOCUMENT_H
#define TEXT_DOCUMENT_H

#include <QDateTime>
#include <QScopedPointer>
#include <QString>
#include <QStringConverter>
#include <QTextDocument>
#include <QUrl>

#include <functional>
#include <memory>

#include "io/textio.h"
#include "util/kerrorcode.h"
#include "util/kresult.h"

namespace ghostwriter
{

class TextDocumentPrivate;

/**
 * @brief Markdown text document with non-GUI file lifecycle operations.
 *
 * This class encapsulates open/load/save/reload/rename logic and reports
 * all I/O failures through KResult values and error signals.
 */
class TextDocument final : public QTextDocument
{
    Q_OBJECT

public:
    using IOResult = KResult<KSuccess, TextIOError>;
    using TextIOFactoryFunction = std::function<std::unique_ptr<TextIO>(const QUrl &, QStringConverter::Encoding)>;

    /**
     * @brief Creates a new untitled draft document backed by @p draftUrl.
     *
     * @param draftName Display name for the draft document.
     * @param draftUrl Local or remote URL for the draft backing file.
     * @param encoding Encoding used for future read/write operations.
     * @param parent Parent QObject.
     * @param textIOFactory Optional factory override, mainly for unit tests.
     *
     * @return KResult holding a valid TextDocument pointer on success.
     */
    static KResult<TextDocument *, TextIOError> create(const QString &draftName,
                                                       const QUrl &draftUrl,
                                                       QStringConverter::Encoding encoding,
                                                       QObject *parent = nullptr,
                                                       TextIOFactoryFunction textIOFactory = defaultTextIOFactory);

    /**
     * @brief Opens an existing document from @p url.
     *
     * @param url URL to open.
     * @param encoding Encoding used to read/write.
     * @param parent Parent QObject.
     * @param textIOFactory Optional factory override.
     *
     * @return KResult holding a valid TextDocument pointer on success.
     */
    static KResult<TextDocument *, TextIOError>
    open(const QUrl &url, QStringConverter::Encoding encoding, QObject *parent = nullptr, TextIOFactoryFunction textIOFactory = defaultTextIOFactory);

    /**
     * @brief Opens a draft document from @p url.
     *
     * @param draftName Draft display name.
     * @param url Draft URL to open.
     * @param encoding Encoding used to read/write.
     * @param parent Parent QObject.
     * @param textIOFactory Optional factory override.
     *
     * @return KResult holding a valid TextDocument pointer on success.
     */
    static KResult<TextDocument *, TextIOError> openDraft(const QString &draftName,
                                                          const QUrl &url,
                                                          QStringConverter::Encoding encoding,
                                                          QObject *parent = nullptr,
                                                          TextIOFactoryFunction textIOFactory = defaultTextIOFactory);

    /**
     * @brief Destroys the document.
     */
    ~TextDocument() override;

    /**
     * @brief Closes this document, releasing resources and making it invalid.
     *
     * After calling close(), the document enters a closed state.
     * Any subsequent operations that return an IOResult will fail
     * with an error indicating the document is closed.
     *
     * @return IOResult indicating success or failure.
     */
    IOResult close();

    /**
     * @brief Returns true when this document has been closed.
     *
     * A closed document is in an invalid state and cannot perform
     * any further I/O operations.
     */
    bool isClosed() const;

    /**
     * @brief Returns the backing URL.
     */
    QUrl url() const;

    /**
     * @brief Returns the local file path for the backing URL.
     */
    QString filePath() const;

    /**
     * @brief Returns the absolute local file path.
     */
    QString absoluteFilePath() const;

    /**
     * @brief Returns the canonical local file path.
     */
    QString canonicalFilePath() const;

    /**
     * @brief Returns a display name suitable for UI.
     */
    QString displayName() const;

    /**
     * @brief Returns true when this document is an untitled draft.
     *
     * Draft documents are always backed by a temporary file on disk
     * but have not yet been saved to a user-chosen location.
     */
    bool isDraft() const;

    /**
     * @brief Returns the current modified state.
     */
    bool modified() const;

    /**
     * @brief Returns true when file permissions currently allow write access.
     */
    bool isWritable() const;

    /**
     * @brief Returns true when the document is set to read-only mode.
     */
    bool isReadOnly() const;

    /**
     * @brief Returns true when an external file change conflict is detected.
     */
    bool hasConflict() const;

    /**
     * @brief Returns the last known in-memory save/load timestamp.
     */
    const QDateTime &lastModified() const;

    /**
     * @brief Returns the last known in-memory save/load timestamp.
     */
    const QDateTime &timestamp() const;

    /**
     * @brief Updates the last known in-memory save/load timestamp.
     */
    void setTimestamp(const QDateTime &timestamp);

    /**
     * @brief Renames the backing file inside its current directory.
     *
     * @param name New file name (not full path).
     *
     * @return IOResult indicating success or failure.
     */
    IOResult rename(const QString &name) noexcept;

    /**
     * @brief Saves the document to the current file URL asynchronously.
     *
     * @return IOResult indicating whether save dispatch started successfully.
     */
    IOResult save();

    /**
     * @brief Saves the document to @p url asynchronously and updates file URL.
     *
     * @param url Destination URL.
     *
     * @return IOResult indicating whether save dispatch started successfully.
     */
    IOResult saveAs(const QUrl &url);

    /**
     * @brief Saves a copy to @p url without changing this document's file URL.
     *
     * @param url Destination URL.
     *
     * @return IOResult indicating success or failure.
     */
    IOResult saveCopyAs(const QUrl &url) const;

    /**
     * @brief Reverts contents from snapshot URL and keeps the current file URL.
     *
     * @param snapshotURL Source snapshot URL.
     *
     * @return IOResult indicating success or failure.
     */
    IOResult revert(const QUrl &snapshotURL);

    /**
     * @brief Reloads current file URL contents from disk.
     *
     * @return IOResult indicating success or failure.
     */
    IOResult reload();

    /**
     * @brief Sets encoding for future read/write operations.
     *
     * @param encoding Encoding value.
     */
    void setEncoding(QStringConverter::Encoding encoding);

    /**
     * @brief Returns current text encoding used for I/O.
     */
    QStringConverter::Encoding encoding() const;

    /**
     * @brief Sets document read-only mode.
     *
     * @param readOnly True to prevent save and edits from manager flows.
     */
    void setReadOnly(bool readOnly);

    /**
     * @brief Assigns a new local file path and updates file metadata.
     *
     * @param filePath Local file path.
     */
    void setFilePath(const QString &filePath);

    /**
     * @brief Clears undo/redo stacks for this document.
     */
    void clearUndoRedoStacks();

signals:
    /**
     * @brief Emitted when modified state changes.
     *
     * @param modified True if unsaved modifications exist.
     */
    void modifiedChanged(bool modified);

    /**
     * @brief Emitted when effective writability changes.
     *
     * @param writeable True if writable.
     */
    void permissionsChanged(bool writeable);

    /**
     * @brief Emitted when text encoding changes.
     *
     * @param encoding New encoding.
     */
    void encodingChanged(QStringConverter::Encoding encoding);

    /**
     * @brief Emitted when an external file change conflict is detected.
     */
    void conflictDetected();

    /**
     * @brief Emitted when save() fails.
     *
     * @param err Error detail.
     */
    void saveError(const KErrorCode<TextIOError> &err);

    /**
     * @brief Emitted when saveAs() fails.
     *
     * @param err Error detail.
     */
    void saveAsError(const KErrorCode<TextIOError> &err);

    /**
     * @brief Emitted when saveCopyAs() fails.
     *
     * @param err Error detail.
     */
    void saveCopyAsError(const KErrorCode<TextIOError> &err);

    /**
     * @brief Emitted when backing URL changes.
     *
     * @param url New URL.
     */
    void fileUrlChanged(const QUrl &url);

    /**
     * @brief Emitted when local file path changes.
     *
     * @param filePath New local path.
     */
    void filePathChanged(const QString &filePath);

    /**
     * @brief Emitted when display name changes.
     *
     * @param displayName New display name.
     */
    void displayNameChanged(const QString &displayName);

    /**
     * @brief Emitted when document is cleared/closed.
     */
    void cleared();

private:
    QScopedPointer<TextDocumentPrivate> d;

    explicit TextDocument(QObject *parent = nullptr);

    static std::unique_ptr<TextIO> defaultTextIOFactory(const QUrl &url, QStringConverter::Encoding encoding);
};

} // namespace ghostwriter

#endif // TEXT_DOCUMENT_H
