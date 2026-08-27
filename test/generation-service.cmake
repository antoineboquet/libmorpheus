# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS
        MORPHEUS_GENER_INDEX_BUILDER
        MORPHEUS_GENERATION_SERVICE_TEST
        MORPHEUS_GENERATION_SERVICE_SOURCE
        MORPHEUS_GENERATION_SERVICE_WORK_DIR
        MORPHEUS_GENERATION_SERVICE_MODE)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(REMOVE_RECURSE "${MORPHEUS_GENERATION_SERVICE_WORK_DIR}")
file(MAKE_DIRECTORY "${MORPHEUS_GENERATION_SERVICE_WORK_DIR}")
set(index "${MORPHEUS_GENERATION_SERVICE_WORK_DIR}/gener.index")
execute_process(
  COMMAND "${MORPHEUS_GENER_INDEX_BUILDER}" "${index}"
          "${MORPHEUS_GENERATION_SERVICE_SOURCE}"
  RESULT_VARIABLE builder_result
)
if(NOT builder_result EQUAL 0)
  message(FATAL_ERROR "generation index builder failed: ${builder_result}")
endif()

set(command "${MORPHEUS_GENERATION_SERVICE_TEST}"
            "${MORPHEUS_GENERATION_SERVICE_MODE}" "${index}")
if(MORPHEUS_GENERATION_SERVICE_MODE STREQUAL "differential" AND
   DEFINED MORPHEUS_GENERATION_FIXTURES)
  list(APPEND command "${MORPHEUS_GENERATION_FIXTURES}")
endif()
execute_process(COMMAND ${command} RESULT_VARIABLE service_result)
if(NOT service_result EQUAL 0)
  message(FATAL_ERROR "generation service test failed: ${service_result}")
endif()
