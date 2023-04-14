/*
 * SPDX-FileCopyrightText: 2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#ifndef EDIT_H
#define EDIT_H

#include "../markdowneditor.h"

namespace ghostwriter
{
class Edit
{
public:
    Edit() { }
    virtual ~Edit() { }
    virtual void operator()(MarkdownEditor *editor) = 0;
};

} // namespace ghostwriter

#endif // EDIT_H
