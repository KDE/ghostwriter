/***********************************************************************
 *
 * Copyright (C) 2020-2022 wereturtle
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

#include <tuple>

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPalette>
#include <QRegularExpression>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>
#include <QTemporaryFile>
#include <QVariant>

#include "3rdparty/QtAwesome/QtAwesome.h"
#include "qcolor.h"
#include "stylesheetbuilder.h"


namespace ghostwriter
{
static const QColor InfoColor("#03A9F4");
static const QColor SuccessColor("#4CAF50");
static const QColor WarningColor("#FFEB3B");
static const QColor ErrorColor("#F44336");

typedef enum
{
    M_FirstBaseColor,
    M_Red = M_FirstBaseColor,
    M_Pink,
    M_Purple,
    M_DeepPurple,
    M_Indigo,
    M_Blue,
    M_LightBlue,
    M_Cyan,
    M_Teal,
    M_Green,
    M_LightGreen,
    M_Lime,
    M_Yellow,
    M_Amber,
    M_Orange,
    M_DeepOrange,
    M_Brown,
    M_Gray,
    M_BlueGray,
    M_BaseColorCount
} MaterialHue;

typedef enum
{
    M_FirstBaseShade
    M_50 = M_FirstBaseShade,
    M_100,
    M_200,
    M_300,
    M_400,
    M_500,
    M_600,
    M_700,
    M_800,
    M_900,
    M_LastBaseShade = M_900,
    M_FirstAccentShade,
    M_A100 = M_FirstAccentShade,
    M_A200,
    M_A400,
    M_A700,
    M_LastAccentShade = M_A700,
    M_ShadeCount
} MaterialShade;

class MaterialColor
{
public:
    MaterialColor(MaterialHue base, MaterialShade shade);
    MaterialColor(const QColor& color);

    ~MaterialColor();

    QColor color() const;

    MaterialColor lighter() const;
    MaterialColor darker() const;

private:
    const QColor m_palette[ColorCount][ShadeCount] =
    {
        [M_Red] = {
            [M_50] = 0xFFFFEBEE
            [M_100] = 0xFFFFCDD2
            [M_200] = 0xFFEF9A9A
            [M_300] = 0xFFE57373
            [M_400] = 0xFFEF5350
            [M_500] = 0xFFF44336
            [M_600] = 0xFFE53935
            [M_700] = 0xFFD32F2F
            [M_800] = 0xFFC62828
            [M_900] = 0xFFB71C1C
            [M_A100] = 0xFFFF8A80
            [M_A200] = 0xFFFF5252
            [M_A400] = 0xFFFF1744
            [M_A700] = 0xFFD50000
        },
        [M_Pink] = {
            [M_50] = 0xFFFCE4EC
            [M_100] = 0xFFF8BBD0
            [M_200] = 0xFFF48FB1
            [M_300] = 0xFFF06292
            [M_400] = 0xFFEC407A
            [M_500] = 0xFFE91E63
            [M_600] = 0xFFD81B60
            [M_700] = 0xFFC2185B
            [M_800] = 0xFFAD1457
            [M_900] = 0xFF880E4F
            [M_A100] = 0xFFFF80AB
            [M_A200] = 0xFFFF4081
            [M_A400] = 0xFFF50057
            [M_A700] = 0xFFC51162
        },
        [M_Purple] = {
            [M_50] = 0xFFF3E5F5
            [M_100] = 0xFFE1BEE7
            [M_200] = 0xFFCE93D8
            [M_300] = 0xFFBA68C8
            [M_400] = 0xFFAB47BC
            [M_500] = 0xFF9C27B0
            [M_600] = 0xFF8E24AA
            [M_700] = 0xFF7B1FA2
            [M_800] = 0xFF6A1B9A
            [M_900] = 0xFF4A148C
            [M_A100] = 0xFFEA80FC
            [M_A200] = 0xFFE040FB
            [M_A400] = 0xFFD500F9
            [M_A700] = 0xFFAA00FF
        },
        [M_DeepPurple] = {
            [M_50] = 0xFFEDE7F6
            [M_100] = 0xFFD1C4E9
            [M_200] = 0xFFB39DDB
            [M_300] = 0xFF9575CD
            [M_400] = 0xFF7E57C2
            [M_500] = 0xFF673AB7
            [M_600] = 0xFF5E35B1
            [M_700] = 0xFF512DA8
            [M_800] = 0xFF4527A0
            [M_900] = 0xFF311B92
            [M_A100] = 0xFFB388FF
            [M_A200] = 0xFF7C4DFF
            [M_A400] = 0xFF651FFF
            [M_A700] = 0xFF6200EA
        },
        [M_Indigo] = {
            [M_50] = 0xFFE8EAF6
            [M_100] = 0xFFC5CAE9
            [M_200] = 0xFF9FA8DA
            [M_300] = 0xFF7986CB
            [M_400] = 0xFF5C6BC0
            [M_500] = 0xFF3F51B5
            [M_600] = 0xFF3949AB
            [M_700] = 0xFF303F9F
            [M_800] = 0xFF283593
            [M_900] = 0xFF1A237E
            [M_A100] = 0xFF8C9EFF
            [M_A200] = 0xFF536DFE
            [M_A400] = 0xFF3D5AFE
            [M_A700] = 0xFF304FFE
        },
        [M_Blue] = {
            [M_50] = 0xFFE3F2FD
            [M_100] = 0xFFBBDEFB
            [M_200] = 0xFF90CAF9
            [M_300] = 0xFF64B5F6
            [M_400] = 0xFF42A5F5
            [M_500] = 0xFF2196F3
            [M_600] = 0xFF1E88E5
            [M_700] = 0xFF1976D2
            [M_800] = 0xFF1565C0
            [M_900] = 0xFF0D47A1
            [M_A100] = 0xFF82B1FF
            [M_A200] = 0xFF448AFF
            [M_A400] = 0xFF2979FF
            [M_A700] = 0xFF2962FF
        },
        [M_LightBlue] = {
            [M_50] = 0xFFE1F5FE
            [M_100] = 0xFFB3E5FC
            [M_200] = 0xFF81D4FA
            [M_300] = 0xFF4FC3F7
            [M_400] = 0xFF29B6F6
            [M_500] = 0xFF03A9F4
            [M_600] = 0xFF039BE5
            [M_700] = 0xFF0288D1
            [M_800] = 0xFF0277BD
            [M_900] = 0xFF01579B
            [M_A100] = 0xFF80D8FF
            [M_A200] = 0xFF40C4FF
            [M_A400] = 0xFF00B0FF
            [M_A700] = 0xFF0091EA
        },
        [M_Cyan] = {
            [M_50] = 0xFFE0F7FA
            [M_100] = 0xFFB2EBF2
            [M_200] = 0xFF80DEEA
            [M_300] = 0xFF4DD0E1
            [M_400] = 0xFF26C6DA
            [M_500] = 0xFF00BCD4
            [M_600] = 0xFF00ACC1
            [M_700] = 0xFF0097A7
            [M_800] = 0xFF00838F
            [M_900] = 0xFF006064
            [M_A100] = 0xFF84FFFF
            [M_A200] = 0xFF18FFFF
            [M_A400] = 0xFF00E5FF
            [M_A700] = 0xFF00B8D4
        },
        [M_Teal] = {
            [M_50] = 0xFFE0F2F1
            [M_100] = 0xFFB2DFDB
            [M_200] = 0xFF80CBC4
            [M_300] = 0xFF4DB6AC
            [M_400] = 0xFF26A69A
            [M_500] = 0xFF009688
            [M_600] = 0xFF00897B
            [M_700] = 0xFF00796B
            [M_800] = 0xFF00695C
            [M_900] = 0xFF004D40
            [M_A100] = 0xFFA7FFEB
            [M_A200] = 0xFF64FFDA
            [M_A400] = 0xFF1DE9B6
            [M_A700] = 0xFF00BFA5
        },
        [M_Green] = {
            [M_50] = 0xFFE8F5E9
            [M_100] = 0xFFC8E6C9
            [M_200] = 0xFFA5D6A7
            [M_300] = 0xFF81C784
            [M_400] = 0xFF66BB6A
            [M_500] = 0xFF4CAF50
            [M_600] = 0xFF43A047
            [M_700] = 0xFF388E3C
            [M_800] = 0xFF2E7D32
            [M_900] = 0xFF1B5E20
            [M_A100] = 0xFFB9F6CA
            [M_A200] = 0xFF69F0AE
            [M_A400] = 0xFF00E676
            [M_A700] = 0xFF00C853
        },
        [M_LightGreen] = {
            [M_50] = 0xFFF1F8E9
            [M_100] = 0xFFDCEDC8
            [M_200] = 0xFFC5E1A5
            [M_300] = 0xFFAED581
            [M_400] = 0xFF9CCC65
            [M_500] = 0xFF8BC34A
            [M_600] = 0xFF7CB342
            [M_700] = 0xFF689F38
            [M_800] = 0xFF558B2F
            [M_900] = 0xFF33691E
            [M_A100] = 0xFFCCFF90
            [M_A200] = 0xFFB2FF59
            [M_A400] = 0xFF76FF03
            [M_A700] = 0xFF64DD17
        },
        [M_Lime] = {
            [M_50] = 0xFFF9FBE7
            [M_100] = 0xFFF0F4C3
            [M_200] = 0xFFE6EE9C
            [M_300] = 0xFFDCE775
            [M_400] = 0xFFD4E157
            [M_500] = 0xFFCDDC39
            [M_600] = 0xFFC0CA33
            [M_700] = 0xFFAFB42B
            [M_800] = 0xFF9E9D24
            [M_900] = 0xFF827717
            [M_A100] = 0xFFF4FF81
            [M_A200] = 0xFFEEFF41
            [M_A400] = 0xFFC6FF00
            [M_A700] = 0xFFAEEA00
        },
        [M_Yellow] = {
            [M_50] = 0xFFFFFDE7
            [M_100] = 0xFFFFF9C4
            [M_200] = 0xFFFFF59D
            [M_300] = 0xFFFFF176
            [M_400] = 0xFFFFEE58
            [M_500] = 0xFFFFEB3B
            [M_600] = 0xFFFDD835
            [M_700] = 0xFFFBC02D
            [M_800] = 0xFFF9A825
            [M_900] = 0xFFF57F17
            [M_A100] = 0xFFFFFF8D
            [M_A200] = 0xFFFFFF00
            [M_A400] = 0xFFFFEA00
            [M_A700] = 0xFFFFD600
        },
        [M_Amber] = {
            [M_50] = 0xFFFFF8E1
            [M_100] = 0xFFFFECB3
            [M_200] = 0xFFFFE082
            [M_300] = 0xFFFFD54F
            [M_400] = 0xFFFFCA28
            [M_500] = 0xFFFFC107
            [M_600] = 0xFFFFB300
            [M_700] = 0xFFFFA000
            [M_800] = 0xFFFF8F00
            [M_900] = 0xFFFF6F00
            [M_A100] = 0xFFFFE57F
            [M_A200] = 0xFFFFD740
            [M_A400] = 0xFFFFC400
            [M_A700] = 0xFFFFAB00
        },
        [M_Orange] = {
            [M_50] = 0xFFFFF3E0
            [M_100] = 0xFFFFE0B2
            [M_200] = 0xFFFFCC80
            [M_300] = 0xFFFFB74D
            [M_400] = 0xFFFFA726
            [M_500] = 0xFFFF9800
            [M_600] = 0xFFFB8C00
            [M_700] = 0xFFF57C00
            [M_800] = 0xFFEF6C00
            [M_900] = 0xFFE65100
            [M_A100] = 0xFFFFD180
            [M_A200] = 0xFFFFAB40
            [M_A400] = 0xFFFF9100
            [M_A700] = 0xFFFF6D00
        },
        [M_DeepOrange] = {
            [M_50] = 0xFFFBE9E7
            [M_100] = 0xFFFFCCBC
            [M_200] = 0xFFFFAB91
            [M_300] = 0xFFFF8A65
            [M_400] = 0xFFFF7043
            [M_500] = 0xFFFF5722
            [M_600] = 0xFFF4511E
            [M_700] = 0xFFE64A19
            [M_800] = 0xFFD84315
            [M_900] = 0xFFBF360C
            [M_A100] = 0xFFFF9E80
            [M_A200] = 0xFFFF6E40
            [M_A400] = 0xFFFF3D00
            [M_A700] = 0xFFDD2C00
        },
        [M_Brown] = {
            [M_50] = 0xFFEFEBE9
            [M_100] = 0xFFD7CCC8
            [M_200] = 0xFFBCAAA4
            [M_300] = 0xFFA1887F
            [M_400] = 0xFF8D6E63
            [M_500] = 0xFF795548
            [M_600] = 0xFF6D4C41
            [M_700] = 0xFF5D4037
            [M_800] = 0xFF4E342E
            [M_900] = 0xFF3E2723
        },
        [M_Gray] = {
            [M_50] = 0xFFFAFAFA
            [M_100] = 0xFFF5F5F5
            [M_200] = 0xFFEEEEEE
            [M_300] = 0xFFE0E0E0
            [M_400] = 0xFFBDBDBD
            [M_500] = 0xFF9E9E9E
            [M_600] = 0xFF757575
            [M_700] = 0xFF616161
            [M_800] = 0xFF424242
            [M_900] = 0xFF212121
        },
        [M_BlueGray] = {
            [M_50] = 0xFFECEFF1
            [M_100] = 0xFFCFD8DC
            [M_200] = 0xFFB0BEC5
            [M_300] = 0xFF90A4AE
            [M_400] = 0xFF78909C
            [M_500] = 0xFF607D8B
            [M_600] = 0xFF546E7A
            [M_700] = 0xFF455A64
            [M_800] = 0xFF37474F
            [M_900] = 0xFF263238
        }
    };
};

/**
 * Returns the luminance of this color on a scale of 0.0 (dark) to
 * 1.0 (light).  Luminance is based on how light or dark a color
 * appears to the human eye.
 */
