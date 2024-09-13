/*
 * SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "logging.h"

#include <QDir>
#include <QtLogging>

namespace ghostwriter
{
void logMessage(QtMsgType type, const QMessageLogContext &context, const QString &message)

{
    FILE *dest = stdout;
    QMessageLogContext shortContext(context.file, context.line, context.function, context.category);

    if ((QtFatalMsg == type) || (QtCriticalMsg == type) || (QtWarningMsg == type)) {
        dest = stderr;
    }

    if (context.file != nullptr) {
        auto filePath = QDir::fromNativeSeparators(context.file);
        auto srcDirIndex = filePath.lastIndexOf("/src/");
        auto offset = 5;

        if (srcDirIndex < 0) {
            srcDirIndex = 0;

            if (filePath.startsWith("src/")) {
                offset = 4;
            } else {
                offset = 0;
            }
        }

        if (offset > 0) {
            filePath = filePath.right(filePath.length() - (srcDirIndex + offset));
        }

        shortContext.file = filePath.toUtf8().data();
    }

    QTextStream stream(dest);

    auto text = qFormatLogMessage(type, shortContext, message);
    stream << text << Qt::endl;
    stream.flush();

    if (QtFatalMsg == type) {
        stream << Qt::endl << "    GAME OVER" << Qt::endl;
        stream << "   Insert Coin" << Qt::endl << Qt::endl;
        stream.flush();
        abort();
    }
}
} // namespace ghostwriter