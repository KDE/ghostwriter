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

#ifndef MESSAGEBOXHELPER_H
#define MESSAGEBOXHELPER_H

#include <QMessageBox>

/**
 * This class provides static convenience methods for dipslaying a QMessageBox
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
            QWidget* parent,
            const QString& text,
            const QString& informativeText,
            QMessageBox::StandardButtons buttons = QMessageBox::Ok,
            QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
        );

        static QMessageBox::StandardButton
        information
        (
            QWidget* parent,
            const QString& text,
            const QString& informativeText,
            QMessageBox::StandardButtons buttons = QMessageBox::Ok,
            QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
        );

        static QMessageBox::StandardButton
        question
        (
            QWidget* parent,
            const QString& text,
            const QString& informativeText,
            QMessageBox::StandardButtons buttons = QMessageBox::Ok,
            QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
        );

        static QMessageBox::StandardButton
        warning
        (
            QWidget* parent,
            const QString& text,
            const QString& informativeText,
            QMessageBox::StandardButtons buttons = QMessageBox::Ok,
            QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
        );

    private:
        static QMessageBox::StandardButton
        showMessageBox
        (
            QWidget* parent,
            QMessageBox::Icon icon,
            const QString& text,
            const QString& informativeText,
            QMessageBox::StandardButtons buttons,
            QMessageBox::StandardButton defaultButton
        );

};

#endif // MESSAGEBOXHELPER_H
