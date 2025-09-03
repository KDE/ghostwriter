/*
 * SPDX-FileCopyrightText: 2022-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef ASYNCTEXTWRITER_H
#define ASYNCTEXTWRITER_H

#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QUrl>
#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <QTextCodec>
#else
#include <QStringDecoder>
#endif

namespace ghostwriter
{
/**
 * Writes document text asynchronously to a file.
 */
class AsyncTextWriterPrivate;
class AsyncTextWriter : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(AsyncTextWriter)

public:
    typedef QStringConverter::Encoding Encoding;

    /**
     * @brief Constructor with file path to which text will be written.
     *
     * @param fileName The file path to which text will be written.
     * @param parent The parent QObject. Defaults to nullptr (no parent).
     */
    AsyncTextWriter(const QUrl &fileName, QObject *parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~AsyncTextWriter();

    /**
     * @brief Returns the file name.
     *
     * @return The file name.
     */
    QUrl fileName() const;

    /**
     * @brief Sets the file name.
     *
     * @param fileName The file name.
     */
    void setFileName(const QUrl &fileName);

    /**
     * @brief Returns the encoding.
     *
     * @return The encoding.
     */
    Encoding encoding() const;

    /**
     * @brief Sets the encoding.
     *
     * @param encoding The encoding to set.
     *
     * @note The default encoding if none is set with this method is UTF-8.
     */
    void setEncoding(Encoding encoding);

    /**
     * @brief Returns whether a write operation on the file is currently in progress.
     *
     * @return true if a write is currently in progress.
     * @return false otherwise.
     */
    bool writeInProgress() const;

    /**
     * @brief Waits for write to finish (if needed) before returning.
     */
    void waitForFinished() const;

    /**
     * @brief Writes the given text to the file.
     *
     * @note Previous contents of the file will be replaced.
     */
    bool write(const QString &text);

signals:
    /**
     * @brief Emitted when the write is complete.
     *
     * @note Signal will not be emitted if an error occurs. (See writeError signal instead.)
     *
     * @see writeError
     */
    void writeComplete();

    /**
     * @brief Emitted when an error occurs while attempting to write to the file.
     *
     * The error description will be set in the errorString parameter.
     *
     * @param errorString The error description.
     */
    void writeError(const QString &errorString);

private:
    QScopedPointer<AsyncTextWriterPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // ASYNCTEXTWRITER_H
