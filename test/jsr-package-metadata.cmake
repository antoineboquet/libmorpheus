# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_PROJECT_VERSION)
  message(FATAL_ERROR
    "MORPHEUS_SOURCE_DIR and MORPHEUS_PROJECT_VERSION are required")
endif()

set(config_path "${MORPHEUS_SOURCE_DIR}/bindings/deno/jsr.json")
file(READ "${config_path}" config)

foreach(field IN ITEMS name version license)
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
string(JSON jsr_default_export GET "${config}" exports .)
string(JSON jsr_data_export GET "${config}" exports ./data)
string(JSON jsr_init_export GET "${config}" exports ./init)
string(JSON jsr_native_export GET "${config}" exports ./native)
if(NOT jsr_default_export STREQUAL "./mod.ts" OR
   NOT jsr_data_export STREQUAL "./data.ts" OR
   NOT jsr_init_export STREQUAL "./init.ts" OR
   NOT jsr_native_export STREQUAL "./native.ts")
  message(FATAL_ERROR
    "Unexpected JSR package exports: ${jsr_default_export}, ${jsr_data_export}, ${jsr_init_export}, ${jsr_native_export}")
endif()

set(expected_files
    LICENSE LICENSES/EMSCRIPTEN.txt LICENSES/MPL-2.0.txt NOTICE README.md
    data.ts data_internal.ts data_manifest.ts gener_index_internal.ts
    gener_manifest.ts gener_preparer.mjs gener_runtime_internal.ts init.ts mod.ts
    native.ts native_internal.ts native_manifest.ts)
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

foreach(required IN ITEMS
        LICENSE LICENSES/EMSCRIPTEN.txt LICENSES/MPL-2.0.txt NOTICE README.md
        data.ts data_internal.ts data_manifest.ts gener_index_internal.ts
        gener_manifest.ts gener_preparer.mjs gener_runtime_internal.ts init.ts mod.ts
        native.ts native_internal.ts native_manifest.ts)
  if(NOT EXISTS "${MORPHEUS_SOURCE_DIR}/bindings/deno/${required}")
    message(FATAL_ERROR "Missing JSR package source: ${required}")
  endif()
endforeach()
