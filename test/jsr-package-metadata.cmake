# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_PROJECT_VERSION)
  message(FATAL_ERROR
    "MORPHEUS_SOURCE_DIR and MORPHEUS_PROJECT_VERSION are required")
endif()

set(config_path "${MORPHEUS_SOURCE_DIR}/bindings/deno/jsr.json")
file(READ "${config_path}" config)

foreach(field IN ITEMS name version license exports)
  string(JSON "jsr_${field}" ERROR_VARIABLE json_error GET "${config}" "${field}")
  if(json_error)
    message(FATAL_ERROR "Invalid ${field} in ${config_path}: ${json_error}")
  endif()
endforeach()

if(NOT jsr_name STREQUAL "@humanities/libmorpheus")
  message(FATAL_ERROR "Unexpected JSR package name: ${jsr_name}")
endif()
if(NOT jsr_version STREQUAL "${MORPHEUS_PROJECT_VERSION}")
  message(FATAL_ERROR
    "JSR version ${jsr_version} differs from project ${MORPHEUS_PROJECT_VERSION}")
endif()
if(NOT jsr_license STREQUAL "AGPL-3.0-or-later")
  message(FATAL_ERROR "Unexpected JSR package license: ${jsr_license}")
endif()
if(NOT jsr_exports STREQUAL "./mod.ts")
  message(FATAL_ERROR "Unexpected JSR package export: ${jsr_exports}")
endif()

set(expected_files LICENSE NOTICE README.md mod.ts)
string(JSON include_count ERROR_VARIABLE json_error
       LENGTH "${config}" publish include)
if(json_error)
  message(FATAL_ERROR "Invalid publish.include in ${config_path}: ${json_error}")
endif()
list(LENGTH expected_files expected_count)
if(NOT include_count EQUAL expected_count)
  message(FATAL_ERROR
    "JSR publish.include has ${include_count} entries; expected ${expected_count}")
endif()

math(EXPR last_include "${include_count} - 1")
foreach(index RANGE 0 ${last_include})
  string(JSON included GET "${config}" publish include ${index})
  list(FIND expected_files "${included}" expected_at)
  if(expected_at EQUAL -1)
    message(FATAL_ERROR "Unexpected JSR package file: ${included}")
  endif()
  list(REMOVE_ITEM expected_files "${included}")
endforeach()
if(expected_files)
  message(FATAL_ERROR "Missing JSR package files: ${expected_files}")
endif()

foreach(required IN ITEMS LICENSE NOTICE README.md mod.ts)
  if(NOT EXISTS "${MORPHEUS_SOURCE_DIR}/bindings/deno/${required}")
    message(FATAL_ERROR "Missing JSR package source: ${required}")
  endif()
endforeach()
