################################################################################
#
# Copyright (C) 2014-2022 wereturtle
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

VERSION = 2.1.5-staging
    
lessThan(QT_MAJOR_VERSION, 5) {
    error("ghostwriter requires Qt 5.8 or greater")
}

isEqual(QT_MAJOR_VERSION, 5) : lessThan(QT_MINOR_VERSION, 8) {
    error("ghostwriter requires Qt 5.8 or greater")
}

TEMPLATE = app

QT += widgets concurrent svg webchannel gui webenginewidgets

equals(QT_MAJOR_VERSION,6): QT += core5compat

#CONFIG += debug
CONFIG += warn_on
CONFIG += c++17

DEFINES += APPVERSION='\\"$${VERSION}\\"'

CONFIG(debug, debug|release) {
    DESTDIR = build/debug
}
else {
    DESTDIR = build/release
}

DEFINES += QT_NO_DEBUG_OUTPUT=1
OBJECTS_DIR = $${DESTDIR}
MOC_DIR = $${DESTDIR}
RCC_DIR = $${DESTDIR}
UI_DIR = $${DESTDIR}

TARGET = ghostwriter

CONFIG+=fontAwesomeFree
include(3rdparty/QtAwesome/QtAwesome.pri)
include(3rdparty/cmark-gfm/cmark-gfm.pri)

# Input

macx {
    QMAKE_LFLAGS         += "-mmacosx-version-min=12.2"
    QMAKE_CXXFLAGS       += "-mmacosx-version-min=12.2"
    QMAKE_CFLAGS_RELEASE += "-mmacosx-version-min=12.2"
    QMAKE_CXXFLAGS       += "-mmacosx-version-min=12.2"

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 12.2

    LIBS += -framework AppKit

    HEADERS += src/spelling/nsspellcheckerprovider.h

    OBJECTIVE_SOURCES += src/spelling/nsspellcheckerprovider.mm

    INCLUDEPATH += /opt/homebrew/Cellar/qt/6.2.2/lib/QtCore5Compat.framework/Versions/A/Headers
} else:win32 {
    include(3rdparty/hunspell/hunspell.pri)
    INCLUDEPATH += 3rdparty/hunspell
    HEADERS += src/spelling/hunspellprovider.h
    SOURCES += src/spelling/hunspellprovider.cpp

} else:unix {
    CONFIG += link_pkgconfig
    PKGCONFIG += hunspell

    HEADERS += src/spelling/hunspellprovider.h
    SOURCES += src/spelling/hunspellprovider.cpp
}

INCLUDEPATH += src src/spelling

HEADERS += \
    src/abstractstatisticswidget.h \
    src/appsettings.h \
    src/asynctextwriter.h \
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
    src/statisticsindicator.h \
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
    src/spelling/dictionary.h \
    src/spelling/dictionaryprovider.h \
    src/spelling/dictionarymanager.h \
    src/spelling/spellchecker.h \
    src/spelling/spellcheckdecorator.h

SOURCES += \
    src/abstractstatisticswidget.cpp \
    src/appmain.cpp \
    src/appsettings.cpp \
    src/asynctextwriter.cpp \
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
    src/statisticsindicator.cpp \
    src/stringobserver.cpp \
    src/stylesheetbuilder.cpp \
    src/theme.cpp \
    src/themeeditordialog.cpp \
    src/themerepository.cpp \
    src/themeselectiondialog.cpp \
    src/timelabel.cpp \
    src/color_button.cpp \
    src/findreplace.cpp \
    src/spelling/dictionarymanager.cpp \
    src/spelling/spellchecker.cpp \
    src/spelling/spellcheckdecorator.cpp

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

    INSTALLS += target icon desktop appdata man qm
}
