if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

foreach(agpl_file IN ITEMS
        include/morpheus/morpheus.h
        src/api/api_internal.h
        src/api/analyze.c
        src/api/context.c
        src/api/result.c
        bindings/deno/mod.ts)
  file(READ "${MORPHEUS_SOURCE_DIR}/${agpl_file}" contents)
  string(FIND "${contents}"
         "SPDX-License-Identifier: AGPL-3.0-or-later" spdx_at)
  if(spdx_at EQUAL -1)
    message(FATAL_ERROR "AGPL SPDX identifier missing from ${agpl_file}")
  endif()
endforeach()

foreach(mpl_file IN ITEMS
        include/morpheus/compat.h
        src/bridge/legacy_values.c
        src/bridge/legacy_values.h
        src/compat/compat.c)
  file(READ "${MORPHEUS_SOURCE_DIR}/${mpl_file}" contents)
  string(FIND "${contents}" "SPDX-License-Identifier: MPL-2.0" spdx_at)
  if(spdx_at EQUAL -1)
    message(FATAL_ERROR "MPL SPDX identifier missing from ${mpl_file}")
  endif()
endforeach()

foreach(license_file IN ITEMS
        LICENSES/MPL-2.0.txt
        LICENSES/AGPL-3.0-or-later.txt
        docs/licensing.md)
  if(NOT EXISTS "${MORPHEUS_SOURCE_DIR}/${license_file}")
    message(FATAL_ERROR "licensing artifact missing: ${license_file}")
  endif()
endforeach()
