/*
 * SPDX-FileCopyrightText: 2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef INLINE_MARKUP_TOGGLE_H
#define INLINE_MARKUP_TOGGLE_H

#include <QScopedPointer>

#include "../../markdown/markdownnode.h"
#include "edit.h"

namespace ghostwriter
{
class InlineMarkupTogglePrivate;
class InlineMarkupToggle : Edit
{
public:
    InlineMarkupToggle(MarkdownNode::NodeType type);
    ~InlineMarkupToggle();

    void operator()(MarkdownEditor *editor) override;

private:
    QScopedPointer<InlineMarkupTogglePrivate> d;
    MarkdownNode::NodeType m_type;
};

} // namespace ghostwriter

#endif // INLINE_MARKUP_TOGGLE_H
