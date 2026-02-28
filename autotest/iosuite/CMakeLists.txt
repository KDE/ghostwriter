# SPDX-FileCopyrightText: 2025 Megan Conkle <megan.conkle@kdemail.net>
#
# SPDX-License-Identifier: GPL-3.0-or-later

cmake_minimum_required(VERSION 3.16)

# Find ECM and include the module
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(ECMAddTests)

find_package(Qt6 REQUIRED COMPONENTS Core Concurrent Test)

qt_standard_project_setup()

# Create the test executables
ecm_add_test(
    textiotest.cpp
    TEST_NAME textio_test
    LINK_LIBRARIES
        PRIVATE
            Qt6::Core
            Qt6::Concurrent
            Qt6::Test
            ghostwriter_io
)

ecm_add_test(
    localtextiotest.cpp
    TEST_NAME localtextio_test
    LINK_LIBRARIES
        PRIVATE
            Qt6::Core
            Qt6::Concurrent
            Qt6::Test
            ghostwriter_io
)

ecm_add_test(
    netsharetextiotest.cpp
    TEST_NAME netsharetextio_test
    LINK_LIBRARIES
        PRIVATE
            Qt6::Core
            Qt6::Concurrent
            Qt6::Test
            ghostwriter_io
)
