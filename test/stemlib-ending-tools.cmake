# SPDX-License-Identifier: AGPL-3.0-or-later

foreach(required IN ITEMS MORPHEUS_BUILDEND MORPHEUS_BUILDDERIV
                          MORPHEUS_INDENDTABLES MORPHEUS_INDDERIVTABLES
                          MORPHEUS_STEMLIB_ROOT MORPHEUS_WORK_DIR)
  if(NOT DEFINED ${required})
    message(FATAL_ERROR "${required} is required")
  endif()
endforeach()

file(REMOVE_RECURSE "${MORPHEUS_WORK_DIR}")
set(empty_root "${MORPHEUS_WORK_DIR}/empty")
set(partial_root "${MORPHEUS_WORK_DIR}/partial")
file(MAKE_DIRECTORY "${empty_root}/Greek")
file(MAKE_DIRECTORY
     "${partial_root}/Greek/endtables/out"
     "${partial_root}/Greek/endtables/indices"
     "${partial_root}/Greek/derivs/out"
     "${partial_root}/Greek/derivs/indices")
file(COPY "${MORPHEUS_STEMLIB_ROOT}/Greek/rule_files"
     DESTINATION "${partial_root}/Greek")

function(expect_failure label root program)
  execute_process(
    COMMAND "${CMAKE_COMMAND}" -E env "MORPHLIB=${root}"
            "${program}" ${ARGN}
    RESULT_VARIABLE result
    OUTPUT_QUIET
    ERROR_QUIET
  )
  if(result EQUAL 0)
    message(FATAL_ERROR "${label} unexpectedly succeeded")
  endif()
endfunction()

expect_failure("buildend without stem-type registry"
               "${empty_root}" "${MORPHEUS_BUILDEND}" nom)
expect_failure("buildderiv without derivation registry"
               "${empty_root}" "${MORPHEUS_BUILDDERIV}" all)
expect_failure("ending index with missing compiled tables"
               "${partial_root}" "${MORPHEUS_INDENDTABLES}" nom)
expect_failure("derivation index with missing compiled tables"
               "${partial_root}" "${MORPHEUS_INDDERIVTABLES}")

if(EXISTS "${partial_root}/Greek/endtables/indices/nendind" OR
   EXISTS "${partial_root}/Greek/endtables/indices/nendind.tmp" OR
   EXISTS "${partial_root}/Greek/derivs/indices/derivind" OR
   EXISTS "${partial_root}/Greek/derivs/indices/derivind.tmp")
  message(FATAL_ERROR "a failed index operation left a partial index")
endif()
