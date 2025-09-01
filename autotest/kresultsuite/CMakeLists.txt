# SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

# Find ECM and include the module
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMAddTests)

find_package(Qt6 REQUIRED COMPONENTS Core Test)

qt_standard_project_setup()

# Create the test executables
ecm_add_test(
    kerrorcodetest.cpp
    TEST_NAME kerrorcode_test
    LINK_LIBRARIES
        PRIVATE
            Qt6::Core
            Qt6::Test
            ghostwriter_utils
)

ecm_add_test(
    kresulttest.cpp
    TEST_NAME kresult_test
    LINK_LIBRARIES
        PRIVATE
            Qt6::Core
            Qt6::Test
            ghostwriter_utils
)
