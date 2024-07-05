/*
 * SPDX-FileCopyrightText: 2020-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <math.h>

#include <QDebug>

#include "chromecolors.h"

namespace ghostwriter
{

typedef enum ColorId {
    FirstColor,
    BackgroundColor = FirstColor,
    TextColor,
    LabelColor,
    FillColor,
    AccentColor,
    AccentFillColor,
    LinkColor,
    HeadingColor,
    CodeColor,
    BlockQuoteColor,
    SeparatorColor,
    SelectedTextFgColor,
    SelectedTextBgColor,
    InfoColor,
    InfoFillColor,
    ErrorColor,
    ErrorFillColor,
    WarningColor,
    WarningFillColor,
    SuccessColor,
    SuccessFillColor,
    SecondaryBackgroundColor,
    SecondaryFillColor,
    TertiaryFillColor,
    SecondaryLabelColor,
    PlaceholderTextColor,
    SelectionFgColor,
    SecondarySeparatorColor,
    GridColor,
    ColorCount
} ColorId;

static const QColor InfoColorValue("#03A9F4");
static const QColor SuccessColorValue("#4CAF50");
static const QColor WarningColorValue("#FFEB3B");
static const QColor ErrorColorValue("#F44336");

/**
 * Returns the luminance of a color on a scale of 0.0 (dark) to
 * 1.0 (light).  Luminance is based on how light or dark a color
 * appears to the human eye.
 */
static double luminance(const QColor &color);

/**
 * Returns the contrast ratio between two colors.
 */
static double contrast(const QColor &rgb1, const QColor &rgb2);

/**
 * Returns a new color based on the foreground color, such that the new color
 * is lightened or darkened to achieve the desired contrast ratio against the
 * given background color.
 */
static QColor shadeToContrastRatio(const QColor &foreground, const QColor &background, double contrastRatio);

/**
 * Returns a mix of two colors.  The weight is a percentage (0-100) of color1
 * that will be used.  In other words, a weight of more 50% indicates that more
 * of color1 will be used, and a weight of less than 50% indicates that more of
 * color2 will be used.  The default weight is 50%, which indicates that equal
 * amounts of both colors will be used.
 */
static QColor mix(const QColor &color1, const QColor &color2, int weight = 50);

class ChromeColorsPrivate
{
public:
    ChromeColorsPrivate()
    {
    }
    ~ChromeColorsPrivate()
    {
    }

    QColor colors[ColorCount][ChromeColors::StateCount];
};

ChromeColors::ChromeColors(const ColorScheme &base)
    : d(new ChromeColorsPrivate)
{
    d->colors[BackgroundColor][NormalState] = base.background;
    d->colors[AccentColor][NormalState] = base.link;
    d->colors[AccentFillColor][NormalState] = mix(base.link, base.background, 25);
    d->colors[LabelColor][NormalState] = base.foreground;
    d->colors[TextColor][NormalState] = base.foreground;

    d->colors[SelectedTextFgColor][NormalState] = base.foreground;
    d->colors[SelectedTextBgColor][NormalState] = base.selection;

    d->colors[LinkColor][NormalState] = base.link;
    d->colors[HeadingColor][NormalState] = base.headingText;
    d->colors[CodeColor][NormalState] = base.codeText;
    d->colors[BlockQuoteColor][NormalState] = base.blockquoteText;
    d->colors[SeparatorColor][NormalState] = base.emphasisMarkup;

    d->colors[InfoColor][NormalState] = mix(InfoColorValue, base.foreground, 85);
    d->colors[InfoFillColor][NormalState] = mix(InfoColorValue, base.background, 25);
    d->colors[ErrorColor][NormalState] = mix(ErrorColorValue, base.foreground, 85);
    d->colors[ErrorFillColor][NormalState] = mix(ErrorColorValue, base.background, 25);
    d->colors[WarningColor][NormalState] = mix(WarningColorValue, base.foreground, 85);
    d->colors[WarningFillColor][NormalState] = mix(WarningColorValue, base.background, 25);
    d->colors[SuccessColor][NormalState] = mix(SuccessColorValue, base.foreground, 85);
    d->colors[SuccessFillColor][NormalState] = mix(SuccessColorValue, base.background, 25);

    QColor chromeColor;
    QColor selectedFgColor = base.foreground;

    bool lightMode = (luminance(base.background) > luminance(base.foreground));

    // If light mode color scheme...
    if (lightMode) {
        // Slightly blend the new UI chrome color with the editor background
        // color to help it match better.
        //
        chromeColor = mix(chromeColor, base.background, 86);

        // Create a UI chrome color based on a lightened editor text color,
        // such that the new color achieves a lower contrast ratio.
        //
        chromeColor = shadeToContrastRatio(base.foreground, base.background, 4.5);

        d->colors[FillColor][NormalState] = shadeToContrastRatio(base.background, chromeColor, 3.0);

        // If the text selection background color is very dark, then make the
        // text selection foreground color light to ensure sufficient contrast.
        //
        if (luminance(base.selection) < 0.5) {
            selectedFgColor = base.background;
        }
    }
    // Else if dark mode color scheme...
    else {
        chromeColor = shadeToContrastRatio(base.foreground, base.background, 5.5);

        d->colors[FillColor][NormalState] = shadeToContrastRatio(base.background, chromeColor, 4.5);

        // If the text selection background color is very light, then make sure
        // the text selection foreground color is dark to ensure sufficient
        // contrast.
        //
        if (luminance(base.selection) >= 0.5) {
            selectedFgColor = base.background;
        }
    }

    d->colors[SelectionFgColor][NormalState] = selectedFgColor;

    d->colors[SecondaryBackgroundColor][NormalState] = mix(chromeColor, base.background, 5);
    d->colors[SecondaryFillColor][NormalState] = mix(chromeColor, base.background, 40);
    d->colors[TertiaryFillColor][NormalState] = mix(chromeColor, base.background, 10);
    d->colors[SecondaryLabelColor][NormalState] = chromeColor;
    d->colors[PlaceholderTextColor][NormalState] = base.foreground;
    d->colors[SelectedTextFgColor][NormalState] = selectedFgColor;
    d->colors[SecondarySeparatorColor][NormalState] = mix(chromeColor, base.background, 20);
    d->colors[GridColor][NormalState] = mix(chromeColor, base.background, 20);

    for (ColorId id = FirstColor; id < ColorCount; id = (ColorId)(id + 1)) {
        QColor baseColor = d->colors[id][NormalState];

        if (lightMode) {
            d->colors[id][PressedState] = baseColor.darker(107);
            d->colors[id][ActiveState] = baseColor.darker(103);
            d->colors[id][HoverState] = baseColor.lighter(103);

            switch (id) {
            case FillColor:
            case AccentFillColor:
            case InfoFillColor:
            case ErrorFillColor:
            case WarningFillColor:
            case SuccessFillColor:
            case SecondaryFillColor:
            case TertiaryFillColor:
                d->colors[id][DisabledState] = baseColor.lighter(105);
                break;
            default:
                d->colors[id][DisabledState] = baseColor.lighter(125);
                break;
            }
        } else {
            d->colors[id][PressedState] = baseColor.darker(120);
            d->colors[id][ActiveState] = baseColor.darker(110);
            d->colors[id][HoverState] = baseColor.lighter(150);
            d->colors[id][DisabledState] = baseColor.darker(200);
        }
    }
}

