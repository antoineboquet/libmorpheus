# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)

foreach(required IN ITEMS MORPHEUS_STEMLIB_ROOT MORPHEUS_STEMLIB_MANIFEST)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(STRINGS "${MORPHEUS_STEMLIB_MANIFEST}" lines)
set(seen_paths)
set(expected_outputs)
foreach(language IN ITEMS Greek Latin)
  foreach(status IN ITEMS active excluded)
    foreach(kind IN ITEMS rule ending derivation)
      set(count_${language}_${status}_${kind} 0)
    endforeach()
  endforeach()
endforeach()

foreach(line IN LISTS lines)
  if(line MATCHES "^[ \t]*#" OR line MATCHES "^[ \t]*$")
    continue()
  endif()
  string(REPLACE "\t" ";" fields "${line}")
  list(LENGTH fields field_count)
  if(NOT field_count EQUAL 5)
    message(FATAL_ERROR "invalid stemlib manifest line: ${line}")
  endif()
  list(GET fields 0 language)
  list(GET fields 1 status)
  list(GET fields 2 kind)
  list(GET fields 3 relative_path)
  list(GET fields 4 expected_sha256)
  string(LENGTH "${expected_sha256}" sha256_length)
  if(NOT language MATCHES "^(Greek|Latin)$" OR
     NOT status MATCHES "^(active|excluded)$" OR
     NOT kind MATCHES "^(rule|ending|derivation)$" OR
     NOT sha256_length EQUAL 64 OR
     NOT expected_sha256 MATCHES "^[0-9a-f]+$")
    message(FATAL_ERROR "invalid stemlib manifest fields: ${line}")
  endif()
  if((kind STREQUAL "rule" AND
      NOT relative_path MATCHES "^rule_files/[A-Za-z0-9_.-]+[.]table$") OR
     (kind STREQUAL "ending" AND
      NOT relative_path MATCHES "^endtables/source/[A-Za-z0-9_.-]+[.]end$") OR
     (kind STREQUAL "derivation" AND
      NOT relative_path MATCHES "^derivs/source/[A-Za-z0-9_.-]+[.]deriv$"))
    message(FATAL_ERROR "unsafe stemlib manifest path: ${relative_path}")
  endif()
  if(kind STREQUAL "rule" AND NOT status STREQUAL "active")
    message(FATAL_ERROR "a required rule table is excluded: ${relative_path}")
  endif()

  set(key "${language}/${relative_path}")
  if(key IN_LIST seen_paths)
    message(FATAL_ERROR "duplicate stemlib manifest path: ${key}")
  endif()
  list(APPEND seen_paths "${key}")
  set(source "${MORPHEUS_STEMLIB_ROOT}/${key}")
  if(NOT EXISTS "${source}")
    message(FATAL_ERROR "stemlib source is missing: ${key}")
  endif()
  file(SHA256 "${source}" actual_sha256)
  if(NOT actual_sha256 STREQUAL expected_sha256)
    message(FATAL_ERROR "stemlib source checksum mismatch: ${key}")
  endif()

  math(EXPR count_${language}_${status}_${kind}
       "${count_${language}_${status}_${kind}} + 1")
  if(kind MATCHES "^(ending|derivation)$")
    get_filename_component(table_name "${relative_path}" NAME_WE)
    if(kind STREQUAL "ending")
      set(output "${language}/endtables/out/${table_name}.out")
    else()
      set(output "${language}/derivs/out/${table_name}.out")
    endif()
    if(status STREQUAL "active")
      if(NOT EXISTS "${MORPHEUS_STEMLIB_ROOT}/${output}")
        message(FATAL_ERROR "active stemlib baseline is missing: ${output}")
      endif()
      list(APPEND expected_outputs "${output}")
    elseif(EXISTS "${MORPHEUS_STEMLIB_ROOT}/${output}")
      message(FATAL_ERROR "excluded stemlib source has a baseline: ${output}")
    endif()
  endif()
endforeach()

foreach(language IN ITEMS Greek Latin)
  foreach(pattern IN ITEMS "rule_files/*.table"
                           "endtables/source/*.end"
                           "derivs/source/*.deriv")
    file(GLOB actual_sources RELATIVE "${MORPHEUS_STEMLIB_ROOT}/${language}"
         "${MORPHEUS_STEMLIB_ROOT}/${language}/${pattern}")
    foreach(relative_path IN LISTS actual_sources)
      if(NOT "${language}/${relative_path}" IN_LIST seen_paths)
        message(FATAL_ERROR
                "unexpected stemlib source is not manifested: ${language}/${relative_path}")
      endif()
    endforeach()
  endforeach()
  foreach(pattern IN ITEMS "endtables/out/*.out" "derivs/out/*.out")
    file(GLOB actual_outputs RELATIVE "${MORPHEUS_STEMLIB_ROOT}/${language}"
         "${MORPHEUS_STEMLIB_ROOT}/${language}/${pattern}")
    foreach(relative_path IN LISTS actual_outputs)
      if(NOT "${language}/${relative_path}" IN_LIST expected_outputs)
        message(FATAL_ERROR
                "unexpected compiled stemlib baseline: ${language}/${relative_path}")
      endif()
    endforeach()
  endforeach()
endforeach()

if(NOT count_Greek_active_rule EQUAL 7 OR
   NOT count_Greek_active_ending EQUAL 139 OR
   NOT count_Greek_excluded_ending EQUAL 7 OR
   NOT count_Greek_active_derivation EQUAL 38 OR
   NOT count_Greek_excluded_derivation EQUAL 5 OR
   NOT count_Latin_active_rule EQUAL 6 OR
   NOT count_Latin_active_ending EQUAL 101 OR
   NOT count_Latin_excluded_ending EQUAL 0 OR
   NOT count_Latin_active_derivation EQUAL 3 OR
   NOT count_Latin_excluded_derivation EQUAL 0)
  message(FATAL_ERROR "stemlib source selection changed without review")
endif()
