if(NOT DEFINED MORPHEUS_FIXTURES OR
   NOT DEFINED MORPHEUS_CRUNCHER OR
   NOT DEFINED MORPHEUS_STEMLIB OR
   NOT DEFINED MORPHEUS_WORK_DIR)
  message(FATAL_ERROR
    "MORPHEUS_FIXTURES, MORPHEUS_CRUNCHER, MORPHEUS_STEMLIB, and MORPHEUS_WORK_DIR are required")
endif()

file(READ "${MORPHEUS_FIXTURES}" fixtures_json)
string(JSON fixture_count LENGTH "${fixtures_json}")
if(fixture_count EQUAL 0)
  message(FATAL_ERROR "fixture file is empty: ${MORPHEUS_FIXTURES}")
endif()

file(MAKE_DIRECTORY "${MORPHEUS_WORK_DIR}")
set(failures "")
math(EXPR fixture_last "${fixture_count} - 1")
foreach(fixture_index RANGE 0 ${fixture_last})
  string(JSON input GET "${fixtures_json}" ${fixture_index} input)
  string(JSON expected GET "${fixtures_json}" ${fixture_index} expected)
  string(
    JSON opts_type_error ERROR_VARIABLE opts_type_error
    TYPE "${fixtures_json}" ${fixture_index} opts
  )
  set(options "")
  if(opts_type_error STREQUAL "NOTFOUND")
    string(JSON opts_type GET "${fixtures_json}" ${fixture_index} opts)
    if(NOT opts_type STREQUAL "ARRAY")
      message(FATAL_ERROR
        "fixture ${fixture_index} has a non-array opts member in ${MORPHEUS_FIXTURES}")
    endif()
    string(JSON option_count LENGTH "${fixtures_json}" ${fixture_index} opts)
    if(option_count GREATER 0)
      math(EXPR option_last "${option_count} - 1")
      foreach(option_index RANGE 0 ${option_last})
        string(JSON option GET "${fixtures_json}" ${fixture_index} opts ${option_index})
        list(APPEND options "${option}")
      endforeach()
    endif()
  endif()

  set(input_file "${MORPHEUS_WORK_DIR}/fixture-${fixture_index}.input")
  file(WRITE "${input_file}" "${input}\n")
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "MORPHLIB=${MORPHEUS_STEMLIB}"
            "${MORPHEUS_CRUNCHER}" ${options}
    INPUT_FILE "${input_file}"
    RESULT_VARIABLE cruncher_result
    OUTPUT_VARIABLE actual
    ERROR_VARIABLE cruncher_error
  )
  if(NOT cruncher_result EQUAL 0 OR NOT actual STREQUAL expected)
    string(APPEND failures
      "\nFixture ${fixture_index}: ${input}\n"
      "Options: ${options}\n"
      "Expected: [${expected}]\n"
      "Actual:   [${actual}]\n"
      "Exit: ${cruncher_result}\n"
      "Stderr: [${cruncher_error}]\n")
  endif()
endforeach()

if(NOT failures STREQUAL "")
  message(FATAL_ERROR
    "Cruncher fixture failures for ${MORPHEUS_FIXTURES}:${failures}")
endif()

message(STATUS "Validated ${fixture_count} cruncher fixtures from ${MORPHEUS_FIXTURES}")
