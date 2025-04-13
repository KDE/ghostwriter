# SPDX-FileCopyrightText: 2024 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(bookmarktest VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

qt_standard_project_setup()

ecm_add_test(
    kresulttest.cpp
    ../../src/util/kresult.h
    ../../src/util/kerrorcode.h
    LINK_LIBRARIES
        PRIVATE
            Qt::Core
            Qt::Test
    TEST_NAME kresulttest
)

enable_testing(true)

