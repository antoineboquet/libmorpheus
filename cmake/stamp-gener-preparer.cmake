# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_GENER_PREPARER)
  message(FATAL_ERROR "MORPHEUS_GENER_PREPARER is required")
endif()
file(READ "${MORPHEUS_GENER_PREPARER}" content)
string(REGEX REPLACE "[ \t]+\n" "\n" content "${content}")
string(REGEX REPLACE "\n\n$" "\n" content "${content}")
set(create_require_original
  "var require=createRequire(import.meta.url)")
set(create_require_portable
  "var requireUrl=Module[\"scriptUrl\"]??import.meta.url;var require=createRequire(requireUrl.startsWith(\"file:\")?requireUrl:new URL(\"file:///gener_preparer.mjs\"))")
string(FIND "${content}" "${create_require_original}" create_require_at)
if(create_require_at EQUAL -1)
  message(FATAL_ERROR
    "Emscripten output no longer contains the expected createRequire call")
endif()
string(REPLACE
  "${create_require_original}" "${create_require_portable}"
  content "${content}")
string(CONCAT header
  "// SPDX-License-Identifier: MPL-2.0 AND MIT\n"
  "// Generated with Emscripten 4.0.15 from the MPL-2.0 source preparer.\n"
)
string(FIND "${content}" "${header}" header_at)
if(NOT header_at EQUAL 0)
  set(content "${header}${content}")
endif()
file(WRITE "${MORPHEUS_GENER_PREPARER}" "${content}")
