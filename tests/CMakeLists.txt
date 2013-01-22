# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2012, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license
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
    add_test(shared-${test_name} shared-${test_name})

    add_executable(embedded-${test_name} ${test_name}.c)
    target_link_libraries(embedded-${test_name} ${CHECK_LIBRARIES}
        embedded_libcork)
    add_test(embedded-${test_name} embedded-${test_name})
endmacro(make_test)

make_test(test-array)
make_test(test-buffer)
make_test(test-core)
make_test(test-dllist)
make_test(test-gc)
make_test(test-hash-table)
make_test(test-managed-buffer)
make_test(test-mempool)
make_test(test-ring-buffer)
make_test(test-slice)
make_test(test-subprocess)
make_test(test-threads)

#-----------------------------------------------------------------------
# Command-line tests

find_program(
    CRAM_EXECUTABLE
    NAMES cram
    HINTS ENV CRAM_DIR
    PATH_SUFFIXES bin
    DOC "Cram test harness"
)

if (CRAM_EXECUTABLE)
    configure_file(ccram ${CMAKE_BINARY_DIR}/ccram COPYONLY)
    add_test(
        cram-tests
        ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}
        ${CMAKE_BINARY_DIR}/ccram
            --cram ${CRAM_EXECUTABLE}
            --tests ${CMAKE_CURRENT_SOURCE_DIR}
    )
else (CRAM_EXECUTABLE)
    message(WARNING "Unable to find cram test harness; skipping tests.")
endif (CRAM_EXECUTABLE)
