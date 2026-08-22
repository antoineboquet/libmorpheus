set(expected_symbols
  morpheus_abi_version
  morpheus_analysis_size
  morpheus_analyze
  morpheus_close
  morpheus_compat_analyze
  morpheus_compat_output_analysis_count
  morpheus_compat_output_data
  morpheus_compat_output_free
  morpheus_compat_output_lemma_count
  morpheus_compat_output_length
  morpheus_open
  morpheus_open_path
  morpheus_result_all_morph_flags
  morpheus_result_copy
  morpheus_result_count
  morpheus_result_free
  morpheus_result_get
  morpheus_result_truncated_fields
  morpheus_status_message
)

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
list(SORT actual_symbols)
list(SORT expected_symbols)
if(NOT actual_symbols STREQUAL expected_symbols)
  message(FATAL_ERROR
    "unexpected public ABI symbols\nexpected: ${expected_symbols}\n"
    "actual: ${actual_symbols}"
  )
endif()
