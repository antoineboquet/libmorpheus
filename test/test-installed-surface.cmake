# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_INSTALL_BINDIR OR
   NOT DEFINED MORPHEUS_INSTALL_INCLUDEDIR OR
   NOT DEFINED MORPHEUS_INSTALL_LIBDIR OR
   NOT DEFINED MORPHEUS_LIBRARY_FILE_NAME OR
   NOT DEFINED MORPHEUS_CRUNCHER_FILE_NAME OR
   NOT DEFINED MORPHEUS_EXPECT_CRUNCHER)
  message(FATAL_ERROR "installed-surface test arguments are incomplete")
endif()

set(test_root "${MORPHEUS_BUILD_DIR}/installed-surface-test")
set(prefix "${test_root}/prefix")
file(REMOVE_RECURSE "${test_root}")

execute_process(
  COMMAND "${CMAKE_COMMAND}" --install "${MORPHEUS_BUILD_DIR}"
          --prefix "${prefix}"
  RESULT_VARIABLE install_result
  OUTPUT_VARIABLE install_output
  ERROR_VARIABLE install_error
)
if(NOT install_result EQUAL 0)
  message(FATAL_ERROR "install failed:\n${install_output}\n${install_error}")
endif()

set(public_header
    "${prefix}/${MORPHEUS_INSTALL_INCLUDEDIR}/morpheus/morpheus.h")
set(compat_header
    "${prefix}/${MORPHEUS_INSTALL_INCLUDEDIR}/morpheus/compat.h")
set(public_library
    "${prefix}/${MORPHEUS_INSTALL_LIBDIR}/${MORPHEUS_LIBRARY_FILE_NAME}")
set(package_directory
    "${prefix}/${MORPHEUS_INSTALL_LIBDIR}/cmake/Morpheus")
set(pkg_config_file
    "${prefix}/${MORPHEUS_INSTALL_LIBDIR}/pkgconfig/libmorpheus.pc")

foreach(required IN ITEMS
    "${public_header}"
    "${compat_header}"
    "${public_library}"
    "${package_directory}/MorpheusConfig.cmake"
    "${package_directory}/MorpheusConfigVersion.cmake"
    "${package_directory}/MorpheusTargets.cmake"
    "${pkg_config_file}")
  if(NOT EXISTS "${required}")
    message(FATAL_ERROR "required installed file is missing: ${required}")
  endif()
endforeach()

if(MORPHEUS_EXPECT_CRUNCHER)
  set(cruncher
      "${prefix}/${MORPHEUS_INSTALL_BINDIR}/${MORPHEUS_CRUNCHER_FILE_NAME}")
  if(NOT EXISTS "${cruncher}")
    message(FATAL_ERROR "installed cruncher is missing: ${cruncher}")
  endif()
endif()

file(
  GLOB_RECURSE installed_headers
  LIST_DIRECTORIES false
  "${prefix}/${MORPHEUS_INSTALL_INCLUDEDIR}/*"
)
list(LENGTH installed_headers installed_header_count)
list(SORT installed_headers)
set(expected_headers "${compat_header}" "${public_header}")
list(SORT expected_headers)
if(NOT installed_header_count EQUAL 2 OR
   NOT installed_headers STREQUAL expected_headers)
  message(FATAL_ERROR
    "private headers entered the installation: ${installed_headers}"
  )
endif()

file(GLOB_RECURSE installed_files LIST_DIRECTORIES false "${prefix}/*")
foreach(installed_file IN LISTS installed_files)
  file(RELATIVE_PATH relative_file "${prefix}" "${installed_file}")
  if(relative_file MATCHES "(^|/)src/" OR
     relative_file MATCHES
       "morpheus_(anal|gener|gkends|gkdict|morphlib|greeklib|build_options)" OR
     relative_file MATCHES "\\.a$")
    message(FATAL_ERROR
      "internal build artifact entered the installation: ${relative_file}"
    )
  endif()
endforeach()
