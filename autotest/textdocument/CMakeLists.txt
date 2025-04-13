# SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later


cmake_minimum_required(VERSION 3.16)

project(librarytest VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Test Concurrent)

qt_standard_project_setup()

ecm_add_test(
    textdocumenttest.cpp
    ../../src/document/textdocument.h
    ../../src/document/textdocument.cpp
    ../../src/document/asynctextwriter.h
    ../../src/document/asynctextwriter.cpp
    LINK_LIBRARIES
        PRIVATE
            Qt::Core
            Qt::Test
            Qt::Concurrent
    TEST_NAME textdocumenttest
)

enable_testing(true)

