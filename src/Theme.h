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

#ifndef THEME_H
#define THEME_H

#include <QString>
#include "MarkdownColorScheme.h"

enum EditorAspect
{
    EditorAspectFirst,
    EditorAspectStretch = EditorAspectFirst,
    EditorAspectCenter,
    EditorAspectLast = EditorAspectCenter
};

enum EditorCorners
{
    EditorCornersFirst,
    EditorCornersRounded = EditorCornersFirst,
    EditorCornersSquare,
    EditorCornersLast = EditorCornersSquare,
};

enum TextWidth
{
    TextWidthNarrow,
    TextWidthMedium,
    TextWidthWide
};

enum PictureAspect
{
    PictureAspectFirst,
    PictureAspectNone = PictureAspectFirst,
    PictureAspectTile,
    PictureAspectCenter,
    PictureAspectStretch,
    PictureAspectScale,
    PictureAspectZoom,
    PictureAspectLast = PictureAspectZoom,
};

struct Theme
{
    QString name;
    MarkdownColorScheme markupColorScheme;

    EditorAspect editorAspect;
    EditorCorners editorCorners;
    PictureAspect backgroundImageAspect;
    QString backgroundImageUrl;
    QColor backgroundColor;

    QColor hudForegroundColor;
    QColor hudBackgroundColor;
};

#endif
