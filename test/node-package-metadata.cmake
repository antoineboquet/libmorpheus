# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

set(node_dir "${MORPHEUS_SOURCE_DIR}/bindings/js/node")
file(READ "${node_dir}/package.json" package)
foreach(field IN ITEMS name version license type)
  string(JSON "node_${field}" ERROR_VARIABLE json_error GET "${package}" "${field}")
  if(json_error)
    message(FATAL_ERROR "Invalid Node package ${field}: ${json_error}")
  endif()
endforeach()
if(NOT node_name STREQUAL "@libmorpheus/node" OR
   NOT node_version STREQUAL "0.1.0" OR
   NOT node_license STREQUAL "AGPL-3.0-or-later" OR
   NOT node_type STREQUAL "module")
  message(FATAL_ERROR
    "Unexpected Node package identity: ${node_name}@${node_version}, ${node_license}, ${node_type}")
endif()

file(READ "${node_dir}/internal/version.js" version_module)
string(FIND "${version_module}"
  "MORPHEUS_NODE_VERSION = \"${node_version}\"" version_at)
if(version_at EQUAL -1)
  message(FATAL_ERROR "Node version module differs from package.json")
endif()

foreach(forbidden_script IN ITEMS preinstall install postinstall)
  string(JSON ignored ERROR_VARIABLE script_error
    GET "${package}" scripts "${forbidden_script}")
  if(NOT script_error)
    message(FATAL_ERROR
      "Node package must not run ${forbidden_script} during npm installation")
  endif()
endforeach()
foreach(forbidden_field IN ITEMS dependencies)
  string(JSON ignored ERROR_VARIABLE field_error GET "${package}" "${forbidden_field}")
  if(NOT field_error)
    message(FATAL_ERROR
      "Initial Node package unexpectedly declares ${forbidden_field}")
  endif()
endforeach()

foreach(platform IN ITEMS
    darwin-arm64 linux-arm64-gnu linux-x64-gnu)
  set(platform_path "${node_dir}/npm/${platform}/package.json")
  file(READ "${platform_path}" platform_package)
  string(JSON platform_name GET "${platform_package}" name)
  string(JSON platform_version GET "${platform_package}" version)
  if(NOT platform_name STREQUAL "@libmorpheus/node-${platform}" OR
     NOT platform_version STREQUAL node_version)
    message(FATAL_ERROR
      "Unexpected Node platform package: ${platform_name}@${platform_version}")
  endif()
  string(JSON optional_version GET
    "${package}" optionalDependencies "${platform_name}")
  if(NOT optional_version STREQUAL node_version)
    message(FATAL_ERROR
      "Main Node package does not pin ${platform_name}@${node_version}")
  endif()
endforeach()

string(JSON default_import GET "${package}" exports . import)
string(JSON default_types GET "${package}" exports . types)
if(NOT default_import STREQUAL "./index.js" OR
   NOT default_types STREQUAL "./index.d.ts")
  message(FATAL_ERROR "Unexpected Node package exports")
endif()
string(JSON native_import GET "${package}" exports ./native import)
string(JSON native_types GET "${package}" exports ./native types)
string(JSON native_bin GET "${package}" bin libmorpheus-native)
if(NOT native_import STREQUAL "./native.js" OR
   NOT native_types STREQUAL "./native.d.ts" OR
   NOT native_bin STREQUAL "./native.js")
  message(FATAL_ERROR "Unexpected Node native acquisition entrypoint")
endif()

foreach(required IN ITEMS
    CMakeLists.txt LICENSE NOTICE README.md addon.c index.d.ts index.js
    internal/archive.js internal/native-internal.js internal/native-manifest.js
    internal/version.js native.d.ts native.js
    package-platform.mjs package.json package.json.license
    test/binding.test.js test/native.test.js)
  if(NOT EXISTS "${node_dir}/${required}")
    message(FATAL_ERROR "Missing Node binding source: ${required}")
  endif()
endforeach()
