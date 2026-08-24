if(NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_PACKAGE_DIR)
  message(FATAL_ERROR
    "MORPHEUS_SOURCE_DIR, MORPHEUS_BUILD_DIR, and MORPHEUS_PACKAGE_DIR are required")
endif()

include("${MORPHEUS_BUILD_DIR}/CPackConfig.cmake")
if(NOT DEFINED CPACK_PACKAGE_VERSION)
  message(FATAL_ERROR "CPackConfig.cmake did not define CPACK_PACKAGE_VERSION")
endif()

set(package_basename "libmorpheus-deno-${CPACK_PACKAGE_VERSION}")
set(package_root "${MORPHEUS_BUILD_DIR}/deno-binding-package")
set(package_dir "${package_root}/${package_basename}")
set(archive "${MORPHEUS_PACKAGE_DIR}/${package_basename}.tar.gz")
set(checksum "${archive}.sha256")

file(REMOVE_RECURSE "${package_root}")
file(MAKE_DIRECTORY "${package_dir}")
file(COPY
  "${MORPHEUS_SOURCE_DIR}/bindings/deno/mod.ts"
  "${MORPHEUS_SOURCE_DIR}/bindings/deno/README.md"
  "${MORPHEUS_SOURCE_DIR}/bindings/deno/LICENSE"
  DESTINATION "${package_dir}"
)
file(WRITE "${package_dir}/NOTICE"
  "This archive contains the Deno binding source for libmorpheus.\n"
  "The native libmorpheus library and stemlib are separate runtime dependencies.\n"
  "See README.md for installation and required Deno permissions.\n"
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
