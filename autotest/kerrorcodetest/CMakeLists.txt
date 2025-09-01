# SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(kerrorcodetest VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

qt_standard_project_setup()

ecm_add_test(
    kerrorcodetest.cpp
    ../../src/util/kerrorcode.h
    LINK_LIBRARIES
        PRIVATE
            Qt::Core
            Qt::Test
    TEST_NAME kerrorcodetest
)

enable_testing(true)
