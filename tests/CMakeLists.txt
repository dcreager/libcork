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

macro(make_test test_name)
    add_executable(${test_name} ${test_name}.c)
    target_link_libraries(${test_name} ${CHECK_LIBRARIES} libAPPNAME)
    add_test(NAME ${test_name} COMMAND ${test_name})
endmacro(make_test)

# Add calls to make_test for each .c test case in the tests/ directory.
# make_test(test-foo)

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
