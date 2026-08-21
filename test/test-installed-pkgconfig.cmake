if(NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_INSTALL_LIBDIR OR
   NOT DEFINED MORPHEUS_PKG_CONFIG OR
   NOT DEFINED MORPHEUS_VERSION)
  message(FATAL_ERROR "installed pkg-config test arguments are incomplete")
endif()

set(test_root "${MORPHEUS_BUILD_DIR}/installed-pkgconfig-test")
set(prefix "${test_root}/prefix")
set(pkgconfig_dir "${prefix}/${MORPHEUS_INSTALL_LIBDIR}/pkgconfig")

file(REMOVE_RECURSE "${test_root}")

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
