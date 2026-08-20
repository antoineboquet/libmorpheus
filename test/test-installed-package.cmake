if(NOT DEFINED MORPHEUS_BUILD_DIR OR
   NOT DEFINED MORPHEUS_SOURCE_DIR OR
   NOT DEFINED MORPHEUS_GENERATOR)
  message(FATAL_ERROR "installed-package test arguments are incomplete")
endif()

set(test_root "${MORPHEUS_BUILD_DIR}/installed-package-test")
set(prefix "${test_root}/prefix")
set(consumer_build "${test_root}/build")
set(consumer_source "${MORPHEUS_SOURCE_DIR}/test/install-consumer")

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
  COMMAND "${CMAKE_COMMAND}" -S "${consumer_source}" -B "${consumer_build}"
          -G "${MORPHEUS_GENERATOR}" "-DCMAKE_PREFIX_PATH=${prefix}"
  RESULT_VARIABLE configure_result
  OUTPUT_VARIABLE configure_output
  ERROR_VARIABLE configure_error
)
if(NOT configure_result EQUAL 0)
  message(FATAL_ERROR
          "consumer configure failed:\n${configure_output}\n${configure_error}")
endif()

execute_process(
  COMMAND "${CMAKE_COMMAND}" --build "${consumer_build}"
  RESULT_VARIABLE build_result
  OUTPUT_VARIABLE build_output
  ERROR_VARIABLE build_error
)
if(NOT build_result EQUAL 0)
  message(FATAL_ERROR
          "consumer build failed:\n${build_output}\n${build_error}")
endif()

execute_process(
  COMMAND "${consumer_build}/morpheus_consumer"
  RESULT_VARIABLE run_result
  OUTPUT_VARIABLE run_output
  ERROR_VARIABLE run_error
)
if(NOT run_result EQUAL 0)
  message(FATAL_ERROR
          "consumer run failed:\n${run_output}\n${run_error}")
endif()
