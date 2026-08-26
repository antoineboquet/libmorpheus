# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS MORPHEUS_GENER_SOURCE_PREPARER
                          MORPHEUS_GENER_INDEX_BUILDER
                          MORPHEUS_GENER_SOURCE_INPUT
                          MORPHEUS_GENER_SOURCE_EXPECTED
                          MORPHEUS_GENER_SOURCE_ORPHAN
                          MORPHEUS_GENER_SOURCE_DERIVED
                          MORPHEUS_GENER_SOURCE_WORK_DIR)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(MAKE_DIRECTORY "${MORPHEUS_GENER_SOURCE_WORK_DIR}")
set(first "${MORPHEUS_GENER_SOURCE_WORK_DIR}/first.txt")
set(second "${MORPHEUS_GENER_SOURCE_WORK_DIR}/second.txt")
set(index "${MORPHEUS_GENER_SOURCE_WORK_DIR}/prepared.mgi")
file(REMOVE "${first}" "${second}" "${index}")

foreach(output IN ITEMS "${first}" "${second}")
  execute_process(
    COMMAND "${MORPHEUS_GENER_SOURCE_PREPARER}"
            "${output}" "${MORPHEUS_GENER_SOURCE_INPUT}"
    RESULT_VARIABLE prepare_result
    OUTPUT_VARIABLE prepare_output
    ERROR_VARIABLE prepare_error
  )
  if(NOT prepare_result EQUAL 0)
    message(FATAL_ERROR
      "generation-source preparation failed (${prepare_result}):\n${prepare_output}${prepare_error}")
  endif()
endforeach()

foreach(expected IN ITEMS "${MORPHEUS_GENER_SOURCE_EXPECTED}" "${second}")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E compare_files "${first}" "${expected}"
    RESULT_VARIABLE compare_result
  )
  if(NOT compare_result EQUAL 0)
    message(FATAL_ERROR "prepared generation source is not reproducible or exact")
  endif()
endforeach()

execute_process(
  COMMAND "${MORPHEUS_GENER_INDEX_BUILDER}" "${index}" "${first}"
  RESULT_VARIABLE index_result
  OUTPUT_VARIABLE index_output
  ERROR_VARIABLE index_error
)
if(NOT index_result EQUAL 0 OR NOT EXISTS "${index}")
  message(FATAL_ERROR
    "prepared generation source was rejected by the index builder (${index_result}):\n${index_output}${index_error}")
endif()

foreach(rejected IN ITEMS "${MORPHEUS_GENER_SOURCE_ORPHAN}"
                          "${MORPHEUS_GENER_SOURCE_DERIVED}")
  set(rejected_output "${MORPHEUS_GENER_SOURCE_WORK_DIR}/rejected.txt")
  file(REMOVE "${rejected_output}")
  execute_process(
    COMMAND "${MORPHEUS_GENER_SOURCE_PREPARER}"
            "${rejected_output}" "${rejected}"
    RESULT_VARIABLE rejected_result
    OUTPUT_QUIET
    ERROR_QUIET
  )
  if(rejected_result EQUAL 0 OR EXISTS "${rejected_output}")
    message(FATAL_ERROR "invalid generation source was not rejected: ${rejected}")
  endif()
endforeach()
