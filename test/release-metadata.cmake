# SPDX-License-Identifier: AGPL-3.0-or-later

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

set(release_decision_path
    "${MORPHEUS_SOURCE_DIR}/docs/release-${MORPHEUS_PROJECT_VERSION}.md")
if(NOT EXISTS "${release_decision_path}")
  message(FATAL_ERROR
    "release decision is missing: docs/release-${MORPHEUS_PROJECT_VERSION}.md"
  )
endif()
file(READ "${release_decision_path}" release_decision)
foreach(expected_release_value IN ITEMS
        "Project version: **${MORPHEUS_PROJECT_VERSION}**"
        "C ABI: **${MORPHEUS_ABI_VERSION}**"
        "SONAME major: **${MORPHEUS_SOVERSION}**")
  string(FIND "${release_decision}" "${expected_release_value}"
              release_value_at)
  if(release_value_at EQUAL -1)
    message(FATAL_ERROR
      "release-${MORPHEUS_PROJECT_VERSION}.md is missing: "
      "${expected_release_value}"
    )
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/docs/releasing.md" releasing_guide)
string(FIND "${releasing_guide}"
  "release-${MORPHEUS_PROJECT_VERSION}.md" current_release_at)
if(current_release_at EQUAL -1)
  message(FATAL_ERROR
    "releasing.md does not identify release-${MORPHEUS_PROJECT_VERSION}.md")
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/.github/workflows/platform.yml"
     platform_workflow)
foreach(expected_workflow_value IN ITEMS
        "publish-jsr:"
        "id-token: write"
        "workflow_id: 'test.yml'"
        "deno publish --dry-run"
        "run: deno publish")
  string(FIND "${platform_workflow}" "${expected_workflow_value}"
              workflow_value_at)
  if(workflow_value_at EQUAL -1)
    message(FATAL_ERROR
      "platform workflow is missing: ${expected_workflow_value}")
  endif()
endforeach()

if(MORPHEUS_PROJECT_VERSION STREQUAL "0.3.0")
  string(FIND "${release_decision}"
    "@humanities/libmorpheus" jsr_package_at)
  if(jsr_package_at EQUAL -1)
    message(FATAL_ERROR
      "release-0.3.0.md does not identify the reserved JSR package")
  endif()
  string(FIND "${release_decision}"
    "generation surface and Deno `generate()`/`generateRaw()` methods are\nexperimental"
    experimental_at)
  if(experimental_at EQUAL -1)
    message(FATAL_ERROR
      "release-0.3.0.md does not retain experimental generation status")
  endif()
endif()
