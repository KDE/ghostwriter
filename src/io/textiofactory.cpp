/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "textiofactory.h"

using namespace ghostwriter;

std::unique_ptr<TextIO> TextIOFactory::createTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding)
{
    if (NetShareTextIO::canHandle(fileUrl)) {
        return std::make_unique<NetShareTextIO>(fileUrl, encoding);
    } else if (LocalTextIO::canHandle(fileUrl)) {
        return std::make_unique<LocalTextIO>(fileUrl, encoding);
    }
    // Add more handlers here as needed.
    return nullptr;
}