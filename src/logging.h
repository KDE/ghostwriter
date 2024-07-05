/*
 * SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef LOGGING_H
#define LOGGING_H

#include <QLoggingCategory>
#include <QMessageLogContext>
#include <QString>
#include <QtDebug>

#include <iostream>

namespace ghostwriter
{
/**
 * Message handler function for directing Qt qDebug(), qWarning(), qError(),
 * etc., messages to stdout/stderr.  Install this function by passing it to
 * qInstallMessageHandler().
 */
void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &message);
} // namespace ghostwriter

#endif // LOGGING_H