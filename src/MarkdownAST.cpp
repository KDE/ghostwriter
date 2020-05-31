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

#include "MarkdownAST.h"

MarkdownAST::MarkdownAST()
{
    root = NULL;
}

MarkdownAST::MarkdownAST(cmark_node* root)
{
    setRoot(root);
}

MarkdownAST::~MarkdownAST()
{
    arena.freeAll();
    root = NULL;
}

MarkdownNode* MarkdownAST::getRoot()
{
    return root;
}

void MarkdownAST::setRoot(cmark_node* root) 
{
    arena.freeAll();

    if (NULL == root)
    {
        this->root = NULL;
        return;
    }

    this->root = arena.allocate();

    // Clone the node into memory that isn't allocated to
    // cmark-gfm's arena memory.
    QStack<cmark_node*> fromNodes;
    QStack<MarkdownNode*> toNodes;

    fromNodes.push(root);
    toNodes.push(this->root);

    while (!fromNodes.isEmpty())
    {
        cmark_node* source = fromNodes.pop();
        MarkdownNode* dest = toNodes.pop();

        dest->setDataFrom(source);

        // Prep children nodes for cloning.
        MarkdownNode* destParent = dest;
        source = cmark_node_first_child(source);

        while (NULL != source)
        {
            fromNodes.push(source);
            dest = arena.allocate();
            destParent->appendChild(dest);
            toNodes.push(dest);
            source = cmark_node_next(source);
        }
    }    
}

MarkdownNode* MarkdownAST::findBlockAtLine(int lineNumber) const
{
    if ((NULL == root) || (MarkdownNode::Invalid == root->getType()))
    {
        return NULL;
    }

    MarkdownNode* candidate = NULL;
    MarkdownNode* current = root->getFirstChild();

    while 
    (
        (NULL != current)
        && (current->isBlockType()) 
        && (MarkdownNode::TableCell != current->getType())
    )
    {
        if 
        (
            (current->getStartLine() <= lineNumber)
            && 
            (
                (lineNumber <= current->getEndLine()) 
                || (0 == current->getEndLine())
            )
        )
        {
            candidate = current;

            switch (current->getType())
            {
                case MarkdownNode::ListItem:
                case MarkdownNode::TaskListItem:
                    return candidate;
                case MarkdownNode::Heading:
                {
                    int lineCount = current->getEndLine() - current->getStartLine() + 1;

                    if (
                        (lineCount > 2) &&
                        (lineNumber == current->getEndLine()))
                    {
                        current = current->getNext();
                    }
                    else
                    {
                        current = current->getFirstChild();
                    }
                    break;
                }
                default:
                    current = current->getFirstChild();
                    break;
            }
        }
        else if (current->getStartLine() > lineNumber)
        {
            return candidate;
        }
        else
        {
            current = current->getNext();
        }
    }

    return candidate;
}

QVector<MarkdownNode*> MarkdownAST::getHeadings() const
{
    QVector<MarkdownNode*> headings;

    if ((NULL == root) || (MarkdownNode::Invalid == root->getType()))
    {
        return headings;
    }

    MarkdownNode* node = root->getFirstChild();

    while (NULL != node)
    {
        if (MarkdownNode::Heading == node->getType())
        {
            headings.append(node);
        }

        node = node->getNext();
    }

    return headings;
}

void MarkdownAST::clear() 
{
    arena.freeAll();
    root = NULL;
}

QString MarkdownAST::toString() const
{
    if (NULL == root)
    {
        return "AST is empty";
    }

    QString text;
    QTextStream stream(&text);
    QStack<MarkdownNode*> nodes;
    QStack<QString> indentation;

    nodes.push(root);
    indentation.push("");

    while (!nodes.empty())
    {
        MarkdownNode* node = nodes.pop();
        QString indent = indentation.pop();
        
        stream << indent << "->" << node->toString() << endl;

        MarkdownNode* child = node->getFirstChild();
        indent += "   ";

        while (NULL != child)
        {
            nodes.push(child);
            indentation.push(indent);
            child = child->getNext();
        }
    }

    return text;
}