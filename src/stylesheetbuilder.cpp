/*
 * SPDX-FileCopyrightText: 2020-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <math.h>

#include <QApplication>
#include <QColor>
#include <QDir>
#include <QFile>
#include <QPalette>
#include <QRegularExpression>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QVariant>

#include "../3rdparty/QtAwesome/QtAwesome.h"
#include "stylesheetbuilder.h"


namespace ghostwriter
{
static const QColor InfoColor("#03A9F4");
static const QColor SuccessColor("#4CAF50");
static const QColor WarningColor("#FFEB3B");
static const QColor ErrorColor("#F44336");

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
static QColor shadeToContrastRatio
(
    const QColor &foreground,
    const QColor &background,
    double contrastRatio
);

/**
 * Returns a mix of two colors.  The weight is a percentage (0-100) of color1
 * that will be used.  In other words, a weight of more 50% indicates that more
 * of color1 will be used, and a weight of less than 50% indicates that more of
 * color2 will be used.  The default weight is 50%, which indicates that equal
 * amounts of both colors will be used.
 */
static QColor mix
(
    const QColor &color1,
    const QColor &color2,
    int weight = 50
);

/**
 * Returns the font family name of the given font with bracketed text removed.
 */
static QString sanitizeFontFamily(const QFont &font)
{
    static QRegularExpression bracketsExpr("\\[.*\\]");

    return font.family().remove(bracketsExpr).trimmed();
}

QString StyleSheetBuilder::m_statIndicatorArrowIconPath = QString();

