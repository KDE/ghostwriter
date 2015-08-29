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

#ifndef LINEPARSER_H
#define LINEPARSER_H

#include <QString>
#include <QList>

#include "Token.h"

/**
 * This abstract class is used in conjunction with QSyntaxHighlighter to parse
 * a line (block) of text in QTextDocument.  Rather than building an abstract
 * syntax tree (AST), it "flattens" the AST into an ordered list of tokens,
 * where the first token is the "root" of the AST, and what follows are the
 * child nodes, likewise in the form of tokens.  Each token stores the position
 * (i.e., column in the line) and length of the token text within the line.
 * This is a far simpler approach when using a line-by-line parsing strategy
 * with the QSyntaxHighlighter than traversing an AST tree built for each line.
 * Extend this class only for simple parsing (i.e., for syntax highlighting
 * in an editor).  Note that the accompanying Token class is especially
 * suited to markup formats (i.e., Markdown, ReStructured Text, etc.), but
 * does not need to be restricted solely to that format.
 *
 * An example is the MarkdownParser, which parses a line into a list of tokens,
 * where the "root" token is the first in the list, and represents the kind
 * of line (paragraph, heading, numbered list, code block, etc.).  Subsequent
 * tokens in the list gradually narrow in scope.  For example, an emphasis
 * token might be returned next, followed by more tokens that are within range
 * of that emphasis token and which will therefore also be italicized in the
 * text editor.  Again, it is much like a flattened AST tree where the nodes
 * have already been traversed and neatly ordered for easy iteration.
 */
class LineParser
{
    public:
        LineParser();
        virtual ~LineParser();

        /**
         * Parses the given line of text.  After calling this method, call
         * getTokens() to fetch the tokens that were parsed.  Likewise, call
         * getState() to determine the parser state for the end of the line,
         * which you can pass this method as the value of the previousState
         * parameter along with the text of the next line in the document.
         * Call backtrackRequested() to see if the parser needs to backtrack
         * one line in order to correctly parse the current line.  Finally,
         * call clear() to clean up the tokens and state for the next call.
         *
         * text - the line of text to be parsed
         * currentState - the current state of the line being passed in from the
         *                last time it was parsed (if at all)
         * previousState - the parser state of the end of the previous line
         *                 in the document
         * nextState - the parser state of the next line in the document
         *
         * Note that the above parser states are intended to be used as the
         * states in QSyntaxHighlighter.  Reading the Qt documentation for
         * that class should clarify the usage of this class.
         */
        virtual void parseLine
        (
            const QString& text,
            int currentState,
            int previousState,
            int nextState
        ) = 0;

        /**
         * Returns the tokens produced by calling parseLine().
         */
        QList<Token> getTokens() const;

        /**
         * Returns the parser state at the end of the line parsed by calling
         * parseLine().
         */
        int getState() const;

        /**
         * Returns true if the parser needs to backtrack one line in order
         * to successfully parse the current line.
         */
        bool backtrackRequested() const;

        /**
         * Clears the parser state, tokens, and backtrack request flag.
         */
        void clear();

    protected:
        /**
         * Call this method to add the given token to the list that will be
         * returned by getTokens().
         */
        void addToken(const Token& token);

        /**
         * Call this method to set the current parser state for the line.
         */
        void setState(int state);

        /**
         * Call this method to ensure backtrackRequested() returns true
         * rather than false.
         */
        void requestBacktrack();

    private:
        int state;
        QList<Token> tokens;
        bool backtrack;

};

#endif
