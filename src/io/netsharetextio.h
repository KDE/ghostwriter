/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef WINNETSHAREIO_H
#define WINNETSHAREIO_H

#include <QCoreApplication>
#include <QStorageInfo>

#include <QString>
#include <QStringConverter>
#include <QUrl>

#include "io/textio.h"
#include "util/kresult.h"

namespace ghostwriter
{
class NetShareTextIO : public TextIO
{
    Q_DECLARE_TR_FUNCTIONS(NetShareTextIO)

public:
    static bool canHandle(const QUrl &fileUrl);

    explicit NetShareTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8)
        : TextIO(fileUrl, encoding)
    {
    }

    ~NetShareTextIO() = default;

    WriteResult write(const QString &text) override;
    ReadResult read() override;

private:
    KResult<size_t, TextIOError> writeWithQFile(const QString &text);
};
} // namespace ghostwriter

#endif // WINNETSHAREIO_H
