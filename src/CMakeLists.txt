# SPDX-FileCopyrightText: 2022-2023 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

add_executable(ghostwriter ../resources.qrc)

target_compile_definitions(ghostwriter PRIVATE APPVERSION="${RELEASE_SERVICE_VERSION}")

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
    editor/actions/inlinemarkuptoggle.cpp
    editor/asynctextwriter.cpp
    editor/markdowndocument.cpp
    editor/markdowneditor.cpp
    editor/markdownhighlighter.cpp
    editor/textblockdata.h
    export/cmarkgfmexporter.cpp
    export/commandlineexporter.cpp
    export/exportdialog.cpp
    export/exporter.cpp
    export/exporterfactory.cpp
    export/exportformat.cpp
    markdown/cmarkgfmapi.cpp
    markdown/markdownast.cpp
    markdown/markdownnode.cpp
    markdown/memoryarena.cpp
    preview/htmlpreview.cpp
    preview/previewproxy.cpp
    preview/sandboxedwebpage.cpp
    settings/appsettings.cpp
    settings/localedialog.cpp
    settings/preferencesdialog.cpp
    settings/previewoptionsdialog.cpp
    settings/simplefontdialog.cpp
    spelling/spellcheckdecorator.cpp
    spelling/spellcheckdialog.cpp
    statistics/abstractstatisticswidget.cpp
    statistics/documentstatistics.cpp
    statistics/documentstatisticswidget.cpp
    statistics/sessionstatistics.cpp
    statistics/sessionstatisticswidget.cpp
    statistics/statisticsindicator.cpp
    theme/colorschemepreviewer.cpp
    theme/stylesheetbuilder.cpp
    theme/theme.cpp
    theme/themeeditordialog.cpp
    theme/themerepository.cpp
    theme/themeselectiondialog.cpp
    appmain.cpp
    bookmark.cpp
    documentmanager.cpp
    findreplace.cpp
    library.cpp
    mainwindow.cpp
    messageboxhelper.cpp
    outlinewidget.cpp
    sidebar.cpp
    timelabel.cpp
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

