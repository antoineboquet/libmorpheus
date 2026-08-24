# Release decision: 0.1.0

Status: candidate accepted for final benchmark and history-preserving merge.

- Project version: **0.1.0**
- C ABI: **1**
- SONAME major: **0**
- Modernization tip reviewed for this decision:
  `29764ada055dea95faa6c76f3fa975e3ffd5fc2f`

## Version and ABI rationale

Version 0.1.0 is the first publication of the opaque native API. ABI version 1
therefore has no previously published `libmorpheus` ABI to preserve. SONAME 0
correctly communicates a pre-1.0 binary-compatibility surface while allowing
compatible fixes and additions to retain the same loader identity. Any change
to exported signatures, numeric constants, or the `morpheus_analysis` layout
after this decision requires a fresh ABI review before tagging.

The C header, Deno binding, public documentation, CMake package, pkg-config
metadata, changelog, project version, and SONAME are checked together by the
`release_metadata` test.

## Published scope

The 0.1.0 native package may contain the shared library, public C header, CMake
and pkg-config discovery metadata, and `cruncher`. It does not contain a
stemlib. The Deno binding remains available from the tagged source tree and
loads the separately installed native library and caller-selected stemlib.

The Alpheios submodule is pinned for tests and compatibility validation. Its data
is not part of the native package. The current container targets copy that
stemlib into their final images, so they may be built and smoke-tested for
qualification but must not be published until the right to redistribute the
embedded data has been confirmed. This is a distribution restriction, not a
technical qualification failure.

Historical standalone utilities remain reference source and are not part of
the supported or installed 0.1.0 surface.

## Remaining gates

1. Run `bench/release.sh` on an otherwise idle controlled Linux or macOS host.
2. Review and accept the validated benchmark report and any baseline changes.
3. Merge the draft modernization PR with a normal merge commit, without squash
   or rebase, and verify that the modernization tip remains an ancestor.
4. Date the changelog, tag the resulting merge commit as `v0.1.0`, and rebuild
   the native artifact from that tag.
