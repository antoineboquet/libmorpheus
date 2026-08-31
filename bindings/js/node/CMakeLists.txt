# SPDX-License-Identifier: AGPL-3.0-or-later

if(WIN32)
  message(FATAL_ERROR "The native libmorpheus runtime does not support Windows")
endif()

find_program(MORPHEUS_NODE_EXECUTABLE NAMES node REQUIRED)
execute_process(
  COMMAND
    "${MORPHEUS_NODE_EXECUTABLE}"
    -p
    "require('node:path').resolve(process.execPath, '../..')"
  OUTPUT_VARIABLE morpheus_node_prefix
  OUTPUT_STRIP_TRAILING_WHITESPACE
  COMMAND_ERROR_IS_FATAL ANY
)
set(
  MORPHEUS_NODE_INCLUDE_DIR
  ""
  CACHE PATH
  "Directory containing node_api.h for the selected Node.js installation"
)
find_path(
  morpheus_node_api_include
  node_api.h
  HINTS
    "${MORPHEUS_NODE_INCLUDE_DIR}"
    "${morpheus_node_prefix}/include/node"
  REQUIRED
)

add_library(libmorpheus_node MODULE addon.c)
target_include_directories(
  libmorpheus_node
  PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
)
target_include_directories(
  libmorpheus_node
  SYSTEM PRIVATE "${morpheus_node_api_include}"
)
target_compile_definitions(libmorpheus_node PRIVATE NAPI_VERSION=8)
target_link_libraries(
  libmorpheus_node
  PRIVATE morpheus_build_options "${CMAKE_DL_LIBS}"
)
if(APPLE)
  target_link_options(libmorpheus_node PRIVATE "LINKER:-undefined,dynamic_lookup")
endif()
set_target_properties(
  libmorpheus_node
  PROPERTIES
    PREFIX ""
    SUFFIX ".node"
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/node"
)
