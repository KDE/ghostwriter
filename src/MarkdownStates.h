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

#ifndef MARKDOWN_STATES_H
#define MARKDOWN_STATES_H

#include "LineParserStates.h"

/*
 * Parser states for use in tracking the parsing state of each block (line) in
 * a Markdown document.  See the userState() method in QTextBlock for details.
 */
enum MarkdownState
{
    MarkdownStateUnknown = LINE_PARSER_STATE_UNKNOWN,
    MarkdownStateParagraphBreak,
    MarkdownStateListLineBreak,
    MarkdownStateParagraph,
    MarkdownStateAtxHeading1,
    MarkdownStateAtxHeading2,
    MarkdownStateAtxHeading3,
    MarkdownStateAtxHeading4,
    MarkdownStateAtxHeading5,
    MarkdownStateAtxHeading6,
    MarkdownStateBlockquote,
    MarkdownStateCodeBlock,
    MarkdownStateInGithubCodeFence,
    MarkdownStateInPandocCodeFence,
    MarkdownStateCodeFenceEnd,
    MarkdownStateComment,
    MarkdownStateHorizontalRule,
    MarkdownStateNumberedList,
    MarkdownStateBulletPointList,
    MarkdownStateSetextHeading1Line1,
    MarkdownStateSetextHeading1Line2,
    MarkdownStateSetextHeading2Line1,
    MarkdownStateSetextHeading2Line2
};

#endif // MARKDOWN_STATES_H
