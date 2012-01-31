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

make_test(test-array)
make_test(test-buffer)
make_test(test-core)
make_test(test-dllist)
make_test(test-gc)
make_test(test-hash-table)
make_test(test-managed-buffer)
make_test(test-ring-buffer)

#-----------------------------------------------------------------------
# Command-line tests

# This will automatically find any test cases in tests/ that should be
# run using the run-script helper script.  This lets you pass in
# arbitary command-line options and stdin to a compiled executable, and
# verify its stdout and stderr.  The expectation is that your
# directories will be named tests/${category}/${test_name}

configure_file(run-test ${CMAKE_BINARY_DIR}/run-test COPYONLY)
configure_file(train ${CMAKE_BINARY_DIR}/train COPYONLY)

file(GLOB_RECURSE OUTPUT_TESTS command)
foreach(OUTPUT_TEST ${OUTPUT_TESTS})
    get_filename_component(test_dir ${OUTPUT_TEST} PATH)
    get_filename_component(test_suffix ${test_dir} NAME)
    get_filename_component(test_parent_dir ${test_dir} PATH)
    get_filename_component(test_prefix ${test_parent_dir} NAME)
    set(test_name "${test_prefix}:${test_suffix}")

    add_test(NAME ${test_name}
             WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
             COMMAND ${CMAKE_BINARY_DIR}/run-test ${test_dir})
endforeach(OUTPUT_TEST ${OUTPUT_TESTS})
