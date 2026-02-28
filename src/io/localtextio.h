/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef LOCALTEXTIO_H
#define LOCALTEXTIO_H

#include <QCoreApplication>
#include <QSaveFile>
#include <QString>
#include <QStringConverter>
#include <QTextStream>
#include <QUrl>

#include "io/textio.h"
#include "util/kresult.h"

namespace ghostwriter
{
class LocalTextIO : public TextIO
{
    Q_DECLARE_TR_FUNCTIONS(LocalTextIO)
public:
    static bool canHandle(const QUrl &fileUrl);

    explicit LocalTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8)
        : TextIO(fileUrl, encoding)
    {
    }

    ~LocalTextIO() = default;

    WriteResult write(const QString &text) override;
    ReadResult read() override;
};
} // namespace ghostwriter

#endif // LOCALTEXTIO_H
