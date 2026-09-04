# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)

foreach(required IN ITEMS MORPHEUS_STEMLIB_ROOT MORPHEUS_STEMLIB_MANIFEST
                          MORPHEUS_STEMLIB_MANIFEST_VALIDATOR
                          MORPHEUS_STEMLIB_STAGER MORPHEUS_STEMLIB_LANGUAGE
                          MORPHEUS_STEMLIB_STAGE MORPHEUS_BUILDEND
                          MORPHEUS_BUILDDERIV MORPHEUS_INDENDTABLES
                          MORPHEUS_INDDERIVTABLES)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()
if(NOT MORPHEUS_STEMLIB_LANGUAGE MATCHES "^(Greek|Latin)$")
  message(FATAL_ERROR "MORPHEUS_STEMLIB_LANGUAGE must be Greek or Latin")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}"
          -DMORPHEUS_STEMLIB_ROOT=${MORPHEUS_STEMLIB_ROOT}
          -DMORPHEUS_STEMLIB_MANIFEST=${MORPHEUS_STEMLIB_MANIFEST}
          -DMORPHEUS_STEMLIB_MANIFEST_VALIDATOR=${MORPHEUS_STEMLIB_MANIFEST_VALIDATOR}
          -DMORPHEUS_STEMLIB_LANGUAGE=${MORPHEUS_STEMLIB_LANGUAGE}
          -DMORPHEUS_STEMLIB_STAGE=${MORPHEUS_STEMLIB_STAGE}
          -P "${MORPHEUS_STEMLIB_STAGER}"
  RESULT_VARIABLE stage_result
  OUTPUT_VARIABLE stage_output
  ERROR_VARIABLE stage_error
)
if(NOT stage_result EQUAL 0)
  message(FATAL_ERROR "stemlib staging failed:\n${stage_output}${stage_error}")
endif()

set(stage_root "${MORPHEUS_STEMLIB_STAGE}")
set(language_root "${stage_root}/${MORPHEUS_STEMLIB_LANGUAGE}")
set(language_option)
if(MORPHEUS_STEMLIB_LANGUAGE STREQUAL "Latin")
  set(language_option -L)
endif()
set(build_environment
    "MORPHLIB=${stage_root}" "LC_ALL=C" "LANG=C" "TZ=UTC")

function(run_producer label program)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env ${build_environment}
            "${program}" ${ARGN}
    WORKING_DIRECTORY "${language_root}"
    RESULT_VARIABLE producer_result
    OUTPUT_VARIABLE producer_output
    ERROR_VARIABLE producer_error
  )
  if(NOT producer_result EQUAL 0)
    message(FATAL_ERROR
            "${label} failed:\n${producer_output}${producer_error}")
  endif()
endfunction()

file(STRINGS "${stage_root}/ending-tables.list" ending_tables)
foreach(table IN LISTS ending_tables)
  run_producer("ending table ${table}" "${MORPHEUS_BUILDEND}"
               ${language_option} "${table}")
endforeach()

file(STRINGS "${stage_root}/derivation-tables.list" derivation_tables)
foreach(table IN LISTS derivation_tables)
  run_producer("derivation table ${table}" "${MORPHEUS_BUILDDERIV}"
               ${language_option} "${table}")
endforeach()

run_producer("nominal ending index" "${MORPHEUS_INDENDTABLES}"
             ${language_option} -f "${stage_root}/ending-tables.list" nom)
run_producer("verb ending index" "${MORPHEUS_INDENDTABLES}"
             ${language_option} -f "${stage_root}/ending-tables.list" verb)
run_producer("derivation index" "${MORPHEUS_INDDERIVTABLES}"
             ${language_option} -f
             "${stage_root}/derivation-index-tables.list")

file(GLOB_RECURSE outputs
     RELATIVE "${stage_root}"
     "${language_root}/endtables/ascii/*.asc"
     "${language_root}/endtables/out/*.out"
     "${language_root}/endtables/indices/*"
     "${language_root}/derivs/ascii/*.asc"
     "${language_root}/derivs/out/*.out"
     "${language_root}/derivs/indices/*")
list(SORT outputs)
set(receipt "# SPDX-License-Identifier: MPL-2.0\n")
set(receipt "${receipt}# path\tsha256\n")
foreach(relative_path IN LISTS outputs)
  file(SHA256 "${stage_root}/${relative_path}" output_sha256)
  string(APPEND receipt "${relative_path}\t${output_sha256}\n")
endforeach()
file(WRITE "${stage_root}/MORPHEUS-STEMLIB-TABLE-OUTPUTS.tsv" "${receipt}")
