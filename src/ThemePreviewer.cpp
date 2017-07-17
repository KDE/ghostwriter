/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
 * Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
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

#include "ThemePreviewer.h"

#include <QPainter>
#include <QPixmap>
#include <QImage>

ThemePreviewer::ThemePreviewer(const Theme& theme, int width, int height)
{
    this->width = width;
    this->height = height;

    renderPreview(theme);
}

ThemePreviewer::~ThemePreviewer()
{
    ;
}

QIcon ThemePreviewer::getIcon()
{
    return thumbnailPreviewIcon;
}

void ThemePreviewer::renderPreview(const Theme& newSettings)
{
    this->theme = newSettings;

    QPixmap thumbnailPixmap(this->width, this->height);
    QPainter painter(&thumbnailPixmap);

    // First, paint the background image, if any.
    if (PictureAspectNone != theme.getBackgroundImageAspect())
    {
        QImage destImg;

        // Load the background image from the file, if available, and zoom.
        if
        (
            !theme.getBackgroundImageUrl().isNull() &&
            !theme.getBackgroundImageUrl().isEmpty())
        {
            QImage srcImg(theme.getBackgroundImageUrl());
            destImg = srcImg.scaled
                (
                    thumbnailPixmap.size(),
                    Qt::KeepAspectRatioByExpanding,
                    Qt::SmoothTransformation
                );
        }

        // Draw the image.
        painter.fillRect
        (
            thumbnailPixmap.rect(),
            QBrush(theme.getBackgroundColor())
        );

        if (!destImg.isNull())
        {
            painter.drawImage
            (
                (thumbnailPixmap.width() - destImg.width()) / 2,
                (thumbnailPixmap.height() - destImg.height()) / 2,
                destImg
            );
        }
    }
    // If there's no background image, then just fill the background color.
    else
    {
        QColor bgColor = theme.getBackgroundColor();

        if (EditorAspectStretch == theme.getEditorAspect())
        {
            bgColor = theme.getEditorBackgroundColor();
        }

        painter.fillRect
        (
            thumbnailPixmap.rect(),
            bgColor.rgb()
        );
    }

    // Next, draw the "editor" background.

    int w = 2 * width / 3;
    int h = height / 4;
    int x = (width) / 3 - 3;
    int y = (3 * height) / 4 - 3;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(theme.getEditorBackgroundColor()));

    if (EditorAspectCenter == theme.getEditorAspect())
    {
        painter.drawRect(x, y, w, h);
    }
    else
    {
        painter.drawRect(0, 0, width, height);
    }

    // Now draw a circle in the "editor" background for each of the text colors.
    int radius = (h / 4);
    int xoffset = w / 3;
    int cx1 = x + (xoffset / 2);
    int cx2 = cx1 + xoffset;
    int cx3 = cx2 + xoffset;
    int cy = y + (h / 2);

    painter.setBrush(QBrush(theme.getDefaultTextColor()));
    painter.drawEllipse(QPoint(cx1, cy), radius, radius);
    painter.setBrush(QBrush(theme.getMarkupColor()));
    painter.drawEllipse(QPoint(cx2, cy), radius, radius);
    painter.setBrush(QBrush(theme.getLinkColor()));
    painter.drawEllipse(QPoint(cx3, cy), radius, radius);

    painter.end();
    thumbnailPreviewIcon = QIcon(thumbnailPixmap);
}
