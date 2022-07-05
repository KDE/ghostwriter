/***********************************************************************
 *
 * Copyright (C) 2014-2022 wereturtle
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

#ifndef MARKDOWN_HIGHLIGHTER_H
#define MARKDOWN_HIGHLIGHTER_H

#include <QSyntaxHighlighter>

#include "markdowndocument.h"
#include "markdowneditor.h"

namespace ghostwriter
{
/**
 * Highlighter for the Markdown text format.
 */
class MarkdownHighlighterPrivate;
class MarkdownHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(MarkdownHighlighter)

public:
    friend class MarkdownHighlighterPrivate;

    /**
     * Constructor.  Takes as a parameter the Markdown editor whose
     * MarkdownDocument is to be highlighted.
     */
    MarkdownHighlighter
    (
        MarkdownEditor *editor,
        const ColorScheme &colors
    );

    /**
     * Destructor.
     */
    ~MarkdownHighlighter();

    /**
     * Overridden method to highlight the given text for the current
     * text block in the document.
     */
    void highlightBlock(const QString &text);

    /**
     * Increases the font size by one point.
     */
    void increaseFontSize();

    /**
     * Decreases the font size by one point.
     */
    void decreaseFontSize();

    /**
     * Sets the color scheme.
     */
    void setColorScheme(const ColorScheme &colors);

    /**
     * Sets whether large heading sizes are enabled.
     */
    void setEnableLargeHeadingSizes(const bool enable);

    /**
     * Sets whether emphasized text is underlined instead of italicized.
     */
    void setUseUnderlineForEmphasis(const bool enable);

    /**
     * Sets whether blockquotes should be italicized.
     */
    void setItalicizeBlockquotes(const bool enable);

    /**
     * Sets the font family and point size.
     */
    void setFont(const QString &fontFamily, const double fontSize);

signals:
    /**
     * FOR INTERNAL USE ONLY
     *
     * This signal is used internally to restart highlighting on the
     * previous line, which is needed for setext headings.  Unfortunately,
     * QSyntaxHighlighter only goes forward in its highlighting, not
     * backwards.  Neither can rehighlightBlock() be called internally,
     * since recursive calls to the class will wipe its state data and will
     * cause the application to crash. This is a workaround to queue a
     * highlighting action for the prior text block in the event system,
     * so that recursion isn't used.
     */
    void highlightBlockAtPosition(int position);

private slots:
    /*
    * Highlights the text block at the given cursor position of the
    * document.  See explanation for highlightBlockAtPosition().
    */
    void onHighlightBlockAtPosition(int position);

private:
    QScopedPointer<MarkdownHighlighterPrivate> d_ptr;
};
} // namespace ghostwriter

#endif
