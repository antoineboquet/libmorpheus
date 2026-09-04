# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)

foreach(required IN ITEMS MORPHEUS_STEMLIB_ROOT MORPHEUS_STEMLIB_MANIFEST
                          MORPHEUS_STEMLIB_MANIFEST_VALIDATOR
                          MORPHEUS_STEMLIB_LANGUAGE MORPHEUS_STEMLIB_STAGE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()
if(NOT MORPHEUS_STEMLIB_LANGUAGE MATCHES "^(Greek|Latin)$")
  message(FATAL_ERROR "MORPHEUS_STEMLIB_LANGUAGE must be Greek or Latin")
endif()

get_filename_component(source_root "${MORPHEUS_STEMLIB_ROOT}" REALPATH)
get_filename_component(stage_root "${MORPHEUS_STEMLIB_STAGE}" ABSOLUTE)
string(FIND "${stage_root}/" "${source_root}/" stage_in_source)
if(stage_in_source EQUAL 0)
  message(FATAL_ERROR "the staging directory must be outside the source stemlib")
endif()
if(EXISTS "${stage_root}")
  message(FATAL_ERROR "the staging directory must not already exist: ${stage_root}")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}"
          -DMORPHEUS_STEMLIB_ROOT=${source_root}
          -DMORPHEUS_STEMLIB_MANIFEST=${MORPHEUS_STEMLIB_MANIFEST}
          -P "${MORPHEUS_STEMLIB_MANIFEST_VALIDATOR}"
  RESULT_VARIABLE validation_result
  OUTPUT_VARIABLE validation_output
  ERROR_VARIABLE validation_error
)
if(NOT validation_result EQUAL 0)
  message(FATAL_ERROR
          "stemlib source validation failed:\n${validation_output}${validation_error}")
endif()

set(language_root "${stage_root}/${MORPHEUS_STEMLIB_LANGUAGE}")
file(MAKE_DIRECTORY
     "${language_root}/endtables/ascii"
     "${language_root}/endtables/out"
     "${language_root}/endtables/indices"
     "${language_root}/derivs/ascii"
     "${language_root}/derivs/out"
     "${language_root}/derivs/indices")

set(receipt "# SPDX-License-Identifier: MPL-2.0\n")
set(receipt "${receipt}# language\tstatus\tkind\tpath\tsha256\n")
set(ending_list)
set(derivation_list)
set(derivation_names)
set(active_count 0)
file(STRINGS "${MORPHEUS_STEMLIB_MANIFEST}" lines)
foreach(line IN LISTS lines)
  if(line MATCHES "^[ \t]*#" OR line MATCHES "^[ \t]*$")
    continue()
  endif()
  string(REPLACE "\t" ";" fields "${line}")
  list(GET fields 0 language)
  list(GET fields 1 status)
  list(GET fields 2 kind)
  list(GET fields 3 relative_path)
  list(GET fields 4 expected_sha256)
  if(NOT language STREQUAL MORPHEUS_STEMLIB_LANGUAGE OR
     NOT status STREQUAL "active")
    continue()
  endif()

  set(source "${source_root}/${language}/${relative_path}")
  set(destination "${language_root}/${relative_path}")
  get_filename_component(destination_directory "${destination}" DIRECTORY)
  file(MAKE_DIRECTORY "${destination_directory}")
  file(COPY_FILE "${source}" "${destination}" ONLY_IF_DIFFERENT)
  file(SHA256 "${destination}" staged_sha256)
  if(NOT staged_sha256 STREQUAL expected_sha256)
    message(FATAL_ERROR "staged input checksum mismatch: ${relative_path}")
  endif()
  string(APPEND receipt "${line}\n")
  math(EXPR active_count "${active_count} + 1")

  if(kind MATCHES "^(ending|derivation)$")
    get_filename_component(table_name "${relative_path}" NAME_WE)
    if(kind STREQUAL "ending")
      string(APPEND ending_list "${table_name}\n")
    else()
      string(APPEND derivation_list "${table_name}\n")
      list(APPEND derivation_names "${table_name}")
    endif()
  endif()
endforeach()

if(active_count EQUAL 0)
  message(FATAL_ERROR "the selected language has no active stemlib inputs")
endif()

file(STRINGS
     "${language_root}/rule_files/derivtypes.table" derivation_registry)
set(derivation_index_list)
foreach(table_name IN LISTS derivation_names)
  set(table_registered 0)
  set(table_indexed 0)
  foreach(registry_line IN LISTS derivation_registry)
    if(registry_line MATCHES
       "^${table_name}[ \t]+[^ \t]+[ \t]+([^ \t]+)")
      set(table_registered 1)
      if(CMAKE_MATCH_1 STREQUAL "reg_deriv")
        set(table_indexed 1)
      endif()
    endif()
  endforeach()
  if(NOT table_registered)
    message(FATAL_ERROR
            "active derivation is absent from its registry: ${table_name}")
  endif()
  if(table_indexed)
    string(APPEND derivation_index_list "${table_name}\n")
  endif()
endforeach()

file(WRITE "${stage_root}/MORPHEUS-STEMLIB-INPUTS.tsv" "${receipt}")
file(WRITE "${stage_root}/ending-tables.list" "${ending_list}")
file(WRITE "${stage_root}/derivation-tables.list" "${derivation_list}")
file(WRITE "${stage_root}/derivation-index-tables.list"
     "${derivation_index_list}")
