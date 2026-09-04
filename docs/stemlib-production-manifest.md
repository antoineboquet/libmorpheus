<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Stemlib production source manifest

`tools/stemlib-source-manifest.tsv` freezes the first reproducible-production
input boundary for Greek and Latin ending, ending-macro, and derivation tables.
Each row has five tab-separated fields:

| Field | Meaning |
| --- | --- |
| `language` | `Greek` or `Latin` |
| `status` | `active` or `excluded` |
| `kind` | `rule`, `ending-basic`, `ending` or `derivation` |
| `path` | Language-relative source path |
| `sha256` | Digest of the exact committed input bytes |

All `rule_files/*.table`, `endtables/basics/*.end`,
`endtables/source/*.end` and `derivs/source/*.deriv` files must occur exactly
once. Rules and ending macros are always active.
An ending or derivation is active when the curated tree contains its compiled
`.out` baseline; this preserves the effective historical selection without
pretending that every registry entry is buildable. A new or removed file, a
digest change, a missing active baseline, or a compiled baseline for an
excluded source fails CI until the manifest and its review evidence change
together.

The twelve explicit Greek exclusions are:

- endings: `conj3`, `conj3io`, `conj4`, `is_ios`, `us_uos2`, `verb_adj`, and
  `vh_vhs`;
- derivations: `cw`, `es_denom`, `ow_fact`, `ow_instr`, and `ww`.

Latin currently has no source-file exclusions in these two groups. This does
not resolve the separate Latin registry entries whose source tables are absent;
they remain an audited corpus gap and must not be silently synthesized.

The manifest is intentionally narrower than a complete stemlib build receipt.
It does not yet select lexical `stemsrc/` inputs, record producer or toolchain
versions, or prove that regenerated outputs match the baselines. The next
stage consumes only active rows into an empty language-scoped staging tree;
later manifests will add nominal and verb sources as their producers are
restored.

## Isolated source staging

`tools/stage-stemlib-sources.cmake` validates the complete manifest before it
writes anything. It rejects a destination inside the source stemlib and rejects
any destination that already exists. For one requested language it then:

1. creates fresh rule, ending, and derivation directories;
2. copies every active input and verifies its staged SHA-256;
3. emits the active subset as `MORPHEUS-STEMLIB-INPUTS.tsv`;
4. emits ordered `ending-tables.list` and `derivation-tables.list` producer
   inputs, plus `derivation-index-tables.list` for the active tables classified
   as `reg_deriv` by the pinned registry.

CI stages Greek and Latin twice into independent directories, revalidates every
copied digest, compares the four metadata files, checks representative
exclusions, and verifies that an existing destination is refused. The stager
does not use global temporary paths and never overlays a prior distribution.

## Table production

`tools/build-stemlib-tables.cmake` consumes a fresh staging destination and the
two ordered lists. It runs `buildend` and `buildderiv` once for every active
table, then builds the nominal, verb, and derivation indexes from those same
lists. Derivation expansion includes all active derivation tables, while its
index is limited to the 18 Greek and two Latin `reg_deriv` tables selected from
the pinned registry. The list-driven index mode filters nominal and verb tables by their
registered stem class, rejects malformed or unknown names, and never treats an
unlisted registry entry as an implicit input.

Every producer runs with `LC_ALL=C`, `LANG=C`, `TZ=UTC`, and `MORPHLIB` fixed to
the staging root. Successful completion emits
`MORPHEUS-STEMLIB-TABLE-OUTPUTS.tsv`, containing the sorted output paths and
their SHA-256 digests. The receipt currently covers expanded ASCII and binary
tables plus their three text indexes; it does not yet contain a compiler
identity or source revision.

CI performs two complete clean table builds for each language. It requires the
two receipts and every received output to be byte-identical, then compares all
568 regenerated files with the checked-in Greek and Latin table baselines.
Textual expansions and indexes must match byte for byte. Binary `.out`
differences are counted separately because the restored writer uses the
qualified explicit serialization rather than historical in-memory structure
bytes. This comparison covers table production only; stem indexes remain
outside this phase.
