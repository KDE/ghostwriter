/*
 * SPDX-FileCopyrightText: 2015-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "theme.h"

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
    : d(new ThemePrivate())
{
    d->readOnly = false;
    d->darkColorSchemeAvailable = false;
}

Theme::Theme(const QString &name, const ColorScheme &colors, const bool builtIn)
    : d(new ThemePrivate())
{
    d->name = name;
    d->readOnly = builtIn;
    d->lightColors = colors;
    d->darkColors = colors;
    d->darkColorSchemeAvailable = false;
}

Theme::Theme(const QString &name, const ColorScheme &lightColors, const ColorScheme &darkColors, const bool builtIn)
    : d(new ThemePrivate())
{
    d->name = name;
    d->readOnly = builtIn;
    d->lightColors = lightColors;
    d->darkColors = darkColors;
    d->darkColorSchemeAvailable = true;
}

Theme::Theme(const Theme &other)
    : d(new ThemePrivate())
{
    d->name = other.d->name;
    d->readOnly = other.d->readOnly;
    d->darkColors = other.d->darkColors;
    d->lightColors = other.d->lightColors;
    d->darkColorSchemeAvailable = other.d->darkColorSchemeAvailable;
}

Theme::~Theme()
{
    ;
}

Theme &Theme::operator=(const Theme &other)
{
    if (this != &other) {
        d->name = other.d->name;
        d->lightColors = other.d->lightColors;
        d->darkColors = other.d->darkColors;
        d->readOnly = other.d->readOnly;
        d->darkColorSchemeAvailable = other.d->darkColorSchemeAvailable;
    }

    return *this;
}

QString Theme::name() const
{
    return d->name;
}

void Theme::Theme::setName(const QString &value)
{
    d->name = value;
}

bool Theme::isReadOnly() const
{
    return d->readOnly;
}

void Theme::setReadOnly(const bool readOnly)
{
    d->readOnly = readOnly;
}

bool Theme::hasDarkColorScheme() const
{
    return d->darkColorSchemeAvailable;
}

const ColorScheme &Theme::darkColorScheme() const
{
    return d->darkColors;
}

void Theme::setDarkColorScheme(const ColorScheme &colors)
{
    d->darkColors = colors;
    d->darkColorSchemeAvailable = true;
}

const ColorScheme &Theme::lightColorScheme() const
{
    return d->lightColors;
}

void Theme::setLightColorScheme(const ColorScheme &colors)
{
    d->lightColors = colors;
}

const ChromeColors Theme::lightChromeColors() const
{
    return ChromeColors(d->lightColors);
}

const ChromeColors Theme::darkChromeColors() const
{
    return ChromeColors(d->darkColors);
}

} // namespace ghostwriter
