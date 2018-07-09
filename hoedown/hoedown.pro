#-------------------------------------------------
#
# Project created by QtCreator 2018-07-08T02:57:41
#
#-------------------------------------------------

QT -= core gui

#TARGET = hoedown
TEMPLATE = lib
CONFIG += staticlib

QMAKE_CFLAGS += -g -O3 -ansi -pedantic -w

HEADERS += \
    autolink.h \
    buffer.h \
    document.h \
    escape.h \
    html.h \
    stack.h \
    version.h

SOURCES += \
    autolink.c \
    buffer.c \
    document.c \
    escape.c \
    html.c \
    html_blocks.c \
    html_smartypants.c \
    stack.c \
    version.c

