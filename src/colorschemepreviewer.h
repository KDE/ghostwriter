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

#ifndef COLOR_SCHEME_PREVIEWER_H
#define COLOR_SCHEME_PREVIEWER_H

#include <QIcon>
#include <QScopedPointer>

#include "colorscheme.h"

namespace ghostwriter
{
/**
 * Renders a thumbnail preview of a theme.
 */
class ColorSchemePreviewerPrivate;
class ColorSchemePreviewer
{
    Q_DECLARE_PRIVATE(ColorSchemePreviewer)

public:
    /**
     * Constructor.  Takes as a parameter the theme for which to render
     * the thumbnail preview, as well as the desired width and height
     * (in pixels) of the preview.  Upon calling this constructor,
     * the preview will be rendered immediately, and thereafter accessible
     * in cached form with a call to icon().
     */
    ColorSchemePreviewer
    (
        const ColorScheme &colors,
        bool builtIn,
        bool valid,
        int width,
        int height,
        qreal dpr = 1.0
    );

    /**
     * Destructor.
     */
    ~ColorSchemePreviewer();

    /**
     * Gets the rendered icon (cached) of the thumbnail preview.
     */
    QIcon icon();

private:
    QScopedPointer<ColorSchemePreviewerPrivate> d_ptr;

};
} // namespace ghostwriter

#endif // COLOR_SCHEME_PREVIEWER_H
