# Runtime benchmarks

`bench/compare.ts` compares the production-facing Deno FFI path with the
compatibility `cruncher` client on the same Beta Code corpus. It is a manual
benchmark rather than a CI performance gate: shared runners are used only for
a one-iteration smoke test.

Build the normal runtime first, then run:

```sh
cmake --preset dev
cmake --build --preset dev

MORPHEUS_LIBRARY="$PWD/build/dev/libmorpheus.so" \
MORPHEUS_STEMLIB="$PWD/stemlib" \
MORPHEUS_CRUNCHER="$PWD/build/dev/cruncher" \
deno run --allow-env --allow-ffi --allow-read --allow-run \
  bench/compare.ts \
  --iterations 20 \
  --warmup 2 \
  --contexts 1,2,4 \
  --cold-samples 10
```

Use `libmorpheus.dylib` on macOS. Pass `--json` to produce a machine-readable
report suitable for archiving with a release or deployment evaluation.

Release reports should identify the exact code, data, and compiler inputs:

```sh
MORPHEUS_LIBRARY="$PWD/build/release/libmorpheus.so" \
MORPHEUS_STEMLIB="$PWD/vendor/alpheios-morpheus/dist/stemlib" \
MORPHEUS_CRUNCHER="$PWD/build/release/cruncher" \
deno run --allow-env --allow-ffi --allow-read --allow-run \
  bench/compare.ts --json \
  --label release-candidate \
  --revision "$(git rev-parse HEAD)" \
  --stemlib-revision "$(git -C vendor/alpheios-morpheus rev-parse HEAD)" \
  --compiler "$(cc --version | head -n 1)" \
  > benchmark.json
```

The JSON schema is versioned. It records the Deno runtime, target platform,
hardware concurrency, corpus path and SHA-256 digest, source and stemlib
revisions, compiler label, run parameters, and raw measurements. The revision
and compiler values may instead be supplied through
`MORPHEUS_BENCHMARK_REVISION`, `MORPHEUS_STEMLIB_REVISION`, and
`MORPHEUS_BENCHMARK_COMPILER`.

## Measurements

The runner reports three execution models:

- `ffi`: structured analysis through the Deno binding, including native result
  copying into TypeScript objects. Each configured context count is measured;
  work is distributed evenly and each context retains its required serial
  queue.
- `cruncher-persistent`: all measured words are sent through one `cruncher`
  process. Its one-time startup is amortized over the corpus, approximating the
  throughput behavior of a retained process worker. The measurement includes
  the compatibility formatter and pipe writes; standard output is discarded
  only after those costs have been paid.
- `cruncher-cold`: one process is started for each sample. This quantifies the
  startup cost but does not represent the existing Bailly process pool.

For each model the report includes elapsed time, operations per second, mean
time per word, and startup time where it can be isolated. FFI measurements also
sample the Deno process RSS, which includes the loaded native library, contexts,
caches, native results, and copied TypeScript objects.

The persistent-process startup is included in its elapsed time and amortized
over every measured word; it is not reported separately. The cold-process
startup value is the mean end-to-end duration per sample. RSS for child
`cruncher` processes is not currently sampled, so memory columns are reported
only for FFI measurements.

The default corpus is the set of unique `input` values from the option-free
Greek fixtures in `test/fixture.json`. This keeps both engines in their shared
strict-case mode; Latin, `-n`, and `-S` variants are excluded instead of being
measured under mismatched settings. Fixture output is deliberately not
compared: this benchmark measures analysis execution, not differential
correctness. Those contracts remain covered by the fixture tests.

## Interpreting results

Run benchmarks on otherwise idle native hardware, use a release build for
deployment decisions, and retain the JSON report with compiler, Deno, stemlib,
CPU, and operating-system metadata. Compare at least:

1. single-context FFI latency against persistent-`cruncher` throughput;
2. FFI scaling across context counts;
3. RSS growth per additional warmed context;
4. cold process startup only as a diagnostic baseline.

Do not infer a production context count from shared CI results. Increase the
context count only while throughput improves without unacceptable RSS growth or
tail latency in the calling application.

## Comparing reports

Pass a prior JSON report with `--baseline` to compare matching engine/context
configurations:

```sh
deno run --allow-env --allow-ffi --allow-read --allow-run \
  bench/compare.ts --baseline previous.json --json > current.json
```

The output adds percentage changes for throughput, mean latency, peak RSS, and
RSS growth. Positive throughput is an improvement; positive latency or memory
is an increase. Reports with different corpus digests or missing engine/context
pairs are rejected rather than producing a misleading comparison.

No timing threshold is enforced in CI because shared-runner variance would make
it unstable. CI type-checks the runner, tests report comparison and corpus
validation deterministically, and executes a one-iteration end-to-end smoke
run. Release decisions use repeated measurements on controlled hardware.
