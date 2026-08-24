if(NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_GENERATOR OR
   NOT DEFINED MORPHEUS_PKG_CONFIG)
  message(FATAL_ERROR "pkg-config layout test arguments are incomplete")
endif()

set(test_root "${MORPHEUS_BUILD_DIR}/pkgconfig-layout-test")
set(nested_build "${test_root}/build")
set(prefix "${test_root}/prefix")
set(nested_libdir "lib/morpheus-multiarch")
set(pkgconfig_dir "${prefix}/${nested_libdir}/pkgconfig")

file(REMOVE_RECURSE "${test_root}")
execute_process(
  COMMAND "${CMAKE_COMMAND}"
          -S "${MORPHEUS_SOURCE_DIR}"
          -B "${nested_build}"
          -G "${MORPHEUS_GENERATOR}"
          -DBUILD_TESTING=OFF
          -DMORPHEUS_BUILD_CRUNCHER=OFF
          "-DCMAKE_INSTALL_LIBDIR=${nested_libdir}"
  RESULT_VARIABLE configure_result
  OUTPUT_VARIABLE configure_output
  ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR
    "nested-libdir configure failed:\n${configure_output}\n${configure_error}"
  )
endif()

file(MAKE_DIRECTORY "${pkgconfig_dir}")
file(COPY "${nested_build}/libmorpheus.pc" DESTINATION "${pkgconfig_dir}")
execute_process(
  COMMAND "${CMAKE_COMMAND}" -E env
          "PKG_CONFIG_PATH=${pkgconfig_dir}"
          "${MORPHEUS_PKG_CONFIG}" --variable=prefix libmorpheus
  RESULT_VARIABLE lookup_result
  OUTPUT_VARIABLE reported_prefix
  ERROR_VARIABLE lookup_error
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
if(NOT lookup_result EQUAL 0)
  message(FATAL_ERROR "nested-libdir prefix lookup failed:\n${lookup_error}")
endif()
file(REAL_PATH "${reported_prefix}" reported_prefix_real)
file(REAL_PATH "${prefix}" expected_prefix_real)
if(NOT reported_prefix_real STREQUAL expected_prefix_real)
  message(FATAL_ERROR
    "nested-libdir prefix ${reported_prefix_real}, expected ${expected_prefix_real}"
  )
endif()
