# SPDX-FileCopyrightText: 2022 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "22")
set(RELEASE_SERVICE_VERSION_MINOR "11")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

# Internal project release version
set(INTERNAL_RELEASE_VERSION_MAJOR "2")
set(INTERNAL_RELEASE_VERSION_MINOR "2")
set(INTERNAL_RELEASE_VERSION_MICRO "70")
set(INTERNAL_RELEASE_VERSION "${INTERNAL_RELEASE_VERSION_MAJOR}.${INTERNAL_RELEASE_VERSION_MINOR}.${INTERNAL_RELEASE_VERSION_MICRO}")

project(ghostwriter VERSION ${RELEASE_SERVICE_VERSION} LANGUAGES CXX)

set(QT_MIN_VERSION "5.15.2")
set(KF5_MIN_VERSION "5.90")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# Append to the module path so modules can be overridden from the command line.
list(APPEND CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(KDEInstallDirs)
include(KDECompilerSettings NO_POLICY_SCOPE)
remove_definitions(-DQT_NO_KEYWORDS)
remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
include(KDECMakeSettings)
include(ECMInstallIcons)
include(ECMAddAppIcon)
include(ECMPoQmTools)
include(FeatureSummary)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/poqm")
    ecm_install_po_files_as_qm(poqm)
endif()

find_package(KF5DocTools ${KF5_MIN_VERSION})
set_package_properties(KF5DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

set(QTAWESOME_USE_FREE ON)
add_subdirectory(3rdparty/QtAwesome)
set(CMARK_TESTS OFF)
add_subdirectory(3rdparty/cmark-gfm EXCLUDE_FROM_ALL)
add_subdirectory(src)

if(KF5DocTools_FOUND)
    kdoctools_install(po)
    add_subdirectory(doc)
endif()

# Add icon files to the application's source files to have CMake bundle them in the executable.
set(ICONS_FOLDER ${CMAKE_CURRENT_SOURCE_DIR}/resources/icons)

set(ICON_FILES
    ${ICONS_FOLDER}/sc-apps-ghostwriter.svg
    ${ICONS_FOLDER}/16-apps-ghostwriter.png
    ${ICONS_FOLDER}/22-apps-ghostwriter.png
    ${ICONS_FOLDER}/32-apps-ghostwriter.png
    ${ICONS_FOLDER}/64-apps-ghostwriter.png
    ${ICONS_FOLDER}/128-apps-ghostwriter.png
    ${ICONS_FOLDER}/256-apps-ghostwriter.png
)

ecm_add_app_icon(ICONS_SOURCES ICONS ${ICON_FILES})
target_sources(ghostwriter PRIVATE ${ICONS_SOURCES})

# See https://cmake.org/cmake/help/v3.15/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html
if(APPLE)
    set_property(
        TARGET ghostwriter
        PROPERTY MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/MacOSBundleInfo.plist.in
    )

    # These are substituted by CMake into MacOSBundleInfo.plist.in.
    set(MACOSX_BUNDLE_DISPLAY_NAME "ghostwriter")
    set(MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.ghostwriter")
    set(MACOSX_BUNDLE_BUNDLE_NAME "ghostwriter")
    set(MACOSX_BUNDLE_DISPLAY_NAME "ghostwriter")
    set(MACOSX_BUNDLE_INFO_STRING "ghostwriter - A Markdown Editor")
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "ghostwriter ${INTERNAL_RELEASE_VERSION} ${RELEASE_SERVICE_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${INTERNAL_RELEASE_VERSION_MAJOR}.${INTERNAL_RELEASE_VERSION_MINOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "2014-2022 the ghostwriter authors")
endif()

install(TARGETS ghostwriter ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_install_icons(
    ICONS ${ICON_FILES}
    DESTINATION ${KDE_INSTALL_ICONDIR}
    THEME hicolor
)

install(
    PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/resources/linux/org.kde.ghostwriter.desktop
    DESTINATION ${KDE_INSTALL_APPDIR}
)

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/resources/linux/org.kde.ghostwriter.metainfo.xml
    DESTINATION ${KDE_INSTALL_METAINFODIR}
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
