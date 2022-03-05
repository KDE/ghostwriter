DEFINES += CMARK_GFM_STATIC_DEFINE
DEFINES += CMARK_GFM_EXTENSIONS_STATIC_DEFINE
DEFINES += CMARK_NO_SHORT_NAMES

INCLUDEPATH += $$PWD/src $$PWD/extensions

HEADERS += \
    $$PWD/src/buffer.h \
    $$PWD/src/chunk.h \
    $$PWD/src/cmark_ctype.h \
    $$PWD/src/cmark-gfm_export.h \
    $$PWD/src/cmark-gfm-extension_api.h \
    $$PWD/src/cmark-gfm.h \
    $$PWD/src/cmark-gfm_version.h \
    $$PWD/src/config.h \
    $$PWD/src/footnotes.h \
    $$PWD/src/houdini.h \
    $$PWD/src/html.h \
    $$PWD/src/inlines.h \
    $$PWD/src/iterator.h \
    $$PWD/src/map.h \
    $$PWD/src/node.h \
    $$PWD/src/parser.h \
    $$PWD/src/plugin.h \
    $$PWD/src/references.h \
    $$PWD/src/registry.h \
    $$PWD/src/render.h \
    $$PWD/src/scanners.h \
    $$PWD/src/syntax_extension.h \
    $$PWD/src/utf8.h \
    $$PWD/extensions/autolink.h \
    $$PWD/extensions/cmark-gfm-core-extensions.h \
    $$PWD/extensions/cmark-gfm-extensions_export.h \
    $$PWD/extensions/ext_scanners.h \
    $$PWD/extensions/strikethrough.h \
    $$PWD/extensions/table.h \
    $$PWD/extensions/tagfilter.h \
    $$PWD/extensions/tasklist.h

SOURCES += \
    $$PWD/src/arena.c \
    $$PWD/src/blocks.c \
    $$PWD/src/buffer.c \
    $$PWD/src/cmark.c \
    $$PWD/src/cmark_ctype.c \
    $$PWD/src/commonmark.c \
    $$PWD/src/footnotes.c \
    $$PWD/src/houdini_href_e.c \
    $$PWD/src/houdini_html_e.c \
    $$PWD/src/houdini_html_u.c \
    $$PWD/src/html.c \
    $$PWD/src/inlines.c \
    $$PWD/src/iterator.c \
    $$PWD/src/latex.c \
    $$PWD/src/linked_list.c \
    $$PWD/src/man.c \
    $$PWD/src/map.c \
    $$PWD/src/node.c \
    $$PWD/src/plaintext.c \
    $$PWD/src/plugin.c \
    $$PWD/src/references.c \
    $$PWD/src/registry.c \
    $$PWD/src/render.c \
    $$PWD/src/scanners.c \
    $$PWD/src/syntax_extension.c \
    $$PWD/src/utf8.c \
    $$PWD/src/xml.c \
    $$PWD/extensions/autolink.c \
    $$PWD/extensions/core-extensions.c \
    $$PWD/extensions/ext_scanners.c \
    $$PWD/extensions/strikethrough.c \
    $$PWD/extensions/table.c \
    $$PWD/extensions/tagfilter.c \
    $$PWD/extensions/tasklist.c
