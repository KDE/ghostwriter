/*
 * SPDX-FileCopyrightText: 2022-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QApplication>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QSaveFile>
#include <QTextStream>
#include <QtConcurrentRun>

#ifdef Q_OS_LINUX
#include <KIO/StoredTransferJob>
#include <QUrl>
#endif

#include "asynctextwriter.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define DEFAULT_STREAM_CODEC QTextCodec::codecForName("UTF-8")
#else
#define DEFAULT_STREAM_CODEC QStringConverter::Utf8;
#endif

namespace ghostwriter
{
class AsyncTextWriterPrivate
{
    Q_DECLARE_PUBLIC(AsyncTextWriter)

public:
    AsyncTextWriterPrivate(AsyncTextWriter *q_ptr)
        : q_ptr(q_ptr)
    {
    }
    ~AsyncTextWriterPrivate()
    {
    }

    AsyncTextWriter *q_ptr;
    QUrl fileName;
    AsyncTextWriter::Encoding encoding;
    QFutureWatcher<QString> *writeFutureWatcher = nullptr;
    bool writeInProgress = false;

    void initialize(const QUrl &fileName);

    /*
     * Writes the given text to the given file path, returning a null
     * string if successful, otherwise an error message.  Note that this
     * method is intended to be run in a separate thread from the main
     * Qt event loop, and should thus never interact with any widgets.
     */
    static QString writeToDisk(const QString &text, const QUrl &fileName, AsyncTextWriter::Encoding encoding);

    /*
     * Handles any errors or tidying up after an asynchronous save operation.
     */
    void onWriteCompleted();
};

AsyncTextWriter::AsyncTextWriter(const QUrl &fileName, QObject *parent)
    : QObject(parent)
    , d_ptr(new AsyncTextWriterPrivate(this))
{
    Q_D(AsyncTextWriter);

    d->initialize(fileName);
}

AsyncTextWriter::~AsyncTextWriter()
{
    waitForFinished();
}

QString AsyncTextWriter::fileName() const
{
    Q_D(const AsyncTextWriter);

    return d->fileName;
}

void AsyncTextWriter::setFileName(const QUrl &fileName)
{
    Q_D(AsyncTextWriter);

    d->fileName = fileName;
}

void AsyncTextWriter::setEncoding(AsyncTextWriter::Encoding encoding)
{
    Q_D(AsyncTextWriter);

    d->encoding = encoding;
}

AsyncTextWriter::Encoding AsyncTextWriter::encoding() const
{
    Q_D(const AsyncTextWriter);

    return d->encoding;
}

bool AsyncTextWriter::writeInProgress() const
{
    Q_D(const AsyncTextWriter);

    return d->writeInProgress;
}

void AsyncTextWriter::waitForFinished() const
{
    Q_D(const AsyncTextWriter);

    if (d->writeFutureWatcher->isRunning() || d->writeFutureWatcher->isStarted()) {
        d->writeFutureWatcher->waitForFinished();
    }

    qApp->processEvents();
}

bool AsyncTextWriter::write(const QString &text)
{
    Q_D(AsyncTextWriter);

    if (d->fileName.isNull() || d->fileName.isEmpty()) {
        return false;
    }

    if (d->writeFutureWatcher->isRunning() || d->writeFutureWatcher->isStarted()) {
        d->writeFutureWatcher->waitForFinished();
    }

    d->writeInProgress = true;

    QFuture<QString> future = QtConcurrent::run(&AsyncTextWriterPrivate::writeToDisk, text, d->fileName, d->encoding);

    d->writeFutureWatcher->setFuture(future);

    if (d->fileName.isNull() || d->fileName.isEmpty()) {
        return false;
    }

    return true;
}

void AsyncTextWriterPrivate::initialize(const QUrl &fileName)
{
    Q_Q(AsyncTextWriter);

    this->fileName = fileName;
    encoding = DEFAULT_STREAM_CODEC;
    writeFutureWatcher = new QFutureWatcher<QString>(q);

    q->connect(writeFutureWatcher, &QFutureWatcher<QString>::finished, [this]() {
        onWriteCompleted();
    });
}

QString AsyncTextWriterPrivate::writeToDisk(const QString &text, const QUrl &fileName, AsyncTextWriter::Encoding encoding)
{
#ifdef Q_OS_LINUX
    // Check if this is a network URL (WebDAV, FTP, etc.) - use KIO on Linux
    if (!fileName.isLocalFile()
        && ((fileName.scheme() == "http") || (fileName.scheme() == "https") || (fileName.scheme() == "webdav") || (fileName.scheme() == "davs")
            || (fileName.scheme() == "ftp") || (fileName.scheme() == "sftp"))) {
        QByteArray data;
        QTextStream stream(&data);
        stream.setEncoding(encoding);
        stream << text;

        auto job = KIO::storedPut(data, fileName, -1);
        job->setUiDelegate(nullptr); // No UI for async operation

        if (!job->exec()) {
            return job->errorString();
        }

        return QString();
    }
#endif
    QString localFilePath = fileName.toLocalFile();
    if (localFilePath.isEmpty()) {
        return QStringLiteral("Invalid local file path.");
    }

    // First try QSaveFile (atomic write) for local files and non-Linux systems
    QSaveFile localAtomicFile(localFilePath);
    localAtomicFile.setDirectWriteFallback(true);

    if (localAtomicFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream stream(&localAtomicFile);
        stream.setEncoding(encoding);
        stream << text;

        if (QFile::NoError == localAtomicFile.error()) {
            if (localAtomicFile.commit()) {
                return QString(); // Success with QSaveFile
            }
        } else {
            localAtomicFile.cancelWriting();
        }
    }

    // If QSaveFile fails (common with WebDAV on Windows), fallback to direct QFile write
    QFile directFile(localFilePath);
    if (!directFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return directFile.errorString();
    }

    QTextStream directStream(&directFile);
    directStream.setEncoding(encoding);
    directStream << text;

    if (QFile::NoError != directFile.error()) {
        return directFile.errorString();
    }

    directFile.close();
    return QString(); // Success with direct write
}

void AsyncTextWriterPrivate::onWriteCompleted()
{
    Q_Q(AsyncTextWriter);

    QString err = this->writeFutureWatcher->result();

    this->writeInProgress = false;

    if (!err.isNull() && !err.isEmpty()) {
        emit q->writeError(err);
        return;
    }

    emit q->writeComplete();
}

} // namespace ghostwriter
