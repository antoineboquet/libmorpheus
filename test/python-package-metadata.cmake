# SPDX-License-Identifier: AGPL-3.0-or-later

if(NOT DEFINED MORPHEUS_SOURCE_DIR)
  message(FATAL_ERROR "MORPHEUS_SOURCE_DIR is required")
endif()

set(python_dir "${MORPHEUS_SOURCE_DIR}/bindings/python")
file(READ "${python_dir}/pyproject.toml" pyproject)
foreach(required IN ITEMS
    "name = \"libmorpheus\""
    "version = \"0.1.0\""
    "requires-python = \">=3.11\""
    "license = \"AGPL-3.0-or-later\""
    "dependencies = []"
    "package-dir = { \"\" = \"src\" }")
  string(FIND "${pyproject}" "${required}" required_at)
  if(required_at EQUAL -1)
    message(FATAL_ERROR "Python package metadata is missing: ${required}")
  endif()
endforeach()

file(READ "${python_dir}/src/libmorpheus/__init__.py" version_module)
string(FIND "${version_module}" "__version__ = \"0.1.0\"" version_at)
if(version_at EQUAL -1)
  message(FATAL_ERROR "Python package version differs from pyproject.toml")
endif()

foreach(required IN ITEMS
    LICENSE NOTICE README.md pyproject.toml
    src/libmorpheus/__init__.py src/libmorpheus/py.typed)
  if(NOT EXISTS "${python_dir}/${required}")
    message(FATAL_ERROR "Missing Python binding source: ${required}")
  endif()
endforeach()

foreach(forbidden IN ITEMS
    libmorpheus.so libmorpheus.dylib stemlib gener.index)
  file(GLOB_RECURSE embedded "${python_dir}/*${forbidden}*")
  if(embedded)
    message(FATAL_ERROR "Python package embeds forbidden runtime data: ${embedded}")
  endif()
endforeach()
