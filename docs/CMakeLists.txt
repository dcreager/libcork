# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

find_program(
  SPHINX_EXECUTABLE
  NAMES sphinx-build
  HINTS ENV SPHINX_DIR
  PATH_SUFFIXES bin
  DOC "Sphinx documentation generator"
)

set(GENERATE_DOC TRUE)

if (NOT SPHINX_EXECUTABLE)
  message(WARNING "Unable to find Sphinx documentation generator")
  set(GENERATE_DOC FALSE)
endif (NOT SPHINX_EXECUTABLE)

if (GENERATE_DOC)
  set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/_doctrees")
  set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")

  add_custom_target(doc ALL
    ${SPHINX_EXECUTABLE}
      -b html
      -d "${SPHINX_CACHE_DIR}"
      -D version="${BASE_VERSION}"
      -D release="${VERSION}"
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${SPHINX_HTML_DIR}"
    COMMENT "Building HTML documentation with Sphinx"
  )

  list(APPEND CLEAN_FILES "${SPHINX_CACHE_DIR}" "${SPHINX_HTML_DIR}")
  set_directory_properties(
    PROPERTIES
    ADDITIONAL_MAKE_CLEAN_FILES "${CLEAN_FILES}"
  )

  install(
    DIRECTORY "${SPHINX_HTML_DIR}"
    DESTINATION "share/doc/${PROJECT_NAME}"
  )
endif (GENERATE_DOC)
