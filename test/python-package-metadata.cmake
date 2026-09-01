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
    "package-dir = { \"\" = \"src\" }"
    "license-files = [\"LICENSE\", \"NOTICE\"]")
  string(FIND "${pyproject}" "${required}" required_at)
  if(required_at EQUAL -1)
    message(FATAL_ERROR "Python package metadata is missing: ${required}")
  endif()
endforeach()

file(READ "${python_dir}/src/libmorpheus/_version.py" version_module)
foreach(required IN ITEMS
    "MORPHEUS_PYTHON_VERSION = \"0.1.0\""
    "MORPHEUS_NATIVE_VERSION = \"0.3.2\""
    "MORPHEUS_NATIVE_ABI_VERSION = 2")
  string(FIND "${version_module}" "${required}" required_at)
  if(required_at EQUAL -1)
    message(FATAL_ERROR "Python version metadata is missing: ${required}")
  endif()
endforeach()

foreach(required IN ITEMS
    LICENSE NOTICE README.md pyproject.toml
    src/libmorpheus/__init__.py src/libmorpheus/_abi.py
    src/libmorpheus/_library.py src/libmorpheus/_types.py
    src/libmorpheus/_version.py
    src/libmorpheus/py.typed test/fixture.c test/runtime_smoke.py
    test/test_binding.py)
  if(NOT EXISTS "${python_dir}/${required}")
    message(FATAL_ERROR "Missing Python binding source: ${required}")
  endif()
endforeach()

foreach(required_file IN ITEMS
    "${MORPHEUS_SOURCE_DIR}/.github/workflows/python-release.yml"
    "${MORPHEUS_SOURCE_DIR}/.github/workflows/pypi-smoke.yml"
    "${MORPHEUS_SOURCE_DIR}/docs/release-python-0.1.0.md")
  if(NOT EXISTS "${required_file}")
    message(FATAL_ERROR "Missing Python release contract: ${required_file}")
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/.github/workflows/python-release.yml"
  release_workflow)
foreach(required IN ITEMS
    "python-v*" "pypa/gh-action-pypi-publish@release/v1"
    "environment:" "name: pypi" "id-token: write")
  string(FIND "${release_workflow}" "${required}" required_at)
  if(required_at EQUAL -1)
    message(FATAL_ERROR "Python publication workflow is missing: ${required}")
  endif()
endforeach()

file(READ "${MORPHEUS_SOURCE_DIR}/.github/workflows/pypi-smoke.yml"
  smoke_workflow)
foreach(required IN ITEMS
    "Publish Python binding" "https://pypi.org/simple"
    "runtime_smoke.py")
  string(FIND "${smoke_workflow}" "${required}" required_at)
  if(required_at EQUAL -1)
    message(FATAL_ERROR "PyPI smoke workflow is missing: ${required}")
  endif()
endforeach()

foreach(forbidden IN ITEMS
    libmorpheus.so libmorpheus.dylib stemlib gener.index)
  file(GLOB_RECURSE embedded "${python_dir}/*${forbidden}*")
  if(embedded)
    message(FATAL_ERROR "Python package embeds forbidden runtime data: ${embedded}")
  endif()
endforeach()