ChromeColors::ChromeColors(const ChromeColors &other)
{
    for (ColorId id = FirstColor; id < ColorCount; id = (ColorId)(id + 1)) {
        for (State state = FirstState; state < StateCount; state = (State)(state + 1)) {
            d->colors[id][state] = other.d->colors[id][state];
        }
    }
}

ChromeColors::~ChromeColors()
{
    ;
}

QColor ChromeColors::color(ColorElem elem, State state) const
{
    if ((elem < ChromeColors::FirstElem) || (elem > ChromeColors::LastElem) || (state < ChromeColors::FirstState) || (state > ChromeColors::LastState)) {
        return QColor();
    }

    return d->colors[elem][state];
}

static double channelLuminance(const float colorChannel)
{
    double result;

    if (colorChannel <= 0.03928) {
        result = colorChannel / 12.92;
    } else {
        result = pow((colorChannel + 0.055) / 1.055, 2.4);
    }

    return result;
}

static double luminance(const QColor &color)
{
    double r, g, b;

    // // Ensure color is non-zero.
    // if (c == QColor(Qt::black)) {
    //     c.setRgb(1, 1, 1);
    // }

    r = channelLuminance(color.redF());
    g = channelLuminance(color.greenF());
    b = channelLuminance(color.blueF());

    return (0.2126 * r) + (0.7152 * g) + (0.0722 * b);
}

static double contrast(const QColor &rgb1, const QColor &rgb2)
{
    double lum1 = luminance(rgb1);
    double lum2 = luminance(rgb2);
    double brightest = lum2;
    double darkest = lum1;

    if (lum1 > lum2) {
        brightest = lum1;
        darkest = lum2;
    }

    return (brightest + 0.05) / (darkest + 0.05);
}

static QColor shadeToContrastRatio(const QColor &foreground, const QColor &background, double contrastRatio)
{
    double fgBrightness = luminance(foreground);
    double bgBrightness = luminance(background);
    int step = 64;

    // If the foreground color is brighter than the background color,
    // then set the HSV brightness step value to go darker rather than
    // lighter.
    //
    if (fgBrightness > bgBrightness) {
        step = -step;
    }

    QColor result = foreground;
    double actualContrastRatio = contrast(foreground, background);

    while ((actualContrastRatio > contrastRatio) && (((step > 0) && (result.value() <= (255 - step))) || ((step < 0) && (result.value() >= -step)))) {
        result.setHsv(result.hue(), result.saturation(), result.value() + step);
        actualContrastRatio = contrast(result, background);

        if (actualContrastRatio < contrastRatio) {
            // Oops!  Went too far!  Back up a step!
            result.setHsv(result.hue(), result.saturation(), result.value() - step);
            actualContrastRatio = contrast(result, background);

            // Make smaller steps toward target ratio from here.
            if (abs(step) > 1) {
                step /= 2;
            } else {
                // Can't go any further. Done!
                break;
            }
        }
    }

    return result;
}

/**
 * Returns a mix of the two channel colors, with the first color parameter
 * weighted by the given value (0.0-1.0).
 */
static inline int mixColorChannel(int c1, int c2, double weight)
{
    return (int)((c1 * weight) + (c2 * (1.0 - weight)));
}

static QColor mix(const QColor &color1, const QColor &color2, int weight)
{
    if ((weight < 0) || weight > 100) {
        qCritical() << "mix(): weight value must be between 0 and 100."
                    << "Value provided:" << weight;

        if (weight < 0) {
            weight = 0;
        } else {
            weight = 100;
        }
    }

    QColor blendedColor(0, 0, 0);
    double normalizedWeight = ((double)weight) / 100.0;

    blendedColor.setRed(mixColorChannel(color1.red(), color2.red(), normalizedWeight));
    blendedColor.setGreen(mixColorChannel(color1.green(), color2.green(), normalizedWeight));
    blendedColor.setBlue(mixColorChannel(color1.blue(), color2.blue(), normalizedWeight));

    return blendedColor;
}

} // namespace ghostwriter
