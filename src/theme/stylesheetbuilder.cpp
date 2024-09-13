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

#include "chromecolors.h"
#include "stylesheetbuilder.h"
#include "theme/svgicontheme.h"

namespace ghostwriter
{
/*
 * Returns the font family name of the given font with bracketed text removed.
 */
static QString sanitizeFontFamily(const QFont &font)
{
    static QRegularExpression bracketsExpr("\\[.*\\]");

    return font.family().remove(bracketsExpr).trimmed();
}

QString StyleSheetBuilder::m_statIndicatorArrowIconPath = QString();

StyleSheetBuilder::StyleSheetBuilder(const ChromeColors &colors,
                                     const SvgIconTheme *iconTheme,
                                     const bool roundedCorners,
                                     const QFont &editorFont,
                                     const QFont &previewTextFont,
                                     const QFont &previewCodeFont)
{
    QString styleSheet;
    QTextStream stream(&styleSheet);

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

    addColor(colors, "$background-color", ChromeColors::Background);
    addColor(colors, "$accent-color", ChromeColors::Accent);
    addColor(colors, "$accent-fill-color", ChromeColors::AccentFill);
    addColor(colors, "$label-color", ChromeColors::Label);
    addColor(colors, "$secondary-label-color", ChromeColors::SecondaryLabel);
    addColor(colors, "$text-color", ChromeColors::Text);
    addColor(colors, "$selected-text-fg-color", ChromeColors::SelectedTextFg);
    addColor(colors, "$selected-text-bg-color", ChromeColors::SelectedTextBg);
    addColor(colors, "$selection-fg-color", ChromeColors::SelectionFg);
    addColor(colors, "$placeholder-text-color", ChromeColors::PlaceholderText);
    addColor(colors, "$link-color", ChromeColors::Link);
    addColor(colors, "$heading-color", ChromeColors::Heading);
    addColor(colors, "$code-color", ChromeColors::Code);
    addColor(colors, "$block-quote-color", ChromeColors::BlockQuote);
    addColor(colors, "$separator-color", ChromeColors::Separator);
    addColor(colors, "$secondary-separator-color", ChromeColors::SecondarySeparator);
    addColor(colors, "$grid-color", ChromeColors::Grid);
    addColor(colors, "$info-color", ChromeColors::Info);
    addColor(colors, "$info-fill-color", ChromeColors::InfoFill);
    addColor(colors, "$error-color", ChromeColors::Error);
    addColor(colors, "$error-fill-color", ChromeColors::ErrorFill);
    addColor(colors, "$warning-color", ChromeColors::Warning);
    addColor(colors, "$warning-fill-color", ChromeColors::WarningFill);
    addColor(colors, "$success-color", ChromeColors::Success);
    addColor(colors, "$success-fill-color", ChromeColors::SuccessFill);
    addColor(colors, "$fill-color", ChromeColors::Fill);
    addColor(colors, "$secondary-fill-color", ChromeColors::SecondaryFill);
    addColor(colors, "$tertiary-fill-color", ChromeColors::TertiaryFill);
    addColor(colors, "$secondary-background-color", ChromeColors::SecondaryBackground);

    // Remove previous cache/temporary files.
    clearCache();

    // Refresh statistics indicator drop-down arrow icon.
    QIcon statIndicatorIcon = iconTheme->icon("status-bar-menu-arrow");

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

void StyleSheetBuilder::addColor(const ChromeColors &colors, const QString &variableName, ChromeColors::ColorElem elem)
{
    m_styleSheetVariables[variableName] = colors.color(elem);
    m_styleSheetVariables[variableName + "-pressed"] = colors.color(elem, ChromeColors::PressedState);
    m_styleSheetVariables[variableName + "-active"] = colors.color(elem, ChromeColors::ActiveState);
    m_styleSheetVariables[variableName + "-hover"] = colors.color(elem, ChromeColors::HoverState);
    m_styleSheetVariables[variableName + "-disabled"] = colors.color(elem, ChromeColors::DisabledState);

    if (variableName.endsWith("fill-color")) {
        m_styleSheetVariables[variableName + "-disabled"] = colors.color(elem).lighter(105);
    } else {
        m_styleSheetVariables[variableName + "-disabled"] = colors.color(elem).lighter(125);
    }
}

} // namespace ghostwriter
