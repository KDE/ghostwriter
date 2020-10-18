################################################################################
#
# Copyright (C) 2014-2020 wereturtle
# Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

lessThan(QT_MAJOR_VERSION, 5) {
    error("ghostwriter requires Qt 5.8 or greater")
}

isEqual(QT_MAJOR_VERSION, 5) : lessThan(QT_MINOR_VERSION, 8) {
    error("ghostwriter requires Qt 5.8 or greater")
}

TEMPLATE = app

QT += widgets concurrent svg webenginewidgets webengine webchannel gui

#CONFIG += debug
CONFIG += warn_on
CONFIG += c++11

# Set program version
isEmpty(VERSION) {
    win32 {
        VERSION = 2.0.0-rc
    } else {
        VERSION = v2.0.0-rc
    }
}
DEFINES += APPVERSION='\\"$${VERSION}\\"'
DEFINES += CMARK_GFM_STATIC_DEFINE
DEFINES += CMARK_GFM_EXTENSIONS_STATIC_DEFINE
DEFINES += CMARK_NO_SHORT_NAMES

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
}
else {
    DESTDIR = build/release
}

#DEFINES += QT_NO_DEBUG_OUTPUT=1
OBJECTS_DIR = $${DESTDIR}
MOC_DIR = $${DESTDIR}
RCC_DIR = $${DESTDIR}
UI_DIR = $${DESTDIR}

TARGET = ghostwriter

CONFIG+=fontAwesomeFree
include(3rdparty/QtAwesome/QtAwesome.pri)

# Input

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

    LIBS += -framework AppKit

    HEADERS += src/spelling/dictionary_provider_nsspellchecker.h

    OBJECTIVE_SOURCES += src/spelling/dictionary_provider_nsspellchecker.mm
} else:win32 {

    DEFINES += HUNSPELL_STATIC

    HEADERS += src/spelling/dictionary_provider_hunspell.h \
        src/spelling/dictionary_provider_voikko.h \
        src/spelling/hunspell/affentry.hxx \
        src/spelling/hunspell/affixmgr.hxx \
        src/spelling/hunspell/atypes.hxx \
        src/spelling/hunspell/baseaffix.hxx \
        src/spelling/hunspell/config.h \
        src/spelling/hunspell/csutil.hxx \
        src/spelling/hunspell/dictmgr.hxx \
        src/spelling/hunspell/filemgr.hxx \
        src/spelling/hunspell/hashmgr.hxx \
        src/spelling/hunspell/htypes.hxx \
        src/spelling/hunspell/hunspell.h \
        src/spelling/hunspell/hunspell.hxx \
        src/spelling/hunspell/hunvisapi.h \
        src/spelling/hunspell/hunvisapi.h.in \
        src/spelling/hunspell/hunzip.hxx \
        src/spelling/hunspell/langnum.hxx \
        src/spelling/hunspell/license.hunspell \
        src/spelling/hunspell/phonet.hxx \
        src/spelling/hunspell/replist.hxx \
        src/spelling/hunspell/suggestmgr.hxx \
        src/spelling/hunspell/w_char.hxx

    SOURCES += src/spelling/dictionary_provider_hunspell.cpp \
        src/spelling/dictionary_provider_voikko.cpp \
        src/spelling/hunspell/affentry.cxx \
        src/spelling/hunspell/affixmgr.cxx \
        src/spelling/hunspell/csutil.cxx \
        src/spelling/hunspell/filemgr.cxx \
        src/spelling/hunspell/hashmgr.cxx \
        src/spelling/hunspell/hunspell.cxx \
        src/spelling/hunspell/hunzip.cxx \
        src/spelling/hunspell/phonet.cxx \
        src/spelling/hunspell/replist.cxx \
        src/spelling/hunspell/suggestmgr.cxx

} else:unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += hunspell

    HEADERS += src/spelling/dictionary_provider_hunspell.h \
        src/spelling/dictionary_provider_voikko.h

    SOURCES += src/spelling/dictionary_provider_hunspell.cpp \
        src/spelling/dictionary_provider_voikko.cpp
}

INCLUDEPATH += src src/spelling src/cmark-gfm/core src/cmark-gfm/extensions

