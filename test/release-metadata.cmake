foreach(required_variable IN ITEMS
        MORPHEUS_SOURCE_DIR
        MORPHEUS_PROJECT_VERSION
        MORPHEUS_ABI_VERSION
        MORPHEUS_SOVERSION)
  if(NOT DEFINED ${required_variable})
    message(FATAL_ERROR "${required_variable} is required")
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/include/morpheus/morpheus.h" c_header)
string(REGEX MATCH
  "#define[ \t]+MORPHEUS_ABI_VERSION[ \t]+([0-9]+)u"
  c_abi_definition "${c_header}"
)
if(NOT "${CMAKE_MATCH_1}" STREQUAL "${MORPHEUS_ABI_VERSION}")
  message(FATAL_ERROR
    "C header ABI ${CMAKE_MATCH_1} differs from CMake ABI ${MORPHEUS_ABI_VERSION}"
  )
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/bindings/deno/mod.ts" deno_binding)
string(REGEX MATCH "const ABI_VERSION = ([0-9]+);" deno_abi_definition
             "${deno_binding}")
if(NOT "${CMAKE_MATCH_1}" STREQUAL "${MORPHEUS_ABI_VERSION}")
  message(FATAL_ERROR
    "Deno ABI ${CMAKE_MATCH_1} differs from CMake ABI ${MORPHEUS_ABI_VERSION}"
  )
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/docs/public-api.md" api_documentation)
set(expected_api_versions
    "The current project version is ${MORPHEUS_PROJECT_VERSION}, the SONAME major is ${MORPHEUS_SOVERSION}, and")
string(FIND "${api_documentation}" "${expected_api_versions}"
            api_versions_at)
if(api_versions_at EQUAL -1)
  message(FATAL_ERROR
    "public-api.md does not document project ${MORPHEUS_PROJECT_VERSION} and SONAME ${MORPHEUS_SOVERSION}"
  )
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/CHANGELOG.md" changelog)
foreach(expected_changelog_value IN ITEMS
        "Target project version: **${MORPHEUS_PROJECT_VERSION}**"
        "C ABI: **${MORPHEUS_ABI_VERSION}**"
        "Shared-library SONAME: **${MORPHEUS_SOVERSION}**")
  string(FIND "${changelog}" "${expected_changelog_value}"
              changelog_value_at)
  if(changelog_value_at EQUAL -1)
    message(FATAL_ERROR
      "CHANGELOG.md is missing: ${expected_changelog_value}"
    )
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/docs/release-0.1.0.md" release_decision)
foreach(expected_release_value IN ITEMS
        "Project version: **${MORPHEUS_PROJECT_VERSION}**"
        "C ABI: **${MORPHEUS_ABI_VERSION}**"
        "SONAME major: **${MORPHEUS_SOVERSION}**")
  string(FIND "${release_decision}" "${expected_release_value}"
              release_value_at)
  if(release_value_at EQUAL -1)
    message(FATAL_ERROR
      "release-0.1.0.md is missing: ${expected_release_value}"
    )
  endif()
endforeach()
