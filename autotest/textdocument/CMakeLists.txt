# SPDX-FileCopyrightText: 2024-2026 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(textdocumenttest VERSION 1.0.0 LANGUAGES CXX)

find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMAddTests)

find_package(Qt6 REQUIRED COMPONENTS Core Concurrent Gui Widgets Test)

find_package(
    KF6
    REQUIRED
    COMPONENTS
    CoreAddons
    KIO
)

qt_standard_project_setup()

ecm_add_test(
    textdocumenttest.cpp
    ../../src/document/textdocument.h
    ../../src/document/textdocument.cpp
    LINK_LIBRARIES
        PRIVATE
            Qt::Core
            Qt::Concurrent
            Qt::Gui
            Qt::Widgets
            Qt::Test
            KF6::CoreAddons
            KF6::KIOCore
            KF6::KIOWidgets
            ghostwriter_io
    TEST_NAME textdocumenttest
)

enable_testing(true)

