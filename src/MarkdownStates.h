/***********************************************************************
 *
 * Copyright (C) 2014-2020 wereturtle
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

/*
 * Line states for use in tracking the line state of each block (line) in
 * a Markdown document.  See the userState() method in QTextBlock for details.
 * 
 * First byte is for specifying whether the element is in a nested structure
 * such as a block quote or code block.  The next byte is for the line
 * state, and the final 2 bytes are used to specify the nesting depth
 * (for elements contained within a list).  Use MarkdownStateMask to
 * extract only the middle line state byte.
 */
typedef int MarkdownState;

const MarkdownState MarkdownStateUnknown              = -1;
const MarkdownState MarkdownStateParagraphBreak       = 0x00000000;
const MarkdownState MarkdownStateParagraph            = 0x00010000;
const MarkdownState MarkdownStateAtxHeading1          = 0x00020000;
const MarkdownState MarkdownStateAtxHeading2          = 0x00030000;
const MarkdownState MarkdownStateAtxHeading3          = 0x00040000;
const MarkdownState MarkdownStateAtxHeading4          = 0x00050000;
const MarkdownState MarkdownStateAtxHeading5          = 0x00060000;
const MarkdownState MarkdownStateAtxHeading6          = 0x00070000;
const MarkdownState MarkdownStateSetextHeading1       = 0x00080000;
const MarkdownState MarkdownStateSetextHeading2       = 0x00090000;
const MarkdownState MarkdownStateHorizontalRule       = 0x000A0000;
const MarkdownState MarkdownStatePipeTableHeader      = 0x000B0000;
const MarkdownState MarkdownStatePipeTableDivider     = 0x000C0000;
const MarkdownState MarkdownStatePipeTableRow         = 0x000D0000;
const MarkdownState MarkdownStateNumberedList         = 0x000E0000;
const MarkdownState MarkdownStateBulletPointList      = 0x000F0000;
const MarkdownState MarkdownStateTaskList             = 0x00100000;
const MarkdownState MarkdownStateBlockquote           = 0x02000000;
const MarkdownState MarkdownStateCodeBlock            = 0x04000000;

const MarkdownState MarkdownStateMask                 = 0x00FF0000;

#endif // MARKDOWN_STATES_H
