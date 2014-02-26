# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2014, RedJack, LLC.
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
    add_test(${test_name} ${test_name})
endmacro(make_test)

# Add calls to make_test for each .c test case in the tests/ directory.
# make_test(test-foo)

#-----------------------------------------------------------------------
# Command-line tests

if(TILERA)
    configure_file(tile-test ${CMAKE_BINARY_DIR}/tile-test COPYONLY)
endif(TILERA)

find_package(PythonInterp)

if (PYTHON_EXECUTABLE)
    configure_file(ccram ${CMAKE_BINARY_DIR}/ccram COPYONLY)
    file(GLOB_RECURSE TESTS "${CMAKE_CURRENT_SOURCE_DIR}/*.t")
    foreach(TEST ${TESTS})
        get_filename_component(TEST_NAME "${TEST}" NAME_WE)
        add_test(
            ${TEST_NAME}
            ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}
            ${CMAKE_BINARY_DIR}/ccram
                --python ${PYTHON_EXECUTABLE}
                --root ${CMAKE_SOURCE_DIR}
                --tests ${TEST}
        )
    endforeach(TEST)
else (PYTHON_EXECUTABLE)
    message(WARNING "Unable to find Python; skipping cram tests.")
endif (PYTHON_EXECUTABLE)
