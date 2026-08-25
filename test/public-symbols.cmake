# SPDX-License-Identifier: AGPL-3.0-or-later

cmake_minimum_required(VERSION 3.25)

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()
include("${MORPHEUS_SOURCE_DIR}/cmake/PublicApi.cmake")
set(expected_symbols ${MORPHEUS_PUBLIC_API_SYMBOLS})

if(MORPHEUS_APPLE)
  execute_process(
    COMMAND "${MORPHEUS_NM}" -gUj "${MORPHEUS_LIBRARY}"
    RESULT_VARIABLE nm_status
    OUTPUT_VARIABLE nm_output
    ERROR_VARIABLE nm_error
  )
  string(REGEX REPLACE "(^|\n)_" "\\1" nm_output "${nm_output}")
else()
  execute_process(
    COMMAND "${MORPHEUS_NM}" -D --defined-only "${MORPHEUS_LIBRARY}"
    RESULT_VARIABLE nm_status
    OUTPUT_VARIABLE nm_output
    ERROR_VARIABLE nm_error
  )
  string(REGEX MATCHALL "[A-Za-z_][A-Za-z0-9_]*($|\n)" symbol_lines
                       "${nm_output}")
  set(nm_output "")
  foreach(symbol IN LISTS symbol_lines)
    string(STRIP "${symbol}" symbol)
    string(APPEND nm_output "${symbol}\n")
  endforeach()
endif()

if(NOT nm_status EQUAL 0)
  message(FATAL_ERROR "nm failed: ${nm_error}")
endif()

string(REPLACE "\n" ";" actual_symbols "${nm_output}")
list(FILTER actual_symbols EXCLUDE REGEX "^$")
# musl's ELF startup objects publish these toolchain-owned entry points.
list(FILTER actual_symbols EXCLUDE REGEX "^_(init|fini)$")
list(SORT actual_symbols)
list(SORT expected_symbols)
if(NOT actual_symbols STREQUAL expected_symbols)
  message(FATAL_ERROR
    "unexpected public ABI symbols\nexpected: ${expected_symbols}\n"
    "actual: ${actual_symbols}"
  )
endif()

if(MORPHEUS_APPLE)
  execute_process(
    COMMAND "${MORPHEUS_NM}" -guj "${MORPHEUS_LIBRARY}"
    RESULT_VARIABLE undefined_status
    OUTPUT_VARIABLE undefined_output
    ERROR_VARIABLE undefined_error
  )
else()
  execute_process(
    COMMAND "${MORPHEUS_NM}" -D --undefined-only "${MORPHEUS_LIBRARY}"
    RESULT_VARIABLE undefined_status
    OUTPUT_VARIABLE undefined_output
    ERROR_VARIABLE undefined_error
  )
endif()

if(NOT undefined_status EQUAL 0)
  message(FATAL_ERROR "nm failed while reading imports: ${undefined_error}")
endif()

string(REPLACE "\n" ";" undefined_lines "${undefined_output}")
foreach(undefined_line IN LISTS undefined_lines)
  string(STRIP "${undefined_line}" undefined_line)
  string(REPLACE "\t" " " undefined_line "${undefined_line}")
  string(REGEX REPLACE "^.* " "" undefined_symbol "${undefined_line}")
  string(REGEX REPLACE "@.*$" "" undefined_symbol "${undefined_symbol}")
  if(MORPHEUS_APPLE)
    string(REGEX REPLACE "^_" "" undefined_symbol "${undefined_symbol}")
  endif()
  if(undefined_symbol STREQUAL "exit" OR undefined_symbol STREQUAL "abort")
    message(FATAL_ERROR
      "libmorpheus must not terminate its host process: ${undefined_line}"
    )
  endif()
endforeach()
