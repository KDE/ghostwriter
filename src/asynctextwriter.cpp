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

#include <QApplication>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QSaveFile>
#include <QtConcurrentRun>
#include <QTextStream>

#include "asynctextwriter.h"

namespace ghostwriter
{
class AsyncTextWriterPrivate
{
    Q_DECLARE_PUBLIC(AsyncTextWriter)

public:
    AsyncTextWriterPrivate(AsyncTextWriter *q_ptr) :
        q_ptr(q_ptr) { }
    ~AsyncTextWriterPrivate() { }

    AsyncTextWriter *q_ptr;
    QString fileName;
    QTextCodec *codec = nullptr;
    QFutureWatcher<QString> *writeFutureWatcher = nullptr;
    bool writeInProgress = false;

    void initialize(const QString &fileName,
        QTextCodec *codec = nullptr);

    /*
    * Writes the given text to the given file path, returning a null
    * string if successful, otherwise an error message.  Note that this
    * method is intended to be run in a separate thread from the main
    * Qt event loop, and should thus never interact with any widgets.
    */
    QString writeToDisk(const QString &text,
        const QString &fileName) const;

    /*
    * Handles any errors or tidying up after an asynchronous save operation.
    */
    void onWriteCompleted();
};

AsyncTextWriter::AsyncTextWriter(const QString &fileName,
    QObject *parent) :
        QObject(parent),
        d_ptr(new AsyncTextWriterPrivate(this))
{
    Q_D(AsyncTextWriter);

    d->initialize(fileName);
}

AsyncTextWriter::~AsyncTextWriter()
{
    ;
}

QString AsyncTextWriter::fileName() const
{
    Q_D(const AsyncTextWriter);

    return d->fileName;
}

void AsyncTextWriter::setFileName(const QString &fileName)
{
    Q_D(AsyncTextWriter);

    d->fileName = QFileInfo(fileName).absoluteFilePath();
}

QTextCodec * AsyncTextWriter::codec() const
{
    Q_D(const AsyncTextWriter);

    return d->codec;
}

void AsyncTextWriter::setCodec(QTextCodec * codec)
{
    Q_D(AsyncTextWriter);

    d->codec = codec;
}

bool AsyncTextWriter::writeInProgress() const
{
    Q_D(const AsyncTextWriter);

    return d->writeInProgress;
}

void AsyncTextWriter::waitForFinished()
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

    if (d->writeFutureWatcher->isRunning()
            || d->writeFutureWatcher->isStarted()) {
        d->writeFutureWatcher->waitForFinished();
    }

    d->writeInProgress = true;

    QFuture<QString> future =
        QtConcurrent::run
        (
            d,
            &AsyncTextWriterPrivate::writeToDisk,
            text,
            d->fileName
        );

    d->writeFutureWatcher->setFuture(future);

    if (d->fileName.isNull() || d->fileName.isEmpty()) {
        return false;
    }

    return true;
}

void AsyncTextWriterPrivate::initialize(const QString &fileName,
    QTextCodec *codec)
{
    Q_Q(AsyncTextWriter);

    this->fileName = QFileInfo(fileName).absoluteFilePath();
    this->codec = codec;
    this->writeFutureWatcher = new QFutureWatcher<QString>(q);

    q->connect(this->writeFutureWatcher,
        &QFutureWatcher<QString>::finished,
        [this]() {
            this->onWriteCompleted();
        }
    );
}

QString AsyncTextWriterPrivate::writeToDisk(const QString &text,
    const QString &fileName) const
{
    QSaveFile file(fileName);
    file.setDirectWriteFallback(true);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return file.errorString();
    }

    // Write contents to disk.
    QTextStream stream(&file);

    if (nullptr != codec) {
        stream.setCodec(codec);
    }

    stream << text;

    if (QFile::NoError != file.error()) {
        file.cancelWriting();
        return file.errorString();
    }

    // Commit changes (and close the file).  All done!
    file.commit();
    return QString();
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

} //namespace ghostwriter