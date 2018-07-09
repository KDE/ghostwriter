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

#ifndef GRAPHICSFADEEFFECT_H
#define GRAPHICSFADEEFFECT_H

#include <QGraphicsEffect>

/**
 * Applies a gradual fade effect at the bottom of the widget.
 */
class GraphicsFadeEffect : public QGraphicsEffect
{
    public:
        /**
         * Constructor.
         */
        GraphicsFadeEffect(QObject* parent = 0);

        /**
         * Destructor.
         */
        virtual ~GraphicsFadeEffect();

        /**
         * Sets the height in pixels of the fade effect that will be applied
         * to the bottom of the widget.
         */
        void setFadeHeight(int height);

        /**
         * Overridden method to draw the effect.
         */
        void draw(QPainter* painter);

    private:
        int fadeHeight;
};

#endif // GRAPHICSFADEEFFECT_H
