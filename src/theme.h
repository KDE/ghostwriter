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

#ifndef THEME_H
#define THEME_H

#include <QColor>
#include <QObject>
#include <QScopedPointer>
#include <QString>

#include "colorscheme.h"

namespace ghostwriter
{
/**
 * Encapsulates a theme for customizing the look and feel of the application.
 */
class ThemePrivate;
class Theme
{
    Q_DECLARE_PRIVATE(Theme)

public:
    /**
     * Constructor.
     */
    Theme();

    /**
     * Constructor that takes theme name, color scheme, and
     * whether the theme is built-in or not as parameters.
     */
    Theme
    (
        const QString &name,
        const ColorScheme &colors,
        const bool builtIn = false
    );

    /**
     * Constructor that takes theme name, a light mode color scheme,
     * a dark mode color scheme, and whether the theme is built-in or not
     * as parameters.
     */
    Theme
    (
        const QString &name,
        const ColorScheme &lightColors,
        const ColorScheme &darkColors,
        const bool builtIn = false
    );

    /**
     * Copy constructor.
     */
    Theme(const Theme &other);

    /**
     * Destructor.
     */
    ~Theme();

    /**
     * Assignment overload.
     */
    Theme &operator=(const Theme &other);

    /**
     * Returns the name of the theme.
     */
    QString name() const;

    /**
     * Sets the name of the theme.
     */
    void setName(const QString &value);

    /**
     * Returns true if the theme is read-only (built-in), false otherwise.
     */
    bool isReadOnly() const;

    /**
     * Sets whether the theme is read-only (built-in).
     */
    void setReadOnly(const bool readOnly);

    /**
     * Returns true if theme supports a dark mode color scheme, false
     * otherwise.
     */
    bool hasDarkColorScheme() const;

    /**
     * Returns the dark mode color scheme.  If dark mode is not supported,
     * this method will return the light mode color scheme.
     */
    const ColorScheme &darkColorScheme() const;

    /**
     * Sets the dark mode color scheme.
     */
    void setDarkColorScheme(const ColorScheme &colors);

    /**
     * Returns the light mode color scheme.
     */
    const ColorScheme &lightColorScheme() const;

    /**
     * Sets the light mode color scheme.
     */
    void setLightColorScheme(const ColorScheme &colors);

private:
    QScopedPointer<ThemePrivate> d_ptr;

};
} // namespace ghostwriter

#endif
