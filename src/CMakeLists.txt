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
set_target_properties(libcork PROPERTIES
    OUTPUT_NAME cork
    SOVERSION 4.0.1)

install(TARGETS libcork
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

#-----------------------------------------------------------------------
# Build a static library to simulate embedding libcork

add_library(embedded_libcork STATIC ${LIBCORK_SRC})
set_target_properties(embedded_libcork PROPERTIES
    OUTPUT_NAME cork-embedded)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(libcork.pc.in libcork.pc @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libcork.pc
        DESTINATION lib/pkgconfig)
