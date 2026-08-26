# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS MORPHEUS_GENER_DERIVATION_ROOT
                          MORPHEUS_GENER_DERIVATION_MANIFEST
                          MORPHEUS_GENER_CORPUS_MANIFEST
                          MORPHEUS_GENER_DERIVATION_SOURCE
                          MORPHEUS_GENER_DERIVATION_ORACLE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(STRINGS "${MORPHEUS_GENER_DERIVATION_MANIFEST}" manifest_lines)
set(rule_paths)
set(active_paths)
set(unreferenced_paths)
set(all_manifest_paths)

foreach(line IN LISTS manifest_lines)
  if(line STREQUAL "" OR line MATCHES "^#")
    continue()
  endif()
  string(REPLACE "\t" ";" fields "${line}")
  list(LENGTH fields field_count)
  if(NOT field_count EQUAL 3)
    message(FATAL_ERROR "invalid derivation manifest line: ${line}")
  endif()
  list(GET fields 0 group)
  list(GET fields 1 expected_sha256)
  list(GET fields 2 relative_path)
  if(NOT group STREQUAL "rules" AND NOT group STREQUAL "active" AND
     NOT group STREQUAL "unreferenced")
    message(FATAL_ERROR "invalid derivation manifest group: ${group}")
  endif()
  string(LENGTH "${expected_sha256}" sha256_length)
  if(NOT sha256_length EQUAL 64 OR
     NOT expected_sha256 MATCHES "^[0-9a-f]+$")
    message(FATAL_ERROR "invalid derivation SHA-256 for ${relative_path}")
  endif()
  if(NOT relative_path MATCHES
     "^(rule_files/derivtypes[.]table|derivs/source/[A-Za-z0-9_]+[.]deriv)$")
    message(FATAL_ERROR "unsafe derivation path: ${relative_path}")
  endif()
  list(FIND all_manifest_paths "${relative_path}" duplicate_at)
  if(NOT duplicate_at EQUAL -1)
    message(FATAL_ERROR "duplicate derivation path: ${relative_path}")
  endif()
  set(absolute_path "${MORPHEUS_GENER_DERIVATION_ROOT}/${relative_path}")
  if(NOT EXISTS "${absolute_path}")
    message(FATAL_ERROR "derivation input is missing: ${relative_path}")
  endif()
  file(SHA256 "${absolute_path}" actual_sha256)
  if(NOT actual_sha256 STREQUAL expected_sha256)
    message(FATAL_ERROR
      "derivation checksum mismatch for ${relative_path}: ${actual_sha256}")
  endif()
  list(APPEND all_manifest_paths "${relative_path}")
  if(group STREQUAL "rules")
    list(APPEND rule_paths "${relative_path}")
  elseif(group STREQUAL "active")
    list(APPEND active_paths "${relative_path}")
  else()
    list(APPEND unreferenced_paths "${relative_path}")
  endif()
endforeach()

if(NOT "${rule_paths}" STREQUAL "rule_files/derivtypes.table")
  message(FATAL_ERROR "unexpected derivation rule-table selection")
endif()

file(STRINGS
  "${MORPHEUS_GENER_DERIVATION_ROOT}/rule_files/derivtypes.table"
  derivation_types)
set(expected_active_paths)
set(active_type_names)
set(type_entry_count 0)
set(illw_entry_count 0)
foreach(line IN LISTS derivation_types)
  string(STRIP "${line}" line)
  if(line STREQUAL "" OR line MATCHES "^#")
    continue()
  endif()
  string(REGEX MATCH "^[^ \t]+" type_name "${line}")
  list(APPEND active_type_names "${type_name}")
  math(EXPR type_entry_count "${type_entry_count} + 1")
  if(type_name STREQUAL "illw")
    math(EXPR illw_entry_count "${illw_entry_count} + 1")
  endif()
  set(type_path "derivs/source/${type_name}.deriv")
  list(FIND expected_active_paths "${type_path}" seen_at)
  if(seen_at EQUAL -1)
    list(APPEND expected_active_paths "${type_path}")
  endif()
endforeach()
list(REMOVE_DUPLICATES active_type_names)

if(NOT "${active_paths}" STREQUAL "${expected_active_paths}")
  message(FATAL_ERROR
    "active derivation manifest no longer matches derivtypes.table order")
endif()
if(NOT type_entry_count EQUAL 39 OR NOT illw_entry_count EQUAL 2)
  message(FATAL_ERROR
    "historical derivation-type anomaly changed: ${type_entry_count} entries, ${illw_entry_count} illw entries")
endif()

