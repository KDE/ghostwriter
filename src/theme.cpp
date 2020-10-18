/***********************************************************************
 *
 * Copyright (C) 2015-2020 wereturtle
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

#include "theme.h"

#include <QDebug>

namespace ghostwriter
{
class ThemePrivate
{
public:
    ThemePrivate()
    {
        ;
    }

    ~ThemePrivate()
    {
        ;
    }

    QString name;
    bool readOnly;
    ColorScheme darkColors;
    ColorScheme lightColors;
    bool darkColorSchemeAvailable;
};

Theme::Theme()
    : d_ptr(new ThemePrivate())
{
    d_func()->readOnly = false;
    d_func()->darkColorSchemeAvailable = false;
}

Theme::Theme
(
    const QString &name,
    const ColorScheme &colors,
    const bool builtIn
)
    : d_ptr(new ThemePrivate())
{
    d_func()->name = name;
    d_func()->readOnly = builtIn;
    d_func()->lightColors = colors;
    d_func()->darkColors = colors;
    d_func()->darkColorSchemeAvailable = false;
}

Theme::Theme
(
    const QString &name,
    const ColorScheme &lightColors,
    const ColorScheme &darkColors,
    const bool builtIn
)
    : d_ptr(new ThemePrivate())
{
    d_func()->name = name;
    d_func()->readOnly = builtIn;
    d_func()->lightColors = lightColors;
    d_func()->darkColors = darkColors;
    d_func()->darkColorSchemeAvailable = true;
}

Theme::Theme(const Theme &other)
    : d_ptr(new ThemePrivate())
{
    d_func()->name = other.d_func()->name;
    d_func()->readOnly = other.d_func()->readOnly;
    d_func()->darkColors = other.d_func()->darkColors;
    d_func()->lightColors = other.d_func()->lightColors;
    d_func()->darkColorSchemeAvailable = other.d_func()->darkColorSchemeAvailable;
}

Theme::~Theme()
{
    ;
}

Theme &Theme::operator=(const Theme &other)
{
    if (this != &other) {
        d_func()->name = other.d_func()->name;
        d_func()->lightColors = other.d_func()->lightColors;
        d_func()->darkColors = other.d_func()->darkColors;
        d_func()->readOnly = other.d_func()->readOnly;
        d_func()->darkColorSchemeAvailable = other.d_func()->darkColorSchemeAvailable;
    }

    return *this;
}

QString Theme::name() const
{
    return d_func()->name;
}

void Theme::Theme::setName(const QString &value)
{
    d_func()->name = value;
}

bool Theme::isReadOnly() const
{
    return d_func()->readOnly;
}

void Theme::setReadOnly(const bool readOnly)
{
    d_func()->readOnly = readOnly;
}

bool Theme::hasDarkColorScheme() const
{
    return d_func()->darkColorSchemeAvailable;
}

const ColorScheme &Theme::darkColorScheme() const
{
    return d_func()->darkColors;
}

void Theme::setDarkColorScheme(const ColorScheme &colors)
{
    d_func()->darkColors = colors;
    d_func()->darkColorSchemeAvailable = true;
}

const ColorScheme &Theme::lightColorScheme() const
{
    return d_func()->lightColors;
}

void Theme::setLightColorScheme(const ColorScheme &colors)
{
    d_func()->lightColors = colors;
}
} // namespace ghostwriter
