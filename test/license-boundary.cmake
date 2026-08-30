# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

set(agpl_files
    .github/workflows/platform.yml
    CHANGELOG.md
    CMakeLists.txt
    cmake/stamp-gener-preparer.cmake
    bindings/deno/NOTICE
    bindings/deno/README.md
    bindings/deno/data.ts
    bindings/deno/data_internal.ts
    bindings/deno/data_manifest.ts
    bindings/deno/data_test.ts
    bindings/deno/gener_index_internal.ts
    bindings/deno/gener_preparer_test.ts
    bindings/deno/gener_runtime_internal.ts
    bindings/deno/mod.ts
    bindings/deno/mod_test.ts
    bindings/deno/native.ts
    bindings/deno/native_internal.ts
    bindings/deno/native_manifest.ts
    bindings/deno/native_test.ts
    bindings/deno/setup.ts
    bindings/deno/setup_internal.ts
    bindings/deno/setup_test.ts
    include/morpheus/morpheus.h
    src/api/gener_index.c
    src/api/gener_index.h
    src/api/api_internal.h
    src/api/analyze.c
    src/api/context.c
    src/api/generation.c
    src/api/result.c
    tools/prepare-runtime-data.sh
    tools/gener-index-builder.c)
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
        bindings/deno/gener_manifest.ts
        bindings/deno/gener_preparer.mjs
        include/morpheus/compat.h
        src/anal/anal_internal.h
        src/anal/cruncher_internal.h
        src/bridge/legacy_values.c
        src/bridge/legacy_values.h
        src/bridge/generation_normalizer.c
        src/bridge/generation_normalizer.h
        src/compat/compat.c
        src/gener/derivation.c
        src/gener/derivation.h
        src/gener/gener_internal.h
        src/gener/generation_service.c
        src/gener/generation_service.h
        src/gener/genwd.proto.h
        src/gkdict/compnoun.proto.h
        src/gkdict/gkdict_internal.h
        src/gkends/gkends_internal.h
        src/greeklib/greeklib_internal.h
        src/greeklib/standalpha.proto.h
        src/morphlib/morphlib_internal.h
        src/morphlib/runtime_context.h
        src/morphlib/runtime_context_internal.h
        src/morphlib/setlang.proto.h
        test/gener-fixture.tsv
        test/gener-derivation-source.txt
        test/gener-derivation-invalid.txt
        test/gener-index-source.txt
        test/gener-index-unexpanded.txt
        test/generation-service-invalid-source.txt
        test/generation-service-source.txt
        test/gener-source-continuations.txt
        test/gener-source-orphan-continuation.txt
        tools/gener-source-preparer.c
        tools/gener-corpus-manifest.tsv
        tools/gener-corpus-exceptions.tsv
        tools/gener-derivation-manifest.tsv)
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

file(READ "${MORPHEUS_SOURCE_DIR}/bindings/deno/jsr.json.license"
     jsr_license)
string(FIND "${jsr_license}"
       "SPDX-License-Identifier: AGPL-3.0-or-later" jsr_spdx_at)
if(jsr_spdx_at EQUAL -1)
  message(FATAL_ERROR
          "AGPL SPDX sidecar missing for bindings/deno/jsr.json")
endif()

foreach(license_file IN ITEMS
        LICENSE-AGPL-3.0-or-later
        LICENSES/MPL-2.0.txt
        LICENSES/AGPL-3.0-or-later.txt
        docs/licensing.md
        docs/license-inventory.md)
  if(NOT EXISTS "${MORPHEUS_SOURCE_DIR}/${license_file}")
    message(FATAL_ERROR "licensing artifact missing: ${license_file}")
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/LICENSE-AGPL-3.0-or-later"
     root_agpl_license)
file(READ "${MORPHEUS_SOURCE_DIR}/LICENSES/AGPL-3.0-or-later.txt"
     canonical_agpl_license)
if(NOT root_agpl_license STREQUAL canonical_agpl_license)
  message(FATAL_ERROR
          "root AGPL discovery copy differs from canonical license text")
endif()
