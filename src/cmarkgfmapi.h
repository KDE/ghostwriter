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

private:
    QScopedPointer<CmarkGfmAPIPrivate> d_ptr;

    /**
     * Constructor.
     */
    CmarkGfmAPI();
};
}

#endif