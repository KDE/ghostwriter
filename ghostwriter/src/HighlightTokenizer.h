/***********************************************************************
 *
 * Copyright (C) 2014-2016 wereturtle
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

#ifndef HIGHLIGHT_TOKENIZER_H
#define HIGHLIGHT_TOKENIZER_H

#include <QString>
#include <QList>
#include <QMap>

#include "Token.h"

/**
 * This abstract class is used in conjunction with QSyntaxHighlighter to
 * tokenize a line (block) of text in QTextDocument.  Rather than having a full-
 * blown parser that produces an abstract syntax tree (AST), we just want to
 * get the ranges of text that need special formatting for syntax highlighting.
 * However, the lexing still needs to be context-sensitive, since markup
 * languages like Markdown are highly context-sensitive.  Thus, this class
 * is aware of line states, which determine token types.  Each token stores
 * the position (i.e., column in the line) and length of the token text within
 * the line.  Tokens are returned in an arbitrary order, often with a token
 * encapsulating a larger range of text being returned first in a list of
 * of tokens, and the remaining tokens having a smaller range within the first
 * token's range.  (See getTokens for details.)  Note that the accompanying
 * Token class is especially suited to markup formats (i.e., Markdown,
 * ReStructured Text, etc.), but does not need to be restricted solely to
 * that format.
 *
 * An example is the MarkdownTokenizer, which parses a line into a list of
 * tokens, where the first token in the list represents the kind of line
 * (paragraph, heading, numbered list, code block, etc.).  Subsequent tokens in
 * the list may gradually narrow in scope.  For example, an emphasis token might
 * be returned next, followed by more tokens that are within range of that
 * emphasis token and which will therefore also be italicized in the text
 * editor.  However, tokens that cover different ranges in the string of
 * text may be returned in different orders, depending on precedence rules.
 *
 * Fortunately the QSyntaxHighlighter that will format the text based on these
 * tokens doesn't need to worry about the token order.  It needs to only loop
 * through the tokens and format the text accordingly.
 */
class HighlightTokenizer
{
    public:
        /**
         * Constructor.
         */
        HighlightTokenizer();

        /**
         * Destructor.
         */
        virtual ~HighlightTokenizer();

        /**
         * Tokenizes the given line of text.  After calling this method, call
         * getTokens() to fetch the tokens that were parsed.  Likewise, call
         * getState() to determine the line state for the end of the line,
         * which you can pass this method as the value of the previousState
         * parameter along with the text of the next line in the document.
         * Call backtrackRequested() to see if the tokenizer needs to backtrack
         * one line in order to correctly tokenize the current line.  Finally,
         * call clear() to clean up the tokens and state for the next call.
         *
         * text - the line of text to be tokenized
         * currentState - the current state of the line being passed in from the
         *                last time it was tokenized (if at all)
         * previousState - the line state of the end of the previous line
         *                 in the document
         * nextState - the line state of the next line in the document
         *
         * Note that the above line states are intended to be used as the
         * states in QSyntaxHighlighter.  Reading the Qt documentation for
         * that class should clarify the usage of this class.
         */
        virtual void tokenize
        (
            const QString& text,
            int currentState,
            int previousState,
            int nextState
        ) = 0;

        /**
         * Returns the tokens produced by calling tokenize().
         */
        QList<Token> getTokens() const;

        /**
         * Returns the line state at the end of the line that was tokenized
         * by calling tokenize().
         */
        int getState() const;

        /**
         * Returns true if the tokenizer needs to backtrack one line in order
         * to successfully tokenize the current line.
         */
        bool backtrackRequested() const;

        /**
         * Clears the line state, tokens, and backtrack request flag.
         */
        void clear();

    protected:
        /**
         * Call this method to add the given token to the list that will be
         * returned by getTokens().
         */
        void addToken(const Token& token);

        /**
         * Call this method to set the current line state for the line.
         */
        void setState(int state);

        /**
         * Call this method to ensure backtrackRequested() returns true
         * rather than false.
         */
        void requestBacktrack();

    private:
        int state;
        QMap<int, Token> tokens;
        bool backtrack;

        /*
         * Compares two tokens' positions for sorting.
         */
        static bool tokenLessThan(const Token& t1, const Token& t2);

};

#endif
