
/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <QFutureWatcher>
#include <QtConcurrent>

#include "io/textio.h"

using namespace ghostwriter;

QFuture<TextIO::WriteResult> TextIO::writeAsync(const QString &text)
{
    // Use QtConcurrent to run the write operation in a separate thread
    return QtConcurrent::run([this, text]() {
        return this->write(text);
    });
}

QFuture<TextIO::ReadResult> TextIO::readAsync()
{
    // Use QtConcurrent to run the read operation in a separate thread
    return QtConcurrent::run([this]() {
        return this->read();
    });
}

TextIOError TextIO::mapToTextIOError(QFileDevice::FileError fileError)
{
    switch (fileError) {
    case QFileDevice::NoError:
        return TextIOError::NoError;
    case QFileDevice::ReadError:
        return TextIOError::ReadError;
    case QFileDevice::WriteError:
        return TextIOError::WriteError;
    case QFileDevice::FatalError:
        return TextIOError::FatalError;
    case QFileDevice::ResourceError:
        return TextIOError::ResourceError;
    case QFileDevice::OpenError:
        return TextIOError::OpenError;
    case QFileDevice::AbortError:
        return TextIOError::AbortError;
    case QFileDevice::TimeOutError:
        return TextIOError::TimeoutError;
    case QFileDevice::UnspecifiedError:
        return TextIOError::UnknownError;
    case QFileDevice::PermissionsError:
        return TextIOError::PermissionDenied;
    default:
        return TextIOError::UnknownError;
    }
}