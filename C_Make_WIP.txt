################################################################################
#
# Copyright (C) 2014-2020 wereturtle
# Copyright (C) 2009, 2010, 2011, 2012, 2013, 2014 Graeme Gott <graeme@gottcode.org>
# CMake configuration (C) 2020 FeRD (Frank Dana)
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
cmake_minimum_required(VERSION 3.1...3.19 FATAL_ERROR)

set(ghostwriter_VERSION_FULL "2.0.0-rc")

# Extract Major.Minor.Patch[.Tweak] version from full string
string(REGEX MATCH
  "([0-9]+\.[0-9]+\.[0-9]+)(\.[0-9]+)?"
  _version
  ${ghostwriter_VERSION_FULL}
)

# Must be set before project() command
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.10" CACHE STRING
  "Deployment Target for MacOS" FORCE)

project(ghostwriter VERSION ${_version} LANGUAGES C CXX)

# Support for standard install paths
include(GNUInstallDirs)
# Report configuration state of dependencies and options
include(FeatureSummary)

set(ghostwriter_SOURCES
  src/abstractstatisticswidget.cpp
  src/appmain.cpp
  src/appsettings.cpp
  src/cmarkgfmapi.cpp
  src/cmarkgfmexporter.cpp
  src/colorschemepreviewer.cpp
  src/commandlineexporter.cpp
  src/documenthistory.cpp
  src/documentmanager.cpp
  src/documentstatistics.cpp
  src/documentstatisticswidget.cpp
  src/exportdialog.cpp
  src/exporter.cpp
  src/exporterfactory.cpp
  src/exportformat.cpp
  src/htmlpreview.cpp
  src/localedialog.cpp
  src/mainwindow.cpp
  src/markdowndocument.cpp
  src/markdowneditor.cpp
  src/markdownhighlighter.cpp
  src/markdownast.cpp
  src/markdownnode.cpp
  src/memoryarena.cpp
  src/messageboxhelper.cpp
  src/outlinewidget.cpp
  src/preferencesdialog.cpp
  src/previewoptionsdialog.cpp
  src/sandboxedwebpage.cpp
  src/sessionstatistics.cpp
  src/sessionstatisticswidget.cpp
  src/sidebar.cpp
  src/simplefontdialog.cpp
  src/stringobserver.cpp
  src/stylesheetbuilder.cpp
  src/textblockdata.cpp
  src/theme.cpp
  src/themeeditordialog.cpp
  src/themerepository.cpp
  src/themeselectiondialog.cpp
  src/timelabel.cpp
  src/color_button.cpp
  src/findreplace.cpp
  src/spelling/dictionary_manager.cpp
  src/spelling/spell_checker.cpp
)

set(ghostwriter_HEADERS
  src/abstractstatisticswidget.h
  src/appsettings.h
  src/cmarkgfmapi.h
  src/cmarkgfmexporter.h
  src/colorscheme.h
  src/colorschemepreviewer.h
  src/commandlineexporter.h
  src/documenthistory.h
  src/documentmanager.h
  src/documentstatistics.h
  src/documentstatisticswidget.h
  src/exportdialog.h
  src/exporter.h
  src/exporterfactory.h
  src/exportformat.h
  src/htmlpreview.h
  src/localedialog.h
  src/mainwindow.h
  src/markdowndocument.h
  src/markdowneditor.h
  src/markdowneditortypes.h
  src/markdownhighlighter.h
  src/markdownast.h
  src/markdownnode.h
  src/markdownstates.h
  src/memoryarena.h
  src/messageboxhelper.h
  src/outlinewidget.h
  src/preferencesdialog.h
  src/previewoptionsdialog.h
  src/sandboxedwebpage.h
  src/sessionstatistics.h
  src/sessionstatisticswidget.h
  src/sidebar.h
  src/simplefontdialog.h
  src/stringobserver.h
  src/stylesheetbuilder.h
  src/textblockdata.h
  src/theme.h
  src/themeeditordialog.h
  src/themerepository.h
  src/themeselectiondialog.h
  src/timelabel.h
  src/findreplace.h
  src/color_button.h
  src/spelling/abstract_dictionary.h
  src/spelling/abstract_dictionary_provider.h
  src/spelling/dictionary_manager.h
  src/spelling/dictionary_ref.h
  src/spelling/spell_checker.h
)

