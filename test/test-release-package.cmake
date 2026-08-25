# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_PACKAGE_DIR)
  message(FATAL_ERROR "MORPHEUS_BUILD_DIR and MORPHEUS_PACKAGE_DIR are required")
endif()

include("${MORPHEUS_BUILD_DIR}/MorpheusPackageLayout.cmake")

set(archive
    "${MORPHEUS_PACKAGE_DIR}/${MORPHEUS_PACKAGE_BASENAME}.tar.gz")
if(NOT EXISTS "${archive}")
  message(FATAL_ERROR "native package is missing: ${archive}")
endif()

file(GLOB checksum_files "${archive}.*")
list(LENGTH checksum_files checksum_file_count)
if(NOT checksum_file_count EQUAL 1)
  message(FATAL_ERROR
    "expected one checksum for ${archive}, found: ${checksum_files}"
  )
endif()
list(GET checksum_files 0 checksum_file)
file(SHA256 "${archive}" actual_checksum)
file(READ "${checksum_file}" recorded_checksum)
string(FIND "${recorded_checksum}" "${actual_checksum}" checksum_at)
if(checksum_at EQUAL -1)
  message(FATAL_ERROR "package checksum does not match ${archive}")
endif()

set(extract_root "${MORPHEUS_BUILD_DIR}/release-package-test")
file(REMOVE_RECURSE "${extract_root}")
file(MAKE_DIRECTORY "${extract_root}")
execute_process(
  COMMAND "${CMAKE_COMMAND}" -E tar xzf "${archive}"
  WORKING_DIRECTORY "${extract_root}"
  RESULT_VARIABLE extract_result
  OUTPUT_VARIABLE extract_output
  ERROR_VARIABLE extract_error
)
if(NOT extract_result EQUAL 0)
  message(FATAL_ERROR
    "could not extract package:\n${extract_output}\n${extract_error}"
  )
endif()

set(prefix "${extract_root}/${MORPHEUS_PACKAGE_BASENAME}")
set(public_header
    "${prefix}/${MORPHEUS_PACKAGE_INCLUDEDIR}/morpheus/morpheus.h")
set(compat_header
    "${prefix}/${MORPHEUS_PACKAGE_INCLUDEDIR}/morpheus/compat.h")
set(public_library
    "${prefix}/${MORPHEUS_PACKAGE_LIBDIR}/${MORPHEUS_PACKAGE_LIBRARY_FILE}")
set(cmake_package
    "${prefix}/${MORPHEUS_PACKAGE_LIBDIR}/cmake/Morpheus")
set(pkg_config_file
    "${prefix}/${MORPHEUS_PACKAGE_LIBDIR}/pkgconfig/libmorpheus.pc")
set(license_directory
    "${prefix}/${MORPHEUS_PACKAGE_DATADIR}/doc/libmorpheus")

foreach(required IN ITEMS
    "${public_header}"
    "${compat_header}"
    "${public_library}"
    "${cmake_package}/MorpheusConfig.cmake"
    "${cmake_package}/MorpheusConfigVersion.cmake"
    "${cmake_package}/MorpheusTargets.cmake"
    "${pkg_config_file}"
    "${license_directory}/MPL-2.0.txt"
    "${license_directory}/AGPL-3.0-or-later.txt"
    "${license_directory}/licensing.md"
    "${license_directory}/provenance.md"
    "${license_directory}/license-inventory.md")
  if(NOT EXISTS "${required}")
    message(FATAL_ERROR "required package file is missing: ${required}")
  endif()
endforeach()

file(READ "${license_directory}/licensing.md" packaged_licensing)
foreach(reference IN ITEMS provenance.md license-inventory.md)
  string(FIND "${packaged_licensing}" "(${reference})" reference_at)
  if(reference_at EQUAL -1)
    message(FATAL_ERROR
      "Packaged licensing guide omits the ${reference} reference")
  endif()
endforeach()

