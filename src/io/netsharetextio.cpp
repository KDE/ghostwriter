/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStorageInfo>
#include <QTextStream>

#include "netsharetextio.h"

using namespace ghostwriter;

bool NetShareTextIO::canHandle(const QUrl &fileUrl)
{
    static const QList<QByteArray> networkFsTypes = {"webclient", "cifs", "smb", "nfs", "davfs", "fuse.sshfs"};

    if (!fileUrl.isLocalFile()) {
        return false;
    }

    QString localPath = fileUrl.toLocalFile();

    qInfo() << "Checking if path is network share:" << fileUrl << "->" << localPath;

    // Direct UNC path check
    if (localPath.startsWith("\\\\")) {
        return true;
    }

    QStorageInfo storage(localPath);

    // Check file system type for known network types
    QByteArray fsType = storage.fileSystemType().toLower();

    if (networkFsTypes.contains(fsType)) {
        return true;
    }

    // Check device path
    QString device = QString::fromLocal8Bit(storage.device());

    if (device.startsWith("\\\\") || device.contains("//")) {
        return true;
    }

    // Check root path resolution for mapped drives
    if ((localPath.length() >= 2) && (localPath.at(1) == ':')) {
        QFileInfo rootInfo(localPath.left(2) + "/");
        QString canonicalPath = rootInfo.canonicalFilePath();

        if (canonicalPath.startsWith("\\\\")) {
            return true;
        }
    }

    // Final check using canonical path, in case of relative path.
    QFileInfo fileInfo(localPath);
    QString canonicalPath = fileInfo.canonicalFilePath();

    if (canonicalPath.startsWith("\\\\")) {
        return true;
    }

    return false;
}

TextIO::WriteResult NetShareTextIO::write(const QString &text)
{
    // For mapped drives and UNC paths, use direct QFile with special handling.
    // QSaveFile often fails on WebDAV and other mapped drives.
    QString localPath = m_fileUrl.toLocalFile();
    QFile file(localPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return {mapToTextIOError(file.error()), tr("Failed to open the file for writing: %1").arg(file.errorString())};
    }

    QTextStream stream(&file);
    stream.setEncoding(m_encoding);
    stream << text;

    auto errorCode = mapToTextIOError(file.error());

    if (TextIOError::UnknownError == errorCode) {
        return {TextIOError::NetworkError, tr("A network write error occurred")};
    }

    if (TextIOError::NoError != errorCode) {
        return {errorCode, tr("A network write error occurred: %1").arg(file.errorString())};
    }

    return {};
}

TextIO::ReadResult NetShareTextIO::read()
{
    QString localPath = m_fileUrl.toLocalFile();
    QFile file(localPath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return {mapToTextIOError(file.error()), tr("Failed to open the file for reading: %1").arg(file.errorString())};
    }

    QTextStream stream(&file);
    stream.setEncoding(m_encoding);

    QString content = stream.readAll();

    auto errorCode = mapToTextIOError(file.error());

    if (TextIOError::UnknownError == errorCode) {
        return {TextIOError::NetworkError, tr("A network read error occurred")};
    }

    if (TextIOError::NoError != errorCode) {
        return {errorCode, tr("An error occurred while reading the file: %1").arg(file.errorString())};
    }

    return content;
}
