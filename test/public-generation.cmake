# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS
    MORPHEUS_GENER_INDEX_BUILDER
    MORPHEUS_PUBLIC_GENERATION_TEST
    MORPHEUS_GENERATION_SOURCE
    MORPHEUS_STEMLIB
    MORPHEUS_PUBLIC_GENERATION_WORK_DIR)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

set(root "${MORPHEUS_PUBLIC_GENERATION_WORK_DIR}/stemlib")
file(REMOVE_RECURSE "${MORPHEUS_PUBLIC_GENERATION_WORK_DIR}")
file(MAKE_DIRECTORY "${root}")
file(COPY "${MORPHEUS_STEMLIB}/Greek" DESTINATION "${root}")

execute_process(
  COMMAND "${MORPHEUS_GENER_INDEX_BUILDER}"
          "${root}/gener.index" "${MORPHEUS_GENERATION_SOURCE}"
  RESULT_VARIABLE build_result
  OUTPUT_VARIABLE build_output
  ERROR_VARIABLE build_error
)
if(NOT build_result EQUAL 0)
  message(FATAL_ERROR
    "public generation-index build failed (${build_result}):\n"
    "${build_output}${build_error}")
endif()

execute_process(
  COMMAND "${MORPHEUS_PUBLIC_GENERATION_TEST}" "${root}" "${MORPHEUS_STEMLIB}"
  RESULT_VARIABLE test_result
  OUTPUT_VARIABLE test_output
  ERROR_VARIABLE test_error
)
if(NOT test_result EQUAL 0)
  message(FATAL_ERROR
    "public generation test failed (${test_result}):\n"
    "${test_output}${test_error}")
endif()
