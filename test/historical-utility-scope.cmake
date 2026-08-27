# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

include("${MORPHEUS_SOURCE_DIR}/cmake/HistoricalUtilities.cmake")

file(
  GLOB_RECURSE morpheus_c_sources
  RELATIVE "${MORPHEUS_SOURCE_DIR}"
  "${MORPHEUS_SOURCE_DIR}/src/*.c"
)

set(discovered_entry_points)
foreach(source IN LISTS morpheus_c_sources)
  file(
    STRINGS "${MORPHEUS_SOURCE_DIR}/${source}" entry_lines
    REGEX "^[ \t]*(int|void)?[ \t]*main[ \t]*\\("
  )
  if(entry_lines)
    list(APPEND discovered_entry_points "${source}")
  endif()
endforeach()

set(classified_entry_points
  ${MORPHEUS_SUPPORTED_ENTRY_POINTS}
  ${MORPHEUS_HISTORICAL_ENTRY_POINTS}
)
list(SORT discovered_entry_points)
list(SORT classified_entry_points)

if(NOT discovered_entry_points STREQUAL classified_entry_points)
  message(FATAL_ERROR
    "Standalone-program classification is stale.\n"
    "Discovered: ${discovered_entry_points}\n"
    "Classified: ${classified_entry_points}"
  )
endif()

file(READ "${MORPHEUS_SOURCE_DIR}/CMakeLists.txt" top_level_cmake)
foreach(source IN LISTS MORPHEUS_HISTORICAL_ENTRY_POINTS)
  string(FIND "${top_level_cmake}" "${source}" target_reference)
  if(NOT target_reference EQUAL -1)
    message(FATAL_ERROR
      "Historical utility ${source} must not enter the CMake target graph"
    )
  endif()
endforeach()

file(READ
  "${MORPHEUS_SOURCE_DIR}/docs/historical-utilities.md"
  historical_policy)
foreach(required_text IN ITEMS
    "experimental lemma-generation feature"
    "GenStemForms()"
    "GenIrregForm()"
    "morpheus_gener_source_preparer"
    "morpheus_gener_index_builder"
    "are not installed")
  string(FIND "${historical_policy}" "${required_text}" required_at)
  if(required_at EQUAL -1)
    message(FATAL_ERROR
      "Historical utility policy omits generation boundary: ${required_text}")
  endif()
endforeach()
