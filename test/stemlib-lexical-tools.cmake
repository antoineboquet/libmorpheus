# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS MORPHEUS_INDEXNOMS MORPHEUS_INDEXVBS
                          MORPHEUS_WORK_DIR)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(REMOVE_RECURSE "${MORPHEUS_WORK_DIR}")
file(MAKE_DIRECTORY "${MORPHEUS_WORK_DIR}")
set(missing_input "${MORPHEUS_WORK_DIR}/missing-input")
set(empty_input "${MORPHEUS_WORK_DIR}/empty-input")
file(WRITE "${empty_input}" "")

function(expect_failure label program)
  execute_process(
    COMMAND "${program}" ${ARGN}
    RESULT_VARIABLE result
    OUTPUT_QUIET
    ERROR_QUIET
  )
  if(result EQUAL 0)
    message(FATAL_ERROR "${label} unexpectedly succeeded")
  endif()
endfunction()

expect_failure("indexnoms without explicit paths" "${MORPHEUS_INDEXNOMS}")
expect_failure("indexvbs without explicit paths" "${MORPHEUS_INDEXVBS}")
expect_failure(
  "indexnoms with missing input"
  "${MORPHEUS_INDEXNOMS}"
  "${missing_input}"
  "${MORPHEUS_WORK_DIR}/nomind"
)
expect_failure(
  "indexvbs with missing input"
  "${MORPHEUS_INDEXVBS}"
  "${missing_input}"
  "${MORPHEUS_WORK_DIR}/vbind"
)
expect_failure(
  "indexnoms with empty input"
  "${MORPHEUS_INDEXNOMS}"
  "${empty_input}"
  "${MORPHEUS_WORK_DIR}/nomind"
)
expect_failure(
  "indexvbs with empty input"
  "${MORPHEUS_INDEXVBS}"
  "${empty_input}"
  "${MORPHEUS_WORK_DIR}/vbind"
)

foreach(unexpected IN ITEMS nomind nomind.lindex vbind vbind.lindex)
  if(EXISTS "${MORPHEUS_WORK_DIR}/${unexpected}")
    message(FATAL_ERROR "failed lexical index left ${unexpected}")
  endif()
endforeach()
