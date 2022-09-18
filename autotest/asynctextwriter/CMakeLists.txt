# SPDX-FileCopyrightText: 2022 Megan Conkle <wereturtle@airpost.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(asynctextwritertest VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test Concurrent Widgets)

if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Test Concurrent Widgets)
endif()

qt_standard_project_setup()

add_executable(asynctextwritertest
    asynctextwritertest.cpp
    ../../src/asynctextwriter.h
    ../../src/asynctextwriter.cpp
)

add_test(asynctextwritertest asynctextwritertest)
enable_testing(true)

target_link_libraries(asynctextwritertest PRIVATE Qt::Core Qt::Test Qt::Concurrent Qt::Widgets)
