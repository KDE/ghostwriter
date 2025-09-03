/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifdef Q_OS_WIN

#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStorageInfo>
#include <QTextStream>

#include "webdavwinio.h"

bool WebDAVWinIO::canHandle(const QUrl &fileUrl)
{
    if (!fileUrl.isLocalFile()) {
        return false;
    }

    QString localPath = fileUrl.toLocalFile();

    // Check if this is a UNC path (\\server\share)
    if (localPath.startsWith("\\\\")) {
        return true;
    }

    // Check if this is a mapped network drive
    QStorageInfo storage(localPath);

    // WebDAV drives typically show up as "WebClient" file system type
    if (storage.fileSystemType() == "WebClient") {
        return true;
    }

    // Additional check: see if it's a network drive
    if (QDir(localPath).exists()) {
        // We could use Windows API here to detect network drives
        // For now, we'll use a heuristic.
        QString driveLetter = localPath.left(2); // e.g., "Z:"

        if ((2 == driveLetter.length()) && (driveLetter.at(1) == ':')) {
            // Check if the drive is a network drive
            // For example, drives above C: are often network drives
            QChar drive = driveLetter.at(0).toUpper();

            // TODO: This is a rough heuristic. Add more more sophisticated detection later.
            if (drive > 'C') {
                return true;
            }
        }
    }

    return false;
}

TextIO::WriteResult WebDAVWinIO::write(const QString &text)
{
    // For mapped drives and UNC paths, use direct QFile with special handling
    QString localPath = m_fileUrl.toLocalFile();

    // Use QFile for network drives. QSaveFile often fails on WebDAV.
    QFile file(localPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return {TextIOError::PermissionDenied, "Failed to open the file for writing."};
    }

    QTextStream stream(&file);
    stream.setEncoding(m_encoding);
    stream << text;

    if (file.error() != QFile::NoError) {
        return {TextIOError::NetworkError, "A network error occurred while writing to the file."};
    }

    return {};
}

TextIO::ReadResult WebDAVWinIO::read()
{
    QString localPath = m_fileUrl.toLocalFile();
    QFile file(localPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {TextIOError::PermissionDenied, "Failed to open the file for reading."};
    }

    QTextStream stream(&file);
    stream.setEncoding(m_encoding);

    QString content = stream.readAll();

    if (file.error() != QFile::NoError) {
        return {TextIOError::NetworkError, "A network error occurred while reading the file."};
    }

    return content;
}

#endif // Q_OS_WIN
