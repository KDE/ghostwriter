/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef TEXTIOFACTORY_H
#define TEXTIOFACTORY_H

#include "io/localtextio.h"
#include "io/netsharetextio.h"
#include "io/textio.h"
#include <memory>

#include "util/kresult.h"

namespace ghostwriter
{

class TextIOFactory
{
public:
    /**
     * @brief Construct a new Text IOFactory object.
     */
    TextIOFactory() = default;

    /**
     * @brief Destroy the TextIOFactory object.
     */
    ~TextIOFactory() = default;

    /**
     * @brief Creates an appropriate TextIO subclass instance based on the given file URL.
     * @param fileUrl The URL of the file to be handled.
     * @param encoding The text encoding to be used. Defaults to UTF-8.
     * @return A pointer to a TextIO instance, or else nullptr if the file URL is unsupported.
     */
    std::unique_ptr<TextIO> createTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8);
};
} // namespace ghostwriter

#endif // TEXTIOFACTORY_H
