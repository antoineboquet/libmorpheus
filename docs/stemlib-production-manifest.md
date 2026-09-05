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
bytes. CI prints a compact TSV summary of the comparison. This comparison
covers table production only; stem indexes remain outside this phase.

The current comparison records 156 Greek and 73 Latin binary differences, with
zero textual or index differences. Their complete sorted path set is pinned in
`test/stemlib-binary-baseline-exceptions.tsv`; CI rejects a missing, unexpected,
duplicated, reordered, malformed, or reclassified exception.

## Lexical production and explicit blockers

`tools/stemlib-lexical-manifest.tsv` adds an ordered lexical boundary without
changing the 379-source table manifest. It inventories every file below both
`stemsrc/` trees, pins selected inputs and exclusions by SHA-256, and declares
Latin `stemsrc/vbs.mpi` as `unavailable`. Prepared `nom.irreg` and `vbs.irreg`
files are inputs here; regenerating those snapshots from their irregular-word
sources remains a later boundary.

The lexical manifest has four tab-separated fields: `language`, `role`, `path`,
and `sha256`. The roles are `nominal`, `verb`, `constraints`, `constraint-tool`,
`excluded`, and `unavailable` (the last uses `-` instead of a digest). Nominal
and verb rows define concatenation order. Greek nominal preparation uses the
pinned `addconstraints.pl` and entity-name input. The historical Latin perfect
stem substitution is retained, but the missing `vbs.mpi` is never silently
omitted. An inventory, digest or unavailable-file change fails validation.

After `build-stemlib-tables.cmake` completes in a fresh stage, run:

```sh
python3 tools/build-stemlib-lexical.py \
  --source stemlib \
  --manifest tools/stemlib-lexical-manifest.tsv \
  --language Greek --stage /absolute/path/to/greek-stage \
  --tools build/dev
```

Use a separately built Latin stage with `--language Latin`. Python 3 and Perl
are build-time dependencies only. The recipe verifies staged table inputs and
outputs, copies and verifies selected lexical sources, fixes the locale,
timezone and `MORPHLIB`, and invokes `indexnoms`, `do_conj`, and `indexvbs` with
explicit paths. It rejects reuse. `lexical/inputs.tsv`, per-producer diagnostics
and `lexical/comparison.json` remain available when a corpus blocks production.
A successful run emits `MORPHEUS-STEMLIB-LEXICAL-OUTPUTS.tsv` covering the four
stem-index files, verb expansion, and odd-key output. No success receipt is
written after a failed or blocked producer. Baseline comparisons explicitly
distinguish identical, different, and unavailable references.

The restored `do_conj` uses the historical binary derivation reader, preserving
its short-conjugation decisions. Its internal CLI is:

```text
do_conj [-I|-L] [-f] INPUT EXPANDED_OUTPUT ODD_KEY_OUTPUT
```

Outputs must not exist. The tool removes its owned outputs on failure. Missing
or truncated tables, oversized fields, invalid requests and unmatched
principal parts are fatal instead of embedding `errorN: nothing found` in an
apparently successful lexical output. The restored code also fixes undefined
returns, missing commas in the principal-part list and two ineffective newline
tests. None of these tools is installed or used by runtime releases.

CTest now takes two independent table builds per language, runs the lexical
chain on small Greek and Latin fixtures, and verifies all twelve output hashes
against `test/stemlib-lexical/outputs.tsv`. The Greek fixture expands present,
future and aorist stems; the Latin fixture also checks that an indexed
derivation need not carry an inflectional stem type. Existing output files,
failed expansions, malformed inputs and missing dependencies are exercised.

**Full-corpus qualification remains blocked.** The same test independently
stages the committed corpora and verifies the following first failures, without
adding binary exceptions or rewriting philological data:

| Producer | Corpus | First blocker |
| --- | --- | --- |
| `indexnoms` | Greek | `*glisa=s`: `eas_eantos` is not a registered stem type. |
| `indexnoms` | Latin | `Jeremiah`: `as_a` is not a registered stem type. |
| `do_conj` | Greek | The explicit request `br / o_stem / vn,-mm,h_hs` has no matching derivation rule. |
| Verb-source assembly | Latin | The historical input `stemsrc/vbs.mpi` is absent. |

A separate exploratory run over the available Latin verb files also encounters
`:de:explicu perfstem`, which requests the absent `derivs/out/perfstem.out`.
The supported recipe stops at the missing source and does not bypass it to
claim a qualified verb build. These are first blockers, not an exhaustive
corpus-error inventory. Full lexical baseline comparisons and source/toolchain
revision receipts remain open; fixture reproducibility does not certify the
complete distribution. The 229 existing table-binary exceptions remain intact.
