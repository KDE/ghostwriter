# SPDX-FileCopyrightText: 2022-2024 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)
if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Default build type: Release" FORCE)
endif()

if (NOT BUILD_TESTING)
    set (BUILD_TESTING OFF)
endif()

message("Generating makefile for ${CMAKE_BUILD_TYPE} build...")

# KF6 only
set(QT_MAJOR_VERSION 6)
set(QT_MIN_VERSION "6.5.0")
set(KF_MAJOR_VERSION 6)
set(KF_DEP_VERSION "6.0.0")
set(KF_MIN_VERSION "6.0.0")

# use C++20 like KF6 itself
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# KDE Application Version, managed by release script
set(RELEASE_SERVICE_VERSION_MAJOR "25")
set(RELEASE_SERVICE_VERSION_MINOR "03")
set(RELEASE_SERVICE_VERSION_MICRO "70")
set(RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(ghostwriter VERSION ${RELEASE_SERVICE_VERSION} LANGUAGES CXX)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)


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
include(ECMAddTests)
include(ECMPoQmTools)
include(KDEClangFormat)
include(KDEGitCommitHooks)
include(FeatureSummary)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/poqm")
    ecm_install_po_files_as_qm(poqm)
endif()

find_package(KF${KF_MAJOR_VERSION}DocTools ${KF_MIN_VERSION})
set_package_properties(KF${KF_MAJOR_VERSION}DocTools PROPERTIES DESCRIPTION
    "Tools to generate documentation"
    TYPE OPTIONAL
)

set(CMARK_TESTS OFF)
add_subdirectory(3rdparty/cmark-gfm EXCLUDE_FROM_ALL)
add_subdirectory(src)

if(KF${QT_MAJOR_VERSION}DocTools_FOUND)
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

if(BUILD_TESTING)
   add_subdirectory(autotest)
endif()


# See https://cmake.org/cmake/help/v3.15/prop_tgt/MACOSX_BUNDLE_INFO_PLIST.html
if(APPLE)
    string(TIMESTAMP CURRENT_YEAR "%Y")

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
    set(MACOSX_BUNDLE_LONG_VERSION_STRING "ghostwriter ${RELEASE_SERVICE_VERSION}")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "${RELEASE_SERVICE_VERSION}")
    set(MACOSX_BUNDLE_COPYRIGHT "2014-${CURRENT_YEAR} the ghostwriter authors")
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

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES src/**/*.cpp src/**/*.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)
