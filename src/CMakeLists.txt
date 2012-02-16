# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

include_directories(../include)

#-----------------------------------------------------------------------
# Build the library

file(GLOB_RECURSE LIBCORK_SRC libcork/*.c)

add_library(libcork SHARED ${LIBCORK_SRC})
target_link_libraries(libcork ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(libcork PROPERTIES
    OUTPUT_NAME cork
    SOVERSION 6.0.0)

#-----------------------------------------------------------------------
# Build a static library to simulate embedding libcork

add_library(embedded_libcork STATIC ${LIBCORK_SRC})
target_link_libraries(embedded_libcork ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(embedded_libcork PROPERTIES
    OUTPUT_NAME cork-embedded)

#-----------------------------------------------------------------------
# Utility commands

file(GLOB_RECURSE CORK_HASH_SRC cork-hash/*.c)
add_executable(cork-hash ${CORK_HASH_SRC})
target_link_libraries(cork-hash embedded_libcork)

#-----------------------------------------------------------------------
# Install everything

install(TARGETS libcork cork-hash
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(libcork.pc.in libcork.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc
        DESTINATION lib/pkgconfig)
