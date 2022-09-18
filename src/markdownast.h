/*
 * SPDX-FileCopyrightText: 2020-2022 Megan Conkle <wereturtle@airpost.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MARKDOWN_AST_H
#define MARKDOWN_AST_H

#include <QScopedPointer>

#include "markdownnode.h"
#include "memoryarena.h"

struct cmark_node;

namespace ghostwriter
{
/**
 * This class encapsulates an abstract syntax tree of Markdown nodes.
 * Use this class to clone a cmark_node AST and perform searches
 * on the tree.
 *
 * Note:  Due to cmark-gfm's memory arena used ot allocate cmark_node
 *        objects being non-reentrant, this class is used to clone
 *        a cmark_node AST to prevent node memory from being overwritten
 *        by another call to the cmark-gfm API.
 */
class MarkdownASTPrivate;
class MarkdownAST
{
    Q_DECLARE_PRIVATE(MarkdownAST)

public:
    /**
     * Constructor
     */
    MarkdownAST();

    /**
     * Constructor.  Clones the given cmark_node AST into a
     * MarkdownNode AST.
     */
    MarkdownAST(cmark_node *root);

    /**
     * Destructor.
     */
    ~MarkdownAST();

    /**
     * Returns the root node of the AST or nullptr if none is set.
     */
    MarkdownNode *root();

    /**
     * Sets the root node of the AST, cloning the given cmark_node AST into
     * a MarkdownNode AST.  Note that calling this routine will free the
     * memory for the prior AST root node.
     */
    void setRoot(cmark_node *root);

    /**
     * Finds the deepest node of type block (vs. inline) at the given
     * line number of the original Markdown text.  Returns nullptr if
     * no node is found at that location.
     */
    MarkdownNode *findBlockAtLine(int lineNumber) const;

    /**
     * Returns a list of all nodes that are of type heading, excluding
     * those that are nested within block quotes or lists.
     */
    QVector<MarkdownNode *> headings() const;

    /**
     * Frees memory for this AST.
     */
    void clear();

    /**
     * Returns a string representation of this tree for use in debugging.
     */
    QString toString() const;

private:
    QScopedPointer<MarkdownASTPrivate> d_ptr;
};
} // namespace ghostwriter

#endif