HEADERS += \
    src/abstractstatisticswidget.h \
    src/appsettings.h \
    src/cmarkgfmapi.h \
    src/cmarkgfmexporter.h \
    src/colorscheme.h \
    src/colorschemepreviewer.h \
    src/commandlineexporter.h \
    src/documenthistory.h \
    src/documentmanager.h \
    src/documentstatistics.h \
    src/documentstatisticswidget.h \
    src/exportdialog.h \
    src/exporter.h \
    src/exporterfactory.h \
    src/exportformat.h \
    src/htmlpreview.h \
    src/localedialog.h \
    src/mainwindow.h \
    src/markdowndocument.h \
    src/markdowneditor.h \
    src/markdowneditortypes.h \
    src/markdownhighlighter.h \
    src/markdownast.h \
    src/markdownnode.h \
    src/markdownstates.h \
    src/memoryarena.h \
    src/messageboxhelper.h \
    src/outlinewidget.h \
    src/preferencesdialog.h \
    src/previewoptionsdialog.h \
    src/sandboxedwebpage.h \
    src/sessionstatistics.h \
    src/sessionstatisticswidget.h \
    src/sidebar.h \
    src/simplefontdialog.h \
    src/stringobserver.h \
    src/stylesheetbuilder.h \
    src/textblockdata.h \
    src/theme.h \
    src/themeeditordialog.h \
    src/themerepository.h \
    src/themeselectiondialog.h \
    src/timelabel.h \
    src/findreplace.h \
    src/color_button.h \
    src/cmark-gfm/core/buffer.h \
    src/cmark-gfm/core/chunk.h \
    src/cmark-gfm/core/cmark_ctype.h \
    src/cmark-gfm/core/cmark-gfm_export.h \
    src/cmark-gfm/core/cmark-gfm-extension_api.h \
    src/cmark-gfm/core/cmark-gfm.h \
    src/cmark-gfm/core/cmark-gfm_version.h \
    src/cmark-gfm/core/config.h \
    src/cmark-gfm/core/footnotes.h \
    src/cmark-gfm/core/houdini.h \
    src/cmark-gfm/core/html.h \
    src/cmark-gfm/core/inlines.h \
    src/cmark-gfm/core/iterator.h \
    src/cmark-gfm/core/map.h \
    src/cmark-gfm/core/node.h \
    src/cmark-gfm/core/parser.h \
    src/cmark-gfm/core/plugin.h \
    src/cmark-gfm/core/references.h \
    src/cmark-gfm/core/registry.h \
    src/cmark-gfm/core/render.h \
    src/cmark-gfm/core/scanners.h \
    src/cmark-gfm/core/syntax_extension.h \
    src/cmark-gfm/core/utf8.h \
    src/cmark-gfm/extensions/autolink.h \
    src/cmark-gfm/extensions/cmark-gfm-core-extensions.h \
    src/cmark-gfm/extensions/cmark-gfm-extensions_export.h \
    src/cmark-gfm/extensions/ext_scanners.h \
    src/cmark-gfm/extensions/strikethrough.h \
    src/cmark-gfm/extensions/table.h \
    src/cmark-gfm/extensions/tagfilter.h \
    src/cmark-gfm/extensions/tasklist.h \
    src/spelling/abstract_dictionary.h \
    src/spelling/abstract_dictionary_provider.h \
    src/spelling/dictionary_manager.h \
    src/spelling/dictionary_ref.h \
    src/spelling/spell_checker.h

