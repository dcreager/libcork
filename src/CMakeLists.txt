# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2015, RedJack, LLC.
# All rights reserved.
#
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/include)

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
    libcork/core/u128.c
    libcork/core/version.c
    libcork/ds/array.c
    libcork/ds/bitset.c
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
# SOVERSION should always equal `current-age`.
#
# Note that changing `current` means that you are releasing a new
# backwards-incompatible version of the library.  This has implications on
# packaging, so once an API has stabilized, these should be a rare occurrence.
#
# [1] http://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html#Updating-version-info

if (ENABLE_SHARED OR ENABLE_SHARED_EXECUTABLES)
    add_library(libcork SHARED ${LIBCORK_SRC})
    target_link_libraries(libcork ${CMAKE_THREAD_LIBS_INIT})
    set_target_properties(libcork PROPERTIES
        OUTPUT_NAME cork
        COMPILE_DEFINITIONS CORK_API=CORK_EXPORT
        CLEAN_DIRECT_OUTPUT 1
        VERSION 15.3.0
        SOVERSION 15
    )

    # We have to install the shared library if the user asked us to, or if the
    # user asked us to link our programs with the shared library.
    install(TARGETS libcork LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif (ENABLE_SHARED OR ENABLE_SHARED_EXECUTABLES)

#-----------------------------------------------------------------------
# Build a static library to simulate embedding libcork

if (ENABLE_STATIC OR NOT ENABLE_SHARED_EXECUTABLES)
    add_library(libcork-static STATIC ${LIBCORK_SRC})
    target_link_libraries(libcork-static ${CMAKE_THREAD_LIBS_INIT})
    set_target_properties(libcork-static PROPERTIES
        OUTPUT_NAME cork
        COMPILE_DEFINITIONS CORK_API=CORK_LOCAL
        CLEAN_DIRECT_OUTPUT 1
    )
endif (ENABLE_STATIC OR NOT ENABLE_SHARED_EXECUTABLES)

if (ENABLE_STATIC)
    # We DON'T have to install the static library if the user asked us to link
    # our programs statically.
    install(TARGETS libcork-static ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif (ENABLE_STATIC)

#-----------------------------------------------------------------------
# Utility commands

set(CORK_HASH_SRC cork-hash/cork-hash.c)
add_executable(cork-hash ${CORK_HASH_SRC})
if (ENABLE_SHARED_EXECUTABLES)
    target_link_libraries(cork-hash libcork)
else (ENABLE_SHARED_EXECUTABLES)
    target_link_libraries(cork-hash libcork-static)
endif (ENABLE_SHARED_EXECUTABLES)

install(TARGETS cork-hash RUNTIME DESTINATION bin)

set(CORK_INITIALIZER_SRC
    cork-initializer/init1.c
    cork-initializer/init2.c
    cork-initializer/main.c
)
add_executable(cork-initializer ${CORK_INITIALIZER_SRC})
if (ENABLE_SHARED_EXECUTABLES)
    target_link_libraries(cork-initializer libcork)
else (ENABLE_SHARED_EXECUTABLES)
    target_link_libraries(cork-initializer libcork-static)
endif (ENABLE_SHARED_EXECUTABLES)

set(CORK_TEST_SRC cork-test/cork-test.c)
add_executable(cork-test ${CORK_TEST_SRC})
if (ENABLE_SHARED_EXECUTABLES)
    target_link_libraries(cork-test libcork)
else (ENABLE_SHARED_EXECUTABLES)
    target_link_libraries(cork-test libcork-static)
endif (ENABLE_SHARED_EXECUTABLES)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(libcork.pc.in ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
