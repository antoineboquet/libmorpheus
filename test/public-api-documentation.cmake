if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

include("${MORPHEUS_SOURCE_DIR}/cmake/PublicApi.cmake")
set(api_declarations)
foreach(public_header IN ITEMS morpheus.h compat.h)
  file(
    STRINGS "${MORPHEUS_SOURCE_DIR}/include/morpheus/${public_header}"
    header_declarations REGEX "^MORPHEUS_API"
  )
  list(APPEND api_declarations ${header_declarations})
endforeach()

set(header_symbols)
foreach(declaration IN LISTS api_declarations)
  string(REGEX MATCH "[A-Za-z_][A-Za-z0-9_]*\\(" function_token
               "${declaration}")
  if(NOT function_token)
    message(FATAL_ERROR "could not parse public declaration: ${declaration}")
  endif()
  string(REGEX REPLACE "\\($" "" function_name "${function_token}")
  list(APPEND header_symbols "${function_name}")
endforeach()

list(SORT header_symbols)
list(SORT MORPHEUS_PUBLIC_API_SYMBOLS)
if(NOT header_symbols STREQUAL MORPHEUS_PUBLIC_API_SYMBOLS)
  message(FATAL_ERROR
    "public API manifest differs from morpheus.h\n"
    "header: ${header_symbols}\nmanifest: ${MORPHEUS_PUBLIC_API_SYMBOLS}"
  )
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/docs/public-api.md" api_documentation)
foreach(symbol IN LISTS MORPHEUS_PUBLIC_API_SYMBOLS)
  string(FIND "${api_documentation}" "`${symbol}()`" documented_at)
  if(documented_at EQUAL -1)
    message(FATAL_ERROR "public function ${symbol}() is undocumented")
  endif()
endforeach()