SOURCES += \
    src/abstractstatisticswidget.cpp \
    src/appmain.cpp \
    src/appsettings.cpp \
    src/cmarkgfmapi.cpp \
    src/cmarkgfmexporter.cpp \
    src/colorschemepreviewer.cpp \
    src/commandlineexporter.cpp \
    src/documenthistory.cpp \
    src/documentmanager.cpp \
    src/documentstatistics.cpp \
    src/documentstatisticswidget.cpp \
    src/exportdialog.cpp \
    src/exporter.cpp \
    src/exporterfactory.cpp \
    src/exportformat.cpp \
    src/htmlpreview.cpp \
    src/localedialog.cpp \
    src/mainwindow.cpp \
    src/markdowndocument.cpp \
    src/markdowneditor.cpp \
    src/markdownhighlighter.cpp \
    src/markdownast.cpp \
    src/markdownnode.cpp \
    src/memoryarena.cpp \
    src/messageboxhelper.cpp \
    src/outlinewidget.cpp \
    src/preferencesdialog.cpp \
    src/previewoptionsdialog.cpp \
    src/sandboxedwebpage.cpp \
    src/sessionstatistics.cpp \
    src/sessionstatisticswidget.cpp \
    src/sidebar.cpp \
    src/simplefontdialog.cpp \
    src/stringobserver.cpp \
    src/stylesheetbuilder.cpp \
    src/theme.cpp \
    src/themeeditordialog.cpp \
    src/themerepository.cpp \
    src/themeselectiondialog.cpp \
    src/timelabel.cpp \
    src/color_button.cpp \
    src/findreplace.cpp \
    src/cmark-gfm/core/arena.c \
    src/cmark-gfm/core/blocks.c \
    src/cmark-gfm/core/buffer.c \
    src/cmark-gfm/core/cmark.c \
    src/cmark-gfm/core/cmark_ctype.c \
    src/cmark-gfm/core/commonmark.c \
    src/cmark-gfm/core/footnotes.c \
    src/cmark-gfm/core/houdini_href_e.c \
    src/cmark-gfm/core/houdini_html_e.c \
    src/cmark-gfm/core/houdini_html_u.c \
    src/cmark-gfm/core/html.c \
    src/cmark-gfm/core/inlines.c \
    src/cmark-gfm/core/iterator.c \
    src/cmark-gfm/core/latex.c \
    src/cmark-gfm/core/linked_list.c \
    src/cmark-gfm/core/man.c \
    src/cmark-gfm/core/map.c \
    src/cmark-gfm/core/node.c \
    src/cmark-gfm/core/plaintext.c \
    src/cmark-gfm/core/plugin.c \
    src/cmark-gfm/core/references.c \
    src/cmark-gfm/core/registry.c \
    src/cmark-gfm/core/render.c \
    src/cmark-gfm/core/scanners.c \
    src/cmark-gfm/core/syntax_extension.c \
    src/cmark-gfm/core/utf8.c \
    src/cmark-gfm/core/xml.c \
    src/cmark-gfm/extensions/autolink.c \
    src/cmark-gfm/extensions/core-extensions.c \
    src/cmark-gfm/extensions/ext_scanners.c \
    src/cmark-gfm/extensions/strikethrough.c \
    src/cmark-gfm/extensions/table.c \
    src/cmark-gfm/extensions/tagfilter.c \
    src/cmark-gfm/extensions/tasklist.c \
    src/spelling/dictionary_manager.cpp \
    src/spelling/spell_checker.cpp

# Generate translations
TRANSLATIONS = $$files(translations/ghostwriter_*.ts)
qtPrepareTool(LRELEASE, lrelease)
updateqm.input = TRANSLATIONS
updateqm.output = $${DESTDIR}/translations/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE -silent ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.CONFIG += no_link target_predeps no_check_exist
QMAKE_EXTRA_COMPILERS += updateqm

RESOURCES += resources.qrc

macx {
    # generate property list for macOS
    ICON = resources/mac/ghostwriter.icns
    QMAKE_INFO_PLIST = resources/mac/Info.plist

    # run macdeployqt after building ghostwriter - copies frameworks & libraries.
    QMAKE_POST_LINK =  macdeployqt $$sprintf("%1/%2/%3.app", $$OUT_PWD, $$DESTDIR, $$TARGET)

    # copy translations using a helper script.
    QMAKE_POST_LINK += $$escape_expand(\n\t) $$PWD/resources/mac/macdeploy_helper.sh \
        $${OUT_PWD}/$${DESTDIR}/$${TARGET}.app/Contents/Resources/translations \
        $${OUT_PWD}/$${DESTDIR}/translations

} else:win32 {
    RC_FILE = resources/windows/ghostwriter.rc
}
else:unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }
    isEmpty(BINDIR) {
        BINDIR = $$PREFIX/bin
    }
    isEmpty(DATADIR) {
        DATADIR = $$PREFIX/share
    }
    DEFINES += DATADIR=\\\"$${DATADIR}/ghostwriter\\\"

    target.path = $$BINDIR

    pixmap.files = resources/linux/icons/ghostwriter.xpm
    pixmap.path = $$DATADIR/pixmaps

    icon.files = resources/linux/icons/hicolor/*
    icon.path = $$DATADIR/icons/hicolor

    desktop.files = resources/linux/ghostwriter.desktop
    desktop.path = $$DATADIR/applications/

    appdata.files = resources/linux/ghostwriter.appdata.xml
    appdata.path = $$DATADIR/metainfo/

    man.files = resources/linux/ghostwriter.1
    man.path = $$PREFIX/share/man/man1

    qm.files = $${DESTDIR}/translations
    qm.path = $$DATADIR/ghostwriter
    qm.CONFIG += no_check_exist

    INSTALLS += target icon pixmap desktop appdata man qm
}