if(MORPHEUS_PACKAGE_CRUNCHER)
  set(cruncher
      "${prefix}/${MORPHEUS_PACKAGE_BINDIR}/${MORPHEUS_PACKAGE_CRUNCHER_FILE}")
  if(NOT EXISTS "${cruncher}")
    message(FATAL_ERROR "packaged cruncher is missing: ${cruncher}")
  endif()
endif()

file(
  GLOB_RECURSE packaged_headers
  LIST_DIRECTORIES false
  "${prefix}/${MORPHEUS_PACKAGE_INCLUDEDIR}/*"
)
list(LENGTH packaged_headers packaged_header_count)
list(SORT packaged_headers)
set(expected_headers "${compat_header}" "${public_header}")
list(SORT expected_headers)
if(NOT packaged_header_count EQUAL 2 OR
   NOT packaged_headers STREQUAL expected_headers)
  message(FATAL_ERROR "private headers entered the package: ${packaged_headers}")
endif()

file(GLOB_RECURSE packaged_files LIST_DIRECTORIES false "${prefix}/*")
foreach(packaged_file IN LISTS packaged_files)
  file(RELATIVE_PATH relative_file "${prefix}" "${packaged_file}")
  if(relative_file MATCHES "(^|/)src/" OR
     relative_file MATCHES
       "morpheus_(anal|gener|gkends|gkdict|morphlib|greeklib|build_options)" OR
     relative_file MATCHES "\\.a$")
    message(FATAL_ERROR
      "internal build artifact entered the package: ${relative_file}"
    )
  endif()
endforeach()

get_filename_component(
  morpheus_source_dir "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE
)
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    "-DMORPHEUS_BUILD_DIR=${MORPHEUS_BUILD_DIR}"
    "-DMORPHEUS_SOURCE_DIR=${morpheus_source_dir}"
    "-DMORPHEUS_GENERATOR=${MORPHEUS_PACKAGE_GENERATOR}"
    "-DMORPHEUS_INSTALL_PREFIX=${prefix}"
    -P "${CMAKE_CURRENT_LIST_DIR}/test-installed-package.cmake"
  RESULT_VARIABLE cmake_consumer_result
  OUTPUT_VARIABLE cmake_consumer_output
  ERROR_VARIABLE cmake_consumer_error
)
if(NOT cmake_consumer_result EQUAL 0)
  message(FATAL_ERROR
    "packaged CMake consumer failed:\n"
    "${cmake_consumer_output}\n${cmake_consumer_error}"
  )
endif()

find_program(morpheus_package_pkg_config pkg-config REQUIRED)
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    "-DMORPHEUS_BUILD_DIR=${MORPHEUS_BUILD_DIR}"
    "-DMORPHEUS_SOURCE_DIR=${morpheus_source_dir}"
    "-DMORPHEUS_INSTALL_LIBDIR=${MORPHEUS_PACKAGE_LIBDIR}"
    "-DMORPHEUS_PKG_CONFIG=${morpheus_package_pkg_config}"
    "-DMORPHEUS_VERSION=${MORPHEUS_PACKAGE_VERSION}"
    "-DMORPHEUS_C_COMPILER=${MORPHEUS_PACKAGE_C_COMPILER}"
    "-DMORPHEUS_EXECUTABLE_SUFFIX=${MORPHEUS_PACKAGE_EXECUTABLE_SUFFIX}"
    "-DMORPHEUS_INSTALL_PREFIX=${prefix}"
    -P "${CMAKE_CURRENT_LIST_DIR}/test-installed-pkgconfig.cmake"
  RESULT_VARIABLE pkg_config_consumer_result
  OUTPUT_VARIABLE pkg_config_consumer_output
  ERROR_VARIABLE pkg_config_consumer_error
)
if(NOT pkg_config_consumer_result EQUAL 0)
  message(FATAL_ERROR
    "packaged pkg-config consumer failed:\n"
    "${pkg_config_consumer_output}\n${pkg_config_consumer_error}"
  )
endif()
