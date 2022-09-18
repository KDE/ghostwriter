/*
 * SPDX-FileCopyrightText: 2015-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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
    Q_D(Theme);

    d->readOnly = false;
    d->darkColorSchemeAvailable = false;
}

Theme::Theme
(
    const QString &name,
    const ColorScheme &colors,
    const bool builtIn
)
    : d_ptr(new ThemePrivate())
{
    Q_D(Theme);

    d->name = name;
    d->readOnly = builtIn;
    d->lightColors = colors;
    d->darkColors = colors;
    d->darkColorSchemeAvailable = false;
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
    Q_D(Theme);

    d->name = name;
    d->readOnly = builtIn;
    d->lightColors = lightColors;
    d->darkColors = darkColors;
    d->darkColorSchemeAvailable = true;
}

Theme::Theme(const Theme &other)
    : d_ptr(new ThemePrivate())
{
    Q_D(Theme);

    d->name = other.d_func()->name;
    d->readOnly = other.d_func()->readOnly;
    d->darkColors = other.d_func()->darkColors;
    d->lightColors = other.d_func()->lightColors;
    d->darkColorSchemeAvailable = other.d_func()->darkColorSchemeAvailable;
}

Theme::~Theme()
{
    ;
}

Theme &Theme::operator=(const Theme &other)
{
    Q_D(Theme);

    if (this != &other) {
        d->name = other.d_func()->name;
        d->lightColors = other.d_func()->lightColors;
        d->darkColors = other.d_func()->darkColors;
        d->readOnly = other.d_func()->readOnly;
        d->darkColorSchemeAvailable = other.d_func()->darkColorSchemeAvailable;
    }

    return *this;
}

QString Theme::name() const
{
    Q_D(const Theme);

    return d->name;
}

void Theme::Theme::setName(const QString &value)
{
    Q_D(Theme);

    d->name = value;
}

bool Theme::isReadOnly() const
{
    Q_D(const Theme);

    return d->readOnly;
}

void Theme::setReadOnly(const bool readOnly)
{
    Q_D(Theme);

    d->readOnly = readOnly;
}

bool Theme::hasDarkColorScheme() const
{
    Q_D(const Theme);

    return d->darkColorSchemeAvailable;
}

const ColorScheme &Theme::darkColorScheme() const
{
    Q_D(const Theme);

    return d->darkColors;
}

void Theme::setDarkColorScheme(const ColorScheme &colors)
{
    Q_D(Theme);

    d->darkColors = colors;
    d->darkColorSchemeAvailable = true;
}

const ColorScheme &Theme::lightColorScheme() const
{
    Q_D(const Theme);

    return d->lightColors;
}

void Theme::setLightColorScheme(const ColorScheme &colors)
{
    Q_D(Theme);

    d->lightColors = colors;
}
} // namespace ghostwriter
