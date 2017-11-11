# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, libcork authors
# Please see the COPYING file in this distribution for license details.
# ----------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME libcork)
set(RELEASE_DATE 2015-09-03)
project(${PROJECT_NAME})
enable_testing()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(ParseArguments)
find_package(Prereqs)
find_package(CTargets)

#-----------------------------------------------------------------------
# Retrieve the current version number

execute_process(
    COMMAND git describe
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE VERSION_RESULT
    OUTPUT_VARIABLE VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(VERSION_RESULT)
    message(FATAL_ERROR
            "Cannot determine version number: " ${VERSION_RESULT})
endif(VERSION_RESULT)
message(STATUS "Current version: " ${VERSION})

string(REGEX REPLACE "-.*" "-dev" BASE_VERSION "${VERSION}")

if(BASE_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-dev)?$")
    set(VERSION_MAJOR "${CMAKE_MATCH_1}")
    set(VERSION_MINOR "${CMAKE_MATCH_2}")
    set(VERSION_PATCH "${CMAKE_MATCH_3}")
else(BASE_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-dev)?$")
    message(FATAL_ERROR "Invalid version number: ${VERSION}")
endif(BASE_VERSION MATCHES "^([0-9]+)\\.([0-9]+)\\.([0-9]+)(-dev)?$")

execute_process(
    COMMAND git rev-parse HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE GIT_SHA1_RESULT
    OUTPUT_VARIABLE GIT_SHA1
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(GIT_SHA1_RESULT)
    message(FATAL_ERROR
            "Cannot determine git commit: " ${GIT_SHA1_RESULT})
endif(GIT_SHA1_RESULT)
message(STATUS "Current revision: " ${GIT_SHA1})

include(GNUInstallDirs)

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

set(ENABLE_SHARED YES CACHE BOOL "Whether to build a shared library")
set(ENABLE_SHARED_EXECUTABLES NO CACHE BOOL
    "Whether to link executables using shared libraries")
set(ENABLE_STATIC YES CACHE BOOL "Whether to build a static library")

if(NOT CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib CACHE STRING
        "The base name of the installation directory for libraries")
endif(NOT CMAKE_INSTALL_LIBDIR)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_definitions(-Wall -Werror)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    add_definitions(-Wall -Werror)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Intel")
    add_definitions(-Wall -Werror)
endif(CMAKE_C_COMPILER_ID STREQUAL "GNU")

#-----------------------------------------------------------------------
# Check for prerequisite libraries

find_package(Threads)
set(THREADS_LDFLAGS "${CMAKE_THREAD_LIBS_INIT}")
set(THREADS_STATIC_LDFLAGS "${CMAKE_THREAD_LIBS_INIT}")

#-----------------------------------------------------------------------
# Include our subdirectories

add_subdirectory(include)
add_subdirectory(share)
add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(docs/old)
