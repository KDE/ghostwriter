/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include <QDialog>
#include <QScopedPointer>

#include "theme.h"

namespace ghostwriter
{

/**
 * Dialog that allows the user to select a different theme, add a new theme,
 * delete a theme, or edit a theme.
 */
class ThemeSelectionDialogPrivate;
class ThemeSelectionDialog : public QDialog
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ThemeSelectionDialog)

public:
    /**
     * Constructor.  Takes as a parameter the name of the theme that is
     * currently styling the applicaton.
     */
    ThemeSelectionDialog
    (
        const QString &currentThemeName,
        const bool darkModeEnabled,
        QWidget *parent = 0
    );

    /**
     * Destructor.
     */
    ~ThemeSelectionDialog();

    /**
     * Gets the currently selected theme.
     */
    const Theme &theme() const;

private:
    QScopedPointer<ThemeSelectionDialogPrivate> d_ptr;
};
} // namespace ghostwriter

#endif // THEMESELECTIONDIALOG_H
