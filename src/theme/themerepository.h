/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef THEME_FACTORY_H
#define THEME_FACTORY_H

#include <QDir>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

#include "theme.h"

namespace ghostwriter
{
/**
 * Singleton class to fetch themes, either built-in or from the hard disk.
 */
class ThemeRepositoryPrivate;
class ThemeRepository : QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(ThemeRepository)

public:
    /**
     * Constructor.  Takes the path to the theme directory.
     */
    ThemeRepository(const QString &path);
    
    /**
     * Destructor.
     */
    ~ThemeRepository();

    /**
     * Gets the list of available theme names, including built-in themes
     * (listed at the front of the list).
     */
    QStringList availableThemes() const;

    /**
     * Gets the default theme.
     */
    Theme defaultTheme() const;

    /**
     * Returns the theme with the given name.  If an error occurs, the
     * err string will be populated with an error message.  In this event,
     * the theme return will be one of the built-in themes.  If no error
     * occurs while loading the desired theme, err will be set to a null
     * QString.
     */
    Theme loadTheme(const QString &name, QString &err) const;

    /**
     * Deletes the theme with the given name from the hard disk.  Note
     * that this operation results in an error for built-in themes.  If an
     * error occurs while attempting to delete the theme, the err string
     * will be populated with an error message string.  Otherwise, err will
     * be set to a null QString.
     */
    void deleteTheme(const QString &name, QString &err);

    /**
     * Saves the theme with the given name to the hard disk.  Note
     * that this operation results in an error for built-in themes.  If an
     * error occurs while attempting to save the theme, the err string
     * will be populated with an error message string.  Otherwise, err will
     * be set to a null QString.
     */
    void saveTheme(const QString &name, Theme &theme, QString &err);

    /**
     * Gets the theme storage location directory.
     */
    QDir themeDirectory() const;

    /**
     * Gets the file path for storing/deleting the theme with the given name.
     */
    QString themeFilePath(const QString &themeName) const;

    /**
     * Returns an untitled theme name that is unique.
     */
    QString generateUntitledThemeName() const;

private:
    QScopedPointer<ThemeRepositoryPrivate> d_ptr;
};
} // namespace ghostwriter

#endif
