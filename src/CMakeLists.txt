# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2012, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license
# details.
# ----------------------------------------------------------------------

include_directories(../include)

#-----------------------------------------------------------------------
# Build the library

set(LIBCORK_SRC
    libcork/cli/commands.c
    libcork/core/allocator.c
    libcork/core/error.c
    libcork/core/gc.c
    libcork/core/hash.c
    libcork/core/ip-address.c
    libcork/core/mempool.c
    libcork/core/timestamp.c
    libcork/ds/array.c
    libcork/ds/buffer.c
    libcork/ds/dllist.c
    libcork/ds/file-stream.c
    libcork/ds/hash-table.c
    libcork/ds/managed-buffer.c
    libcork/ds/ring-buffer.c
    libcork/ds/slice.c
    libcork/posix/directory-walker.c
    libcork/posix/env.c
    libcork/posix/exec.c
    libcork/posix/files.c
    libcork/posix/process.c
    libcork/posix/subprocess.c
    libcork/pthreads/thread.c
)

# Update the VERSION and SOVERSION properties below according to the following
# rules (taken from [1]):
#
# VERSION = current.revision.age
#
#   1. Start with a VERSION of `0.0.0` for each shared library.
#   2. Update VERSION only immediately before a public release of your software.
#      More frequent updates are unnecessary, and only guarantee that the
#      current interface number gets larger faster.
#   3. If the library source code has changed at all since the last update, then
#      increment `revision` (`c.r.a` becomes `c.r+1.a`).
#   4. If any interfaces have been added, removed, or changed since the last
#      update, increment `current`, and set `revision` to 0.
#   5. If any interfaces have been added since the last public release, then
#      increment `age`.
#   6. If any interfaces have been removed or changed since the last public
#      release, then set `age` to 0.
#
# SOVERSION should always equal `current`.
#
# Note that changing `current` means that you are releasing a new
# backwards-incompatible version of the library.  This has implications on
# packaging, so once an API has stabilized, these should be a rare occurrence.
#
# [1] http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info

add_library(libcork SHARED ${LIBCORK_SRC})
target_link_libraries(libcork ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(libcork PROPERTIES
    OUTPUT_NAME cork
    COMPILE_DEFINITIONS CORK_API=CORK_EXPORT
    VERSION 11.0.2
    SOVERSION 11)

#-----------------------------------------------------------------------
# Build a static library to simulate embedding libcork

add_library(embedded_libcork STATIC ${LIBCORK_SRC})
target_link_libraries(embedded_libcork ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(embedded_libcork PROPERTIES
    OUTPUT_NAME cork-embedded
    COMPILE_DEFINITIONS CORK_API=CORK_LOCAL
)

#-----------------------------------------------------------------------
# Utility commands

set(CORK_HASH_SRC cork-hash/cork-hash.c)
add_executable(cork-hash ${CORK_HASH_SRC})
target_link_libraries(cork-hash embedded_libcork)

set(CORK_INITIALIZER_SRC
    cork-initializer/init1.c
    cork-initializer/init2.c
    cork-initializer/main.c
)
add_executable(cork-initializer ${CORK_INITIALIZER_SRC})
target_link_libraries(cork-initializer embedded_libcork)

set(CORK_TEST_SRC cork-test/cork-test.c)
add_executable(cork-test ${CORK_TEST_SRC})
target_link_libraries(cork-test embedded_libcork)

#-----------------------------------------------------------------------
# Install everything

# Note that we don't install cork-test; that's just for our test cases!
install(TARGETS libcork cork-hash
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(libcork.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc
        DESTINATION lib/pkgconfig)
