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
#include <QtGlobal>

#include "3rdparty/cmark-gfm/src/cmark-gfm.h"

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
    Q_D(MarkdownAST);
    
    d->root = nullptr;
}

MarkdownAST::MarkdownAST(cmark_node *root)
    : d_ptr(new MarkdownASTPrivate())
{    
    setRoot(root);
}

MarkdownAST::~MarkdownAST()
{
    Q_D(MarkdownAST);
    
    d->arena.freeAll();
    d->root = nullptr;
}

MarkdownNode *MarkdownAST::root()
{
    Q_D(MarkdownAST);
    
    return d->root;
}

void MarkdownAST::setRoot(cmark_node *root)
{
    Q_D(MarkdownAST);
    
    d->arena.freeAll();

    if (nullptr == root) {
        d->root = nullptr;
        return;
    }

    d->root = d->arena.allocate();

    // Clone the node into memory that isn't allocated to
    // cmark-gfm's arena memory.
    QStack<cmark_node *> fromNodes;
    QStack<MarkdownNode *> toNodes;

    fromNodes.push(root);
    toNodes.push(d->root);

    while (!fromNodes.isEmpty()) {
        cmark_node *source = fromNodes.pop();
        MarkdownNode *dest = toNodes.pop();

        dest->setDataFrom(source);

        // Prep children nodes for cloning.
        MarkdownNode *destParent = dest;
        source = cmark_node_first_child(source);

        while (NULL != source) {
            fromNodes.push(source);
            dest = d->arena.allocate();
            destParent->appendChild(dest);
            toNodes.push(dest);
            source = cmark_node_next(source);
        }
    }
}

MarkdownNode *MarkdownAST::findBlockAtLine(int lineNumber) const
{
    Q_D(const MarkdownAST);
    
    if ((nullptr == d->root) || (MarkdownNode::Invalid == d->root->type())) {
        return nullptr;
    }

    MarkdownNode *candidate = nullptr;
    MarkdownNode *current = d->root->firstChild();

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
    Q_D(const MarkdownAST);
    
    QVector<MarkdownNode *> headings;

    if ((nullptr == d->root) || (MarkdownNode::Invalid == d->root->type())) {
        return headings;
    }

    MarkdownNode *node = d->root->firstChild();

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
    Q_D(MarkdownAST);
    
    d->arena.freeAll();
    d->root = nullptr;
}

QString MarkdownAST::toString() const
{
    Q_D(const MarkdownAST);
    
    if (nullptr == d->root) {
        return "AST is empty";
    }

    QString text;
    QTextStream stream(&text);
    QStack<MarkdownNode *> nodes;
    QStack<QString> indentation;

    nodes.push(d->root);
    indentation.push("");

    while (!nodes.empty()) {
        MarkdownNode *node = nodes.pop();
        QString indent = indentation.pop();


#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        stream << indent << "->" << node->toString() << Qt::endl;
#else
        stream << indent << "->" << node->toString() << endl;
#endif

        MarkdownNode *child = node->lastChild();
        indent += "   ";

        while (nullptr != child) {
            nodes.push(child);
            indentation.push(indent);
            child = child->previous();
        }
    }

    return text;
}
} // namespace ghostwriter
