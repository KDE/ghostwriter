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
set(INTERNAL_RELEASE_VERSION "2.2.70")

project(ghostwriter VERSION ${RELEASE_SERVICE_VERSION} LANGUAGES CXX)

set(QT_MIN_VERSION "5.15")
# set(KF5_MIN_VERSION "5.90")
# set(QT_MAJOR_VERSION "5")
# set(QT_MINOR_VERSION "15")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_PACKAGE_DEPENDENCIES
    Core
    Concurrent
    Widgets
    Svg
    WebChannel
    Gui
    WebEngineWidgets
)

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED COMPONENTS
    ${QT_PACKAGE_DEPENDENCIES}
)

if (NOT Qt5_FOUND)
    find_package(Qt6 REQUIRED COMPONENTS
        ${QT_PACKAGE_DEPENDENCIES}
        Qt6Core5Compat
    )
endif()

# if (QT_MAJOR_VERSION STREQUAL "6")
#     find_package(Qt6Core5Compat)
# endif()
set(QT_MAJOR_VERSION ${QT_DEFAULT_MAJOR_VERSION})

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
# set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

# configure_file(ghostwriterconfig.h.in ghostwriterconfig.h)

# include(KDEInstallDirs)
# include(KDECompilerSettings NO_POLICY_SCOPE)
# include(KDECMakeSettings)
# include(ECMGenerateHeaders)
# include(ECMInstallIcons)
include(FeatureSummary)

set(QTAWESOME_USE_FREE ON)
add_subdirectory(3rdparty/QtAwesome)
add_subdirectory(3rdparty/cmark-gfm)
add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
