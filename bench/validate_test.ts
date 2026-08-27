// SPDX-License-Identifier: AGPL-3.0-or-later

import type { BenchmarkReport, Measurement } from "./compare.ts";
import {
  type ReleaseBenchmarkExpectations,
  validateReleaseBenchmark,
} from "./validate.ts";

const sourceRevision = "1".repeat(40);
const stemlibRevision = "2".repeat(40);
const expected: ReleaseBenchmarkExpectations = {
  label: "release-candidate",
  sourceRevision,
  stemlibRevision,
  compiler: "cc 1.0",
};

function measurement(
  engine: string,
  contexts: number,
  workload: "analysis" | "generation" = "analysis",
): Measurement {
  return {
    engine,
    workload,
    lemma: workload === "generation"
      ? (engine.includes("maximal") ? "i(/hmi" : "lo/gos")
      : null,
    contexts,
    operations: 10,
    durationMs: 1,
    operationsPerSecond: 10_000,
    meanMicroseconds: 100,
    startupMs: engine === "ffi" ? 0.5 : null,
    results: workload === "generation" || engine === "ffi" ? 10 : null,
    peakRssBytes: engine === "ffi" ? 1024 : null,
    rssGrowthBytes: engine === "ffi" ? 128 : null,
  };
}

function report(): BenchmarkReport {
  return {
    schemaVersion: 2,
    generatedAt: "2026-08-23T00:00:00.000Z",
    label: expected.label,
    platform: Deno.build,
    runtime: Deno.version,
    hardwareConcurrency: 4,
    sourceRevision,
    stemlibRevision,
    compiler: expected.compiler,
    corpus: "test/fixture.json",
    corpusSha256: "a".repeat(64),
    uniqueWords: 10,
    iterations: 20,
    warmupIterations: 2,
    generationSmallLemma: "lo/gos",
    generationMaximalLemma: "i(/hmi",
    generationIndexSha256: "b".repeat(64),
    measurements: [
      measurement("ffi", 1),
      measurement("ffi", 2),
      measurement("ffi", 4),
      measurement("cruncher-persistent", 1),
      measurement("cruncher-cold", 0),
      measurement("generation-small-warm", 1, "generation"),
      measurement("generation-small-warm", 2, "generation"),
      measurement("generation-small-warm", 4, "generation"),
      measurement("generation-maximal-warm", 1, "generation"),
      measurement("generation-maximal-warm", 2, "generation"),
      measurement("generation-maximal-warm", 4, "generation"),
      measurement("generation-small-cold", 0, "generation"),
      measurement("generation-maximal-cold", 0, "generation"),
    ],
  };
}

Deno.test("complete release benchmark is accepted", () => {
  validateReleaseBenchmark(report(), expected);
});

Deno.test("release benchmark must match the intended revision", () => {
  const mismatched = { ...report(), sourceRevision: "3".repeat(40) };
  let rejected = false;
  try {
    validateReleaseBenchmark(mismatched, expected);
  } catch {
    rejected = true;
  }
  if (!rejected) throw new Error("mismatched source revision was accepted");
});

Deno.test("release benchmark requires all execution models", () => {
  const incomplete = {
    ...report(),
    measurements: report().measurements.slice(0, 3),
  };
  let rejected = false;
  try {
    validateReleaseBenchmark(incomplete, expected);
  } catch {
    rejected = true;
  }
  if (!rejected) throw new Error("incomplete benchmark was accepted");
});
