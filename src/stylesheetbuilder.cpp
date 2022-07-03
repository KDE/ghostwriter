/***********************************************************************
 *
 * Copyright (C) 2020-2021 wereturtle
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

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QPalette>
#include <QRegularExpression>
#include <QTextStream>
#include <QDebug>
#include <QTemporaryFile>

#include "3rdparty/QtAwesome/QtAwesome.h"
#include "stylesheetbuilder.h"


namespace ghostwriter
{

QString StyleSheetBuilder::m_statIndicatorArrowIconPath = QString();

QString StyleSheetBuilder::m_htmlPreviewSass;

StyleSheetBuilder::StyleSheetBuilder(const ColorScheme &colors,
        const bool roundedCorners,
        const QFont& previewTextFont,
        const QFont& previewCodeFont)
{
    QString styleSheet;
    QTextStream stream(&styleSheet);

    this->m_htmlPreviewTextFont = previewTextFont;
    this->m_htmlPreviewCodeFont = previewCodeFont;

    this->m_backgroundColor = colors.background;
    this->m_foregroundColor = colors.foreground;
    this->m_accentColor = colors.link;
    this->m_selectedBgColor = colors.selection;

    // If the background color is brighter than the foreground color...
    if (luminance(this->m_backgroundColor) > luminance(this->m_foregroundColor)) {
        // Create a UI chrome color based on a lightened editor text color,
        // such that the new color achieves a lower contrast ratio.
        //
        this->m_interfaceTextColor = lightenToMatchContrastRatio
                                     (
                                         this->m_foregroundColor,
                                         colors.background,
                                         2.0
                                     );

        // Slightly blend the new UI chrome color with the editor background color
        // to help it match the colors better.
        //
        this->m_interfaceTextColor = applyAlpha(this->m_interfaceTextColor, colors.background, 220);

        m_selectedFgColor = this->m_foregroundColor;

        if (luminance(this->m_selectedBgColor) < 0.5) {
            m_selectedFgColor = this->m_backgroundColor;
        }
    }
    // Else if the foreground color is brighter than the background color...
    else {
        this->m_interfaceTextColor = this->m_foregroundColor.darker(120);
        m_selectedFgColor = this->m_foregroundColor;

        if (luminance(this->m_selectedBgColor) >= 0.5) {
            m_selectedFgColor = this->m_backgroundColor;
        }
    }

    this->m_pressedColor = applyAlpha(this->m_interfaceTextColor, colors.background, 30);
    this->m_hoverColor = this->m_pressedColor;

    this->m_faintColor = applyAlpha(colors.foreground, colors.background, 30);
    
    this->m_headingColor = colors.headingText.name();
    this->m_codeColor = colors.codeText.name();
    this->m_linkColor = colors.link.name();
    this->m_blockquoteColor = colors.blockquoteText.name();
    this->m_thickBorderColor = colors.emphasisMarkup.name();

    // Remove previous cache/temporary files.
    clearCache();

    // Refresh statistics indicator drop-down arrow icon.
    this->m_awesome = new QtAwesome();
    this->m_awesome->initFontAwesome();
    QVariantMap options;
    options.insert("color", this->m_interfaceTextColor);
    QIcon statIndicatorIcon = this->m_awesome->icon(style::stfas, fa::chevroncircleup, options);

    QTemporaryFile tempIconFile(QDir::tempPath() + "/XXXXXX.png");
    tempIconFile.setAutoRemove(false);

    if (tempIconFile.open()) {
        m_statIndicatorArrowIconPath = tempIconFile.fileName();
        statIndicatorIcon.pixmap(16, 16).save(&tempIconFile, "PNG");
        tempIconFile.close();
    }

    // Create the style sheets.
    buildScrollBarStyleSheet(roundedCorners);
    buildEditorStyleSheet();
    buildSplitterStyleSheet();
    buildStatusBarStyleSheet();
    buildStatusBarWidgetStyleSheet();
    buildFindReplaceStyleSheet();
    buildLayoutStyleSheet();
    buildSidebarStyleSheet();
    buildSidebarWidgetStyleSheet();
    buildStatusLabelStyleSheet();
    buildHtmlPreviewCss(roundedCorners);
}


StyleSheetBuilder::~StyleSheetBuilder()
{
    ;
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

QString StyleSheetBuilder::layoutStyleSheet()
{
    return m_layoutStyleSheet;
}

QString StyleSheetBuilder::splitterStyleSheet()
{
    return m_splitterStyleSheet;
}

QString StyleSheetBuilder::editorStyleSheet()
{
    return m_editorStyleSheet;
}

QString StyleSheetBuilder::statusBarStyleSheet()
{
    return m_statusBarStyleSheet;
}

QString StyleSheetBuilder::statusBarWidgetsStyleSheet()
{
    return m_statusBarWidgetStyleSheet;
}

QString StyleSheetBuilder::statusLabelStyleSheet()
{
    return m_statusLabelStyleSheet;
}

QString StyleSheetBuilder::findReplaceStyleSheet() 
{
    return m_findReplaceStyleSheet;
}

QString StyleSheetBuilder::sidebarStyleSheet()
{
    return m_sidebarStyleSheet;
}

QString StyleSheetBuilder::sidebarWidgetStyleSheet()
{
    return m_sidebarWidgetStyleSheet;
}

QString StyleSheetBuilder::htmlPreviewCss() 
{
    return m_htmlPreviewCss;
}

QColor StyleSheetBuilder::interfaceTextColor() 
{
    return m_interfaceTextColor;
}

QColor StyleSheetBuilder::faintColor() 
{
    return m_faintColor;
}

void StyleSheetBuilder::buildScrollBarStyleSheet(const bool roundedCorners)
{
    m_scrollBarStyleSheet = "";

    QTextStream stream(&m_scrollBarStyleSheet);
    QString scrollBarRadius = "0px";
    QString scrollAreaPadding = "3px 3px 0px 3px";
    QColor baseScrollColor = this->m_foregroundColor;
    baseScrollColor.setAlpha(50);

    if (roundedCorners) {
        scrollBarRadius = "3px";
    }

    stream
            << "QAbstractScrollArea::corner { background: transparent } "
            << "QAbstractScrollArea { padding: "
            << scrollAreaPadding
            << "; margin: 0 } "
            << "QScrollBar::horizontal { border: 0; background: transparent; height: 16px; margin: 5px } "
            << "QScrollBar::handle:horizontal { border: 0; background: "
            << baseScrollColor.name(QColor::HexArgb)
            << "; min-width: 50px; border-radius: "
            << scrollBarRadius
            << "; } "
            << "QScrollBar::vertical { border: 0; background: transparent; width: 16px; margin: 5px } "
            << "QScrollBar::vertical:hover { background: "
            << baseScrollColor.name(QColor::HexArgb)
            << "; border-radius: "
            << scrollBarRadius
            << " } "
            << "QScrollBar::horizontal:hover { background: "
            << baseScrollColor.name(QColor::HexArgb)
            << "; border-radius: "
            << scrollBarRadius
            << " } "
            << "QScrollBar::handle:vertical { border: 0; background: "
            << baseScrollColor.name(QColor::HexArgb)
            << "; min-height: 50px; border-radius: "
            << scrollBarRadius
            << "; } "
            << "QScrollBar::handle:vertical:hover { background: "
            << this->m_accentColor.name()
            << " } "
            << "QScrollBar::handle:horizontal:hover { background: "
            << this->m_accentColor.name()
            << " } "
            << "QScrollBar::add-line { background: transparent; border: 0 } "
            << "QScrollBar::sub-line { background: transparent; border: 0 } "
            ;
}

void StyleSheetBuilder::buildLayoutStyleSheet()
{
    QString styleSheet = "";
    QTextStream stream(&styleSheet);

    stream
            << "#editorLayoutArea { background-color: "
            << this->m_backgroundColor.name()
            << "; margin: 0; border: 0 }"
            ;

    this->m_layoutStyleSheet = styleSheet;
}

void StyleSheetBuilder::buildSplitterStyleSheet()
{
    QString styleSheet = "";
    QTextStream stream(&this->m_splitterStyleSheet);

    stream
            << "QSplitter::handle { border: 0; padding: 0; margin: 0; background-color: "
            << this->m_faintColor.name()
            << " } "
            << "QSplitter::handle:vertical { height: 1px } "
            << "QSplitter::handle:horizontal { width: 1px } "
            ;
}

void StyleSheetBuilder::buildEditorStyleSheet()
{
    m_editorStyleSheet = "";

    QTextStream stream(&m_editorStyleSheet);

    stream
            << "QPlainTextEdit { border: 0; "
            << "margin: 0; padding: 5px; background-color: "
            << this->m_backgroundColor.name()
            << "; color: "
            << this->m_foregroundColor.name()
            << "; selection-color: "
            << this->m_selectedFgColor.name()
            << "; selection-background-color: "
            << this->m_selectedBgColor.name()
            << " } "
            << m_scrollBarStyleSheet
            ;
}

void StyleSheetBuilder::buildStatusBarStyleSheet()
{
    m_statusBarStyleSheet = "";

    QTextStream stream(&m_statusBarStyleSheet);

    stream
            << "QStatusBar { margin: 0; padding: 0; border-top: 1px solid "
            << this->m_faintColor.name()
            << "; border-left: 0; border-right: 0; border-bottom: 0; background: "
            << this->m_backgroundColor.name()
            << "; color: "
            << this->m_interfaceTextColor.name()
            << " } "
            ;
}

void StyleSheetBuilder::buildStatusBarWidgetStyleSheet()
{
    m_statusBarWidgetStyleSheet = "";

    QTextStream stream(&m_statusBarWidgetStyleSheet);

    // Set the label and button font size to a fixed point size,
    // since on Windows using the default QLabel and QPushButton
    // font sizes results in tiny font sizes for these.  We need
    // them to stand out a little bit more than a typical label
    // or button.
    //
    int fontSize = 11;

    stream
            << "QLabel { font-size: "
            << fontSize
            << "pt; margin: 0px; padding: 5px; border: 0; background: transparent; color: "
            << this->m_interfaceTextColor.name()
            << " } "
            << "QPushButton { padding: 5 5 5 5; margin: 0; border: 0; border-radius: 5px; "
            << "color: "
            << this->m_interfaceTextColor.name()
            << "; background-color: "
            << this->m_backgroundColor.name()
            << "; font-size: 16px; width: 32px } "
            << "QPushButton:pressed, QPushButton:flat, QPushButton:checked, QPushButton:hover { padding: 5 5 5 5; margin: 0; color: "
            << this->m_interfaceTextColor.name()
            << "; background-color: "
            << this->m_pressedColor.name()
            << " } "
            << "QPushButton#showSidebarButton:hover { color: "
            << this->m_foregroundColor.name()
            << "; background-color: transparent }"
            << "QComboBox {"
            << "    height: 22px;"
            << "    border: 0px;"
            << "    margin: 0;"
            << "    padding: 0;"
            << "    color: " << this->m_interfaceTextColor.name() << "; "
            << "    background-color: " << this->m_backgroundColor.name() << "; "
            << "} "
            << "QComboBox:hover {"
            << "    border-bottom: 2px solid " << this->m_accentColor.name() << ";"
            << "} "
            << "QListView {"
            << "    padding: 0px; "
            << "    margin: 0px; "
            << "    color: " << this->m_foregroundColor.name() << ";"
            << "    background-color: " << this->m_backgroundColor.name() << "; "
            << "} "
            << "QListView::item { background-color: transparent; } "
            << "QListView::item:selected {"
            << "    background-color: " << this->m_selectedBgColor.name() << ";"
            << "    color: " << this->m_selectedFgColor.name() << ";"
            << " } "
            << "QComboBox::drop-down {"
            << "    border: 0px;"
            << "    margin: 0;"
            << "    padding: 0;"
            << "    height: 20px; "
            << "    width: 20px; "
            << "} "
            << "QComboBox::down-arrow { "
            << "    border: 0px;"
            << "    margin: 0;"
            << "    padding: 0px;"
            << "    height: 14px; "
            << "    width: 14px; "
            << "    image: url(" << this->m_statIndicatorArrowIconPath << ");"
            << "} "
            << "QComboBox::drop-down:hover { "
            << "    border-radius: 10px;"
            << "    background-color: " << this->m_pressedColor.name() << "; "
            << "} "
            ;
}

void StyleSheetBuilder::buildStatusLabelStyleSheet()
{
    m_statusLabelStyleSheet = "";

    QTextStream stream(&m_statusLabelStyleSheet);

    stream
            << "color: "
            << this->m_interfaceTextColor.name()
            << "; background-color: "
            << this->m_hoverColor.name()
            << "; border-radius: 5px; padding: 3px"
            ;
}

void StyleSheetBuilder::buildFindReplaceStyleSheet() 
{
    m_findReplaceStyleSheet = "";

    QTextStream stream(&m_findReplaceStyleSheet);

    int fontSize = 11;

    stream
        << "QLabel { font-size: "
        << fontSize
        << "pt; margin: 0px; padding: 5px; border: 0; background: transparent; color: "
        << this->m_interfaceTextColor.name()
        << " } "
        << "QPushButton { font-size: 16px; padding: 5 5 5 5; margin: 0; border: 0; border-radius: 5px; color: "
        << this->m_interfaceTextColor.name()
        << "; background-color: "
        << this->m_pressedColor.name()
        << "; min-width: 16px } "
        << "QPushButton[checkable=\"true\"] { font-size: 16px; background: transparent; "
        << "; min-width: 32px; height: 16px } "
        << "QPushButton:pressed, QPushButton:checked, QPushButton:hover { background-color: "
        << this->m_pressedColor.name()
        << " } "
        << "QPushButton:flat { background: transparent; } "
        << "QPushButton:hover { color: "
        << this->m_foregroundColor.name()
        << " } "
        << "QPushButton#findReplaceCloseButton { font-size: 12px } "
        << "QLineEdit { color: "
        << this->m_foregroundColor.name()
        << "; background-color: "
        << this->m_pressedColor.name()
        << "; border: 1px solid "
        << this->m_pressedColor.name()
        << "; border-radius: 3px "
        << "; selection-color: "
        << this->m_selectedFgColor.name()
        << "; selection-background-color: "
        << this->m_selectedBgColor.name()
        << " } "
        ;
}

void StyleSheetBuilder::buildSidebarStyleSheet()
{
    m_sidebarStyleSheet = "";

    QTextStream stream(&m_sidebarStyleSheet);

    stream << "#sidebar { border: 0; margin: 0; padding: 0; background-color: "
           << this->m_backgroundColor.name()
           << " } "
           << "QStackedWidget { border: 0; padding: 1; margin: 0; "
           << "; background-color: "
           << this->m_backgroundColor.name()
           << "; border-width: 0px; } "
           << "QPushButton[checkable=\"true\"] { icon-size: 22px; min-width: 40px; max-width: 40px; height: 40px; outline: none; margin: 0; padding: 0; border: 0; background-color: "
           << this->m_backgroundColor.name()
           << "; color: "
           << this->m_interfaceTextColor.name()
           << "; border-width: 0px; border-left-width: 3px; border-style: solid; border-color: "
           << this->m_backgroundColor.name()
           << " } QPushButton:checked { border-color: "
           << this->m_accentColor.name()
           << "; color: "
           << this->m_foregroundColor.name()
           << "; background-color: "
           << this->m_pressedColor.name()
           << " } QPushButton:checked:hover { border-color: "
           << this->m_accentColor.name()
           << "; color: "
           << this->m_foregroundColor.name()
           << "; background-color: "
           << this->m_pressedColor.name()
           << " } "
           << "QPushButton[checkable=\"false\"] { icon-size: 22px; padding: 0; margin: 0; border: 0; border-radius: 5px; background-color: "
           << this->m_backgroundColor.name()
           << "; color: "
           << this->m_interfaceTextColor.name()
           << "; width: 40px; height: 40px } "
           << "QPushButton:hover { color: "
           << this->m_foregroundColor.name()
           << "; background-color: transparent }"
           << "  QMenu { color: "
           << this->m_foregroundColor.name()
           << "; background-color: "
           << this->m_backgroundColor.name()
           << " } "
           << "QMenu::item { background-color: transparent; } "
           << "QMenu::item:selected { background-color: "
           << this->m_selectedBgColor.name()
           << "; color: "
           << this->m_selectedFgColor.name()
           << " } "
           ;
}

void StyleSheetBuilder::buildSidebarWidgetStyleSheet()
{
    m_sidebarWidgetStyleSheet = "";

    QTextStream stream(&m_sidebarWidgetStyleSheet);
    int sidebarFontSize = 11;

    // Important!  For QListWidget (used in sidebar), set
    // QListWidget { outline: none } for the style sheet to get rid of the
    // focus rectangle without losing keyboard focus capability.
    // Unfortunately, this property isn't in the Qt documentation, so
    // it's being documented here for posterity's sake.
    //

    stream
            << "QListWidget { outline: none; border: 0; padding: 1; background-color: "
            << this->m_backgroundColor.name()
            << "; color: "
            << this->m_foregroundColor.name()
            << "; font-size: "
            << sidebarFontSize
            << "pt; font-weight: normal } QListWidget::item { border: 0; padding: 1 0 1 0; margin: 0; background-color: "
            << this->m_backgroundColor.name()
            << "; color: "
            << this->m_foregroundColor.name()
            << "; font-weight: normal } "
            << "QListWidget::item:selected { border-radius: 0px; color: "
            << this->m_selectedFgColor.name()
            << "; background-color: "
            << this->m_selectedBgColor.name()
            << " } "
            << "QLabel { border: 0; padding: 0; margin: 0; background-color: transparent; "
            << "font-size: "
            << sidebarFontSize
            << "pt; color: "
            << this->m_foregroundColor.name()
            << " } "
            << m_scrollBarStyleSheet
            ;
}

void StyleSheetBuilder::buildHtmlPreviewCss(const bool roundedCorners) 
{
    if (m_htmlPreviewSass.isNull() || m_htmlPreviewSass.isEmpty())
    {
        QFile cssFile(":/resources/preview.css");

        if (!cssFile.open(QIODevice::ReadOnly)) {
            cssFile.close();
            qWarning() << "Failed to load built-in HTML preview style sheet.";
            return;
        }

        QTextStream inStream(&cssFile);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        inStream.setCodec("UTF-8");
#else
        inStream.setEncoding(QStringConverter::Utf8);
#endif

        inStream.setAutoDetectUnicode(true);
        m_htmlPreviewSass = inStream.readAll();
        cssFile.close();
    }

    QColor baseScrollColor = this->m_foregroundColor;
    baseScrollColor.setAlpha(50);

    QString scrollBarBorderRadius = "0px";

    if (roundedCorners) {
        scrollBarBorderRadius = "3px";
    }

    m_htmlPreviewCss = m_htmlPreviewSass;    
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$textColor", m_foregroundColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$backgroundColor", m_backgroundColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$textFont", m_htmlPreviewTextFont.family().remove(QRegularExpression("\\[.*\\]")).trimmed());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$fontSize", QString("%1pt").arg(m_htmlPreviewTextFont.pointSize()));
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$headingColor", m_headingColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$faintColor", m_faintColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$blockBackground", m_faintColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$codeColor", m_codeColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$linkColor", m_linkColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$blockquoteColor", m_blockquoteColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$thickBorderColor", m_thickBorderColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$scrollBarThumbColor", 
        QString("rgba(%1, %2, %3, %4)")
        .arg(baseScrollColor.red()).arg(baseScrollColor.green()).arg(baseScrollColor.blue())
        .arg(baseScrollColor.alphaF()));
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$scrollBarThumbHoverColor", m_accentColor.name());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$scrollBarTrackColor",
        QString("rgba(%1, %2, %3, %4)")
        .arg(baseScrollColor.red()).arg(baseScrollColor.green()).arg(baseScrollColor.blue())
        .arg(baseScrollColor.alphaF()));
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$scrollBarBorderRadius", scrollBarBorderRadius);
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$monospaceFont", m_htmlPreviewCodeFont.family().remove(QRegularExpression("\\[.*\\]")).trimmed());
    m_htmlPreviewCss = m_htmlPreviewCss.replace("$codeFontSize", QString("%1pt").arg(m_htmlPreviewCodeFont.pointSize()));
}

// Algorithm taken from *Grokking the GIMP* by Carey Bunks,
// section 5.3.
//
// Grokking the GIMP
// by Carey Bunks
// Copyright (c) 2000 by New Riders Publishing, www.newriders.com
// ISBN 0-7357-0924-6.
//
double StyleSheetBuilder::luminance(const QColor &color) const
{
    QColor c = color;

    // Ensure color is non-zero.
    if (c == QColor(Qt::black)) {
        c.setRgb(1, 1, 1);
    }

    return (0.30 * c.redF()) + (0.59 * c.greenF()) + (0.11 * c.blueF());
}

QColor StyleSheetBuilder::lightenToMatchContrastRatio
(
    const QColor &foreground,
    const QColor &background,
    double contrastRatio
) const
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

QColor StyleSheetBuilder::applyAlpha
(
    const QColor &foreground,
    const QColor &background,
    int alpha
)
{
    if ((alpha < 0) || alpha > 255) {
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
} // namespace ghostwriter
