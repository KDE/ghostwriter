/*
 * SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "networktextio.h"
#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

NetworkTextIO::NetworkTextIO(const QUrl &fileUrl, QStringConverter::Encoding encoding)
    : TextIO(fileUrl, encoding)
    , m_manager(std::make_unique<QNetworkAccessManager>())
{
}

bool NetworkTextIO::canHandle(const QUrl &fileUrl)
{
    const QString scheme = fileUrl.scheme().toLower();
    return scheme == "http" || scheme == "https" || scheme == "webdav" || scheme == "davs";
}

TextIO::WriteResult NetworkTextIO::write(const QString &text)
{
    QNetworkRequest request(m_fileUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain; charset=utf-8");

    // For WebDAV, we might need additional headers.
    if (m_fileUrl.scheme().toLower().contains("webdav") || m_fileUrl.scheme().toLower().contains("dav")) {
        request.setRawHeader("Depth", "0");
    }

    QByteArray data;
    QTextStream stream(&data);
    stream.setEncoding(m_encoding);
    stream << text;

    QNetworkReply *reply = m_manager->put(request, data);

    // Wait for completion with timeout
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(30000); // 30 second timeout

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    if (!timer.isActive()) {
        reply->deleteLater();
        return {TextIOError::NetworkError, "Network operation timed out"};
    }

    timer.stop();

    if (reply->error() != QNetworkReply::NoError) {
        TextIOError error = networkErrorToTextIOError(reply->error());
        static QString errmsg = reply->errorString();
        reply->deleteLater();
        return {error, errmsg.toUtf8().constData()};
    }

    size_t bytesWritten = data.size();
    reply->deleteLater();
    return {};
}

KResult<QString, TextIOError> NetworkTextIO::read()
{
    QNetworkRequest request(m_fileUrl);
    QNetworkReply *reply = m_manager->get(request);

    // Similar event loop pattern as write()...
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(30000);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    if (!timer.isActive()) {
        reply->deleteLater();
        return KResult<QString, TextIOError>::error(TextIOError::NetworkError);
    }
    timer.stop();

    if (reply->error() != QNetworkReply::NoError) {
        TextIOError error = networkErrorToTextIOError(reply->error());
        reply->deleteLater();
        return KResult<QString, TextIOError>::error(error);
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    QTextStream stream(data);
    stream.setEncoding(m_encoding);
    QString text = stream.readAll();

    return KResult<QString, TextIOError>::success(text);
}

TextIOError NetworkTextIO::networkErrorToTextIOError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::AuthenticationRequiredError:
        return TextIOError::AuthenticationError;
    case QNetworkReply::ContentAccessDenied:
    case QNetworkReply::ContentOperationNotPermittedError:
        return TextIOError::PermissionDenied;
    case QNetworkReply::ContentNotFoundError:
        return TextIOError::FileNotFound;
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::TimeoutError:
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::NetworkSessionFailedError:
        return TextIOError::NetworkError;
    default:
        return TextIOError::UnknownError;
    }
}
