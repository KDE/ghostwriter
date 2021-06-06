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

#include <QMutex>

#include "3rdparty/cmark-gfm/core/cmark-gfm-extension_api.h"
#include "3rdparty/cmark-gfm/extensions/cmark-gfm-core-extensions.h"

#include "cmarkgfmapi.h"

namespace ghostwriter
{
class CmarkGfmAPIPrivate
{
public:

    static CmarkGfmAPI *instance;

    CmarkGfmAPIPrivate()
    {
        ;
    };

    ~CmarkGfmAPIPrivate()
    {
        ;
    };

    // cmark-gfm extensions to enable.
    cmark_syntax_extension *tableExt;
    cmark_syntax_extension *strikethroughExt;
    cmark_syntax_extension *autolinkExt;
    cmark_syntax_extension *tagfilterExt;
    cmark_syntax_extension *tasklistExt;

    QMutex apiMutex;
};

CmarkGfmAPI *CmarkGfmAPIPrivate::instance = nullptr;

CmarkGfmAPI *CmarkGfmAPI::instance()
{
    if (nullptr == CmarkGfmAPIPrivate::instance) {
        CmarkGfmAPIPrivate::instance = new CmarkGfmAPI();
    }

    return CmarkGfmAPIPrivate::instance;
}

CmarkGfmAPI::~CmarkGfmAPI()
{
    cmark_arena_reset();
}

MarkdownAST *CmarkGfmAPI::parse(const QString &text, const bool smartTypographyEnabled)
{
    Q_D(CmarkGfmAPI);

    int opts = CMARK_OPT_DEFAULT | CMARK_OPT_FOOTNOTES | CMARK_OPT_UNSAFE;

    if (smartTypographyEnabled) {
        opts |= CMARK_OPT_SMART;
    }

    d->apiMutex.lock();

    cmark_mem *mem = cmark_get_arena_mem_allocator();
    cmark_parser *parser = cmark_parser_new_with_mem(opts, mem);

    cmark_parser_attach_syntax_extension(parser, d->tableExt);
    cmark_parser_attach_syntax_extension(parser, d->strikethroughExt);
    cmark_parser_attach_syntax_extension(parser, d->autolinkExt);
    cmark_parser_attach_syntax_extension(parser, d->tagfilterExt);
    cmark_parser_attach_syntax_extension(parser, d->tasklistExt);

    // Use Latin1 instead of UTF-8 since the column numbers and even
    // the nodes returned in the AST are shifted or missing when
    // UTF-8 characters longer than 1 byte are encountered.
    //
    cmark_parser_feed(parser, text.toLatin1().data(), text.toLocal8Bit().length());

    cmark_node *root = cmark_parser_finish(parser);
    MarkdownAST *ast = new MarkdownAST(root);
    cmark_parser_free(parser);
    cmark_node_free(root);
    cmark_arena_reset();

    d->apiMutex.unlock();

    return ast;
}

QString CmarkGfmAPI::renderToHtml(const QString &text, const bool smartTypographyEnabled)
{
    Q_D(CmarkGfmAPI);
    
    int opts = CMARK_OPT_DEFAULT | CMARK_OPT_FOOTNOTES | CMARK_OPT_UNSAFE;

    if (smartTypographyEnabled) {
        opts |= CMARK_OPT_SMART;
    }

    d->apiMutex.lock();

    cmark_mem *mem = cmark_get_arena_mem_allocator();
    cmark_parser *parser = cmark_parser_new_with_mem(opts, mem);

    cmark_parser_attach_syntax_extension(parser, d->tableExt);
    cmark_parser_attach_syntax_extension(parser, d->strikethroughExt);
    cmark_parser_attach_syntax_extension(parser, d->autolinkExt);
    cmark_parser_attach_syntax_extension(parser, d->tagfilterExt);
    cmark_parser_attach_syntax_extension(parser, d->tasklistExt);

    cmark_parser_feed(parser, text.toUtf8().data(), text.toLocal8Bit().length());

    cmark_node *root = cmark_parser_finish(parser);
    char *output = cmark_render_html(root, opts, cmark_parser_get_syntax_extensions(parser));
    QString html = QString::fromUtf8(output);

    cmark_parser_free(parser);
    cmark_arena_reset();

    d->apiMutex.unlock();

    return html;
}

CmarkGfmAPI::CmarkGfmAPI()
    : d_ptr(new CmarkGfmAPIPrivate())
{
    Q_D(CmarkGfmAPI);
    
    cmark_gfm_core_extensions_ensure_registered();
    d->tableExt = cmark_find_syntax_extension("table");
    d->strikethroughExt = cmark_find_syntax_extension("strikethrough");
    d->autolinkExt = cmark_find_syntax_extension("autolink");
    d->tagfilterExt = cmark_find_syntax_extension("tagfilter");
    d->tasklistExt = cmark_find_syntax_extension("tasklist");
}
}
