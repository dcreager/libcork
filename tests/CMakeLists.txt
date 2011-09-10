# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

include_directories(../include)
link_directories(${CMAKE_CURRENT_BINARY_DIR}/../src)

#-----------------------------------------------------------------------
# Check for prerequisite libraries

find_package(PkgConfig)

pkg_check_modules(CHECK REQUIRED check)
include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

#-----------------------------------------------------------------------
# Build the test cases

# For each test cases, we create two executables: one that links with a
# shared libcork, and the other that simulates embedding libcork into
# another project.  For the embedded version, we need to know which of
# the libcork submodules should be included when compiling the test
# case.  These are provided as additional parameters to the make_test
# macro.

macro(make_test test_name)
    add_executable(shared-${test_name} ${test_name}.c)
    target_link_libraries(shared-${test_name} ${CHECK_LIBRARIES}
        libcork)
    add_test(NAME shared-${test_name} COMMAND shared-${test_name})

    add_executable(embedded-${test_name} ${test_name}.c)
    target_link_libraries(embedded-${test_name} ${CHECK_LIBRARIES}
        embedded_libcork)
    add_test(NAME embedded-${test_name} COMMAND embedded-${test_name})
endmacro(make_test)

make_test(test-buffer)
make_test(test-core)
make_test(test-dllist)
make_test(test-gc)
make_test(test-hash-table)
make_test(test-managed-buffer)
