if(NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_PACKAGE_DIR)
  message(FATAL_ERROR "MORPHEUS_BUILD_DIR and MORPHEUS_PACKAGE_DIR are required")
endif()

include("${MORPHEUS_BUILD_DIR}/CPackConfig.cmake")
set(package_basename "libmorpheus-deno-${CPACK_PACKAGE_VERSION}")
set(archive "${MORPHEUS_PACKAGE_DIR}/${package_basename}.tar.gz")
set(checksum "${archive}.sha256")
foreach(required IN ITEMS "${archive}" "${checksum}")
  if(NOT EXISTS "${required}")
    message(FATAL_ERROR "Deno binding package is missing: ${required}")
  endif()
endforeach()

file(SHA256 "${archive}" actual_checksum)
file(READ "${checksum}" recorded_checksum)
string(FIND "${recorded_checksum}" "${actual_checksum}" checksum_at)
if(checksum_at EQUAL -1)
  message(FATAL_ERROR "Deno binding package checksum does not match")
endif()

set(extract_root "${MORPHEUS_BUILD_DIR}/deno-binding-package-test")
file(REMOVE_RECURSE "${extract_root}")
file(MAKE_DIRECTORY "${extract_root}")
execute_process(
  COMMAND "${CMAKE_COMMAND}" -E tar xzf "${archive}"
  WORKING_DIRECTORY "${extract_root}"
  RESULT_VARIABLE extract_result
  OUTPUT_VARIABLE extract_output
  ERROR_VARIABLE extract_error
)
if(NOT extract_result EQUAL 0)
  message(FATAL_ERROR
    "could not extract Deno binding package:\n${extract_output}\n${extract_error}")
endif()

set(binding_dir "${extract_root}/${package_basename}")
foreach(required IN ITEMS mod.ts README.md LICENSE NOTICE)
  if(NOT EXISTS "${binding_dir}/${required}")
    message(FATAL_ERROR "Deno binding package is missing ${required}")
  endif()
endforeach()

find_program(deno_program deno REQUIRED)
execute_process(
  COMMAND "${deno_program}" check mod.ts
  WORKING_DIRECTORY "${binding_dir}"
  RESULT_VARIABLE check_result
  OUTPUT_VARIABLE check_output
  ERROR_VARIABLE check_error
)
if(NOT check_result EQUAL 0)
  message(FATAL_ERROR
    "packaged Deno binding does not type-check:\n${check_output}\n${check_error}")
endif()
