/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef NETWORKTEXTIO_H
#define NETWORKTEXTIO_H

#include "textio.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

/**
 * @brief TextIO implementation for HTTP/HTTPS/WebDAV using QNetworkAccessManager.
 */
class NetworkTextIO : public TextIO
{
public:
    explicit NetworkTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding = QStringConverter::Utf8);

    WriteResult write(const QString &text) override;
    ReadResult read() override;

    /**
     * @brief Checks if the given URL can be handled by this implementation.
     */
    static bool canHandle(const QUrl &fileUrl);

private:
    TextIOError networkErrorToTextIOError(QNetworkReply::NetworkError error);
    std::unique_ptr<QNetworkAccessManager> m_manager;
};

#endif // NETWORKTEXTIO_H
