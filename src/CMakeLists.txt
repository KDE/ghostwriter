# SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(ghostwriter ../resources.qrc)

target_compile_definitions(ghostwriter PRIVATE APPVERSION="${INTERNAL_RELEASE_VERSION}")

target_include_directories(ghostwriter PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/cmark-gfm/src"
    "${PROJECT_SOURCE_DIR}/3rdparty/QtAwesome"
)

ecm_create_qm_loader(ghostwriter_QM_LOADER ghostwriter_qt)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
target_sources(ghostwriter PRIVATE
    abstractstatisticswidget.cpp
    appmain.cpp
    appsettings.cpp
    asynctextwriter.cpp
    bookmark.cpp
    cmarkgfmapi.cpp
    cmarkgfmexporter.cpp
    colorschemepreviewer.cpp
    commandlineexporter.cpp
    documentmanager.cpp
    documentstatistics.cpp
    documentstatisticswidget.cpp
    exportdialog.cpp
    exporter.cpp
    exporterfactory.cpp
    exportformat.cpp
    htmlpreview.cpp
    library.cpp
    localedialog.cpp
    mainwindow.cpp
    markdowndocument.cpp
    markdowneditor.cpp
    markdownhighlighter.cpp
    markdownast.cpp
    markdownnode.cpp
    memoryarena.cpp
    messageboxhelper.cpp
    outlinewidget.cpp
    preferencesdialog.cpp
    previewoptionsdialog.cpp
    previewproxy.cpp
    sandboxedwebpage.cpp
    sessionstatistics.cpp
    sessionstatisticswidget.cpp
    sidebar.cpp
    simplefontdialog.cpp
    statisticsindicator.cpp
    stylesheetbuilder.cpp
    textblockdata.h
    theme.cpp
    themeeditordialog.cpp
    themerepository.cpp
    themeselectiondialog.cpp
    timelabel.cpp
    findreplace.cpp
    spelling/spellcheckdecorator.cpp
    spelling/spellcheckdialog.cpp
    ${ghostwriter_QM_LOADER}
)

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(Hunspell REQUIRED IMPORTED_TARGET hunspell)
    target_link_libraries(ghostwriter PRIVATE PkgConfig::Hunspell)
endif()

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED COMPONENTS
    Core
    Concurrent
    Widgets
    Svg
    WebChannel
    Gui
    WebEngineWidgets
)

if (QT_MAJOR_VERSION STREQUAL "6")
    find_package(Qt6 REQUIRED COMPONENTS Core5Compat)
endif()

find_package(
    KF5 ${KF5_MIN_VERSION}
    REQUIRED
    COMPONENTS
    Sonnet
    CoreAddons
    XmlGui
    ConfigWidgets
    WidgetsAddons
)

# For whatever reason, MSVC has issues statically linking with cmark-gfm.
if (MSVC)
    set(CMARK_LIBS
        libcmark-gfm
        libcmark-gfm-extensions
    )
else()
    set(CMARK_LIBS
        libcmark-gfm_static
        libcmark-gfm-extensions_static
    )
endif()

target_link_libraries(ghostwriter PRIVATE
    Qt::Core
    Qt::Concurrent
    Qt::Widgets
    Qt::Svg
    Qt::WebChannel
    Qt::Gui
    Qt::WebEngineWidgets
    KF5::SonnetCore
    KF5::SonnetUi
    KF5::CoreAddons
    KF5::XmlGui
    KF5::ConfigWidgets
    QtAwesome
    ${CMARK_LIBS}
)

if (QT_MAJOR_VERSION STREQUAL "6")
    target_link_libraries(ghostwriter PRIVATE
        Qt::Core5Compat
    )
endif()

set_target_properties(ghostwriter PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

