################################################################################
#
# Copyright (C) 2014-2018 wereturtle
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
    error("ghostwriter requires Qt 5.2 or greater")
}

isEqual(QT_MAJOR_VERSION, 5) : lessThan(QT_MINOR_VERSION, 2) {
    error("ghostwriter requires Qt 5.2 or greater")
}

TEMPLATE = app

QT += printsupport webkitwidgets widgets concurrent svg

CONFIG -= debug
CONFIG += warn_on

# Set program version
isEmpty(VERSION) {
    VERSION = v1.7.1
}
DEFINES += APPVERSION='\\"$${VERSION}\\"'


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

# Input

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

    LIBS += -framework AppKit

    HEADERS += src/spelling/dictionary_provider_nsspellchecker.h

    OBJECTIVE_SOURCES += src/spelling/dictionary_provider_nsspellchecker.mm
} else:win32 {

    INCLUDEPATH += src/spelling/hunspell

    HEADERS += src/spelling/dictionary_provider_hunspell.h \
        src/spelling/dictionary_provider_voikko.h

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

INCLUDEPATH += src src/spelling

HEADERS += src/MainWindow.h \
    src/MarkdownEditor.h \
    src/Token.h \
    src/HtmlPreview.h \
    src/ExportFormat.h \
    src/Exporter.h \
    src/Theme.h \
    src/ThemeFactory.h \
    src/CommandLineExporter.h \
    src/TextBlockData.h \
    src/HudWindowTypes.h \
    src/HudWindow.h \
    src/ThemeSelectionDialog.h \
    src/ThemePreviewer.h \
    src/ThemeEditorDialog.h \
    src/ExporterFactory.h \
    src/ColorHelper.h \
    src/MarkdownEditorTypes.h \
    src/AppSettings.h \
    src/DocumentManager.h \
    src/TextDocument.h \
    src/DocumentHistory.h \
    src/ExportDialog.h \
    src/Outline.h \
    src/MarkdownStates.h \
    src/MarkdownHighlighter.h \
    src/MarkdownStyles.h \
    src/MessageBoxHelper.h \
    src/GraphicsFadeEffect.h \
    src/SundownExporter.h \
    src/StyleSheetManagerDialog.h \
    src/SimpleFontDialog.h \
    src/HighlighterLineStates.h \
    src/HighlightTokenizer.h \
    src/MarkdownTokenizer.h \
    src/TimeLabel.h \
    src/LocaleDialog.h \
    src/AbstractStatisticsWidget.h \
    src/DocumentStatistics.h \
    src/DocumentStatisticsWidget.h \
    src/SessionStatistics.h \
    src/SessionStatisticsWidget.h \
    src/PreferencesDialog.h \
    src/PreviewOptionsDialog.h \
    src/find_dialog.h \
    src/image_button.h \
    src/color_button.h \
    src/spelling/abstract_dictionary.h \
    src/spelling/abstract_dictionary_provider.h \
    src/spelling/dictionary_manager.h \
    src/spelling/dictionary_ref.h \
    src/spelling/spell_checker.h \
    src/sundown/autolink.h \
    src/sundown/buffer.h \
    src/sundown/houdini.h \
    src/sundown/html_blocks.h \
    src/sundown/html.h \
    src/sundown/markdown.h \
    src/sundown/stack.h

SOURCES += src/AppMain.cpp \
    src/MainWindow.cpp \
    src/MarkdownEditor.cpp \
    src/Token.cpp \
    src/HtmlPreview.cpp \
    src/ExportFormat.cpp \
    src/Theme.cpp \
    src/ThemeFactory.cpp \
    src/CommandLineExporter.cpp \
    src/HudWindow.cpp \
    src/ThemeSelectionDialog.cpp \
    src/ThemePreviewer.cpp \
    src/ThemeEditorDialog.cpp \
    src/Exporter.cpp \
    src/ExporterFactory.cpp \
    src/ColorHelper.cpp \
    src/AppSettings.cpp \
    src/DocumentManager.cpp \
    src/TextDocument.cpp \
    src/DocumentHistory.cpp \
    src/ExportDialog.cpp \
    src/Outline.cpp \
    src/MarkdownHighlighter.cpp \
    src/MessageBoxHelper.cpp \
    src/GraphicsFadeEffect.cpp \
    src/StyleSheetManagerDialog.cpp \
    src/SimpleFontDialog.cpp \
    src/SundownExporter.cpp \
    src/HighlightTokenizer.cpp \
    src/MarkdownTokenizer.cpp \
    src/TimeLabel.cpp \
    src/LocaleDialog.cpp \
    src/AbstractStatisticsWidget.cpp \
    src/SessionStatistics.cpp \
    src/SessionStatisticsWidget.cpp \
    src/DocumentStatistics.cpp \
    src/DocumentStatisticsWidget.cpp \
    src/PreferencesDialog.cpp \
    src/PreviewOptionsDialog.cpp \
    src/find_dialog.cpp \
    src/image_button.cpp \
    src/color_button.cpp \
    src/spelling/dictionary_manager.cpp \
    src/spelling/spell_checker.cpp \
    src/sundown/autolink.c \
    src/sundown/buffer.c \
    src/sundown/houdini_href_e.c \
    src/sundown/houdini_html_e.c \
    src/sundown/html_smartypants.c \
    src/sundown/html.c \
    src/sundown/markdown.c \
    src/sundown/stack.c

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
} else:unix {
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
    appdata.path = $$DATADIR/appdata/

    man.files = resources/linux/ghostwriter.1
    man.path = $$PREFIX/share/man/man1

    qm.files = $${DESTDIR}/translations/*.qm
    qm.path = $$DATADIR/ghostwriter/translations
    qm.CONFIG += no_check_exist

    INSTALLS += target icon pixmap desktop appdata man qm
}
