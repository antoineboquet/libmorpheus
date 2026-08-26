<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Greek generation corpus

This document freezes the source universe from which the first complete Greek
generation index will be prepared. It records selection and provenance; it does
not make the raw stem sources a runtime dependency.

## Pinned source

The manifest targets the Alpheios submodule commit
`4632415fe93c85e9fdca47a0c5a13f31385f0023`. Every selected path and its
SHA-256 digest is recorded in `tools/gener-corpus-manifest.tsv`. CI recomputes
all digests against `vendor/alpheios-morpheus/dist/stemlib/Greek` and rejects a
missing, additional, reordered, or modified historical build input.

The manifest contains 49 files in two provenance groups:

- 45 nominal-build inputs: the six `stemsrc/nom.*` files, the 37
  `stemsrc/nom[0-9]*` files, `stemsrc/lsj.nom`, and
  `stemsrc/lsj.byhand`;
- four verb-build inputs, in historical concatenation order:
  `stemsrc/vbs.irreg`, `stemsrc/vbs.simp.ml`,
  `stemsrc/vbs.simp.02.new`, and `stemsrc/lsj.vbs`.

The group labels describe the old Makefile paths, not the actual part of
speech. Three `:de:` verb derivations occur in nominal-build inputs, while the
verb-build inputs contain a small number of noun and adjective records. The
preparer must therefore process the ordered manifest as one heterogeneous
source universe.

## Transformation inventory

Before expansion, the pinned files contain:

| Source group | Lemma blocks | `:no:` | `:aj:` | `:vs:` | `:wd:` | `:vb:` | `:de:` | `;` | `@` |
| --- | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| Nominal build | 98,651 | 74,875 | 26,178 | 1 | 6,487 | 0 | 3 | 0 | 1,109 |
| Verb build | 10,051 | 1 | 13 | 1,512 | 0 | 1,728 | 10,596 | 6,776 | 2,344 |

There are consequently two distinct offline transformations:

1. continuation expansion, which combines `@` constraints with the preceding
   regular or irregular base record;
2. derivation expansion, which resolves `:de:` plus its `;` and `@` requests
   through the pinned derivation tables and emits explicit `:vs:` records.

The first transformation is now implemented by the bounded offline source
preparer for active `:no:`, `:aj:`, `:vs:`, `:wd:`, and `:vb:` records. It
reproduces the historical rule precisely: retain the record tag, stem, and
first ASCII-key token, then append the text following `@`. Every continuation
is based on the preceding explicit record rather than on the preceding
continuation. State is reset at each lemma and input-file boundary, so malformed
input cannot inherit the old front end's file-scope buffer.

Of the 3,453 continuations in the pinned corpus, 2,011 follow an active explicit
generation record and 1,441 belong to a `:de:`/`;` derivation sequence. The one
remaining occurrence follows disabled `-:vs:` and `#:vs:` lines in the
`ghra/skw` block; it is deliberately rejected as orphaned instead of attaching
to an unrelated earlier record. The derivation-associated continuations remain
the next preparation step.

The historical `do_conj` output is useful as a differential oracle, but it is
not itself the normalized input contract: it preserves control records, marks
some of them with `-`, and uses global mutable buffers. The supported preparer
will emit only lemma and explicit generation records accepted by the version 1
index builder.

## Duplicate policy still to qualify

The historical analysis build concatenates overlapping author files and LSJ
files before sorting and indexing. A generation lookup must avoid multiplying
identical paradigms while retaining genuinely distinct blocks for the same
lemma. Exact raw-block identity is insufficient because continuations and
derivations change the semantic record set.

The final deduplication rule will therefore be fixed after expansion by
comparison with the historical generator. Until then, the source manifest
preserves every ordered input and the index format continues to support
multiple blocks for one canonical key.

## Licensing

The manifest and any prepared corpus or index are derived from the pinned stem
sources and retain their applicable licensing and provenance. The independently
written validation and preparation code may be AGPL-3.0-or-later; executing it
does not relicense the data.
