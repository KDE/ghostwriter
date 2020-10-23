DEFINES += CMARK_GFM_STATIC_DEFINE
DEFINES += CMARK_GFM_EXTENSIONS_STATIC_DEFINE
DEFINES += CMARK_NO_SHORT_NAMES

INCLUDEPATH += $$PWD/core $$PWD/extensions

HEADERS += \
    $$PWD/core/buffer.h \
    $$PWD/core/chunk.h \
    $$PWD/core/cmark_ctype.h \
    $$PWD/core/cmark-gfm_export.h \
    $$PWD/core/cmark-gfm-extension_api.h \
    $$PWD/core/cmark-gfm.h \
    $$PWD/core/cmark-gfm_version.h \
    $$PWD/core/config.h \
    $$PWD/core/footnotes.h \
    $$PWD/core/houdini.h \
    $$PWD/core/html.h \
    $$PWD/core/inlines.h \
    $$PWD/core/iterator.h \
    $$PWD/core/map.h \
    $$PWD/core/node.h \
    $$PWD/core/parser.h \
    $$PWD/core/plugin.h \
    $$PWD/core/references.h \
    $$PWD/core/registry.h \
    $$PWD/core/render.h \
    $$PWD/core/scanners.h \
    $$PWD/core/syntax_extension.h \
    $$PWD/core/utf8.h \
    $$PWD/extensions/autolink.h \
    $$PWD/extensions/cmark-gfm-core-extensions.h \
    $$PWD/extensions/cmark-gfm-extensions_export.h \
    $$PWD/extensions/ext_scanners.h \
    $$PWD/extensions/strikethrough.h \
    $$PWD/extensions/table.h \
    $$PWD/extensions/tagfilter.h \
    $$PWD/extensions/tasklist.h

SOURCES += \
    $$PWD/core/arena.c \
    $$PWD/core/blocks.c \
    $$PWD/core/buffer.c \
    $$PWD/core/cmark.c \
    $$PWD/core/cmark_ctype.c \
    $$PWD/core/commonmark.c \
    $$PWD/core/footnotes.c \
    $$PWD/core/houdini_href_e.c \
    $$PWD/core/houdini_html_e.c \
    $$PWD/core/houdini_html_u.c \
    $$PWD/core/html.c \
    $$PWD/core/inlines.c \
    $$PWD/core/iterator.c \
    $$PWD/core/latex.c \
    $$PWD/core/linked_list.c \
    $$PWD/core/man.c \
    $$PWD/core/map.c \
    $$PWD/core/node.c \
    $$PWD/core/plaintext.c \
    $$PWD/core/plugin.c \
    $$PWD/core/references.c \
    $$PWD/core/registry.c \
    $$PWD/core/render.c \
    $$PWD/core/scanners.c \
    $$PWD/core/syntax_extension.c \
    $$PWD/core/utf8.c \
    $$PWD/core/xml.c \
    $$PWD/extensions/autolink.c \
    $$PWD/extensions/core-extensions.c \
    $$PWD/extensions/ext_scanners.c \
    $$PWD/extensions/strikethrough.c \
    $$PWD/extensions/table.c \
    $$PWD/extensions/tagfilter.c \
    $$PWD/extensions/tasklist.c