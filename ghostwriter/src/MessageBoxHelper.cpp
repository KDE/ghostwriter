/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include <QCoreApplication>

#include "MessageBoxHelper.h"


QMessageBox::StandardButton
MessageBoxHelper::critical
(
    QWidget* parent,
    const QString& text,
    const QString& informativeText,
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
    QWidget* parent,
    const QString& text,
    const QString& informativeText,
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
    QWidget* parent,
    const QString& text,
    const QString& informativeText,
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
    QWidget* parent,
    const QString& text,
    const QString& informativeText,
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
    QWidget* parent,
    QMessageBox::Icon icon,
    const QString& text,
    const QString& informativeText,
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
