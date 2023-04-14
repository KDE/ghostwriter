/*
 * SPDX-FileCopyrightText: 2020-2023 Megan Conkle <megan.conkle@kdemail.net>
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef CMARK_PROCESSOR_H
#define CMARK_PROCESSOR_H

#include <QScopedPointer>

#include "markdownast.h"

namespace ghostwriter
{
/**
 * This class wraps the cmark-gfm API to make it thread-safe.
 */
class CmarkGfmAPIPrivate;
class CmarkGfmAPI
{
    Q_DECLARE_PRIVATE(CmarkGfmAPI)

public:
    /**
     * Returns the single instance of this class.
     */
    static CmarkGfmAPI *instance();

    /**
     * Destructor.
     */
    ~CmarkGfmAPI();

    /**
     * Parses the given Markdown text, returning an AST representation.
     * of the text.  Pass in true for smartTypographyEnabled to enable
     * smart typography.
     */
    MarkdownAST *parse(const QString &text, const bool smartTypographyEnabled);

    /**
     * Returns HTML text for the Markdown text.  Pass in true for
     * smartTypographyEnabled to enable smart typography.
     */
    QString renderToHtml(const QString &text, const bool smartTypographyEnabled);

protected:
    /**
     * Constructor.
     */
    CmarkGfmAPI();

private:
    QScopedPointer<CmarkGfmAPIPrivate> d_ptr;

};
}

#endif
