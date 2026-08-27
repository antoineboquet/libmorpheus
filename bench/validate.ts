// SPDX-License-Identifier: AGPL-3.0-or-later

import type { BenchmarkReport, Measurement } from "./compare.ts";

export interface ReleaseBenchmarkExpectations {
  readonly label: string;
  readonly sourceRevision: string;
  readonly stemlibRevision: string;
  readonly compiler: string;
}

const fullGitObjectId = /^[0-9a-f]{40}$/i;
const sha256 = /^[0-9a-f]{64}$/i;

function requireString(value: unknown, name: string): asserts value is string {
  if (typeof value !== "string" || !value.trim()) {
    throw new Error(`${name} must be a non-empty string`);
  }
}

function requireInteger(value: unknown, name: string, minimum: number): void {
  if (
    typeof value !== "number" ||
    !Number.isSafeInteger(value) ||
    value < minimum
  ) {
    throw new Error(
      `${name} must be an integer greater than or equal to ${minimum}`,
    );
  }
}

function requireFinite(value: unknown, name: string, minimum: number): void {
  if (typeof value !== "number" || !Number.isFinite(value) || value < minimum) {
    throw new Error(
      `${name} must be a finite number greater than or equal to ${minimum}`,
    );
  }
}

function validateMeasurement(measurement: Measurement, index: number): void {
  requireString(measurement.engine, `measurements[${index}].engine`);
  if (!["analysis", "generation"].includes(measurement.workload)) {
    throw new Error(`measurements[${index}].workload is invalid`);
  }
  if (measurement.workload === "generation") {
    requireString(measurement.lemma, `measurements[${index}].lemma`);
    requireFinite(measurement.results, `measurements[${index}].results`, 1);
  } else if (measurement.lemma !== null) {
    throw new Error(`measurements[${index}].lemma must be null for analysis`);
  }
  requireInteger(measurement.contexts, `measurements[${index}].contexts`, 0);
  requireInteger(measurement.operations, `measurements[${index}].operations`, 1);
  requireFinite(measurement.durationMs, `measurements[${index}].durationMs`, 0);
  requireFinite(
    measurement.operationsPerSecond,
    `measurements[${index}].operationsPerSecond`,
    0,
  );
  requireFinite(
    measurement.meanMicroseconds,
    `measurements[${index}].meanMicroseconds`,
    0,
  );
  for (const [name, value] of [
    ["startupMs", measurement.startupMs],
    ["results", measurement.results],
    ["peakRssBytes", measurement.peakRssBytes],
  ] as const) {
    if (value !== null) requireFinite(value, `measurements[${index}].${name}`, 0);
  }
  if (measurement.rssGrowthBytes !== null) {
    requireFinite(
      Math.abs(measurement.rssGrowthBytes),
      `measurements[${index}].rssGrowthBytes`,
      0,
    );
  }
}

export function validateReleaseBenchmark(
  report: BenchmarkReport,
  expected: ReleaseBenchmarkExpectations,
): void {
  if (report.schemaVersion !== 2) {
    throw new Error("unsupported benchmark schema");
  }
  requireString(report.generatedAt, "generatedAt");
  const generatedAt = new Date(report.generatedAt);
  if (
    Number.isNaN(generatedAt.valueOf()) ||
    generatedAt.toISOString() !== report.generatedAt
  ) {
    throw new Error("generatedAt must be an ISO timestamp");
  }

  for (const [name, actual, wanted] of [
    ["label", report.label, expected.label],
    ["sourceRevision", report.sourceRevision, expected.sourceRevision],
    ["stemlibRevision", report.stemlibRevision, expected.stemlibRevision],
    ["compiler", report.compiler, expected.compiler],
  ] as const) {
    requireString(actual, name);
    requireString(wanted, `expected ${name}`);
    if (actual !== wanted) {
      throw new Error(`${name} does not match the release input`);
    }
  }
  if (!fullGitObjectId.test(report.sourceRevision!)) {
    throw new Error("sourceRevision must be a full Git object ID");
  }
  if (!fullGitObjectId.test(report.stemlibRevision!)) {
    throw new Error("stemlibRevision must be a full Git object ID");
  }

  requireString(report.platform.os, "platform.os");
  requireString(report.platform.arch, "platform.arch");
  requireString(report.runtime.deno, "runtime.deno");
  requireString(report.runtime.v8, "runtime.v8");
  requireString(report.runtime.typescript, "runtime.typescript");
  requireInteger(report.hardwareConcurrency, "hardwareConcurrency", 1);
  requireString(report.corpus, "corpus");
  if (!sha256.test(report.corpusSha256)) {
    throw new Error("corpusSha256 must be a SHA-256 digest");
  }
  requireInteger(report.uniqueWords, "uniqueWords", 1);
  requireInteger(report.iterations, "iterations", 1);
  requireInteger(report.warmupIterations, "warmupIterations", 0);
  requireString(report.generationSmallLemma, "generationSmallLemma");
  requireString(report.generationMaximalLemma, "generationMaximalLemma");
  if (!sha256.test(report.generationIndexSha256!)) {
    throw new Error("generationIndexSha256 must be a SHA-256 digest");
  }
  if (!Array.isArray(report.measurements)) {
    throw new Error("measurements must be an array");
  }
  report.measurements.forEach(validateMeasurement);

  const configurations = new Set(
    report.measurements.map(({ engine, contexts }) => `${engine}:${contexts}`),
  );
  for (const required of [
    "ffi:1",
    "ffi:2",
    "ffi:4",
    "cruncher-persistent:1",
    "cruncher-cold:0",
    "generation-small-warm:1",
    "generation-small-warm:2",
    "generation-small-warm:4",
    "generation-maximal-warm:1",
    "generation-maximal-warm:2",
    "generation-maximal-warm:4",
    "generation-small-cold:0",
    "generation-maximal-cold:0",
  ]) {
    if (!configurations.has(required)) {
      throw new Error(`release report lacks ${required}`);
    }
  }
}

function parseCommandLine(arguments_: readonly string[]) {
  const reportPath = arguments_[0];
  if (!reportPath || reportPath.startsWith("--")) {
    throw new Error(
      "usage: validate.ts REPORT --label LABEL --revision SHA " +
        "--stemlib-revision SHA --compiler COMPILER",
    );
  }
  const values = new Map<string, string>();
  for (let index = 1; index < arguments_.length; index += 2) {
    const option = arguments_[index];
    const value = arguments_[index + 1];
    if (!option?.startsWith("--") || value === undefined) {
      throw new Error("release benchmark options require name-value pairs");
    }
    if (values.has(option)) throw new Error(`duplicate option: ${option}`);
    values.set(option, value);
  }
  const take = (option: string) => {
    const value = values.get(option);
    if (value === undefined) throw new Error(`${option} is required`);
    values.delete(option);
    return value;
  };
  const expected = {
    label: take("--label"),
    sourceRevision: take("--revision"),
    stemlibRevision: take("--stemlib-revision"),
    compiler: take("--compiler"),
  };
  if (values.size) throw new Error(`unknown option: ${values.keys().next().value}`);
  return { reportPath, expected };
}

if (import.meta.main) {
  const { reportPath, expected } = parseCommandLine(Deno.args);
  const report = JSON.parse(await Deno.readTextFile(reportPath)) as BenchmarkReport;
  validateReleaseBenchmark(report, expected);
  console.log(`validated release benchmark: ${reportPath}`);
}