file(GLOB all_source_paths
  RELATIVE "${MORPHEUS_GENER_DERIVATION_ROOT}"
  "${MORPHEUS_GENER_DERIVATION_ROOT}/derivs/source/*.deriv")
set(manifest_source_paths ${active_paths} ${unreferenced_paths})
list(SORT all_source_paths)
list(SORT manifest_source_paths)
if(NOT "${all_source_paths}" STREQUAL "${manifest_source_paths}")
  message(FATAL_ERROR "derivation manifest does not cover every source table")
endif()

set(expected_unreferenced_paths
  derivs/source/cw.deriv
  derivs/source/es_denom.deriv
  derivs/source/ow_fact.deriv
  derivs/source/ow_instr.deriv
  derivs/source/ww.deriv)
if(NOT "${unreferenced_paths}" STREQUAL "${expected_unreferenced_paths}")
  message(FATAL_ERROR "historical unreferenced derivation set changed")
endif()

list(LENGTH active_paths active_count)
list(LENGTH unreferenced_paths unreferenced_count)
if(NOT active_count EQUAL 38 OR NOT unreferenced_count EQUAL 5)
  message(FATAL_ERROR
    "unexpected derivation corpus size: ${active_count} active, ${unreferenced_count} unreferenced")
endif()

file(STRINGS "${MORPHEUS_GENER_CORPUS_MANIFEST}" corpus_manifest_lines)
set(invalid_derivations)
foreach(manifest_line IN LISTS corpus_manifest_lines)
  if(manifest_line STREQUAL "" OR manifest_line MATCHES "^#")
    continue()
  endif()
  string(REPLACE "\t" ";" corpus_fields "${manifest_line}")
  list(GET corpus_fields 2 source_path)
  file(STRINGS "${MORPHEUS_GENER_DERIVATION_ROOT}/${source_path}"
       source_lines REGEX "^:de:")
  foreach(source_line IN LISTS source_lines)
    string(REGEX MATCH "^:de:[^ \t]*[ \t]+([^ \t]+)"
           derivation_match "${source_line}")
    if(NOT derivation_match)
      set(type_name "<missing>")
    else()
      set(type_name "${CMAKE_MATCH_1}")
    endif()
    list(FIND active_type_names "${type_name}" type_at)
    if(type_at EQUAL -1)
      list(APPEND invalid_derivations
        "${source_path}:${type_name}")
    endif()
  endforeach()
endforeach()

set(expected_invalid_derivations
  "stemsrc/nom.proper:as_a"
  "stemsrc/vbs.simp.ml:e_stem,epic"
  "stemsrc/vbs.simp.ml:ew"
  "stemsrc/vbs.simp.ml:reg_conj,syll_aug"
  "stemsrc/vbs.simp.ml:melitt"
  "stemsrc/vbs.simp.ml:w_stem"
  "stemsrc/vbs.simp.ml:numi,poetic")
if(NOT "${invalid_derivations}" STREQUAL "${expected_invalid_derivations}")
  message(FATAL_ERROR
    "unresolved generation derivations changed: ${invalid_derivations}")
endif()

file(SHA256 "${MORPHEUS_GENER_DERIVATION_SOURCE}" source_fixture_sha256)
file(SHA256 "${MORPHEUS_GENER_DERIVATION_ORACLE}" oracle_fixture_sha256)
if(NOT source_fixture_sha256 STREQUAL
   "b01c28e6f0eadd09fedde75721b78d69ba0c84762744ededaa8d4274d0a3b128")
  message(FATAL_ERROR "generation derivation source fixture changed")
endif()
if(NOT oracle_fixture_sha256 STREQUAL
   "14eda4f57cfba9c269f63de8ba476750a0be4443510df7b992f918d1f5c389a0")
  message(FATAL_ERROR "generation derivation oracle fixture changed")
endif()

file(STRINGS "${MORPHEUS_GENER_DERIVATION_ORACLE}" oracle_lines)
set(oracle_lemma_count 0)
set(oracle_record_count 0)
foreach(line IN LISTS oracle_lines)
  if(line MATCHES "^:le:")
    math(EXPR oracle_lemma_count "${oracle_lemma_count} + 1")
  elseif(line MATCHES "^:(vs|aj|no):")
    math(EXPR oracle_record_count "${oracle_record_count} + 1")
  else()
    message(FATAL_ERROR "unexpected derivation oracle line: ${line}")
  endif()
endforeach()
if(NOT oracle_lemma_count EQUAL 4 OR NOT oracle_record_count EQUAL 9)
  message(FATAL_ERROR
    "unexpected derivation oracle size: ${oracle_lemma_count} lemmas, ${oracle_record_count} records")
endif()
