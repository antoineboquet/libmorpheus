# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

set(agpl_files
    .github/workflows/platform.yml
    CHANGELOG.md
    CMakeLists.txt
    bindings/deno/README.md
    bindings/deno/mod.ts
    bindings/deno/mod_test.ts
    include/morpheus/morpheus.h
    src/api/api_internal.h
    src/api/analyze.c
    src/api/context.c
    src/api/result.c)
file(GLOB agpl_support_files
     RELATIVE "${MORPHEUS_SOURCE_DIR}"
     "${MORPHEUS_SOURCE_DIR}/bench/*.sh"
     "${MORPHEUS_SOURCE_DIR}/bench/*.ts"
     "${MORPHEUS_SOURCE_DIR}/cmake/*.cmake"
     "${MORPHEUS_SOURCE_DIR}/cmake/*.in"
     "${MORPHEUS_SOURCE_DIR}/docs/*.md"
     "${MORPHEUS_SOURCE_DIR}/test/*.c"
     "${MORPHEUS_SOURCE_DIR}/test/*.cmake"
     "${MORPHEUS_SOURCE_DIR}/test/install-consumer/CMakeLists.txt"
     "${MORPHEUS_SOURCE_DIR}/test/install-consumer/*.c")
list(APPEND agpl_files ${agpl_support_files})
list(REMOVE_DUPLICATES agpl_files)

foreach(agpl_file IN LISTS agpl_files)
  file(READ "${MORPHEUS_SOURCE_DIR}/${agpl_file}" contents)
  string(FIND "${contents}"
         "SPDX-License-Identifier: AGPL-3.0-or-later" spdx_at)
  if(spdx_at EQUAL -1)
    message(FATAL_ERROR "AGPL SPDX identifier missing from ${agpl_file}")
  endif()
endforeach()

foreach(mpl_file IN ITEMS
        include/morpheus/compat.h
        src/anal/anal_internal.h
        src/anal/cruncher_internal.h
        src/bridge/legacy_values.c
        src/bridge/legacy_values.h
        src/compat/compat.c
        src/gener/gener_internal.h
        src/gener/genwd.proto.h
        src/gkdict/compnoun.proto.h
        src/gkdict/gkdict_internal.h
        src/gkends/gkends_internal.h
        src/greeklib/greeklib_internal.h
        src/greeklib/standalpha.proto.h
        src/morphlib/morphlib_internal.h
        src/morphlib/runtime_context.h
        src/morphlib/runtime_context_internal.h
        src/morphlib/setlang.proto.h)
  file(READ "${MORPHEUS_SOURCE_DIR}/${mpl_file}" contents)
  string(FIND "${contents}" "SPDX-License-Identifier: MPL-2.0" spdx_at)
  if(spdx_at EQUAL -1)
    message(FATAL_ERROR "MPL SPDX identifier missing from ${mpl_file}")
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/CMakePresets.json.license"
     presets_license)
string(FIND "${presets_license}"
       "SPDX-License-Identifier: AGPL-3.0-or-later" presets_spdx_at)
if(presets_spdx_at EQUAL -1)
  message(FATAL_ERROR
          "AGPL SPDX sidecar missing for CMakePresets.json")
endif()

foreach(license_file IN ITEMS
        LICENSES/MPL-2.0.txt
        LICENSES/AGPL-3.0-or-later.txt
        docs/licensing.md
        docs/license-inventory.md)
  if(NOT EXISTS "${MORPHEUS_SOURCE_DIR}/${license_file}")
    message(FATAL_ERROR "licensing artifact missing: ${license_file}")
  endif()
endforeach()
