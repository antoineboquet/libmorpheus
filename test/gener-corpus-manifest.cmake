# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS MORPHEUS_GENER_CORPUS_ROOT
                          MORPHEUS_GENER_CORPUS_MANIFEST)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

if(NOT IS_DIRECTORY "${MORPHEUS_GENER_CORPUS_ROOT}/stemsrc")
  message(FATAL_ERROR
    "Greek stem source directory is missing: ${MORPHEUS_GENER_CORPUS_ROOT}/stemsrc")
endif()

file(STRINGS "${MORPHEUS_GENER_CORPUS_MANIFEST}" manifest_lines)
set(manifest_nominal)
set(manifest_verbs)
set(manifest_paths)

foreach(line IN LISTS manifest_lines)
  if(line STREQUAL "" OR line MATCHES "^#")
    continue()
  endif()
  string(REPLACE "\t" ";" fields "${line}")
  list(LENGTH fields field_count)
  if(NOT field_count EQUAL 3)
    message(FATAL_ERROR "invalid generation-corpus manifest line: ${line}")
  endif()
  list(GET fields 0 group)
  list(GET fields 1 expected_sha256)
  list(GET fields 2 relative_path)
  if(NOT group STREQUAL "nominal" AND NOT group STREQUAL "verb")
    message(FATAL_ERROR "invalid generation-corpus group: ${group}")
  endif()
  string(LENGTH "${expected_sha256}" sha256_length)
  if(NOT sha256_length EQUAL 64 OR
     NOT expected_sha256 MATCHES "^[0-9a-f]+$")
    message(FATAL_ERROR "invalid SHA-256 for ${relative_path}")
  endif()
  if(NOT relative_path MATCHES "^stemsrc/[A-Za-z0-9._-]+$")
    message(FATAL_ERROR "unsafe generation-corpus path: ${relative_path}")
  endif()
  list(FIND manifest_paths "${relative_path}" duplicate_at)
  if(NOT duplicate_at EQUAL -1)
    message(FATAL_ERROR "duplicate generation-corpus path: ${relative_path}")
  endif()
  set(absolute_path "${MORPHEUS_GENER_CORPUS_ROOT}/${relative_path}")
  if(NOT EXISTS "${absolute_path}")
    message(FATAL_ERROR "generation-corpus input is missing: ${relative_path}")
  endif()
  file(SHA256 "${absolute_path}" actual_sha256)
  if(NOT actual_sha256 STREQUAL expected_sha256)
    message(FATAL_ERROR
      "generation-corpus checksum mismatch for ${relative_path}: ${actual_sha256}")
  endif()
  list(APPEND manifest_paths "${relative_path}")
  if(group STREQUAL "nominal")
    list(APPEND manifest_nominal "${relative_path}")
  else()
    list(APPEND manifest_verbs "${relative_path}")
  endif()
endforeach()

file(GLOB nominal_files
  RELATIVE "${MORPHEUS_GENER_CORPUS_ROOT}"
  "${MORPHEUS_GENER_CORPUS_ROOT}/stemsrc/nom.*"
  "${MORPHEUS_GENER_CORPUS_ROOT}/stemsrc/nom[0-9]*"
)
list(FILTER nominal_files EXCLUDE REGEX "~$")
list(APPEND nominal_files stemsrc/lsj.nom stemsrc/lsj.byhand)
set(verb_files
  stemsrc/vbs.irreg
  stemsrc/vbs.simp.ml
  stemsrc/vbs.simp.02.new
  stemsrc/lsj.vbs
)

if(NOT "${manifest_nominal}" STREQUAL "${nominal_files}")
  message(FATAL_ERROR
    "nominal generation-corpus manifest no longer matches the historical build order")
endif()
if(NOT "${manifest_verbs}" STREQUAL "${verb_files}")
  message(FATAL_ERROR
    "verb generation-corpus manifest no longer matches the historical build order")
endif()

list(LENGTH manifest_nominal nominal_count)
list(LENGTH manifest_verbs verb_count)
if(NOT nominal_count EQUAL 45 OR NOT verb_count EQUAL 4)
  message(FATAL_ERROR
    "unexpected generation-corpus size: ${nominal_count} nominal, ${verb_count} verb")
endif()
