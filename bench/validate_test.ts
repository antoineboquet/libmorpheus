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

function measurement(engine: string, contexts: number): Measurement {
  return {
    engine,
    contexts,
    operations: 10,
    durationMs: 1,
    operationsPerSecond: 10_000,
    meanMicroseconds: 100,
    startupMs: engine === "ffi" ? 0.5 : null,
    analyses: engine === "ffi" ? 10 : null,
    peakRssBytes: engine === "ffi" ? 1024 : null,
    rssGrowthBytes: engine === "ffi" ? 128 : null,
  };
}

function report(): BenchmarkReport {
  return {
    schemaVersion: 1,
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
    measurements: [
      measurement("ffi", 1),
      measurement("ffi", 2),
      measurement("ffi", 4),
      measurement("cruncher-persistent", 1),
      measurement("cruncher-cold", 0),
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
