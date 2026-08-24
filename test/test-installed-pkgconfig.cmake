if(NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_INSTALL_LIBDIR OR
   NOT DEFINED MORPHEUS_PKG_CONFIG OR
   NOT DEFINED MORPHEUS_VERSION OR
   NOT DEFINED MORPHEUS_C_COMPILER OR
   NOT DEFINED MORPHEUS_EXECUTABLE_SUFFIX)
  message(FATAL_ERROR "installed pkg-config test arguments are incomplete")
endif()

set(test_root "${MORPHEUS_BUILD_DIR}/installed-pkgconfig-test")
if(DEFINED MORPHEUS_INSTALL_PREFIX)
  set(prefix "${MORPHEUS_INSTALL_PREFIX}")
else()
  set(prefix "${test_root}/prefix")
endif()
set(pkgconfig_dir "${prefix}/${MORPHEUS_INSTALL_LIBDIR}/pkgconfig")
set(consumer_source "${MORPHEUS_SOURCE_DIR}/test/install-consumer/main.c")
set(consumer_binary
    "${test_root}/morpheus-pkgconfig-consumer${MORPHEUS_EXECUTABLE_SUFFIX}")

file(REMOVE_RECURSE "${test_root}")
file(MAKE_DIRECTORY "${test_root}")

if(NOT DEFINED MORPHEUS_INSTALL_PREFIX)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" --install "${MORPHEUS_BUILD_DIR}"
            --prefix "${prefix}"
    RESULT_VARIABLE install_result
    OUTPUT_VARIABLE install_output
    ERROR_VARIABLE install_error
  )
  if(NOT install_result EQUAL 0)
    message(FATAL_ERROR "install failed:\n${install_output}\n${install_error}")
  endif()
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env
          "PKG_CONFIG_PATH=${pkgconfig_dir}"
          "${MORPHEUS_PKG_CONFIG}" --modversion libmorpheus
  RESULT_VARIABLE pkg_config_result
  OUTPUT_VARIABLE pkg_config_version
  ERROR_VARIABLE pkg_config_error
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT pkg_config_result EQUAL 0)
  message(FATAL_ERROR
          "pkg-config lookup failed:\n${pkg_config_error}")
endif()
if(NOT pkg_config_version STREQUAL MORPHEUS_VERSION)
  message(FATAL_ERROR
          "pkg-config reported ${pkg_config_version}, expected ${MORPHEUS_VERSION}")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env
          "PKG_CONFIG_PATH=${pkgconfig_dir}"
          "${MORPHEUS_PKG_CONFIG}" --variable=prefix libmorpheus
  RESULT_VARIABLE prefix_result
  OUTPUT_VARIABLE reported_prefix
  ERROR_VARIABLE prefix_error
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT prefix_result EQUAL 0)
  message(FATAL_ERROR "pkg-config prefix lookup failed:\n${prefix_error}")
endif()
file(REAL_PATH "${reported_prefix}" reported_prefix_real)
file(REAL_PATH "${prefix}" expected_prefix_real)
if(NOT reported_prefix_real STREQUAL expected_prefix_real)
  message(FATAL_ERROR
    "pkg-config prefix ${reported_prefix_real}, expected ${expected_prefix_real}"
  )
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env
          "PKG_CONFIG_PATH=${pkgconfig_dir}"
          "${MORPHEUS_PKG_CONFIG}" --cflags --libs libmorpheus
  RESULT_VARIABLE flags_result
  OUTPUT_VARIABLE consumer_flags
  ERROR_VARIABLE flags_error
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT flags_result EQUAL 0)
  message(FATAL_ERROR "pkg-config flags lookup failed:\n${flags_error}")
endif()
separate_arguments(consumer_flags NATIVE_COMMAND "${consumer_flags}")

execute_process(
  COMMAND "${MORPHEUS_C_COMPILER}" "${consumer_source}"
          -o "${consumer_binary}" ${consumer_flags}
  RESULT_VARIABLE compile_result
  OUTPUT_VARIABLE compile_output
  ERROR_VARIABLE compile_error
)
if(NOT compile_result EQUAL 0)
  message(FATAL_ERROR
    "pkg-config consumer compile failed:\n${compile_output}\n${compile_error}"
  )
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env
          "LD_LIBRARY_PATH=${prefix}/${MORPHEUS_INSTALL_LIBDIR}"
          "DYLD_LIBRARY_PATH=${prefix}/${MORPHEUS_INSTALL_LIBDIR}"
          "${consumer_binary}"
  RESULT_VARIABLE run_result
  OUTPUT_VARIABLE run_output
  ERROR_VARIABLE run_error
)
if(NOT run_result EQUAL 0)
  message(FATAL_ERROR
    "pkg-config consumer run failed:\n${run_output}\n${run_error}"
  )
endif()
