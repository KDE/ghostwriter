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

#ifndef COLORHELPER_H
#define COLORHELPER_H

class QColor;
class QString;

class ColorHelper
{
    public:
        /**
         * Applies an alpha effect to the given foreground color against the
         * given background color, returning a blend of the two colors.  In
         * other words, new color returned will have foreground color appearing
         * to be transparent or semi-transparent as though it were colored
         * on top of the background color.  The alpha value used is from
         * the foreground color's alpha() channel.
         */
        static QColor applyAlpha
        (
            const QColor& foreground,
            const QColor& background
        );

        /**
         * Applies an alpha effect to the given foreground color against the
         * given background color, returning a blend of the two colors.  In
         * other words, new color returned will have foreground color appearing
         * to be transparent or semi-transparent as though it were colored
         * on top of the background color.  The alpha value used is from
         * the alpha parameter passed in, and will be applied to all channels.
         *
         * alpha parameter must be within the range of 0 to 255.
         */
        static QColor applyAlpha
        (
            const QColor& foreground,
            const QColor& background,
            int alpha
        );

        /**
         * Gets string representation of the given color in RGB format for
         * use in style sheets.
         */
         static QString toRgbString(const QColor& color);

        /**
         * Gets string representation of the given color in RGBA format for
         * use in style sheets.
         */
         static QString toRgbaString(const QColor& color);

        /**
         * Returns luminance of the given color on a scale of 0.0 (dark) to
         * 1.0 (light).  Luminance is based on how light or dark a color
         * appears to the human eye.
         */
        static double getLuminance(const QColor& color);


        static QColor adjustBrightness(const QColor& color, int delta);

    private:
        static inline int applyAlphaToChannel
        (
            const int foreground,
            const int background,
            const qreal alpha
        )
        {
            return (int) ((foreground * alpha) + (background * (1.0 - alpha)));
        }
};

#endif // ALPHACOLOREFFECT_H
