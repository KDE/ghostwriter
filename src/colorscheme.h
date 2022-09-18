/*
 * SPDX-FileCopyrightText: 2020-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef COLORSCHEME_H
#define COLORSCHEME_H

#include <QColor>

namespace ghostwriter
{
typedef struct ColorScheme {
    QColor foreground;
    QColor background;
    QColor selection;
    QColor cursor;
    QColor link;
    QColor image;
    QColor inlineHtml;
    QColor headingText;
    QColor headingMarkup;
    QColor emphasisText;
    QColor emphasisMarkup;
    QColor blockquoteText;
    QColor blockquoteMarkup;
    QColor divider;
    QColor listMarkup;
    QColor codeText;
    QColor codeMarkup;
    QColor error;
} ColorScheme;
} // namespace ghostwriter

#endif // COLORSCHEME_H
