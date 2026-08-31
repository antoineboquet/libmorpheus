<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Stemlib production audit

This audit records what can and cannot currently be reproduced from the
repository. It covers the compiled analysis stemlibs, not the separate
`gener.index` preparation pipeline documented in
[`gener-corpus.md`](gener-corpus.md).

## Conclusion

`libmorpheus` can consume and integrity-check pinned stemlib trees, but it does
not yet provide a supported stemlib compiler. The historical sources and build
drivers are present, and Alpheios demonstrates that the Greek pipeline still
runs on Linux. Neither the inherited makefiles nor the Alpheios automation is a
hermetic, fail-closed, byte-reproducible build.

The two production baselines have different status:

| Baseline | Runtime path | Production status |
| --- | --- | --- |
| Perseids Greek and Latin | `stemlib/` | Compiled snapshot introduced by commit `55f6ea6359a341d81fd0e6dc33f754f68119602f` in 2018 and unchanged since; no current producer runs in this repository. |
| Alpheios Greek | `vendor/alpheios-morpheus/dist/stemlib` | Output committed by the Alpheios stemlib workflow and pinned here at `4632415fe93c85e9fdca47a0c5a13f31385f0023`; this is the Greek reference dataset. |

Restoring reproducible production is therefore a distinct future task. It must
not be treated as a side effect of building the native runtime.

## Data layers

The historical tree mixes inputs, intermediates and runtime outputs. A future
build must model them as separate stages:

| Layer | Principal paths | Role |
| --- | --- | --- |
| Curated inputs | `stemsrc/`, `endtables/basics/`, `endtables/source/`, `derivs/source/`, `rule_files/` | Lexical entries and morphology rules maintained by humans or imported from external lexica. |
| Expanded tables | `endtables/ascii/`, `endtables/out/`, `derivs/ascii/`, `derivs/out/` | Text and binary tables emitted by `buildend` and `buildderiv`. |
| Search indexes | `steminds/`, `endtables/indices/`, `derivs/indices/` | Runtime lookup data emitted by `indexnoms`, `indexvbs`, `indendtables` and `indderivtables`. |
| Distribution | Alpheios `dist/stemlib/`; the bundled Perseids `stemlib/` snapshot | Selected rules, lexical data and compiled outputs consumed by Morpheus. |

Alpheios makes the distinction visible. Its `stemlib/Greek` directory contains
343 input files and no compiled `steminds`, ending outputs or indexes. Its
`dist/stemlib/Greek` directory contains 237 distribution files. The Alpheios
build compiles in the input tree and copies selected results into `dist/`; only
`dist/` is committed by its workflow. Consequently,
`vendor/alpheios-morpheus/stemlib/Greek` is not a runtime stemlib.

The bundled Perseids tree does not preserve that boundary: it contains 711
Greek files and 372 Latin files, including both sources and generated outputs.

## Existing production chain

The inherited chain is split across several directories:

| Stage | Programs | Historical build definition |
| --- | --- | --- |
| Build native tools | libraries plus all programs below | `src/makefile` and component makefiles |
| Expand irregular words | `buildword` | `src/gkends/expwordmain.c` |
| Expand ending tables | `buildend` | `src/gkends/expendmain.c` |
| Index ending tables | `indendtables` | `src/gkends/imain.c` |
| Expand derivations | `buildderiv` | `src/gkends/expsuffmain.c` |
| Index derivations | `indderivtables` | `src/gkends/smain.c` |
| Expand verb stems | `do_conj` | `src/gener/conjmain.c` |
| Build nominal and verbal indexes | `indexnoms`, `indexvbs` | `src/gkdict/indexnoms.main.c`, `src/gkdict/indexvbs.main.c` |
| Orchestrate one language | all of the above | `stemlib/Greek/makefile`, `stemlib/Latin/makefile` |

The modern CMake build compiles the reusable libraries and runtime but exposes
none of these eight historical producer programs as targets. A normal CMake
build therefore cannot rebuild a stemlib. The legacy route first installs the
programs into `bin/`, appends that directory to `PATH`, then invokes each
language makefile twice.

Alpheios is the only observed active producer. At the pinned revision its
GitHub workflow uses Ubuntu 22.04, `build-essential`, `flex-old`, and
`CFLAGS='-std=gnu89 -fcommon'`; `build_stemlib.sh` invokes the Greek makefile
twice and overlays selected output directories into `dist/stemlib/Greek`.

## Greek baseline

The reference inputs and runtime outputs are pinned together by the Alpheios
submodule revision. The Deno acquisition command additionally verifies the
selected 237-file distribution tree with the digest recorded in
`bindings/deno/internal/data_manifest.ts`. This proves acquisition integrity;
it does not prove that rebuilding the inputs produces the same tree.

