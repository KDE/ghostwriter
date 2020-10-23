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

#include <QStack>
#include <QTextStream>

#include "3rdparty/cmark-gfm/core/cmark-gfm.h"

#include "markdownast.h"

namespace ghostwriter
{
class MarkdownASTPrivate
{
public:
    MarkdownASTPrivate()
    {
        ;
    }

    ~MarkdownASTPrivate()
    {
        ;
    }

    MemoryArena<MarkdownNode> arena;
    MarkdownNode *root;
};

MarkdownAST::MarkdownAST()
    : d_ptr(new MarkdownASTPrivate())
{
    d_func()->root = nullptr;
}

MarkdownAST::MarkdownAST(cmark_node *root)
    : d_ptr(new MarkdownASTPrivate())
{
    setRoot(root);
}

MarkdownAST::~MarkdownAST()
{
    d_func()->arena.freeAll();
    d_func()->root = nullptr;
}

MarkdownNode *MarkdownAST::root()
{
    return d_func()->root;
}

void MarkdownAST::setRoot(cmark_node *root)
{
    d_func()->arena.freeAll();

    if (nullptr == root) {
        d_func()->root = nullptr;
        return;
    }

    d_func()->root = d_func()->arena.allocate();

    // Clone the node into memory that isn't allocated to
    // cmark-gfm's arena memory.
    QStack<cmark_node *> fromNodes;
    QStack<MarkdownNode *> toNodes;

    fromNodes.push(root);
    toNodes.push(d_func()->root);

    while (!fromNodes.isEmpty()) {
        cmark_node *source = fromNodes.pop();
        MarkdownNode *dest = toNodes.pop();

        dest->setDataFrom(source);

        // Prep children nodes for cloning.
        MarkdownNode *destParent = dest;
        source = cmark_node_first_child(source);

        while (NULL != source) {
            fromNodes.push(source);
            dest = d_func()->arena.allocate();
            destParent->appendChild(dest);
            toNodes.push(dest);
            source = cmark_node_next(source);
        }
    }
}

MarkdownNode *MarkdownAST::findBlockAtLine(int lineNumber) const
{
    if ((nullptr == d_func()->root) || (MarkdownNode::Invalid == d_func()->root->type())) {
        return nullptr;
    }

    MarkdownNode *candidate = nullptr;
    MarkdownNode *current = d_func()->root->firstChild();

    while
    (
        (nullptr != current)
        && (current->isBlockType())
        && (MarkdownNode::TableCell != current->type())
    ) {
        if
        (
            (current->startLine() <= lineNumber)
            &&
            (
                (lineNumber <= current->endLine())
                || (0 == current->endLine())
            )
        ) {
            candidate = current;

            switch (current->type()) {
            case MarkdownNode::ListItem:
            case MarkdownNode::TaskListItem:
                return candidate;
            case MarkdownNode::Heading: {
                int lineCount = current->endLine() - current->startLine() + 1;

                if (
                    (lineCount > 2) &&
                    (lineNumber == current->endLine())) {
                    current = current->next();
                } else {
                    current = current->firstChild();
                }
                break;
            }
            default:
                current = current->firstChild();
                break;
            }
        } else if (current->startLine() > lineNumber) {
            return candidate;
        } else {
            current = current->next();
        }
    }

    return candidate;
}

QVector<MarkdownNode *> MarkdownAST::headings() const
{
    QVector<MarkdownNode *> headings;

    if ((nullptr == d_func()->root) || (MarkdownNode::Invalid == d_func()->root->type())) {
        return headings;
    }

    MarkdownNode *node = d_func()->root->firstChild();

    while (nullptr != node) {
        if (MarkdownNode::Heading == node->type()) {
            headings.append(node);
        }

        node = node->next();
    }

    return headings;
}

void MarkdownAST::clear()
{
    d_func()->arena.freeAll();
    d_func()->root = nullptr;
}

QString MarkdownAST::toString() const
{
    if (nullptr == d_func()->root) {
        return "AST is empty";
    }

    QString text;
    QTextStream stream(&text);
    QStack<MarkdownNode *> nodes;
    QStack<QString> indentation;

    nodes.push(d_func()->root);
    indentation.push("");

    while (!nodes.empty()) {
        MarkdownNode *node = nodes.pop();
        QString indent = indentation.pop();

        stream << indent << "->" << node->toString() << endl;

        MarkdownNode *child = node->firstChild();
        indent += "   ";

        while (nullptr != child) {
            nodes.push(child);
            indentation.push(indent);
            child = child->next();
        }
    }

    return text;
}
} // namespace ghostwriter