StyleSheetBuilder::StyleSheetBuilder(const ColorScheme &colors,
        const bool roundedCorners,
        const QFont &editorFont,
        const QFont &previewTextFont,
        const QFont &previewCodeFont)
{
    QString styleSheet;
    QTextStream stream(&styleSheet);

    bool lightMode = (luminance(colors.background) > luminance(colors.foreground));

    m_styleSheetVariables["$editor-font-family"] = sanitizeFontFamily(editorFont);
    m_styleSheetVariables["$body-font-family"] = sanitizeFontFamily(previewTextFont);
    m_styleSheetVariables["$code-font-family"] = sanitizeFontFamily(previewCodeFont);
    m_styleSheetVariables["$editor-font-size"] = QString("%1pt").arg(editorFont.pointSize());
    m_styleSheetVariables["$body-font-size"] = QString("%1pt").arg(previewTextFont.pointSize());
    m_styleSheetVariables["$code-font-size"] = QString("%1pt").arg(previewCodeFont.pointSize());

    if (roundedCorners) {
        m_styleSheetVariables["$scrollbar-border-radius"] = "3px";
        m_styleSheetVariables["$default-border-radius"] = "5px";
    } else {
        m_styleSheetVariables["$scrollbar-border-radius"] = "0";
        m_styleSheetVariables["$default-border-radius"] = "0";
    }

    m_styleSheetVariables["$background-color"] = colors.background;
    m_styleSheetVariables["$accent-color"] = colors.link;
    m_styleSheetVariables["$accent-fill-color"] = mix(colors.link, colors.background, 25);
    m_styleSheetVariables["$label-color"] = colors.foreground;
    m_styleSheetVariables["$text-color"] = colors.foreground;

    m_styleSheetVariables["$selected-text-fg-color"] = colors.foreground;
    m_styleSheetVariables["$selected-text-bg-color"] = colors.selection;

    m_styleSheetVariables["$link-color"] = colors.link.name();
    m_styleSheetVariables["$heading-color"] = colors.headingText;
    m_styleSheetVariables["$code-color"] = colors.codeText;
    m_styleSheetVariables["$block-quote-color"] = colors.blockquoteText;
    m_styleSheetVariables["$separator-color"] = colors.emphasisMarkup;

    m_styleSheetVariables["$info-color"] = mix(InfoColor, colors.foreground, 85);
    m_styleSheetVariables["$info-fill-color"] = mix(InfoColor, colors.background, 25);
    m_styleSheetVariables["$error-color"] = mix(ErrorColor, colors.foreground, 85);
    m_styleSheetVariables["$error-fill-color"] = mix(ErrorColor, colors.background, 25);
    m_styleSheetVariables["$warning-color"] = mix(WarningColor, colors.foreground, 85);
    m_styleSheetVariables["$warning-fill-color"] = mix(WarningColor, colors.background, 25);
    m_styleSheetVariables["$success-color"] = mix(SuccessColor, colors.foreground, 85);
    m_styleSheetVariables["$success-fill-color"] = mix(SuccessColor, colors.background, 25);

    QColor chromeColor;
    QColor selectedFgColor = colors.foreground;

    // If light mode color scheme...
    if (lightMode) {

        // Slightly blend the new UI chrome color with the editor background
        // color to help it match better.
        //
        chromeColor = mix(chromeColor, colors.background, 86);

        // Create a UI chrome color based on a lightened editor text color,
        // such that the new color achieves a lower contrast ratio.
        //
        chromeColor =
            shadeToContrastRatio(
                colors.foreground,
                colors.background,
                4.5
            );
        m_styleSheetVariables["$fill-color"] = shadeToContrastRatio(colors.background, chromeColor, 3.0);

        // If the text selection background color is very dark, then make the
        // text selection foreground color light to ensure sufficient contrast.
        //
        if (luminance(colors.selection) < 0.5) {
            selectedFgColor = colors.background;
        }        
    }
    // Else if the dark mode color scheme...
    else {
        chromeColor =
            shadeToContrastRatio(
                colors.foreground,
                colors.background,
                5.5
            );
        m_styleSheetVariables["$fill-color"] = shadeToContrastRatio(colors.background, chromeColor, 4.5);

        // If the text selection background color is very light, then make sure
        // the text selection foreground color is dark to ensure sufficient
        // contrast.
        //
        if (luminance(colors.selection) >= 0.5) {
            selectedFgColor = colors.background;
        }
    }

    m_styleSheetVariables["$secondary-background-color"] = mix(chromeColor, colors.background, 5);
    m_styleSheetVariables["$secondary-fill-color"] = mix(chromeColor, colors.background, 40);
    m_styleSheetVariables["$tertiary-fill-color"] = mix(chromeColor, colors.background, 10);
    m_styleSheetVariables["$secondary-label-color"] = chromeColor;
    m_styleSheetVariables["$placeholder-text-color"] = colors.foreground;
    m_styleSheetVariables["$selection-fg-color"] = selectedFgColor;
    m_styleSheetVariables["$secondary-separator-color"] = mix(chromeColor, colors.background, 20);
    m_styleSheetVariables["$grid-color"] = mix(chromeColor, colors.background, 20);

    for (QString key : m_styleSheetVariables.keys()) {
        if (key.endsWith("-color")) {
            QColor baseColor = m_styleSheetVariables.value(key).value<QColor>();

            if (lightMode) {
                m_styleSheetVariables[key + "-pressed"] = baseColor.darker(107);
                m_styleSheetVariables[key + "-active"] = baseColor.darker(103);
                m_styleSheetVariables[key + "-hover"] = baseColor.lighter(103);

                if (key.endsWith("fill-color")) {
                    m_styleSheetVariables[key + "-disabled"] = baseColor.lighter(105);
                } else {
                    m_styleSheetVariables[key + "-disabled"] = baseColor.lighter(125);
                }
            } else {
                m_styleSheetVariables[key + "-pressed"] = baseColor.darker(120);
                m_styleSheetVariables[key + "-active"] = baseColor.darker(110);
                m_styleSheetVariables[key + "-hover"] = baseColor.lighter(150);
                m_styleSheetVariables[key + "-disabled"] = baseColor.darker(200);
            }
        }
    }

    // Remove previous cache/temporary files.
    clearCache();

    // Refresh statistics indicator drop-down arrow icon.
    this->m_awesome = new QtAwesome();
    this->m_awesome->initFontAwesome();

    QVariantMap options;
    options.insert("color", chromeColor);
    QIcon statIndicatorIcon = this->m_awesome->icon(style::stfas, fa::chevroncircleup, options);

    QTemporaryFile tempIconFile(QDir::tempPath() + "/XXXXXX.png");
    tempIconFile.setAutoRemove(false);

    if (tempIconFile.open()) {
        m_statIndicatorArrowIconPath = tempIconFile.fileName();
        statIndicatorIcon.pixmap(16, 16).save(&tempIconFile, "PNG");
        tempIconFile.close();

        m_styleSheetVariables["$status-indicator-icon-path"] = m_statIndicatorArrowIconPath;
    }
}

