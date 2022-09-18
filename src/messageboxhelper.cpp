/*
 * SPDX-FileCopyrightText: 2014-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCoreApplication>

#include "messageboxhelper.h"

namespace ghostwriter
{
QMessageBox::StandardButton
MessageBoxHelper::critical
(
    QWidget *parent,
    const QString &text,
    const QString &informativeText,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton
)
{
    return
        showMessageBox
        (
            parent,
            QMessageBox::Critical,
            text, informativeText,
            buttons,
            defaultButton
        );
}

QMessageBox::StandardButton
MessageBoxHelper::information
(
    QWidget *parent,
    const QString &text,
    const QString &informativeText,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton
)
{
    return
        showMessageBox
        (
            parent,
            QMessageBox::Information,
            text, informativeText,
            buttons,
            defaultButton
        );
}

QMessageBox::StandardButton
MessageBoxHelper::question
(
    QWidget *parent,
    const QString &text,
    const QString &informativeText,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton
)
{
    return
        showMessageBox
        (
            parent,
            QMessageBox::Question,
            text, informativeText,
            buttons,
            defaultButton
        );
}

QMessageBox::StandardButton
MessageBoxHelper::warning
(
    QWidget *parent,
    const QString &text,
    const QString &informativeText,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton
)
{
    return
        showMessageBox
        (
            parent,
            QMessageBox::Warning,
            text, informativeText,
            buttons,
            defaultButton
        );
}

QMessageBox::StandardButton
MessageBoxHelper::showMessageBox
(
    QWidget *parent,
    QMessageBox::Icon icon,
    const QString &text,
    const QString &informativeText,
    QMessageBox::StandardButtons buttons,
    QMessageBox::StandardButton defaultButton
)
{
    QMessageBox messageBox(parent);
    messageBox.setWindowTitle(QCoreApplication::applicationName());
    messageBox.setText(text);
    messageBox.setInformativeText(informativeText);
    messageBox.setStandardButtons(buttons);
    messageBox.setDefaultButton(defaultButton);
    messageBox.setIcon(icon);

    return (QMessageBox::StandardButton) messageBox.exec();
}
} // namespace ghostwriter
