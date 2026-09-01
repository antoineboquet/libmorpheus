<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# libmorpheus for Python

The `libmorpheus` Python distribution provides a typed, pure-Python facade over
the stable libmorpheus C ABI. It uses the standard-library `ctypes` module and
therefore needs no CPython extension or interpreter-specific native wheel.

The package contains neither the native runtime nor linguistic stem data. As
with the JavaScript bindings, applications provide an independently installed
shared library and stemlib. Acquisition commands will be added before the
first release.

## Intended API

```python
from libmorpheus import Language, Library, Option

with Library("/path/to/libmorpheus.so") as library:
    with library.context("/path/to/stemlib", Language.GREEK) as context:
        analyses = context.analyze("a)/nqrwpos", Option.STRICT_CASE)
```

The normalized API will preserve multiple analyses, nullable scalar traits,
combined masks, dialects, and dual forms. Raw numeric ABI records will remain
available for callers that need lossless low-level access.

Calls are synchronous at the Python surface. `ctypes.CDLL` releases the GIL
during native calls, and the binding serializes calls made through one context;
distinct contexts can therefore be submitted to separate Python threads.

## Distribution model

The Python binding has its own version and will use `python-v<version>` tags.
Version 0.1.0 targets native libmorpheus 0.3.2 and C ABI 2. The intended PyPI
project and import name are both `libmorpheus`.

The package will be published through a PyPI pending trusted publisher tied to
the repository's eventual Python release workflow. A pending publisher creates
the project on first use, so Python does not require npm's placeholder-package
bootstrap.

This binding is under active development and is not yet published.
