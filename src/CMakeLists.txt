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
    libcork/threads/descriptor.c
)

add_library(libcork SHARED ${LIBCORK_SRC})
target_link_libraries(libcork ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(libcork PROPERTIES
    OUTPUT_NAME cork
    SOVERSION 8.0.0)

#-----------------------------------------------------------------------
# Build a static library to simulate embedding libcork

add_library(embedded_libcork STATIC ${LIBCORK_SRC})
target_link_libraries(embedded_libcork ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(embedded_libcork PROPERTIES
    OUTPUT_NAME cork-embedded)

#-----------------------------------------------------------------------
# Utility commands

set(CORK_HASH_SRC cork-hash/cork-hash.c)
add_executable(cork-hash ${CORK_HASH_SRC})
target_link_libraries(cork-hash embedded_libcork)

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
configure_file(libcork.pc.in libcork.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc
        DESTINATION lib/pkgconfig)
