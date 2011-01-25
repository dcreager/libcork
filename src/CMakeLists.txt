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

file(GLOB_RECURSE LIBAPPNAME_SRC libAPPNAME/*.c)

add_library(libAPPNAME SHARED ${LIBAPPNAME_SRC})
set_target_properties(libAPPNAME PROPERTIES
    COMPILE_DEFINITIONS G_LOG_DOMAIN="APPNAME"
    OUTPUT_NAME appname
    SOVERSION 0.0.0)
target_link_libraries(libAPPNAME
    ${GLIB_LIBRARIES})

install(TARGETS libAPPNAME
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(libAPPNAME.pc.in libAPPNAME.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libAPPNAME.pc
        DESTINATION lib/pkgconfig)
