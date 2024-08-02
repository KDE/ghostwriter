# SPDX-FileCopyrightText: 2022-2023 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(asynctextwritertest VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Test Concurrent Widgets)

qt_standard_project_setup()

ecm_add_test(
    asynctextwritertest.cpp
    ../../src/editor/asynctextwriter.h
    ../../src/editor/asynctextwriter.cpp
    LINK_LIBRARIES
        PRIVATE
            Qt::Core
            Qt::Test
            Qt::Concurrent
            Qt::Widgets
    TEST_NAME asynctextwritertest
)

enable_testing(true)

