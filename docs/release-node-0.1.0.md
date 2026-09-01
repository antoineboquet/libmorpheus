<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Node.js binding 0.1.0 release decision

Version 0.1.0 is the first candidate release of the independently versioned
Node.js binding. It targets native libmorpheus 0.3.2 and C ABI 2. This document
records the intended contract; it does not authorize or record creation of the
`node-v0.1.0` tag.

## Distribution contract

The release consists of four coordinated public npm packages at the same
version:

- `@libmorpheus/node`, the typed ESM facade and acquisition commands;
- `@libmorpheus/node-darwin-arm64`;
- `@libmorpheus/node-linux-arm64-gnu`;
- `@libmorpheus/node-linux-x64-gnu`.

The three platform packages contain only the stable Node-API adapter. The
facade pins them as exact optional dependencies and runs no installation
script. Neither layer embeds the native runtime or linguistic data. Applications
acquire those separately or provide `MORPHEUS_LIBRARY` and
`MORPHEUS_STEMLIB` themselves.

## Behavioral contract

The binding exposes asynchronous Greek and Latin analysis and experimental
Greek generation. Its normalized records preserve:

- every interpretation returned by the native runtime;
- nullable scalar grammatical traits;
- combined dialect and geographic masks;
- dual number and multiple generated interpretations;
- raw numeric records, truncation markers, and morphological flags.

Calls through one context are serialized. Separate contexts may run
concurrently on Node's worker pool. Context and library closure is explicit and
idempotent, and a library refuses to close while contexts remain open.

## Acquisition contract

The `/native`, `/data`, and `/setup` exports and their command-line entrypoints
pin immutable upstream revisions and verify downloads before installation.
Native acquisition selects the declared libmorpheus 0.3.2 archive. Data
acquisition offers the audited Alpheios or Perseids trees; only Alpheios can
prepare the experimental generation index. Receipts record source revisions,
digests, binding version, native version, and native ABI.

## Qualification and publication

Linux CI builds the Node-API addon, runs semantic tests against real stem data,
exercises acquisition, and inspects npm package contents. The platform workflow
repeats addon and binding tests on Linux x64 glibc, Linux arm64 glibc, and macOS
arm64, then retains one staged npm package per target.

On an authorized `node-v0.1.0` tag, the Node publication workflow must wait for
both tagged workflows on the exact commit. It installs the facade and Linux x64
addon from the retained tarballs, imports the facade without an addon override,
and publishes platform packages before the facade. npm trusted publishing uses
GitHub OIDC and provenance.

The first release additionally requires the documented one-time `0.0.0`
bootstrap because npm trusted-publisher configuration requires existing package
records. The placeholders must use the `bootstrap` dist-tag, never `latest`.
After publication, the public-package smoke test must acquire runtime and both
datasets in an empty application and verify Greek analysis, Latin multiple
interpretations, and Greek dual generation.

## Licensing

The facade, Node-API adapter, acquisition code, workflows, and this release
decision are AGPL-3.0-or-later. Prepared generation sources retain their
inherited MPL-2.0 boundary. The optional addon packages contain no stem data;
downloaded upstream data retains its own recorded license and provenance.

## Remaining authorization

Before publication, a maintainer must:

1. interactively publish and inspect the four inert bootstrap packages;
2. configure `node-release.yml` as the trusted publisher for every package;
3. confirm the final tagged CI candidate;
4. explicitly authorize creation of `node-v0.1.0`.