double luminance(const QColor &color);

/**
 * Returns a new color based on the foreground color,
 * such that the new color is lightened to achieve the desired
 * contrast ratio against the given background color.
 *
 * Note:
 *
 *   This method assumes that this foreground color is darker
 *   than the background color.  Passing in a background color
 *   that is darker than this color results in
 *   this original color being returned.
 */
static QColor lightenToMatchContrastRatio
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
        const QFont &previewTextFont,
        const QFont &previewCodeFont)
{
    QString styleSheet;
    QTextStream stream(&styleSheet);

    bool lightMode = (luminance(colors.background) > luminance(colors.foreground));

    m_styleSheetVariables["$body-font-family"] = sanitizeFontFamily(previewTextFont);
    m_styleSheetVariables["$code-font-family"] = sanitizeFontFamily(previewCodeFont);
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
        // Create a UI chrome color based on a lightened editor text color,
        // such that the new color achieves a lower contrast ratio.
        //
        chromeColor =
            lightenToMatchContrastRatio(
                colors.foreground,
                colors.background,
                2.0
            );

        // Slightly blend the new UI chrome color with the editor background
        // color to help it match better.
        //
        chromeColor = mix(chromeColor, colors.background, 86);

        // If the text selection background color is very dark, then make the
        // text selection foreground color light to ensure sufficient contrast.
        //
        if (luminance(colors.selection) < 0.5) {
            selectedFgColor = colors.background;
        }
    }
    // Else if the dark mode color scheme...
    else {
        chromeColor = colors.foreground.darker(120);

        // If the text selection background color is very light, then make sure
        // the text selection foreground color is dark to ensure sufficient
        // contrast.
        //
        if (luminance(colors.selection) >= 0.5) {
            selectedFgColor = colors.background;
        }
    }

    m_styleSheetVariables["$secondary-background-color"] = mix(chromeColor, colors.background, 5);
    m_styleSheetVariables["$fill-color"] = mix(chromeColor, colors.background, 20);
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

