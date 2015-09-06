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

#ifndef THEMEPREVIEWER_H
#define THEMEPREVIEWER_H

#include <QIcon>

#include "Theme.h"

/**
 * Renders a thumbnail preview of a theme.
 */
class ThemePreviewer
{
    public:
        /**
         * Constructor.  Takes as a parameter the theme for which to render
         * the thumbnail preview, as well as the desired width and height
         * (in pixels) of the preview.  Upon calling this constructor,
         * the preview will be rendered immediately, and thereafter accessible
         * in cached form with a call to getIcon().
         */
        ThemePreviewer(const Theme& theme, int width, int height);

        /**
         * Destructor.
         */
        ~ThemePreviewer();

        /**
         * Gets the rendered icon (cached) of the thumbnail preview.
         */
        QIcon getIcon();

        /**
         * Renders the thumbnail preview for the given theme to easily update
         * the icon for new theme settings.
         */
        void renderPreview(const Theme& newSettings);

    private:
        Theme theme;
        int width;
        int height;
        QIcon thumbnailPreviewIcon;

};

#endif // THEMEPREVIEWER_H
