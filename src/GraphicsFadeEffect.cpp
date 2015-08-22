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

#include <QPainter>
#include <QPixmap>

#include "GraphicsFadeEffect.h"

GraphicsFadeEffect::GraphicsFadeEffect(QObject* parent)
    : QGraphicsEffect(parent)
{
    fadeHeight = 50;
}

GraphicsFadeEffect::~GraphicsFadeEffect()
{

}

void GraphicsFadeEffect::setFadeHeight(int height)
{
    fadeHeight = height;
}

void GraphicsFadeEffect::draw(QPainter* painter)
{
    QPixmap pixmap = sourcePixmap();

    QPainter p(&pixmap);

    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);

    int rectHeight = fadeHeight;
    int alpha = 255;
    int step = (alpha / rectHeight) + 1;
    QColor color(0, 0, 0, alpha);

    for (int y = pixmap.height() - rectHeight; y < pixmap.height(); y++)
    {
        color.setAlpha(alpha);
        p.setPen(color);
        p.setBrush(color);
        p.drawLine(0, y, pixmap.width() - 1, y);

        if (alpha >= step)
        {
            alpha -= step;
        }
        else
        {
            alpha = 0;
        }
    }

    p.end();

    painter->drawPixmap(pixmap.rect(), pixmap);
}