// Algorithm taken from *Grokking the GIMP* by Carey Bunks,
// section 5.3.
//
// Grokking the GIMP
// by Carey Bunks
// Copyright (c) 2000 by New Riders Publishing, www.newriders.com
// ISBN 0-7357-0924-6.
//
double luminance(const QColor &color)
{
    QColor c = color;

    // Ensure color is non-zero.
    if (c == QColor(Qt::black)) {
        c.setRgb(1, 1, 1);
    }

    return (0.30 * c.redF()) + (0.59 * c.greenF()) + (0.11 * c.blueF());
}

QColor lightenToMatchContrastRatio
(
    const QColor &foreground,
    const QColor &background,
    double contrastRatio
)
{
    double fgBrightness = luminance(foreground);
    double bgBrightness = luminance(background);

    // If the background color is brighter than the foreground color...
    if (bgBrightness > fgBrightness) {
        double actualContrastRatio = bgBrightness / fgBrightness;
        double colorFactor = contrastRatio / actualContrastRatio;

        QColor result = foreground;

        // Ensure color is non-zero.
        if (result == QColor(Qt::black)) {
            result.setRgb(1, 1, 1);
        }

        qreal r = result.redF() / colorFactor;
        qreal g = result.greenF() / colorFactor;
        qreal b = result.blueF() / colorFactor;

        if (r > 1.0) {
            r = 1.0;
        }

        if (g > 1.0) {
            g = 1.0;
        }

        if (b > 1.0) {
            b = 1.0;
        }

        result.setRgbF(r, g, b);

        return result;
    } else {
        // This algorithm cannot change the foreground color when it is
        // lighter than the background color, so return this color's
        // original value.
        //
        return foreground;
    }
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

static
std::pair<MaterialColor, MaterialShade>
nearestPaletteMatch(const QColor &color)
{
    if 
}

static std::pair<QColor, QColor> duoShades(const QColor &color, int count)
{

}

static std::tuple<QColor, QColor, QColor> trioShades(const QColor &color)
{

}

} // namespace ghostwriter
