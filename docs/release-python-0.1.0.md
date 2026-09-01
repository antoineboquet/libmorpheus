<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Python binding 0.1.0 release decision

Version 0.1.0 is the first candidate release of the independently versioned
Python binding. It targets native libmorpheus 0.3.2 and C ABI 2. This document
records the intended contract; it does not authorize or record creation of the
`python-v0.1.0` tag.

## Distribution contract

The PyPI project and import package are both named `libmorpheus`. The release
contains one `py3-none-any` wheel and one source distribution. The implementation
uses only Python's standard-library `ctypes` FFI and carries a `py.typed` marker;
it has no runtime dependencies and no interpreter- or platform-specific native
extension.

Neither distribution embeds the native libmorpheus runtime, a stemlib, nor a
generation index. Applications pass the separately installed shared-library and
stemlib paths to `Library` and `Library.context`. Version 0.1.0 does not promise
a Python-native acquisition command.

## Behavioral contract

The binding exposes synchronous Greek and Latin analysis and experimental Greek
generation. Its normalized immutable records preserve:

- every analysis and generated morphological interpretation;
- nullable scalar grammatical traits;
- combined dialect, geographic, gender, case, and voice masks;
- dual number and duplicate surfaces with distinct interpretations;
- raw numeric records, truncation markers, and morphological flags.

Calls through one context are serialized with a reentrant lock. Distinct
contexts can be submitted to separate Python threads, and `ctypes.CDLL` releases
the GIL while calling the native runtime. Context and library closure is
explicit, idempotent, and available through context managers; a library refuses
to close while contexts remain open.

`GenerationOptions.result_limit` is a hard safety ceiling. Exceeding it raises a
typed `MorpheusError` with `Status.RESULT_LIMIT_EXCEEDED` rather than returning a
silently truncated list. Dual forms remain enabled unless callers explicitly set
`exclude_duals=True`.

## Qualification and publication

Linux CI tests Python 3.11 and 3.14. Each interpreter compiles an independent
fake ABI library for boundary and error-path tests, then exercises the actual
compiled libmorpheus runtime with Greek and Latin stem data and a deterministic
generation fixture. The package build must produce exactly one universal wheel
and one source distribution containing the typed facade, AGPL license, and
notice but no runtime or linguistic data.

An authorized `python-v0.1.0` tag makes `python-release.yml` wait for tagged
Linux CI on the exact commit. It verifies the independently declared native
0.3.2 release, checks both distributions, installs the wheel in an isolated
environment, and publishes through PyPI trusted publishing in the `pypi` GitHub
environment. The public-package smoke test then installs exactly version 0.1.0
from PyPI and repeats Greek analysis, Latin multiple-analysis, and Greek
dual-generation checks.

The PyPI pending publisher must name project `libmorpheus`, owner
`defense-humanites`, repository `libmorpheus`, workflow `python-release.yml`, and
environment `pypi`. This creates the project on first OIDC publication without a
placeholder release or long-lived token.

## Licensing

The Python facade, tests, workflows, and this release decision are
AGPL-3.0-or-later. The separately installed native runtime remains a
mixed-license work, and stem data retains its recorded upstream licensing and
provenance. The Python distributions contain neither component.

## Remaining authorization

Before publication, a maintainer must:

1. configure the `pypi` GitHub environment and the PyPI pending publisher;
2. confirm the final tagged CI candidate and package file lists;
3. explicitly authorize creation of `python-v0.1.0`.
