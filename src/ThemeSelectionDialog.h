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

#ifndef THEMESELECTIONDIALOG_H
#define THEMESELECTIONDIALOG_H

#include <QDialog>
#include <QWidget>

#include "Theme.h"

class QListWidget;

/**
 * Dialog that allows the user to select a different theme, add a new theme,
 * delete a theme, or edit a theme.
 */
class ThemeSelectionDialog : public QDialog
{
    Q_OBJECT

    public:
        /**
         * Constructor.  Takes as a parameter the name of the theme that is
         * currently styling the applicaton.
         */
        ThemeSelectionDialog
        (
            const QString& currentThemeName,
            QWidget* parent = 0
        );

        /**
         * Destructor.
         */
        ~ThemeSelectionDialog();

    signals:
        /**
         * Emitted when the user has selected a theme so that it can be applied
         * (previewed) to the application look and feel.
         */
        void applyTheme(const Theme& theme);

    private slots:
        void onThemeSelected();
        void onNewTheme();
        void onDeleteTheme();
        void onEditTheme();
        void onThemeUpdated(const Theme& theme);

    private:
        QListWidget* themeListWidget;
        Theme currentTheme;
        bool currentThemeIsValid;
        bool currentThemeIsNew;
        int iconWidth;
        int iconHeight;

        // Use this list to count where the custom
        // themes start in the theme selection list.
        //
        QStringList builtInThemes;
};

#endif // THEMESELECTIONDIALOG_H
