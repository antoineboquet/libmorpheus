import {
  type BenchmarkReport,
  compareReports,
  type Measurement,
} from "./compare.ts";

function measurement(
  engine: string,
  contexts: number,
  operationsPerSecond: number,
  meanMicroseconds: number,
  peakRssBytes: number | null = null,
): Measurement {
  return {
    engine,
    contexts,
    operations: 10,
    durationMs: 1,
    operationsPerSecond,
    meanMicroseconds,
    startupMs: null,
    analyses: null,
    peakRssBytes,
    rssGrowthBytes: peakRssBytes,
  };
}

function report(measurements: readonly Measurement[]): BenchmarkReport {
  return {
    schemaVersion: 1,
    generatedAt: "2026-01-01T00:00:00.000Z",
    label: null,
    platform: Deno.build,
    runtime: Deno.version,
    hardwareConcurrency: 1,
    sourceRevision: null,
    stemlibRevision: null,
    compiler: null,
    corpus: "fixture.json",
    corpusSha256: "same-corpus",
    uniqueWords: 1,
    iterations: 1,
    warmupIterations: 0,
    measurements,
  };
}

Deno.test("benchmark reports compare normalized measurements", () => {
  const comparison = compareReports(
    report([measurement("ffi", 1, 100, 10, 1000)]),
    report([measurement("ffi", 1, 125, 8, 1100)]),
  );
  if (comparison.length !== 1) throw new Error("missing comparison");
  if (comparison[0].throughputChangePercent !== 25) {
    throw new Error("incorrect throughput change");
  }
  if (Math.abs(comparison[0].meanLatencyChangePercent + 20) > 1e-9) {
    throw new Error("incorrect latency change");
  }
  if (Math.abs((comparison[0].peakRssChangePercent ?? 0) - 10) > 1e-9) {
    throw new Error("incorrect RSS change");
  }
});

Deno.test("benchmark comparison rejects a different corpus", () => {
  const baseline = report([measurement("ffi", 1, 100, 10)]);
  const current = { ...baseline, corpusSha256: "different-corpus" };
  let rejected = false;
  try {
    compareReports(baseline, current);
  } catch {
    rejected = true;
  }
  if (!rejected) throw new Error("different corpus was accepted");
});

Deno.test("benchmark comparison requires matching engine configurations", () => {
  let rejected = false;
  try {
    compareReports(
      report([measurement("ffi", 1, 100, 10)]),
      report([measurement("ffi", 2, 100, 10)]),
    );
  } catch {
    rejected = true;
  }
  if (!rejected) throw new Error("missing baseline measurement was accepted");
});
