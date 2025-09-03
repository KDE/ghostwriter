/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef WEBDAVWINIO_H
#define WEBDAVWINIO_H

#ifdef Q_OS_WIN

#include <QStorageInfo>

#include "textio.h"
#include <KResult>
#include <QString>
#include <QStringConverter>
#include <QUrl>

class WebDAVWinIO : public TextIO
{
public:
    explicit WebDAVWinIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8);

    WriteResult write(const QString &text) override;
    ReadResult read() override;

    static bool canHandle(const QUrl &fileUrl);

private:
    KResult<size_t, TextIOError> writeWithQFile(const QString &text);
};

#endif // Q_OS_WIN

#endif // WEBDAVWINIO_H
