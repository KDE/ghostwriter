/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MESSAGEBOXHELPER_H
#define MESSAGEBOXHELPER_H

#include <QMessageBox>

namespace ghostwriter
{
/**
 * This class provides static convenience methods for displaying a QMessageBox
 * using text and informative text rather than title and text as the original
 * QMessageBox static methods do.  The window title in all cases is set
 * to the application name.
 */
class MessageBoxHelper
{
public:
    static QMessageBox::StandardButton
    critical
    (
        QWidget *parent,
        const QString &text,
        const QString &informativeText,
        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
        QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
    );

    static QMessageBox::StandardButton
    information
    (
        QWidget *parent,
        const QString &text,
        const QString &informativeText,
        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
        QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
    );

    static QMessageBox::StandardButton
    question
    (
        QWidget *parent,
        const QString &text,
        const QString &informativeText,
        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
        QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
    );

    static QMessageBox::StandardButton
    warning
    (
        QWidget *parent,
        const QString &text,
        const QString &informativeText,
        QMessageBox::StandardButtons buttons = QMessageBox::Ok,
        QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
    );

private:
    static QMessageBox::StandardButton
    showMessageBox
    (
        QWidget *parent,
        QMessageBox::Icon icon,
        const QString &text,
        const QString &informativeText,
        QMessageBox::StandardButtons buttons,
        QMessageBox::StandardButton defaultButton
    );
};
} // namespace ghostwriter

#endif // MESSAGEBOXHELPER_H
