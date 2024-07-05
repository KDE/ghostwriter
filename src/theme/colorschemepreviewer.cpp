/*
 * SPDX-FileCopyrightText: 2020-2023 Megan Conkle <megan.conkle@kdemail.net>
 * SPDX-FileCopyrightText: 2009-2014 Graeme Gott <graeme@gottcode.org>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <QCommonStyle>
#include <QDebug>
#include <QPainter>
#include <QPixmap>
#include <QStaticText>
#include <QTextEdit>

#include "colorschemepreviewer.h"

namespace ghostwriter
{

class ColorSchemePreviewerPrivate
{
public:
    ColorSchemePreviewerPrivate()
    {
        ;
    }

    ~ColorSchemePreviewerPrivate()
    {
        ;
    }

    QIcon thumbnailPreviewIcon;
    static const QString loremIpsum;
};

const QString ColorSchemePreviewerPrivate::loremIpsum =
    "<p><h2><strong>"
    "<font color='@headingMarkup'># </font>"
    "<font color='@headingText'>Lorem ipsum</font></strong></h2></p>"
    "<p>Lorem ipsum "
    "<em>"
    "<font color='@emphasisMarkup'>*</font>"
    "<font color='@emphasisText'>dolor</font>"
    "<font color='@emphasisMarkup'>*</font>"
    "</em> "
    "sit amet, "
    "<strong>"
    "<font color='@emphasisMarkup'>**</font>"
    "<font color='@emphasisText'>consectetur</font>"
    "<font color='@emphasisMarkup'>**</font>"
    "</strong> "
    "adipiscing "
    "<font color='@link'>[elit]</font>. "
    "Etiam "
    "<font color='@codeMarkup'>`</font>"
    "<font color='@codeText'>aliquam</font>"
    "<font color='@codeMarkup'>`</font>, "
    "diam.</p>";

ColorSchemePreviewer::ColorSchemePreviewer
(
    const ColorScheme &colors,
    bool builtIn,
    bool valid,
    int width,
    int height,
    qreal dpr
) : d_ptr(new ColorSchemePreviewerPrivate())
{
    Q_D(ColorSchemePreviewer);
    
    QString text = d->loremIpsum;

    text.replace("@headingMarkup", colors.headingMarkup.name());
    text.replace("@headingText", colors.headingText.name());
    text.replace("@emphasisMarkup", colors.emphasisMarkup.name());
    text.replace("@emphasisText", colors.emphasisText.name());
    text.replace("@link", colors.link.name());
    text.replace("@codeMarkup", colors.codeMarkup.name());
    text.replace("@codeText", colors.codeText.name());

    QTextEdit textEdit;
    textEdit.setHtml(text);
    textEdit.setFrameStyle(QFrame::NoFrame);
    textEdit.setAutoFillBackground(false);
    textEdit.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    textEdit.setFixedSize(width, height);
    textEdit.setStyle(new QCommonStyle());
    textEdit.setStyleSheet
    (
        "QTextEdit { color: "
        + colors.foreground.name()
        + "; background-color: "
        + colors.background.name()
        + "; font-family: Roboto Mono, monospace; font-size: 20px }"
    );

    QPixmap thumbnailPixmap(width * dpr, height * dpr);
    thumbnailPixmap.setDevicePixelRatio(dpr);
    QPainter painter(&thumbnailPixmap);
    painter.fillRect(thumbnailPixmap.rect(), colors.background);
    painter.setRenderHints(QPainter::Antialiasing);
    textEdit.render(&painter);

    if (!valid || builtIn) {
        // QString symbol = QChar(fa::lock);
        QColor color = colors.foreground;

        if (!valid) {
            // symbol = QChar(fa::exclamationtriangle);
            color = colors.error;
        }

        // QFont font(d->awesome->font(style::stfas, 16));
        // font.setPixelSize(22);

        // QFontMetricsF metrics(font);
        // int x = width - metrics.boundingRect(symbol).width() - 2;
        // int y = 2;

        // painter.setFont(font);
        // painter.setBrush(Qt::NoBrush);
        // painter.setPen(color);
        // painter.drawStaticText(x, y, QStaticText(symbol));
    }

    painter.end();
    d->thumbnailPreviewIcon = thumbnailPixmap;
}

ColorSchemePreviewer::~ColorSchemePreviewer()
{
    ;
}

QIcon ColorSchemePreviewer::icon()
{
    Q_D(ColorSchemePreviewer);
    
    return d->thumbnailPreviewIcon;
}
} // namespace ghostwriter
