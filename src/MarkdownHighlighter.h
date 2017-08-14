/***********************************************************************
 *
 * Copyright (C) 2014-2017 wereturtle
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

#include "spelling/dictionary_ref.h"
#include "MarkdownTokenizer.h"
#include "MarkdownStyles.h"
#include "Token.h"
#include "TextDocument.h"
#include "MarkdownEditor.h"

class QColor;
class QRegExp;
class QString;
class QTextCharFormat;
class MarkdownEditor;

/**
 * Highlighter for the Markdown text format.
 */
class MarkdownHighlighter : public QSyntaxHighlighter
{
	Q_OBJECT

	public:
        /**
         * Constructor.  Takes as a parameter the Markdown editor whose
         * TextDocument is to be highlighted.
         */
        MarkdownHighlighter(MarkdownEditor* editor);

        /**
         * Destructor.
         */
        ~MarkdownHighlighter();

        /**
         * Overridden method to highlight the given text for the current
         * text block in the document.
         */
		void highlightBlock(const QString& text);

        /**
         * Sets the dictionary to use for live spell checking.
         */
        void setDictionary(const DictionaryRef& dictionary);

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
        void setColorScheme
        (
            const QColor& defaultTextColor,
            const QColor& backgroundColor,
            const QColor& markupColor,
            const QColor& linkColor,
            const QColor& headingColor,
            const QColor& emphasisColor,
            const QColor& blockquoteColor,
            const QColor& codeColor,
            const QColor& spellingErrorColor
        );

        /**
         * Sets whether large heading sizes are enabled.
         */
        void setEnableLargeHeadingSizes(const bool enable);

        /**
         * Sets whether emphasized text is underlined instead of italicized.
         */
        void setUseUnderlineForEmphasis(const bool enable);

        /**
         * Sets the font family and point size.
         */
		void setFont(const QString& fontFamily, const double fontSize);

        /**
         * Sets whether live spell checking is enabled.
         */
        void setSpellCheckEnabled(const bool enabled);

        /**
         * Sets the blockquote style.
         */
        void setBlockquoteStyle(const BlockquoteStyle style);

        /**
         * Sets whether manual linebreaks (2 spaces at the end of a line) will be highlighted
         */
        void setHighlightLineBreaks(bool enable);

    signals:
        /**
         * Notifies listeners that a heading was found in the document in the
         * given QTextBlock and with the given level and heading text.
         */
        void headingFound(int level, const QString& text, QTextBlock block);

        /**
         * Notifies listeners that a heading was discovered to have been removed
         * from the document at the given cursor position.
         */
        void headingRemoved(int position);

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

    public slots:
        /**
         * Signalled by a text editor when the user has resumed typing.
         * This signal is used to ensure spell checking is not performed
         * for the current word the cursor is on while the user is still
         * typing.
         */
        void onTypingResumed();

        /**
         * Signalled by a text editor when the user has ceased typing.
         * This signal is used to ensure spell checking is performed
         * for the current word the cursor is on after the user has
         * stopped typing.
         */
        void onTypingPaused();

    private slots:
        /*
         * Highlights the text block at the given cursor position of the
         * document.  See explanation for highlightBlockAtPosition().
         */
        void onHighlightBlockAtPosition(int position);

        /*
         * Determines if the text block being removed is a heading, and if so,
         * emits the headingRemoved() signal.
         */
        void onTextBlockRemoved(const QTextBlock& block);

    private:
        MarkdownEditor* editor;
        MarkdownTokenizer* tokenizer;
        DictionaryRef dictionary;
        bool spellCheckEnabled;
        bool typingPaused;
        bool useUndlerlineForEmphasis;
        bool highlightLineBreaks;
        bool inBlockquote;
        BlockquoteStyle blockquoteStyle;
        QColor defaultTextColor;
        QColor backgroundColor;
        QColor markupColor;
        QColor linkColor;
        QColor headingColor;
        QColor emphasisColor;
        QColor blockquoteColor;
        QColor codeColor;
        QColor spellingErrorColor;

		QTextCharFormat defaultFormat;
        bool applyStyleToMarkup[TokenLast];
        QColor colorForToken[TokenLast];
        bool emphasizeToken[TokenLast];
        bool strongToken[TokenLast];
        bool strongMarkup[TokenLast];
        bool strikethroughToken[TokenLast];
        int fontSizeIncrease[TokenLast];

        void spellCheck(const QString& text);
        void setupTokenColors();
        void setupHeadingFontSize(bool useLargeHeadings);

        void applyFormattingForToken(const Token& token);
        void storeHeadingData(const Token& token, const QString& text);

        /*
         * Returns true if the given QTextBlock userState indicates that the
         * text block contains a heading.
         */
        bool isHeadingBlockState(int state) const;

};

#endif
