<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# libmorpheus for Python

The `libmorpheus` Python distribution provides a typed, pure-Python facade over
the stable libmorpheus C ABI. It uses the standard-library `ctypes` module and
therefore needs no CPython extension or interpreter-specific native wheel.

The package contains neither the native runtime nor linguistic stem data. As
with the JavaScript bindings, applications provide an independently installed
shared library and stemlib. Version 0.1.0 keeps acquisition outside the package
and accepts explicit filesystem paths.

## API

```python
from libmorpheus import GenerationOptions, Language, Library, Number, Option

with Library("/path/to/libmorpheus.so") as library:
    with library.context("/path/to/stemlib", Language.GREEK) as context:
        analyses = context.analyze("a)/nqrwpos", Option.STRICT_CASE)
        dual_forms = context.generate(
            "lo/gos", GenerationOptions(number=Number.DUAL)
        )
```

The normalized API preserves multiple analyses and generation interpretations,
nullable scalar traits, combined masks, dialects, and dual forms. Raw numeric
ABI records remain available through `analyze_raw()` and `generate_raw()` for
callers that need lossless low-level access. Generation is experimental, as it
is in the native ABI, and requires a stemlib containing `gener.index`.

`GenerationOptions.result_limit` is a safety ceiling: exceeding a nonzero limit
raises `MorpheusError` with `Status.RESULT_LIMIT_EXCEEDED`; it does not silently
truncate the result. `exclude_duals=True` is available for applications that
choose not to expose dual forms.

Calls are synchronous at the Python surface. `ctypes.CDLL` releases the GIL
during native calls, and the binding serializes calls made through one context;
distinct contexts can therefore be submitted to separate Python threads.

## Distribution model

The Python binding has its own version and will use `python-v<version>` tags.
Version 0.1.0 targets native libmorpheus 0.3.2 and C ABI 2. The intended PyPI
project and import name are both `libmorpheus`.

The package exposes this relationship as `MORPHEUS_PYTHON_VERSION`,
`MORPHEUS_NATIVE_VERSION`, and `MORPHEUS_NATIVE_ABI_VERSION`; `__version__`
remains the conventional alias for the Python package version.

The package will be published through a PyPI pending trusted publisher tied to
the repository's eventual Python release workflow. A pending publisher creates
the project on first use, so Python does not require npm's placeholder-package
bootstrap.

Release tags run the Python 3.11/3.14 matrix against both an ABI fixture and a
compiled native runtime, build the universal wheel and source distribution,
then publish through the `pypi` GitHub environment. A separate workflow installs
the public wheel into an empty virtual environment and repeats Greek analysis,
Latin multiple-analysis, and Greek dual-generation checks.

This binding is under active development and is not yet published.
