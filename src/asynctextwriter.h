/***********************************************************************
 *
 * Copyright (C) 2022 wereturtle
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

#ifndef ASYNCTEXTWRITER_H
#define ASYNCTEXTWRITER_H

#include <QtGlobal>
#include <QObject>
#include <QScopedPointer>
#include <QString>

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
    // typedef encoding/codec to simplify trasition to Qt 6 while still
    // maintaining backward compatibility with Qt 5.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    typedef QTextCodec* Encoding;
#else
    typedef QStringConverter::Encoding Encoding;
#endif

    /**
     * Constructor with file path to which text will be written.
     */
    AsyncTextWriter(const QString &fileName,
        QObject *parent = nullptr);

    /**
     * Destructor.
     */
    ~AsyncTextWriter();

    /**
     * Returns the file name.
     */
    QString fileName() const;

    /**
     * Sets the file name.
     */
    void setFileName(const QString &fileName);

    /**
     * Returns the encoding.
     */
    Encoding encoding() const;

    /**
     * Sets the encoding.  The default encoding if none is
     * set with this method is UTF-8.
     */
    void setEncoding(Encoding encoding);

    /**
     * Returns true if a write is currently in progress, false otherwise.
     */
    bool writeInProgress() const;

    /**
     * Waits for write to finish (if needed) before returning.
     */
    void waitForFinished();

    /**
     * Writes the given text to the file.  Note: Previous contents of the file
     * will be replaced.
     */
    bool write(const QString &text);

signals:
    /**
     * Emitted when the write is complete.  Signal will not be emitted if
     * an error occurs.  (See writeError signal instead.)
     */
    void writeComplete();

    /**
     * Emitted when an error occurs while attempting to write to the file.
     * The error description will be set in the errorString parameter.
     */
    void writeError(const QString &errorString);

private:
    QScopedPointer<AsyncTextWriterPrivate> d_ptr;
};
} //namespace ghostwriter

#endif // ASYNCTEXTWRITER_H