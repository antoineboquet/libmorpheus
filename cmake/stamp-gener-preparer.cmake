# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_GENER_PREPARER)
  message(FATAL_ERROR "MORPHEUS_GENER_PREPARER is required")
endif()
file(READ "${MORPHEUS_GENER_PREPARER}" content)
string(REGEX REPLACE "[ \t]+\n" "\n" content "${content}")
string(REGEX REPLACE "\n\n$" "\n" content "${content}")
string(CONCAT header
  "// SPDX-License-Identifier: MPL-2.0 AND MIT\n"
  "// Generated with Emscripten 4.0.15 from the MPL-2.0 source preparer.\n"
)
string(FIND "${content}" "${header}" header_at)
if(NOT header_at EQUAL 0)
  file(WRITE "${MORPHEUS_GENER_PREPARER}" "${header}${content}")
endif()
