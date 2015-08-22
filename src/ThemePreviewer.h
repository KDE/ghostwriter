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

class ThemePreviewer
{
    public:
        ThemePreviewer(const Theme& theme, int width, int height);
        ~ThemePreviewer();

        QIcon getIcon();
        void renderPreview(const Theme& newSettings);

    private:
        Theme theme;
        int width;
        int height;
        QIcon thumbnailPreviewIcon;

};

#endif // THEMEPREVIEWER_H
