# build sundown static lib.

QT -= core gui

TEMPLATE = lib
CONFIG += staticlib


QMAKE_CFLAGS += -g -O3 -fPIC -w

HEADERS += \
    autolink.h \
    buffer.h \
    houdini.h \
    html.h \
    html_blocks.h \
    markdown.h \
    stack.h

SOURCES += \
    autolink.c \
    buffer.c \
    houdini_href_e.c \
    houdini_html_e.c \
    html.c \
    html_smartypants.c \
    markdown.c \
    stack.c


