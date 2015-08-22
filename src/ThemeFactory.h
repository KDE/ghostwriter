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

#ifndef THEME_FACTORY_H
#define THEME_FACTORY_H

#include <QString>
#include <QStringList>
#include <QDir>

#include "Theme.h"

class QSettings;

class ThemeFactory
{
    public:
        static ThemeFactory* getInstance();
        ~ThemeFactory();

        void loadLightTheme(Theme& theme) const;
        void loadDarkTheme(Theme& theme) const;
        QStringList getAvailableThemes();
        void loadTheme(const QString& name, Theme& theme, QString& err) const;
        void deleteTheme(const QString& name, QString& err);
        void saveTheme(const QString& name, Theme& theme, QString& err);
        QDir getThemeDirectory() const;
        QDir getDirectoryForTheme(const QString& name) const;
        QString generateUntitledThemeName() const;

        static const QString LIGHT_THEME_NAME;
        static const QString DARK_THEME_NAME;


    private:
        static ThemeFactory* instance;
        QStringList availableThemes;
        QString themeDirectoryPath;
        QDir themeDirectory;

        ThemeFactory();

        bool extractColorSetting
        (
            const QSettings& settings,
            const QString& key,
            QColor& value,
            QString& err
        ) const;

        bool extractIntSetting
        (
            const QSettings& settings,
            const QString& key,
            int& value,
            int min,
            int max,
            QString& err
        ) const;

        bool extractStringSetting
        (
            const QSettings& settings,
            const QString& key,
            QString& value,
            QString& err
        ) const;
};

#endif