The pin was audited against Alpheios `master` on 2026-08-31. Upstream was then
at `2f1a30d65ed7ae9c6120dbf64d730b863be412e4`, 25 commits after the pin, with
changes in 32 stem-source files, 55 ending-table files, two derivation files and
three rule files. Updating the submodule is therefore a corpus upgrade, not a
routine dependency refresh. It requires a new production build, tree diff,
analysis regression tests, generation-corpus qualification and benchmark
evidence.

## Perseids and Latin baseline

The repository preserves the Perseids history through
`ab6898ffed335fc6169fa02c9940657a9b5a78e0`. The compiled Greek and Latin files
under `stemlib/` were added by the earlier commit
`55f6ea6359a341d81fd0e6dc33f754f68119602f` with the description “include linux
stemlibs”. No later commit changed that tree.

The corresponding 2018 Docker recipe used Ubuntu 18.04, `build-essential`,
`flex`, `-std=gnu89`, and two make passes for each language. The image tag and
packages were not pinned by digest or version, and the commit contains no
output manifest or toolchain receipt. The checked-in bytes are thus an exact
baseline but not a reproducible-build proof.

For both languages, committed `lsj.nom`/`lsj.vbs` or `ls.nom`/`vbs.latin`
snapshots allow the compiled chain to start without the original lexicon
exports. The rules that create those snapshots still reference
`/local/text/lsj/lemmata` and `/local/text/ls/lemmata`. Those external inputs
are neither vendored nor versioned. Rebuilding the runtime indexes from the
committed curated inputs and reproducing the complete lexicon-import process
are therefore separate goals. The latter is currently impossible from this
repository alone.

## Reproducibility blockers

The following issues must be resolved before a regenerated tree can replace a
published baseline:

1. **Unsupported producers.** CMake does not build the eight producer
   executables; the legacy makefiles require GNU89-era compilation and flex.
2. **Non-hermetic recipes.** The language makefiles depend on ambient `PATH`,
   `MORPHLIB`, shell utilities, Perl behavior and absolute `/local/text/...`
   paths. Tool and dependency versions are not locked.
3. **Shared temporary files.** Both languages use fixed paths such as
   `/tmp/nommorph` and `/tmp/vbmorph`, preventing isolated or parallel builds.
4. **Incorrect dependency paths.** Rules name `stemind/nomind` and
   `stemind/vbind`, while the programs write `steminds/...`. These targets are
   never satisfied and obscure the real dependency graph.
5. **Masked failures.** Setup rules use `mkdir ... || true`. Alpheios
   `build_stemlib.sh` is not fail-fast, so a failed command need not stop later
   copies and cleanup.
6. **Stateful distribution assembly.** The Alpheios script overlays files onto
   an existing `dist/` tree instead of creating an empty staging directory.
   Removed or renamed outputs can survive a later build.
7. **Unverified two-pass behavior.** The second make pass is documented as
   discovering additional stems, but the pipeline neither states which stage
   requires iteration nor proves convergence. It does not compare pass two
   with pass three or two independent clean builds.
8. **No production manifest.** Neither upstream recipe records normalized
   input hashes, producer revision, exact toolchain, locale, output list and
   output digests in one receipt.
9. **No rebuild comparison.** Existing CI tests runtime behavior and pinned
   downloads, but no job performs two clean stemlib builds and compares their
   outputs byte for byte.

The recent explicit little-endian ending-table and index I/O work reduces one
class of host-format variance. It does not by itself make ordering, staging,
iteration or the rest of the toolchain deterministic.

## Acceptance criteria for the restoration phase

The later reproducible-build work should be accepted only when it provides all
of the following:

- explicit CMake targets for producer programs, kept internal and
  MPL-covered;
- a fresh, language-scoped staging directory with no writes to the source tree
  or global `/tmp` paths;
- pinned build environment, locale and tool versions;
- an explicit ordered input manifest for Greek and Latin, with missing and
  unexpected inputs rejected;
- a documented convergence rule, or removal of the historical second pass;
- a machine-readable receipt containing source revisions and input/output
  digests;
- two independent clean builds whose selected distribution trees are
  byte-identical;
- comparison against the current Perseids Latin baseline and the pinned
  Alpheios Greek baseline, with every intentional difference reviewed;
- the existing Greek and Latin fixtures, public API tests, null/multiple
  analysis cases, dialects, duals and Greek generation qualification still
  passing.

Until those criteria are met, compiled stemlibs remain pinned external or
checked-in data dependencies. Runtime releases and binding releases must not
silently regenerate or rewrite them.
