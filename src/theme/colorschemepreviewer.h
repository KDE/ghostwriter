/*
 * SPDX-FileCopyrightText: 2014-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef COLOR_SCHEME_PREVIEWER_H
#define COLOR_SCHEME_PREVIEWER_H

#include <QIcon>
#include <QScopedPointer>

#include <editor/colorscheme.h>

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
