################################################################################
#
# Copyright (C) 2022 wereturtle
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
################################################################################

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
