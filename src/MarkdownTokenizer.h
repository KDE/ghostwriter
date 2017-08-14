/***********************************************************************
 *
 * Copyright (C) 2014, 2015 wereturtle
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


#ifndef MARKDOWNTOKENIZER_H
#define MARKDOWNTOKENIZER_H

#include "HighlightTokenizer.h"

class QRegExp;
class QString;

enum MarkdownTokenType
{
    TokenUnknown = -1,
    TokenAtxHeading1,
    TokenAtxHeading2,
    TokenAtxHeading3,
    TokenAtxHeading4,
    TokenAtxHeading5,
    TokenAtxHeading6,
    TokenSetextHeading1Line1,
    TokenSetextHeading1Line2,
    TokenSetextHeading2Line1,
    TokenSetextHeading2Line2,
    TokenEmphasis,
    TokenStrong,
    TokenStrikethrough,
    TokenVerbatim,
    TokenHtmlTag,
    TokenHtmlEntity,
    TokenAutomaticLink,
    TokenInlineLink,
    TokenReferenceLink,
    TokenReferenceDefinition,
    TokenImage,
    TokenHtmlComment,
    TokenNumberedList,
    TokenBulletPointList,
    TokenTaskListChecked,
    TokenTaskListUnchecked,
    TokenHorizontalRule,
    TokenLineBreak,
    TokenBlockquote,
    TokenCodeBlock,
    TokenGithubCodeFence,
    TokenPandocCodeFence,
    TokenCodeFenceEnd,
    TokenMention,
    TokenTableHeader,
    TokenTableDivider,
    TokenTablePipe,
    TokenLast
};

/**
 * Tokenizes one line of Markdown text at a time.  See documentation for
 * HighlightTokenizer class for details.
 */
class MarkdownTokenizer : public HighlightTokenizer
{
    public:
        MarkdownTokenizer();
        ~MarkdownTokenizer();

        void tokenize
        (
            const QString& text,
            int currentState,
            int previousState,
            int nextState
        );

    private:
        int currentState;
        int previousState;
        int nextState;

        QRegExp paragraphBreakRegex;
        QRegExp heading1SetextRegex;
        QRegExp heading2SetextRegex;
        QRegExp blockquoteRegex;
        QRegExp githubCodeFenceStartRegex;
        QRegExp githubCodeFenceEndRegex;
        QRegExp pandocCodeFenceStartRegex;
        QRegExp pandocCodeFenceEndRegex;
        QRegExp numberedListRegex;
        QRegExp numberedNestedListRegex;
        QRegExp hruleRegex;
        QRegExp lineBreakRegex;
        QRegExp emphasisRegex;
        QRegExp strongRegex;
        QRegExp strikethroughRegex;
        QRegExp verbatimRegex;
        QRegExp htmlTagRegex;
        QRegExp htmlEntityRegex;
        QRegExp automaticLinkRegex;
        QRegExp inlineLinkRegex;
        QRegExp referenceLinkRegex;
        QRegExp referenceDefinitionRegex;
        QRegExp imageRegex;
        QRegExp htmlInlineCommentRegex;
        QRegExp mentionRegex;
        QRegExp pipeTableDividerRegex;


        bool tokenizeSetextHeadingLine1(const QString& text);
        bool tokenizeSetextHeadingLine2(const QString& text);
        bool tokenizeAtxHeading(const QString& text);

        bool tokenizeNumberedList(const QString& text);
        bool tokenizeBulletPointList(const QString& text);
        bool tokenizeHorizontalRule(const QString& text);
        bool tokenizeLineBreak(const QString& text);
        bool tokenizeBlockquote(const QString& text);
        bool tokenizeCodeBlock(const QString& text);
        bool tokenizeMultilineComment(const QString& text);

        bool tokenizeInline(const QString& text);
        void tokenizeVerbatim(QString& text);
        void tokenizeHtmlComments(QString& text);
        void tokenizeTableHeaderRow(QString& text);
        bool tokenizeTableDivider(const QString& text);
        void tokenizeTableRow(QString& text);

        /*
         * Tokenizes a block of text, searching for all occurrances of regex.
         * Occurrances are set to the given token type and added to the list of
         * tokens.  The markupStartCount and markupEndCount values are used to
         * indicate how many markup special characters preceed and follow the
         * main text, respectively.
         *
         * For example, if the matched string is "**bold**", and
         * markupStartCount = 2 and markupEndCount = 2, then the asterisks
         * preceeding and following the word "bold" will be set as opening and
         * closing markup in the token.
         *
         * If replaceMarkupChars is true, then the markupStartCount and
         * markupEndCount characters will be replaced with a dummy character in
         * the text QString so that subsequent parsings of the same line do not
         * pick up the original characters.
         *
         * If replaceAllChars is true instead, then the entire matched text will
         * be replaced with dummy characters--again, for ease in parsing the
         * same line for other regular expression matches.
         */
        void tokenizeMatches
        (
            MarkdownTokenType tokenType,
            QString& text,
            QRegExp& regex,
            const int markupStartCount = 0,
            const int markupEndCount = 0,
            const bool replaceMarkupChars = false,
            const bool replaceAllChars = false
        );

        /*
         * Replaces escaped characters in text so they aren't picked up
         * during parsing.  Returns a copy of the input text string
         * with the escaped characters replaced with a dummy character.
         */
        QString dummyOutEscapeCharacters(const QString& text) const;

};

#endif