add_executable(ghostwriter
  ${ghostwriter_SOURCES}
  resources.qrc
)

target_include_directories(ghostwriter PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}
)

set_target_properties(ghostwriter PROPERTIES
  AUTOMOC TRUE
  AUTORCC TRUE
)

target_compile_features(ghostwriter PRIVATE
  cxx_std_11
  # XXX: Plus any specific features the code uses from this list:
  # https://cmake.org/cmake/help/latest/prop_gbl/CMAKE_CXX_KNOWN_FEATURES.html
)

# Compile warnings
target_compile_options(ghostwriter PRIVATE
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:-Wall>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

if(WIN32)
  target_compile_definitions(ghostwriter PRIVATE
    APPVERSION="${ghostwriter_VERSION_FULL}"
  )
  target_sources(ghostwriter PRIVATE
    resources/windows/ghostwriter.rc
  )
else()
  target_compile_definitions(ghostwriter PRIVATE
    APPVERSION="v${ghostwriter_VERSION_FULL}"
  )
  if(APPLE)
    target_link_libraries(ghostwriter PRIVATE
      -framework AppKit
    )
    # Variables used in Info.plist template
    set(ICON resources/mac/ghostwriter.icns)
    set(SHORT_VERSION ${PROJECT_VERSION})
    configure_file(resources/mac/Info.plist
      Info.plist
    )
    set_target_properties(ghostwriter PROPERTIES
      MACOSX_BUNDLE TRUE
      MACOSX_BUNDLE_INFO_PLIST
        ${CMAKE_CURRENT_BUILD_DIR}/Info.plist
    )
  endif()
endif()

# Qt5 dependencies
set(ghostwriter_QT_COMPONENTS
  Gui
  Widgets
  Concurrent
  Svg
  WebChannel
  WebEngine
  WebEngineWidgets
)
#list(JOIN ghostwriter_QT_COMPONENTS " " _comp_list)
find_package(Qt5 5.8 COMPONENTS ${ghostwriter_QT_COMPONENTS} REQUIRED)
#unset(_comp_list)

foreach(_comp ${ghostwriter_QT_COMPONENTS})
  target_link_libraries(ghostwriter PRIVATE Qt5::${_comp})
endforeach()

target_sources(ghostwriter PRIVATE
  ${ghostwriter_TRANSLATIONS}
)

# QtAwesome support for Font Awesome Free
set(QTAWESOME_USE_FREE TRUE CACHE BOOL "Select Font Awesome Free" FORCE)
add_subdirectory(3rdparty/QtAwesome)
target_link_libraries(ghostwriter PRIVATE QtAwesome)

# Markdown support
add_subdirectory(3rdparty/cmark-gfm)
target_link_libraries(ghostwriter PRIVATE CommonMarkGfm)

# Spellchecking
target_include_directories(ghostwriter PRIVATE
  src/spelling
)
if(APPLE)
  target_sources(ghostwriter PRIVATE
    src/spelling/dictionary_provider_nsspellchecker.mm
  )
else()
  target_sources(ghostwriter PRIVATE
    src/spelling/dictionary_provider_hunspell.cpp
    src/spelling/dictionary_provider_voikko.cpp
  )
  if(WIN32)
    add_subdirectory(3rdparty/hunspell)
  elseif(UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(hunspell REQUIRED IMPORTED_TARGET hunspell)
    target_link_libraries(ghostwriter PRIVATE PkgConfig::hunspell)
  endif()
endif()

# Translations
add_subdirectory(translations)

# Installation
install(TARGETS ghostwriter
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
if(UNIX AND NOT APPLE)
  install(DIRECTORY resources/linux/icons/hicolor
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons
  )
  install(FILES resources/linux/ghostwriter.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
  )
  install(FILES resources/linux/ghostwriter.appdata.xml
    DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo
  )
  install(FILES resources/linux/ghostwriter.1
    DESTINATION ${CMAKE_INSTALL_MANDIR}/man1
  )
endif()

feature_summary(WHAT ALL
  INCLUDE_QUIET_PACKAGES
  FATAL_ON_MISSING_REQUIRED_PACKAGES
  DESCRIPTION "ghostwriter configuration:")
