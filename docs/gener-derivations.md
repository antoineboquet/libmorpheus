<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Greek generation derivations

This document fixes the inputs and reference behavior for expanding `:de:`,
`;`, and their `@` continuations before construction of the generation index.
The runtime library will consume only the resulting explicit `:vs:`, `:aj:`,
and `:no:` records.

## Pinned rule corpus

The rule corpus comes from Alpheios commit
`4632415fe93c85e9fdca47a0c5a13f31385f0023`. Its checksummed manifest is
`tools/gener-derivation-manifest.tsv` and contains:

- the ordered `rule_files/derivtypes.table` registry;
- 38 unique rule files referenced by its 39 entries;
- five source rule files present on disk but absent from the registry:
  `cw`, `es_denom`, `ow_fact`, `ow_instr`, and `ww`.

The difference is historical, not an inference by this project. The registry
declares `illw` twice, with numeric values `037` and `041`. CI preserves both
the duplicate declaration and the complete set of unreferenced files so that a
future upstream correction cannot silently change generation behavior.

## Differential oracle

`test/gener-derivation-source.txt` exercises:

- explicit present, future, and aorist principal-part requests;
- a continuation of a principal-part request;
- a regular derivation with no explicit principal part;
- an attached suffix request;
- a preverb;
- present reduplication.

It was passed to the pinned Alpheios `do_conj` executable in `full` mode after
building the pinned derivation tables. Control lines were then removed, leaving
only lemma and active generation records in
`test/gener-derivation-oracle.expected`.

`full` mode is the relevant oracle for generation. The historical short mode
is an analysis-index optimization: it suppresses records considered redundant
and can leave regular `:de:` controls active for later analysis tooling. Such
controls are not accepted by the generation index.

The future preparer must match the oracle's record semantics and stable order;
it need not reproduce diagnostic text, disabled `-` control lines, blank-line
placement, or file-scope state.

## Invalid source records

Seven `:de:` records in the frozen generation corpus do not name one of the 38
active derivation types under the historical parser:

| Source | Line | Parsed derivation type | Problem |
| --- | ---: | --- | --- |
| `stemsrc/nom.proper` | 7,941 | `as_a` | No registered rule. |
| `stemsrc/vbs.simp.ml` | 3,928 | `e_stem,epic` | Comma is attached to the type token. |
| `stemsrc/vbs.simp.ml` | 6,272 | `ew` | No registered rule. |
| `stemsrc/vbs.simp.ml` | 9,214 | `reg_conj,syll_aug` | Comma is attached to the type token. |
| `stemsrc/vbs.simp.ml` | 15,142 | `melitt` | Space after `:de:` leaves the stem empty and shifts the tokens. |
| `stemsrc/vbs.simp.ml` | 22,130 | `w_stem` | Stem type used where a derivation type is required. |
| `stemsrc/vbs.simp.ml` | 25,428 | `numi,poetic` | Comma is attached to the type token. |

The historical program reports missing rule files for these records. The
supported preparer must likewise reject them with source locations. It must not
silently split commas, substitute similarly named types, or reinterpret an
unreferenced rule file. Any correction belongs in a separately reviewed data
patch with its own provenance and differential evidence.

## Implementation boundary

The legacy `conjsys.c` and `combconj.c` front ends use file-scope mutable
buffers and can create diagnostic side files. They remain provenance and oracle
code, not runtime dependencies. The supported implementation will keep parser
state per invocation, use bounded owned storage, load only manifest-approved
rules, and call the inherited morphology primitives through an internal MPL
boundary.
