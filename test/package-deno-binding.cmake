# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_PACKAGE_DIR)
  message(FATAL_ERROR
    "MORPHEUS_SOURCE_DIR, MORPHEUS_BUILD_DIR, and MORPHEUS_PACKAGE_DIR are required")
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/jsr.json" jsr_config)
string(JSON deno_version ERROR_VARIABLE version_error
       GET "${jsr_config}" version)
if(version_error OR NOT deno_version MATCHES "^[0-9]+\\.[0-9]+\\.[0-9]+$")
  message(FATAL_ERROR "Invalid Deno binding version: ${version_error}")
endif()

set(package_basename "libmorpheus-deno-${deno_version}")
set(package_root "${MORPHEUS_BUILD_DIR}/deno-binding-package")
set(package_dir "${package_root}/${package_basename}")
set(archive "${MORPHEUS_PACKAGE_DIR}/${package_basename}.tar.gz")
set(checksum "${archive}.sha256")

file(REMOVE_RECURSE "${package_root}")
file(MAKE_DIRECTORY "${package_dir}")
file(COPY
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/mod.ts"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/data.ts"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/native.ts"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/setup.ts"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/README.md"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/LICENSE"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/NOTICE"
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/jsr.json"
  DESTINATION "${package_dir}"
)
file(COPY
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/internal"
  DESTINATION "${package_dir}"
)
file(COPY
  "${MORPHEUS_SOURCE_DIR}/bindings/js/deno/LICENSES"
  DESTINATION "${package_dir}"
)

file(MAKE_DIRECTORY "${MORPHEUS_PACKAGE_DIR}")
execute_process(
  COMMAND "${CMAKE_COMMAND}" -E tar cfz "${archive}" --format=gnutar
          "${package_basename}"
  WORKING_DIRECTORY "${package_root}"
  RESULT_VARIABLE archive_result
  OUTPUT_VARIABLE archive_output
  ERROR_VARIABLE archive_error
)
if(NOT archive_result EQUAL 0)
  message(FATAL_ERROR
    "could not create Deno binding archive:\n${archive_output}\n${archive_error}")
endif()

file(SHA256 "${archive}" archive_sha256)
file(WRITE "${checksum}" "${archive_sha256}  ${package_basename}.tar.gz\n")
