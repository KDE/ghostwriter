/***********************************************************************
 *
 * Copyright (C) 2014-2018 wereturtle
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
#include <QList>

#include "Theme.h"

class QSettings;

/**
 * Singleton class to fetch themes, either built-in or from the hard disk.
 */
class ThemeFactory
{
    public:
        /**
         * Gets the single instance of this class.
         */
        static ThemeFactory* getInstance();

        /**
         * Destructor.
         */
        ~ThemeFactory();

        /**
         * Gets the list of available theme names, including built-in themes
         * (listed at the front of the list).
         */
        QStringList getAvailableThemes() const;

        /**
         * Gets a theme that can be used with printing to paper.
         */
        Theme getPrinterFriendlyTheme() const;

        /**
         * Returns the theme with the given name.  If an error occurs, the
         * err string will be populated with an error message.  In this event,
         * the theme return will be one of the built-in themes.  If no error
         * occurs while loading the desired theme, err will be set to a null
         * QString.
         */
        Theme loadTheme(const QString& name, QString& err) const;

        /**
         * Deletes the theme with the given name from the hard disk.  Note
         * that this operation results in an error for built-in themes.  If an
         * error occurs while attempting to delete the theme, the err string
         * will be populated with an error message string.  Otherwise, err will
         * be set to a null QString.
         */
        void deleteTheme(const QString& name, QString& err);

        /**
         * Saves the theme with the given name to the hard disk.  Note
         * that this operation results in an error for built-in themes.  If an
         * error occurs while attempting to save the theme, the err string
         * will be populated with an error message string.  Otherwise, err will
         * be set to a null QString.
         */
        void saveTheme(const QString& name, Theme& theme, QString& err);

        /**
         * Gets the theme storage location directory.
         */
        QDir getThemeDirectory() const;

        /**
         * Gets the directory path for storing the theme with the given name.
         */
        QDir getDirectoryForTheme(const QString& name) const;

        /**
         * Returns an untitled theme name that is unique.
         */
        QString generateUntitledThemeName() const;

    private:
        static const QString CLASSIC_LIGHT_THEME_NAME;
        static const QString CLASSIC_DARK_THEME_NAME;
        static const QString PLAINSTRACTION_LIGHT_THEME_NAME;
        static const QString PLAINSTRACTION_DARK_THEME_NAME;
        static ThemeFactory* instance;

        QList<Theme> builtInThemes;
        QStringList customThemeNames;
        QString themeDirectoryPath;
        QDir themeDirectory;

        ThemeFactory();

        void loadClassicLightTheme();
        void loadClassicDarkTheme();
        void loadPlainstractionLightTheme();
        void loadPlainstractionDarkTheme();

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