StyleSheetBuilder::~StyleSheetBuilder()
{
    if (nullptr != this->m_awesome) {
        delete this->m_awesome;
        this->m_awesome = nullptr;
    }
}

void StyleSheetBuilder::clearCache()
{
    // Remove previous cache/temporary file of statistics indicator drop-down arrow icon.
    if (!m_statIndicatorArrowIconPath.isNull() && !m_statIndicatorArrowIconPath.isEmpty()) {
        QFile iconFile(m_statIndicatorArrowIconPath);

        if (iconFile.exists()) {
            iconFile.remove();
        }
    }
}

QString StyleSheetBuilder::widgetStyleSheet()
{
    return compileStyleSheet(":/resources/widgets.qss");
}

QString StyleSheetBuilder::htmlPreviewStyleSheet()
{
    return compileStyleSheet(":/resources/preview.css");
}

QString StyleSheetBuilder::stringValueOf(const QString &variableName) const {
    QVariant value = m_styleSheetVariables.value(variableName);

    if (!value.isValid()) {
        qCritical() << "Undefined variable"
                    << variableName << "in style sheet";
        return QString();
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    if (QMetaType::QString == (QMetaType::Type) value.type()) {
#else
    if (QMetaType::QString == value.typeId()) {
#endif
        return value.toString();
    }
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    else if (QMetaType::QColor == (QMetaType::Type) value.type()) {
#else
    else if (QMetaType::QColor == value.typeId()) {
#endif
        QColor color = value.value<QColor>();

        if (color.alpha() < 255) {
            return color.name(QColor::HexArgb);
        } else {
            return color.name(QColor::HexRgb);
        }
    }
    else {
        qCritical() << "Invalid variable type used for" << variableName;
        return QString();
    }
}

QString StyleSheetBuilder::compileStyleSheet(const QString &path) const
{
    QString compiled = "";

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }

    QTextStream in(&file);
    QTextStream out(&compiled);

    QString variable;

    while (!in.atEnd()) {
        QChar ch;
        in >> ch;

        if (variable.isNull() && ('$' == ch)) {
            variable = ch;
        }
        else if (!variable.isNull()) {
            if (ch.isLetterOrNumber() || ('-' == ch) || ('_' == ch)) {
                variable += ch;
            }
            else {
                QString value = stringValueOf(variable);

                if (value.isNull()) {
                    return QString();
                }

                out << value;
                out << ch;
                variable = QString();
            }
        }
        else {
            out << ch;
        }
    }

    if (!variable.isNull()) {
        QString value = stringValueOf(variable);

        if (value.isNull()) {
            return QString();
        }

        out << value;
    }

    return compiled;
}

double channelLuminance(const float colorChannel)
{
    double result;

    if (colorChannel <= 0.03928) {
        result = colorChannel / 12.92;
    } else {
        result = pow((colorChannel + 0.055) / 1.055, 2.4);
    }

    return result;
}

double luminance(const QColor &color)
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

double contrast(const QColor &rgb1, const QColor &rgb2)
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

QColor shadeToContrastRatio
(
    const QColor &foreground,
    const QColor &background,
    double contrastRatio
)
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

    while ((actualContrastRatio > contrastRatio) 
            && (((step > 0) && (result.value() <= (255 - step)))
                || ((step < 0) && (result.value() >= -step)))) {
        result.setHsv(result.hue(), result.saturation(),
            result.value() + step);
        actualContrastRatio = contrast(result, background);

        if (actualContrastRatio < contrastRatio) {
            // Oops!  Went too far!  Back up a step!
            result.setHsv(result.hue(), result.saturation(),
                result.value() - step);
            actualContrastRatio = contrast(result, background);
            
            // Make smaller steps toward target ratio from here.
            if (abs(step) > 1) {
                step /= 2;
            }
            else {
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

QColor mix
(
    const QColor &color1,
    const QColor &color2,
    int weight
)
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
    double normalizedWeight = ((double) weight) / 100.0;

    blendedColor.setRed(mixColorChannel(color1.red(), color2.red(), normalizedWeight));
    blendedColor.setGreen(mixColorChannel(color1.green(), color2.green(), normalizedWeight));
    blendedColor.setBlue(mixColorChannel(color1.blue(), color2.blue(), normalizedWeight));

    return blendedColor;
}

} // namespace ghostwriter
