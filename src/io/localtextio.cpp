/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "localtextio.h"

#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QTextStream>

using namespace ghostwriter;

bool LocalTextIO::canHandle(const QUrl &fileUrl)
{
    return fileUrl.isLocalFile() && !QFileInfo(fileUrl.toLocalFile()).isDir();
}

TextIO::WriteResult LocalTextIO::write(const QString &text)
{
    QString localPath = m_fileUrl.toLocalFile();

    // Try QSaveFile first for atomic writes
    QSaveFile saveFile(localPath);

    if (saveFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream stream(&saveFile);
        stream.setEncoding(m_encoding);
        stream << text;

        if ((saveFile.error() == QFileDevice::NoError) && saveFile.commit()) {
            return {};
        }
    }

    // Fallback to QFile if QSaveFile fails
    QFile file(localPath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return {mapToTextIOError(file.error()), tr("Failed to open the file for writing: %1").arg(file.errorString())};
    }

    QTextStream stream(&file);
    stream.setEncoding(m_encoding);
    stream << text;

    auto errorCode = mapToTextIOError(file.error());

    if (TextIOError::NoError != errorCode) {
        return {errorCode, tr("An error occurred while writing to the file: %1").arg(file.errorString())};
    }

    return {};
}

TextIO::ReadResult LocalTextIO::read()
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

    if (TextIOError::NoError != errorCode) {
        return {errorCode, tr("An error occurred while reading the file: %1").arg(file.errorString())};
    }

    return content;
}
