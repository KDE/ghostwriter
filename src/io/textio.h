/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef TEXTIO_H
#define TEXTIO_H

#include <QFileDevice>
#include <QFuture>
#include <QString>
#include <QStringConverter>
#include <QUrl>

#include "util/kerrorcode.h"
#include "util/kresult.h"

namespace ghostwriter
{
/**
 * @brief Error codes for TextIO operations.
 */
enum class TextIOError {
    NoError = 0, ///< No error occurred.
    NetworkError, ///< A network error occurred (e.g., when accessing a remote file).
    AuthenticationError, ///< An authentication error occurred.
    PermissionDenied, ///< Permission to access the file was denied.
    FileNotFound, ///< The specified file was not found.
    OpenError, ///< An error occurred while opening the file.
    ReadError, ///< An error occurred while reading from the file.
    WriteError, ///< An error occurred while writing to the file.
    RemoveError, ///< An error occurred while removing the file.
    RenameError, ///< An error occurred while renaming the file.
    ResourceError, ///< A resource error occurred.
    TimeoutError, ///< The operation timed out.
    AbortError, ///< The operation was aborted.
    FatalError, ///< A fatal error occurred.
    UnknownError ///< An unknown error occurred.
};

class TextIO
{
public:
    using WriteResult = KResult<KSuccess, TextIOError>;
    using ReadResult = KResult<QString, TextIOError>;

    static bool canHandle(const QUrl &fileUrl)
    {
        Q_UNUSED(fileUrl);
        return false;
    }

    /**
     * @brief Constructs a new TextIO object.
     *
     * @param fileUrl File path to read from or write to.
     * @param encoding Encoding to use when reading or writing text. Defaults to UTF-8.
     */
    explicit TextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8)
        : m_fileUrl(fileUrl)
        , m_encoding(encoding) { };

    /**
     * @brief Destroys the TextIO object.
     */
    virtual ~TextIO() = default;

    // Pure virtual methods

    /**
     * @brief Writes the given text to the file.
     *
     * @param text Text to write to the file.
     * @return WriteResult On success, returns true.
     * @return WriteResult On failure, returns a KErrorCode<TextIOError>.
     */
    virtual WriteResult write(const QString &text) = 0;

    /**
     * @brief Reads text from the file.
     *
     * @return ReadResult On success, returns the text read from the file.
     * @return ReadResult On failure, returns a KError<TextIOError>.
     */
    virtual ReadResult read() = 0;

    // Common functionality

    /**
     * @brief Asynchronously writes the given text to the file.
     *
     * @param text Text to write to the file.
     * @return QFuture<WriteResult> A future that will contain the result of the write operation.
     */
    QFuture<WriteResult> writeAsync(const QString &text);

    /**
     * @brief Asynchronously reads text from the file.
     *
     * @return QFuture<ReadResult> A future that will contain the result of the read operation.
     */
    QFuture<ReadResult> readAsync();

    /**
     * @brief Returns the file path.
     *
     * @return QUrl The file path.
     */
    QUrl fileUrl() const
    {
        return m_fileUrl;
    }

    /**
     * @brief Returns the file encoding.
     *
     * @return QStringConverter::Encoding The file encoding.
     */
    constexpr QStringConverter::Encoding encoding() const
    {
        return m_encoding;
    }

    /**
     * @brief Sets the file encoding.
     *
     * @param encoding The file encoding.
     */
    constexpr void setEncoding(QStringConverter::Encoding encoding)
    {
        m_encoding = encoding;
    }

protected:
    QUrl m_fileUrl;
    QStringConverter::Encoding m_encoding;

    TextIOError mapToTextIOError(QFileDevice::FileError fileError);
};
} // namespace ghostwriter

#endif // TEXTIO_H
