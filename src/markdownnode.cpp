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

#include <QQueue>
#include <QRegularExpression>
#include <QSharedPointer>
#include <QStack>

#include "3rdparty/cmark-gfm/core/cmark-gfm.h"
#include "3rdparty/cmark-gfm/extensions/cmark-gfm-core-extensions.h"

#include "markdownnode.h"

namespace ghostwriter
{
MarkdownNode::MarkdownNode() :
    m_type(Invalid),
    m_parent(NULL),
    m_prev(NULL),
    m_next(NULL),
    m_firstChild(NULL),
    m_lastChild(NULL),
    m_startLine(0),
    m_endLine(0),
    m_position(0),
    m_length(0),
    m_fenceChar('\0'),
    m_headingLevel(0),
    m_listStartNum(0)
{
    ;
}

MarkdownNode::MarkdownNode
(
    cmark_node *node
)
{
    MarkdownNode();

    if (NULL == node) {
        return;
    }

    setDataFrom(node);
}

MarkdownNode::~MarkdownNode()
{
    ;
}

void MarkdownNode::setDataFrom(cmark_node *node)
{
    // Copy data.
    m_type = nodeType(node);
    m_position = cmark_node_get_start_column(node) - 1;
    m_length = cmark_node_get_end_column(node) - cmark_node_get_start_column(node) + 1;
    m_startLine = cmark_node_get_start_line(node);
    m_endLine = cmark_node_get_end_line(node);

    if (!isBlockType()) {
        m_text = QString::fromUtf8(cmark_node_get_literal(node));
    }

    if (CodeBlock == m_type) {
        int len;
        int offset;
        char ch;

        bool fenced = cmark_node_get_fenced(node, &len, &offset, &ch);

        if (fenced) {
            m_fenceChar = ch;
        }
    } else if (Heading == m_type) {
        m_headingLevel = cmark_node_get_heading_level(node);
        m_text = QString::fromUtf8(cmark_node_get_string_content(node));
        m_text = this->m_text.simplified();
    }
}

MarkdownNode *MarkdownNode::parent() const
{
    return m_parent;
}

void MarkdownNode::appendChild(MarkdownNode *node)
{
    if (NULL != node) {
        node->m_parent = this;

        if (NULL == m_firstChild) {
            m_firstChild = node;
            m_lastChild = node;
            node->m_prev = NULL;
            node->m_next = NULL;
        } else {
            m_lastChild->m_next = node;
            node->m_prev = m_lastChild;
            node->m_next = NULL;
            m_lastChild = node;
        }
    }
}

MarkdownNode *MarkdownNode::firstChild() const
{
    return m_firstChild;
}

MarkdownNode *MarkdownNode::lastChild() const
{
    return m_lastChild;
}

MarkdownNode *MarkdownNode::previous() const
{
    return m_prev;
}

MarkdownNode *MarkdownNode::next() const
{
    return m_next;
}

QString MarkdownNode::toString() const
{
    int left = 20;
    int right = 20;
    QString text = this->text();

    if (text.isNull()) {
        text = "<<Empty Node>>";
    }

    if (left > text.length()) {
        left = text.length();
    }

    right = text.length() - left;

    if (right < 0) {
        right = 0;
    }

    return QString("> [lines %1 - %2][col %3, len %5] %6 -> %7")
           .arg(startLine())
           .arg(endLine())
           .arg(position())
           .arg(length())
           .arg(toString(m_type))
           .arg(this->text().left(left) + "..." + this->text().right(right));
}

bool MarkdownNode::isInvalid() const
{
    return (Invalid == m_type);
}

MarkdownNode::NodeType MarkdownNode::type() const
{
    return m_type;
}

int MarkdownNode::position() const
{
    return m_position;
}

int MarkdownNode::length() const
{
    return m_length;
}

int MarkdownNode::startLine() const
{
    return m_startLine;
}

int MarkdownNode::endLine() const
{
    return m_endLine;
}

QString MarkdownNode::text() const
{
    return m_text;
}

bool MarkdownNode::isBlockType() const
{
    return
        (
            (m_type >= FirstBlockType)
            && (m_type <= LastBlockType)
        );
}

bool MarkdownNode::isInlineType() const
{
    return
        (
            (m_type >= FirstInlineType)
            && (m_type <= LastInlineType)
        );
}

int MarkdownNode::headingLevel() const
{
    return m_headingLevel;
}

bool MarkdownNode::isSetextHeading() const
{
    return
        (
            (Heading == m_type)
            &&
            ((endLine() - startLine() + 1) > 1)
        );
}

bool MarkdownNode::isAtxHeading() const
{
    return
        (
            (Heading == m_type)
            &&
            !isSetextHeading()
        );
}

bool MarkdownNode::isInsideBlockquote() const
{
    MarkdownNode *parent = this->parent();

    while (NULL != parent) {
        if (BlockQuote == parent->type()) {
            return true;
        }

        parent = parent->parent();
    }

    return false;
}

bool MarkdownNode::isFencedCodeBlock() const
{
    return m_fenceChar != '\0';
}

bool MarkdownNode::isNumberedListItem() const
{
    return
        (
            (ListItem == m_type)
            &&
            (this->parent() != NULL)
            &&
            (NumberedList == this->parent()->type())
        );
}

int MarkdownNode::listItemNumber() const
{
    int startNum = m_listStartNum;
    int count = 1;

    MarkdownNode *p = m_prev;

    while ((p != NULL) && (p != m_parent)) {
        count++;
        p = p->previous();
    }

    return startNum + count;
}

bool MarkdownNode::isBulletListItem() const
{
    return
        (
            (ListItem == m_type)
            &&
            (this->parent() != NULL)
            &&
            (BulletList == this->parent()->type())
        );
}

MarkdownNode::NodeType MarkdownNode::nodeType(cmark_node *node)
{
    switch (cmark_node_get_type(node)) {
    case CMARK_NODE_DOCUMENT:
        return Document;
    case CMARK_NODE_BLOCK_QUOTE:
        return BlockQuote;
    case CMARK_NODE_LIST:
        switch (cmark_node_get_list_type(node)) {
        case CMARK_ORDERED_LIST:
            return NumberedList;
        case CMARK_BULLET_LIST:
            return BulletList;
        default:
            return Invalid;
        }
        break;
    case CMARK_NODE_ITEM:
        if (0 == strcmp(cmark_node_get_type_string(node), "tasklist")) {
            return TaskListItem;
        }

        return ListItem;
    case CMARK_NODE_CODE_BLOCK:
        return CodeBlock;
    case CMARK_NODE_HTML_BLOCK:
        return HtmlBlock;
    case CMARK_NODE_PARAGRAPH:
        return Paragraph;
    case CMARK_NODE_HEADING:
        return Heading;
    case CMARK_NODE_THEMATIC_BREAK:
        return ThematicBreak;
    case CMARK_NODE_FOOTNOTE_DEFINITION:
        return FootnoteDefinition;
    case CMARK_NODE_TEXT:
        return Text;
    case CMARK_NODE_SOFTBREAK:
        return Softbreak;
    case CMARK_NODE_LINEBREAK:
        return Linebreak;
    case CMARK_NODE_CODE:
        return Code;
    case CMARK_NODE_HTML_INLINE:
        return HtmlInline;
    case CMARK_NODE_EMPH:
        return Emph;
    case CMARK_NODE_STRONG:
        return Strong;
    case CMARK_NODE_LINK:
        return Link;
    case CMARK_NODE_IMAGE:
        return Image;
    case CMARK_NODE_FOOTNOTE_REFERENCE:
        return FootnoteReference;
    default:
        if (0 == strcmp(cmark_node_get_type_string(node), "table")) {
            return Table;
        } else if (0 == strcmp(cmark_node_get_type_string(node), "table_row")) {
            return TableRow;
        } else if (0 == strcmp(cmark_node_get_type_string(node), "table_header")) {
            return TableHeading;
        } else if (0 == strcmp(cmark_node_get_type_string(node), "table_cell")) {
            return TableCell;
        } else if (0 == strcmp(cmark_node_get_type_string(node), "strikethrough")) {
            return Strikethrough;
        }
    }

    return Invalid;
}

QString MarkdownNode::toString(NodeType nodeType) const
{
    switch (nodeType) {
    case MarkdownNode::Invalid:
        return "Invalid";
    case MarkdownNode::Document:
        return "Document";
    case MarkdownNode::BlockQuote:
        return "BlockQuote";
    case MarkdownNode::NumberedList:
        return "NumberedList";
    case MarkdownNode::BulletList:
        return "BulletList";
    case MarkdownNode::TaskListItem:
        return "TaskList";
    case MarkdownNode::ListItem:
        return "ListItem";
    case MarkdownNode::CodeBlock:
        return "CodeBlock";
    case MarkdownNode::HtmlBlock:
        return "HtmlBlock";
    case MarkdownNode::Paragraph:
        return "Paragraph";
    case MarkdownNode::Heading:
        return "Heading";
    case MarkdownNode::ThematicBreak:
        return "ThematicBreak";
    case MarkdownNode::FootnoteDefinition:
        return "FootnoteDefinition";
    case MarkdownNode::Table:
        return "Table";
    case MarkdownNode::TableHeading:
        return "TableHeading";
    case MarkdownNode::TableRow:
        return "TableRow";
    case MarkdownNode::TableCell:
        return "TableCell";
    case MarkdownNode::Text:
        return "Text";
    case MarkdownNode::Softbreak:
        return "Softbreak";
    case MarkdownNode::Linebreak:
        return "Linebreak";
    case MarkdownNode::Code:
        return "Code";
    case MarkdownNode::HtmlInline:
        return "HtmlInline";
    case MarkdownNode::Emph:
        return "Emph";
    case MarkdownNode::Strong:
        return "Strong";
    case MarkdownNode::Link:
        return "Link";
    case MarkdownNode::Image:
        return "Image";
    case MarkdownNode::Strikethrough:
        return "Strikethrough";
    case MarkdownNode::FootnoteReference:
        return "FootnoteReference";
    default:
        return QString("%1").arg(static_cast<std::uint32_t>(nodeType));
    }
}
} // namespace ghostwriter