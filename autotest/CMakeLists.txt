# SPDX-FileCopyrightText: 2022-2025 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

project(unittest VERSION 1.0.0 LANGUAGES CXX)

add_subdirectory(asynctextwriter)
add_subdirectory(bookmark)
add_subdirectory(kerrorcodetest)
add_subdirectory(kresulttest)
add_subdirectory(library)

enable_testing(true)
