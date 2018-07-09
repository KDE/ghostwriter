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

#include <QColor>
#include <QString>

#include "ColorHelper.h"

QColor ColorHelper::applyAlpha(const QColor& foreground, const QColor& background)
{
    QColor blendedColor(0, 0, 0);

    blendedColor.setRed
    (
        applyAlphaToChannel
        (
            foreground.red(),
            background.red(),
            foreground.alphaF()
        )
    );

    blendedColor.setGreen
    (
        applyAlphaToChannel
        (
            foreground.green(),
            background.green(),
            foreground.alphaF()
        )
    );

    blendedColor.setBlue
    (
        applyAlphaToChannel
        (
            foreground.blue(),
            background.blue(),
            foreground.alphaF()
        )
    );

    return blendedColor;
}

QColor ColorHelper::applyAlpha
(
    const QColor& foreground,
    const QColor& background,
    int alpha
)
{
    if ((alpha < 0) || alpha > 255)
    {
        qCritical("ColorHelper::applyAlpha: alpha value must be "
            "between 0 and 255. Value provided = %d",
            alpha);
    }

    QColor blendedColor(0, 0, 0);
    qreal normalizedAlpha = alpha / 255.0;

    blendedColor.setRed
    (
        applyAlphaToChannel
        (
            foreground.red(),
            background.red(),
            normalizedAlpha
        )
    );

    blendedColor.setGreen
    (
        applyAlphaToChannel
        (
            foreground.green(),
            background.green(),
            normalizedAlpha
        )
    );

    blendedColor.setBlue
    (
        applyAlphaToChannel
        (
            foreground.blue(),
            background.blue(),
            normalizedAlpha
        )
    );

    return blendedColor;
}


QString ColorHelper::toRgbString(const QColor& color)
{
    return QString("rgb(%1, %2, %3)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue());
}

QString ColorHelper::toRgbaString(const QColor& color)
{
    return QString("rgba(%1, %2, %3, %4)")
        .arg(color.red())
        .arg(color.green())
        .arg(color.blue())
        .arg(color.alpha());
}

// Algorithm taken from *Grokking the GIMP* by Carey Bunks,
// section 5.3.
//
// Grokking the GIMP
// by Carey Bunks
// Copyright (c) 2000 by New Riders Publishing, www.newriders.com
// ISBN 0-7357-0924-6.
//
double ColorHelper::getLuminance(const QColor& color)
{
    QColor c = color;

    // Ensure color is non-zero.
    if (c == QColor(Qt::black))
    {
        c.setRgb(1, 1, 1);
    }

    return (0.30 * c.redF()) + (0.59 * c.greenF()) + (0.11 * c.blueF());
}

QColor ColorHelper::lightenToMatchContrastRatio
(
    const QColor& foreground,
    const QColor& background,
    double contrastRatio
)
{
    double fgBrightness = getLuminance(foreground);
    double bgBrightness = getLuminance(background);

    // If the background color is brighter than the foreground color...
    if (bgBrightness > fgBrightness)
    {
        double actualContrastRatio = bgBrightness / fgBrightness;
        double colorFactor = contrastRatio / actualContrastRatio;

        QColor result = foreground;

        // Ensure color is non-zero.
        if (result == QColor(Qt::black))
        {
            result.setRgb(1, 1, 1);
        }

        qreal r = result.redF() / colorFactor;
        qreal g = result.greenF() / colorFactor;
        qreal b = result.blueF() / colorFactor;

        if (r > 1.0)
        {
            r = 1.0;
        }

        if (g > 1.0)
        {
            g = 1.0;
        }

        if (b > 1.0)
        {
            b = 1.0;
        }

        result.setRgbF(r, g, b);

        return result;
    }
    else
    {
        // This algorithm cannot change the foreground color when it is
        // lighter than the background color, so return the original
        // foreground color.
        //
        return foreground;
    }
}
