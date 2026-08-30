# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
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
if(NOT jsr_license STREQUAL "AGPL-3.0-or-later")
  message(FATAL_ERROR "Unexpected JSR package license: ${jsr_license}")
endif()

set(version_path "${MORPHEUS_SOURCE_DIR}/bindings/deno/internal/version.ts")
file(READ "${version_path}" version_source)
string(REGEX MATCH
  "MORPHEUS_DENO_VERSION[ \t]*=[ \t]*\"([0-9]+\\.[0-9]+\\.[0-9]+)\""
  deno_version_definition "${version_source}")
set(deno_version "${CMAKE_MATCH_1}")
if(NOT deno_version_definition OR
   NOT "${jsr_version}" STREQUAL "${deno_version}")
  message(FATAL_ERROR
    "JSR version ${jsr_version} differs from Deno binding ${deno_version}")
endif()
string(REGEX MATCH
  "MORPHEUS_NATIVE_VERSION[ \t]*=[ \t]*\"([0-9]+\\.[0-9]+\\.[0-9]+)\""
  native_version_definition "${version_source}")
set(native_version "${CMAKE_MATCH_1}")
if(NOT native_version_definition)
  message(FATAL_ERROR "Deno binding native runtime version is missing")
endif()
string(REGEX MATCH
  "MORPHEUS_NATIVE_ABI_VERSION[ \t]*=[ \t]*([0-9]+)"
  native_abi_definition "${version_source}")
set(native_abi "${CMAKE_MATCH_1}")
if(NOT native_abi_definition)
  message(FATAL_ERROR "Deno binding native ABI version is missing")
endif()
file(READ "${MORPHEUS_SOURCE_DIR}/bindings/deno/mod.ts" deno_binding)
string(FIND "${deno_binding}"
       "const ABI_VERSION = MORPHEUS_NATIVE_ABI_VERSION;"
       deno_abi_definition_at)
if(deno_abi_definition_at EQUAL -1)
  message(FATAL_ERROR
    "Deno binding does not consume its declared native ABI")
endif()
file(READ "${MORPHEUS_SOURCE_DIR}/bindings/deno/internal/native_manifest.ts"
     native_manifest)
string(FIND "${native_manifest}" "MORPHEUS_NATIVE_VERSION"
            native_version_use_at)
string(FIND "${native_manifest}" "MORPHEUS_DENO_VERSION"
            deno_version_leak_at)
if(native_version_use_at EQUAL -1 OR NOT deno_version_leak_at EQUAL -1)
  message(FATAL_ERROR
    "Native asset selection is not independent from the Deno version")
endif()
string(JSON jsr_default_export GET "${config}" exports .)
string(JSON jsr_data_export GET "${config}" exports ./data)
string(JSON jsr_native_export GET "${config}" exports ./native)
string(JSON jsr_setup_export GET "${config}" exports ./setup)
if(NOT jsr_default_export STREQUAL "./mod.ts" OR
   NOT jsr_data_export STREQUAL "./data.ts" OR
   NOT jsr_native_export STREQUAL "./native.ts" OR
   NOT jsr_setup_export STREQUAL "./setup.ts")
  message(FATAL_ERROR
    "Unexpected JSR package exports: ${jsr_default_export}, ${jsr_data_export}, ${jsr_native_export}, ${jsr_setup_export}")
endif()

file(GLOB binding_root_files
     LIST_DIRECTORIES false
     RELATIVE "${MORPHEUS_SOURCE_DIR}/bindings/deno"
     "${MORPHEUS_SOURCE_DIR}/bindings/deno/*")
set(expected_root_files
    LICENSE NOTICE README.md data.ts jsr.json jsr.json.license mod.ts native.ts
    setup.ts)
foreach(root_file IN LISTS binding_root_files)
  list(FIND expected_root_files "${root_file}" expected_at)
  if(expected_at EQUAL -1)
    message(FATAL_ERROR "Unexpected file at Deno binding root: ${root_file}")
  endif()
  list(REMOVE_ITEM expected_root_files "${root_file}")
endforeach()
if(expected_root_files)
  message(FATAL_ERROR "Missing Deno binding root files: ${expected_root_files}")
endif()

set(expected_files
    LICENSE LICENSES/EMSCRIPTEN.txt LICENSES/MPL-2.0.txt NOTICE README.md
    data.ts internal/data_internal.ts internal/data_manifest.ts
    internal/gener_index_internal.ts internal/gener_manifest.ts
    internal/gener_preparer.mjs internal/gener_runtime_internal.ts
    internal/native_internal.ts internal/native_manifest.ts
    internal/setup_internal.ts internal/version.ts mod.ts native.ts setup.ts)
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
        data.ts internal/data_internal.ts internal/data_manifest.ts
        internal/gener_index_internal.ts internal/gener_manifest.ts
        internal/gener_preparer.mjs internal/gener_runtime_internal.ts
        internal/native_internal.ts internal/native_manifest.ts
        internal/setup_internal.ts internal/version.ts mod.ts native.ts setup.ts)
  if(NOT EXISTS "${MORPHEUS_SOURCE_DIR}/bindings/deno/${required}")
    message(FATAL_ERROR "Missing JSR package source: ${required}")
  endif()
endforeach()
