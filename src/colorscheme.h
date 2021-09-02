/***********************************************************************
 *
 * Copyright (C) 2020 wereturtle
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
