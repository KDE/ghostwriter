/*
 * SPDX-FileCopyrightText: 2020-2024 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MARKDOWN_NODE_H
#define MARKDOWN_NODE_H

#include <QChar>
#include <QString>

struct cmark_node;

namespace ghostwriter
{
/**
 * Markdown node wrapper for cmark-gfm node.
 */
class MarkdownNode
{
public:

    typedef enum {
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
        cmark_node *node
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
    MarkdownNode *parent() const;

    /**
     * Appends the given node as a child to this node.
     */
    void appendChild(MarkdownNode *node);

    /**
     * Returns the first child of this node.
     */
    MarkdownNode *firstChild() const;

    /**
     * Returns the last child of this node.
     */
    MarkdownNode *lastChild() const;

    /**
     * Returns the previous sibling node.
     */
    MarkdownNode *previous() const;

    /**
     * Returns the next sibling node.
     */
    MarkdownNode *next() const;

    /**
     * Returns the node type.
     */
    NodeType type() const;

    /**
     * Returns the column position of this node from the original Markdown text,
     * starting at a value of 0. For line breaks, -1 may be returned.
     *
     * NOTE: cmark-gfm returns column numbers that are actually byte number
     * positions rather than UTF-8 character positions.  The value returned
     * by this method MUST be converted into the UTF-8 character position
     * before use.
     */
    int position() const;

    /**
     * Returns the length of the text represented by this node from the original
     * Markdown text.
     *
     * NOTE: cmark-gfm returns column numbers and string lengths that are
     * actually byte number positions and number of bytes rather than UTF-8
     * character positions and character counts.  The value returned
     * by this method MUST be converted into the UTF-8 length before use.
     */
    int length() const;

    /**
     * Returns the start line of this node in the
     * original Markdown text.
     */
    int startLine() const;

    /**
     * Returns the end line of this node in the
     * original Markdown text.
     */
    int endLine() const;

    /**
     * Returns the text contained in this node.
     */
    QString text() const;

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
    int headingLevel() const;

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
    int listItemNumber() const;

    /**
     * Returns true if this node has a bullet list item type.
     */
    bool isBulletListItem() const;

private:
    NodeType m_type;
    MarkdownNode *m_parent;
    MarkdownNode *m_prev;
    MarkdownNode *m_next;
    MarkdownNode *m_firstChild;
    MarkdownNode *m_lastChild;
    QString m_text;
    int m_startLine;
    int m_endLine;
    int m_position;
    int m_length;
    int m_markupIndent;
    bool m_inBreak; // Line break or soft break

    // NOTE: Don't encapsulate any of the following fields
    //       in a union construct, as alignment padding will
    //       negate the space-saving benefit.

    // Fence character used for fenced code blocks, or else null character.
    unsigned char m_fenceChar;

    // Heading level if node is a heading.
    unsigned char m_headingLevel;

    // Numbered list starting number if node is a numbered list item.
    int m_listStartNum;

    NodeType nodeType(cmark_node *node);

    QString toString(NodeType nodeType) const;
};
} // namespace ghostwriter

#endif
