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

#ifndef MARKDOWN_NODE_H
#define MARKDOWN_NODE_H

#include <QChar>
#include <QString>
#include "cmark-gfm.h"


/**
 * Markdown node wrapper for cmark-gfm node.
 */
class MarkdownNode
{
    public:

        typedef enum
        {
            Invalid,

            // Block

            FirstBlockType,
            Document = FirstBlockType,
            BlockQuote,
            NumberedList,
            BulletList,
            TaskListItem,
            ListItem,
            CodeBlock,
            HtmlBlock,
            Paragraph,
            Heading,
            ThematicBreak,
            FootnoteDefinition,
            Table,
            TableHeading,
            TableRow,
            TableCell,
            LastBlockType = TableCell,

            // Inline

            Text,
            FirstInlineType = Text,
            Softbreak,
            Linebreak,
            Code,
            HtmlInline,
            Emph,
            Strong,
            Link,
            Image,
            Strikethrough,
            FootnoteReference,
            LastInlineType = FootnoteReference
        } NodeType;

        /**
         * Constructor.
         */
        MarkdownNode();

        /**
         * Constructor.  Copies data from provided cmark_node.
         */
        MarkdownNode
        (
            cmark_node* node
        );

        /**
         * Destructor.
         */
        ~MarkdownNode();

        /**
         * Copies data from the provided cmark_node.
         */
        void setDataFrom(cmark_node *node);

        /**
         * Returns a string representation of this node.
         */
        QString toString() const;

        /**
         * Returns whether this node has valid data.
         */
        bool isInvalid() const;

        /**
         * Returns this node's parent node.
         */
        MarkdownNode* getParent() const;

        /**
         * Appends the given node as a child to this node.
         */
        void appendChild(MarkdownNode* node);

        /**
         * Returns the first child of this node.
         */
        MarkdownNode* getFirstChild() const;
        
        /**
         * Returns the last child of this node.
         */
        MarkdownNode* getLastChild() const;

        /**
         * Returns the previous sibling node.
         */
        MarkdownNode* getPrevious() const;

        /**
         * Returns the next sibling node.
         */
        MarkdownNode* getNext() const;

        /**
         * Returns the node type.
         */
        NodeType getType() const;

        /**
         * Returns the column position of this node
         * from the original Markdown text, starting
         * at a value of 0.
         */
        int getPosition() const;

        /**
         * Returns the length of the text represented
         * by this node from the original Markdown text.
         */
        int getLength() const;

        /**
         * Returns the start line of this node in the
         * original Markdown text.
         */
        int getStartLine() const;

        /**
         * Returns the end line of this node in the
         * original Markdown text.
         */
        int getEndLine() const;

        /**
         * Returns the text contained in this node.
         */
        QString getText() const;

        /**
         * Returns true of this node has a block type.
         */
        bool isBlockType() const;

        /**
         * Returns true of this node has an inline type.
         */
        bool isInlineType() const;

        /**
         * Returns the heading level of this node if it
         * has a heading type, otherwise 0.
         */
        int getHeadingLevel() const;

        /**
         * Returns true if the node has a setext heading type.
         */
        bool isSetextHeading() const;

        /**
         * Returns true if the node has an ATX heading type.
         */
        bool isAtxHeading() const;

        /**
         * Returns true if the node has an ancestor node
         * that has a block quote type.
         */
        bool isInsideBlockquote() const;

        /**
         * Returns true if the node has a fenced code block type.
         */
        bool isFencedCodeBlock() const;

        /**
         * Returns true if the node has a numbered list item type.
         */
        bool isNumberedListItem() const;

        /**
         * Returns the list item number if this node has a list item type.
         */
        int getListItemNumber() const;

        /**
         * Returns true if this node has a bullet list item type.
         */
        bool isBulletListItem() const;

    private:
        NodeType type;
        MarkdownNode* parent;
        MarkdownNode* prev;
        MarkdownNode* next;
        MarkdownNode* firstChild;
        MarkdownNode* lastChild;
        QString text;
        int startLine;
        int endLine;
        int position;
        int length;

        // NOTE: Don't bother encapsulating any of the following fields
        //       in a union construct, as alignment padding will
        //       negate the space-saving benefit.

        // Fence character used for fenced code blocks, or else null character.
        unsigned char fenceChar;

        // Heading level if node is a heading.
        unsigned char headingLevel;

        // Numbered list starting number if node is a numbered list item.
        int listStartNum;

        NodeType getNodeType(cmark_node* node);

        QString toString(NodeType nodeType) const;
};

#endif
