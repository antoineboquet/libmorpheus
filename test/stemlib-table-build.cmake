# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)

foreach(required IN ITEMS MORPHEUS_STEMLIB_ROOT MORPHEUS_STEMLIB_MANIFEST
                          MORPHEUS_STEMLIB_MANIFEST_VALIDATOR
                          MORPHEUS_STEMLIB_STAGER MORPHEUS_STEMLIB_BUILDER
                          MORPHEUS_BUILDEND MORPHEUS_BUILDDERIV
                          MORPHEUS_INDENDTABLES MORPHEUS_INDDERIVTABLES
                          MORPHEUS_WORK_DIR)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(REMOVE_RECURSE "${MORPHEUS_WORK_DIR}")
file(MAKE_DIRECTORY "${MORPHEUS_WORK_DIR}")

function(build_and_validate language expected_outputs)
  set(stage "${MORPHEUS_WORK_DIR}/${language}")
  execute_process(
    COMMAND "${CMAKE_COMMAND}"
            -DMORPHEUS_STEMLIB_ROOT=${MORPHEUS_STEMLIB_ROOT}
            -DMORPHEUS_STEMLIB_MANIFEST=${MORPHEUS_STEMLIB_MANIFEST}
            -DMORPHEUS_STEMLIB_MANIFEST_VALIDATOR=${MORPHEUS_STEMLIB_MANIFEST_VALIDATOR}
            -DMORPHEUS_STEMLIB_STAGER=${MORPHEUS_STEMLIB_STAGER}
            -DMORPHEUS_STEMLIB_LANGUAGE=${language}
            -DMORPHEUS_STEMLIB_STAGE=${stage}
            -DMORPHEUS_BUILDEND=${MORPHEUS_BUILDEND}
            -DMORPHEUS_BUILDDERIV=${MORPHEUS_BUILDDERIV}
            -DMORPHEUS_INDENDTABLES=${MORPHEUS_INDENDTABLES}
            -DMORPHEUS_INDDERIVTABLES=${MORPHEUS_INDDERIVTABLES}
            -P "${MORPHEUS_STEMLIB_BUILDER}"
    RESULT_VARIABLE build_result
    OUTPUT_VARIABLE build_output
    ERROR_VARIABLE build_error
  )
  if(NOT build_result EQUAL 0)
    message(FATAL_ERROR
            "${language} table build failed:\n${build_output}${build_error}")
  endif()

  set(receipt "${stage}/MORPHEUS-STEMLIB-TABLE-OUTPUTS.tsv")
  if(NOT EXISTS "${receipt}")
    message(FATAL_ERROR "${language} table build produced no receipt")
  endif()
  file(STRINGS "${receipt}" receipt_lines)
  set(output_count 0)
  foreach(line IN LISTS receipt_lines)
    if(line MATCHES "^[ \t]*#" OR line MATCHES "^[ \t]*$")
      continue()
    endif()
    string(REPLACE "\t" ";" fields "${line}")
    list(LENGTH fields field_count)
    if(NOT field_count EQUAL 2)
      message(FATAL_ERROR "invalid table-output receipt line: ${line}")
    endif()
    list(GET fields 0 relative_path)
    list(GET fields 1 expected_sha256)
    set(output "${stage}/${relative_path}")
    if(NOT EXISTS "${output}")
      message(FATAL_ERROR "received table output is missing: ${relative_path}")
    endif()
    file(SHA256 "${output}" actual_sha256)
    if(NOT actual_sha256 STREQUAL expected_sha256)
      message(FATAL_ERROR "received table output changed: ${relative_path}")
    endif()
    math(EXPR output_count "${output_count} + 1")
  endforeach()
  if(NOT output_count EQUAL expected_outputs)
    message(FATAL_ERROR
            "unexpected ${language} table-output count: ${output_count}")
  endif()
endfunction()

build_and_validate(Greek 357)
build_and_validate(Latin 211)
