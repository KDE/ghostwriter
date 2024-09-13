# SPDX-FileCopyrightText: 2022-2023 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(UI_FILES settings/preferencesdialog.ui)

add_executable(ghostwriter ../resources.qrc ../icons.qrc ${UI_FILES})

target_compile_definitions(ghostwriter PRIVATE APPVERSION="${RELEASE_SERVICE_VERSION}")

target_include_directories(ghostwriter PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_BINARY_DIR}/../3rdparty/cmark-gfm/src"
)

ecm_create_qm_loader(ghostwriter_QM_LOADER ghostwriter_qt)

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
    theme/chromecolors.cpp
    theme/colorschemepreviewer.cpp
    theme/stylesheetbuilder.cpp
    theme/svgicontheme.cpp
    theme/theme.cpp
    theme/themeeditordialog.cpp
    theme/themerepository.cpp
    theme/themeselectiondialog.cpp
    appactions.cpp
    appmain.cpp
    bookmark.cpp
    documentmanager.cpp
    findreplace.cpp
    library.cpp
    logging.cpp
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
    Concurrent
    Core
    Gui
    Svg
    WebChannel
    WebEngineWidgets
    Widgets
)

find_package(
    KF${KF_MAJOR_VERSION} ${KF_MIN_VERSION}
    REQUIRED
    COMPONENTS
    ConfigWidgets
    CoreAddons
    Sonnet
    WidgetsAddons
    XmlGui
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
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::Svg
    Qt::WebChannel
    Qt::WebEngineWidgets
    Qt::Widgets
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::ConfigWidgets
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::SonnetCore
    KF${KF_MAJOR_VERSION}::SonnetUi
    KF${KF_MAJOR_VERSION}::XmlGui
    ${CMARK_LIBS}
)

set_target_properties(ghostwriter PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

