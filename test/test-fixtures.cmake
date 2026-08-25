# SPDX-License-Identifier: AGPL-3.0-or-later

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
  string(JSON fixture_member_count LENGTH
         "${fixtures_json}" ${fixture_index})
  set(has_options FALSE)
  if(fixture_member_count GREATER 0)
    math(EXPR fixture_member_last "${fixture_member_count} - 1")
    foreach(fixture_member_index RANGE 0 ${fixture_member_last})
      string(JSON fixture_member MEMBER
             "${fixtures_json}" ${fixture_index} ${fixture_member_index})
      if(fixture_member STREQUAL "opts")
        set(has_options TRUE)
        break()
      endif()
    endforeach()
  endif()

  set(options "")
  if(has_options)
    string(JSON options_type TYPE
           "${fixtures_json}" ${fixture_index} opts)
    if(NOT options_type STREQUAL "ARRAY")
      message(FATAL_ERROR
        "fixture ${fixture_index} has a non-array opts member in ${MORPHEUS_FIXTURES}")
    endif()
    string(JSON option_count LENGTH
           "${fixtures_json}" ${fixture_index} opts)
    if(option_count GREATER 0)
      math(EXPR option_last "${option_count} - 1")
      foreach(option_index RANGE 0 ${option_last})
        string(JSON option_type TYPE
               "${fixtures_json}" ${fixture_index} opts ${option_index})
        if(NOT option_type STREQUAL "STRING")
          message(FATAL_ERROR
            "fixture ${fixture_index} option ${option_index} is not a string")
        endif()
        string(JSON option GET
               "${fixtures_json}" ${fixture_index} opts ${option_index})
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
