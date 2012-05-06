# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011-2012, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME APPNAME)
project(${PROJECT_NAME})
enable_testing()

#-----------------------------------------------------------------------
# Retrieve the current version number

execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/version.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE VERSION_RESULT
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE)
if(VERSION_RESULT)
    message(FATAL_ERROR "Cannot determine version number")
endif(VERSION_RESULT)
# This causes an annoying extra prompt in ccmake.
# message("Current version: " ${VERSION})

string(REGEX REPLACE "-dev.*" "-dev" BASE_VERSION "${VERSION}")

#-----------------------------------------------------------------------
# Check for prerequisite libraries

find_package(PkgConfig)

pkg_check_modules(CORK REQUIRED libcork)
include_directories(${CORK_INCLUDE_DIRS})
link_directories(${CORK_LIBRARY_DIRS})

#-----------------------------------------------------------------------
# Set some options

if(APPLE)
    if (NOT CMAKE_INSTALL_NAME_DIR)
        set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib")
    endif (NOT CMAKE_INSTALL_NAME_DIR)
endif(APPLE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_COMPILER_IS_GNUCC)
    add_definitions(-Wall -Werror)
endif(CMAKE_COMPILER_IS_GNUCC)

#-----------------------------------------------------------------------
# Include our subdirectories

add_subdirectory(docs)
add_subdirectory(include)
add_subdirectory(share)
add_subdirectory(src)
add_subdirectory(tests)
