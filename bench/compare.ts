// SPDX-License-Identifier: AGPL-3.0-or-later

import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "../bindings/deno/mod.ts";

interface Fixture {
  readonly input: string;
  readonly opts?: readonly string[];
}

interface Configuration {
  readonly fixturePath: string;
  readonly iterations: number;
  readonly warmupIterations: number;
  readonly contextCounts: readonly number[];
  readonly coldSamples: number;
  readonly json: boolean;
  readonly baselinePath: string | null;
  readonly label: string | null;
  readonly sourceRevision: string | null;
  readonly stemlibRevision: string | null;
  readonly compiler: string | null;
  readonly generationSmallLemma: string | null;
  readonly generationMaximalLemma: string | null;
}

export interface Measurement {
  readonly engine: string;
  readonly workload: "analysis" | "generation";
  readonly lemma: string | null;
  readonly contexts: number;
  readonly operations: number;
  readonly durationMs: number;
  readonly operationsPerSecond: number;
  readonly meanMicroseconds: number;
  readonly startupMs: number | null;
  readonly results: number | null;
  readonly peakRssBytes: number | null;
  readonly rssGrowthBytes: number | null;
}

export interface BenchmarkReport {
  readonly schemaVersion: 2;
  readonly generatedAt: string;
  readonly label: string | null;
  readonly platform: typeof Deno.build;
  readonly runtime: typeof Deno.version;
  readonly hardwareConcurrency: number;
  readonly sourceRevision: string | null;
  readonly stemlibRevision: string | null;
  readonly compiler: string | null;
  readonly corpus: string;
  readonly corpusSha256: string;
  readonly uniqueWords: number;
  readonly iterations: number;
  readonly warmupIterations: number;
  readonly generationSmallLemma: string | null;
  readonly generationMaximalLemma: string | null;
  readonly generationIndexSha256: string | null;
  readonly measurements: readonly Measurement[];
}

export interface BenchmarkComparison {
  readonly engine: string;
  readonly contexts: number;
  readonly throughputChangePercent: number;
  readonly meanLatencyChangePercent: number;
  readonly peakRssChangePercent: number | null;
  readonly rssGrowthChangePercent: number | null;
}

const encoder = new TextEncoder();

function positiveInteger(value: string, option: string, allowZero = false): number {
  const parsed = Number(value);
  if (!Number.isSafeInteger(parsed) || parsed < (allowZero ? 0 : 1)) {
    throw new Error(`${option} requires ${allowZero ? "a non-negative" : "a positive"} integer`);
  }
  return parsed;
}

function parseArguments(arguments_: readonly string[]): Configuration {
  let fixturePath = "test/fixture.json";
  let iterations = 20;
  let warmupIterations = 2;
  let contextCounts: readonly number[] = [1, 2, 4];
  let coldSamples = 10;
  let json = false;
  let baselinePath: string | null = null;
  let label: string | null = null;
  let sourceRevision: string | null = null;
  let stemlibRevision: string | null = null;
  let compiler: string | null = null;
  let generationSmallLemma: string | null = null;
  let generationMaximalLemma: string | null = null;

  for (let index = 0; index < arguments_.length; index++) {
    const argument = arguments_[index];
    const next = () => {
      const value = arguments_[++index];
      if (value === undefined) throw new Error(`${argument} requires a value`);
      return value;
    };
    switch (argument) {
      case "--fixture": fixturePath = next(); break;
      case "--iterations": iterations = positiveInteger(next(), argument); break;
      case "--warmup":
        warmupIterations = positiveInteger(next(), argument, true);
        break;
      case "--contexts": {
        const values = next().split(",").map((value) =>
          positiveInteger(value, argument)
        );
        contextCounts = [...new Set(values)];
        break;
      }
      case "--cold-samples":
        coldSamples = positiveInteger(next(), argument, true);
        break;
      case "--baseline": baselinePath = next(); break;
      case "--label": label = next(); break;
      case "--revision": sourceRevision = next(); break;
      case "--stemlib-revision": stemlibRevision = next(); break;
      case "--compiler": compiler = next(); break;
      case "--generation-small": generationSmallLemma = next(); break;
      case "--generation-maximal": generationMaximalLemma = next(); break;
      case "--json": json = true; break;
      default: throw new Error(`unknown argument: ${argument}`);
    }
  }
  if ((generationSmallLemma === null) !== (generationMaximalLemma === null)) {
    throw new Error(
      "--generation-small and --generation-maximal must be provided together",
    );
  }
  return {
    fixturePath,
    iterations,
    warmupIterations,
    contextCounts,
    coldSamples,
    json,
    baselinePath,
    label,
    sourceRevision,
    stemlibRevision,
    compiler,
    generationSmallLemma,
    generationMaximalLemma,
  };
}

function requireEnvironment(name: string): string {
  const value = Deno.env.get(name);
  if (!value) throw new Error(`${name} is required`);
  return value;
}

async function loadWords(path: string): Promise<readonly string[]> {
  const fixtures = JSON.parse(await Deno.readTextFile(path)) as Fixture[];
  const words = [...new Set(fixtures
    .filter(({ opts }) => !opts?.length)
    .map(({ input }) => input)
    .filter(Boolean))];
  if (!words.length) throw new Error(`no benchmark words found in ${path}`);
  return words;
}

async function corpusSha256(words: readonly string[]): Promise<string> {
  const digest = new Uint8Array(await crypto.subtle.digest(
    "SHA-256",
    encoder.encode(JSON.stringify(words)),
  ));
  return [...digest].map((value) => value.toString(16).padStart(2, "0")).join("");
}

async function fileSha256(path: string): Promise<string> {
  const digest = new Uint8Array(await crypto.subtle.digest(
    "SHA-256",
    await Deno.readFile(path),
  ));
  return [...digest].map((value) => value.toString(16).padStart(2, "0")).join("");
}

function percentageChange(current: number, baseline: number): number {
  if (!Number.isFinite(current) || !Number.isFinite(baseline) || baseline === 0) {
    throw new Error("benchmark values must be finite and baseline values nonzero");
  }
  return (current / baseline - 1) * 100;
}

export function compareReports(
  baseline: BenchmarkReport,
  current: BenchmarkReport,
): readonly BenchmarkComparison[] {
  if (baseline.schemaVersion !== 2 || current.schemaVersion !== 2) {
    throw new Error("unsupported benchmark report schema");
  }
  if (baseline.corpusSha256 !== current.corpusSha256) {
    throw new Error("benchmark corpus differs from baseline");
  }
  if (
    baseline.generationIndexSha256 !== current.generationIndexSha256 ||
    baseline.generationSmallLemma !== current.generationSmallLemma ||
    baseline.generationMaximalLemma !== current.generationMaximalLemma
  ) {
    throw new Error("generation benchmark inputs differ from baseline");
  }
  const baselineMeasurements = new Map(
    baseline.measurements.map((measurement) =>
      [
        `${measurement.engine}:${measurement.contexts}:${measurement.lemma ?? ""}`,
        measurement,
      ] as const
    ),
  );
  return current.measurements.map((measurement) => {
    const previous = baselineMeasurements.get(
      `${measurement.engine}:${measurement.contexts}:${measurement.lemma ?? ""}`,
    );
    if (!previous) {
      throw new Error(
        `baseline lacks ${measurement.engine} with ${measurement.contexts} contexts`,
      );
    }
    const optionalChange = (
      value: number | null,
      baselineValue: number | null,
    ) => value === null || baselineValue === null
      ? null
      : baselineValue === 0
      ? (value === 0 ? 0 : null)
      : percentageChange(value, baselineValue);
    return {
      engine: measurement.engine,
      contexts: measurement.contexts,
      throughputChangePercent: percentageChange(
        measurement.operationsPerSecond,
        previous.operationsPerSecond,
      ),
      meanLatencyChangePercent: percentageChange(
        measurement.meanMicroseconds,
        previous.meanMicroseconds,
      ),
      peakRssChangePercent: optionalChange(
        measurement.peakRssBytes,
        previous.peakRssBytes,
      ),
      rssGrowthChangePercent: optionalChange(
        measurement.rssGrowthBytes,
        previous.rssGrowthBytes,
      ),
    };
  });
}

function summarize(
  engine: string,
  workload: "analysis" | "generation",
  lemma: string | null,
  contexts: number,
  operations: number,
  durationMs: number,
  startupMs: number | null,
  results: number | null,
  peakRssBytes: number | null,
  baselineRssBytes: number | null,
): Measurement {
  return {
    engine,
    workload,
    lemma,
    contexts,
    operations,
    durationMs,
    operationsPerSecond: operations * 1000 / durationMs,
    meanMicroseconds: durationMs * 1000 / operations,
    startupMs,
    results,
    peakRssBytes,
    rssGrowthBytes: peakRssBytes === null || baselineRssBytes === null
      ? null
      : peakRssBytes - baselineRssBytes,
  };
}

async function benchmarkFfi(
  libraryPath: string,
  stemlibPath: string,
  words: readonly string[],
  configuration: Configuration,
  contextCount: number,
): Promise<Measurement> {
  const baselineRss = Deno.memoryUsage().rss;
  const startupStart = performance.now();
  const library = new MorpheusLibrary(libraryPath);
  const contexts = Array.from(
    { length: contextCount },
    () => library.createContext(stemlibPath, MorpheusLanguage.Greek),
  );
  const startupMs = performance.now() - startupStart;

  try {
    for (let iteration = 0; iteration < configuration.warmupIterations; iteration++) {
      await Promise.all(words.map((word, index) =>
        contexts[index % contexts.length].analyze(word, MorpheusOption.StrictCase)
      ));
    }

    let peakRss = Deno.memoryUsage().rss;
    const sampler = setInterval(() => {
      peakRss = Math.max(peakRss, Deno.memoryUsage().rss);
    }, 5);
    const operations = configuration.iterations * words.length;
    const start = performance.now();
    let counts: readonly number[];
    try {
      counts = await Promise.all(contexts.map(async (context, worker) => {
        let analyses = 0;
        for (let operation = worker; operation < operations; operation += contexts.length) {
          analyses += (await context.analyze(
            words[operation % words.length],
            MorpheusOption.StrictCase,
          )).length;
        }
        return analyses;
      }));
    } finally {
      clearInterval(sampler);
    }
    const durationMs = performance.now() - start;
    peakRss = Math.max(peakRss, Deno.memoryUsage().rss);
    return summarize(
      "ffi",
      "analysis",
      null,
      contextCount,
      operations,
      durationMs,
      startupMs,
      counts.reduce((sum, count) => sum + count, 0),
      peakRss,
      baselineRss,
    );
  } finally {
    await Promise.all(contexts.map((context) => context.close()));
    library.close();
  }
}

async function runCruncher(
  cruncherPath: string,
  stemlibPath: string,
  words: readonly string[],
): Promise<void> {
  const child = new Deno.Command(cruncherPath, {
    args: ["-T"],
    env: { MORPHLIB: stemlibPath },
    stdin: "piped",
    stdout: "null",
    stderr: "null",
  }).spawn();
  const writer = child.stdin.getWriter();
  await writer.write(encoder.encode(`${words.join("\n")}\n`));
  await writer.close();
  const status = await child.status;
  if (!status.success) throw new Error(`cruncher exited with status ${status.code}`);
}

async function benchmarkPersistentCruncher(
  cruncherPath: string,
  stemlibPath: string,
  words: readonly string[],
  configuration: Configuration,
): Promise<Measurement> {
  const warmupWords = Array.from(
    { length: configuration.warmupIterations },
    () => words,
  ).flat();
  if (warmupWords.length) await runCruncher(cruncherPath, stemlibPath, warmupWords);
  const measuredWords = Array.from(
    { length: configuration.iterations },
    () => words,
  ).flat();
  const start = performance.now();
  await runCruncher(cruncherPath, stemlibPath, measuredWords);
  const durationMs = performance.now() - start;
  return summarize(
    "cruncher-persistent",
    "analysis",
    null,
    1,
    measuredWords.length,
    durationMs,
    null,
    null,
    null,
    null,
  );
}

async function benchmarkColdCruncher(
  cruncherPath: string,
  stemlibPath: string,
  words: readonly string[],
  samples: number,
): Promise<Measurement | null> {
  if (!samples) return null;
  const start = performance.now();
  for (let sample = 0; sample < samples; sample++) {
    await runCruncher(cruncherPath, stemlibPath, [words[sample % words.length]]);
  }
  const durationMs = performance.now() - start;
  return summarize(
    "cruncher-cold",
    "analysis",
    null,
    0,
    samples,
    durationMs,
    durationMs / samples,
    null,
    null,
    null,
  );
}

async function benchmarkWarmGeneration(
  libraryPath: string,
  stemlibPath: string,
  lemma: string,
  label: "small" | "maximal",
  configuration: Configuration,
  contextCount: number,
): Promise<Measurement> {
  const baselineRss = Deno.memoryUsage().rss;
  const startupStart = performance.now();
  const library = new MorpheusLibrary(libraryPath);
  const contexts = Array.from(
    { length: contextCount },
    () => library.createContext(stemlibPath, MorpheusLanguage.Greek),
  );
  const startupMs = performance.now() - startupStart;
  try {
    await Promise.all(contexts.map((context) =>
      context.generate(lemma, { resultLimit: 65_536 })
    ));
    for (
      let iteration = 0;
      iteration < configuration.warmupIterations;
      iteration++
    ) {
      await Promise.all(contexts.map((context) =>
        context.generate(lemma, { resultLimit: 65_536 })
      ));
    }

    let peakRss = Deno.memoryUsage().rss;
    const sampler = setInterval(() => {
      peakRss = Math.max(peakRss, Deno.memoryUsage().rss);
    }, 5);
    const operations = configuration.iterations * contextCount;
    const start = performance.now();
    let counts: readonly number[];
    try {
      counts = await Promise.all(contexts.map(async (context) => {
        let results = 0;
        for (
          let iteration = 0;
          iteration < configuration.iterations;
          iteration++
        ) {
          results += (await context.generate(
            lemma,
            { resultLimit: 65_536 },
          )).length;
        }
        return results;
      }));
    } finally {
      clearInterval(sampler);
    }
    const durationMs = performance.now() - start;
    peakRss = Math.max(peakRss, Deno.memoryUsage().rss);
    return summarize(
      `generation-${label}-warm`,
      "generation",
      lemma,
      contextCount,
      operations,
      durationMs,
      startupMs,
      counts.reduce((sum, count) => sum + count, 0),
      peakRss,
      baselineRss,
    );
  } finally {
    await Promise.all(contexts.map((context) => context.close()));
    library.close();
  }
}

async function benchmarkColdGeneration(
  libraryPath: string,
  stemlibPath: string,
  lemma: string,
  label: "small" | "maximal",
  samples: number,
): Promise<Measurement | null> {
  if (!samples) return null;
  const baselineRss = Deno.memoryUsage().rss;
  const library = new MorpheusLibrary(libraryPath);
  let peakRss = baselineRss;
  let results = 0;
  const start = performance.now();
  try {
    for (let sample = 0; sample < samples; sample++) {
      const context = library.createContext(stemlibPath, MorpheusLanguage.Greek);
      try {
        results += (await context.generate(
          lemma,
          { resultLimit: 65_536 },
        )).length;
        peakRss = Math.max(peakRss, Deno.memoryUsage().rss);
      } finally {
        await context.close();
      }
    }
  } finally {
    library.close();
  }
  const durationMs = performance.now() - start;
  return summarize(
    `generation-${label}-cold`,
    "generation",
    lemma,
    0,
    samples,
    durationMs,
    durationMs / samples,
    results,
    peakRss,
    baselineRss,
  );
}

function printable(measurement: Measurement) {
  return {
    engine: measurement.engine,
    workload: measurement.workload,
    lemma: measurement.lemma ?? "n/a",
    contexts: measurement.contexts,
    operations: measurement.operations,
    duration_ms: measurement.durationMs.toFixed(2),
    ops_per_second: measurement.operationsPerSecond.toFixed(2),
    mean_us: measurement.meanMicroseconds.toFixed(2),
    startup_ms: measurement.startupMs === null
      ? "n/a"
      : measurement.startupMs.toFixed(2),
    results: measurement.results ?? "n/a",
    peak_rss_mib: measurement.peakRssBytes === null
      ? "n/a"
      : (measurement.peakRssBytes / 1024 / 1024).toFixed(2),
    rss_growth_mib: measurement.rssGrowthBytes === null
      ? "n/a"
      : (measurement.rssGrowthBytes / 1024 / 1024).toFixed(2),
  };
}

function printableComparison(comparison: BenchmarkComparison) {
  const percent = (value: number | null) => value === null
    ? "n/a"
    : `${value >= 0 ? "+" : ""}${value.toFixed(2)}%`;
  return {
    engine: comparison.engine,
    contexts: comparison.contexts,
    throughput: percent(comparison.throughputChangePercent),
    mean_latency: percent(comparison.meanLatencyChangePercent),
    peak_rss: percent(comparison.peakRssChangePercent),
    rss_growth: percent(comparison.rssGrowthChangePercent),
  };
}

if (import.meta.main) {
  const configuration = parseArguments(Deno.args);
  const libraryPath = requireEnvironment("MORPHEUS_LIBRARY");
  const stemlibPath = requireEnvironment("MORPHEUS_STEMLIB");
  const cruncherPath = requireEnvironment("MORPHEUS_CRUNCHER");
  const words = await loadWords(configuration.fixturePath);
  const measurements: Measurement[] = [];

  for (const contextCount of configuration.contextCounts) {
    measurements.push(await benchmarkFfi(
      libraryPath,
      stemlibPath,
      words,
      configuration,
      contextCount,
    ));
  }
  measurements.push(await benchmarkPersistentCruncher(
    cruncherPath,
    stemlibPath,
    words,
    configuration,
  ));
  const cold = await benchmarkColdCruncher(
    cruncherPath,
    stemlibPath,
    words,
    configuration.coldSamples,
  );
  if (cold) measurements.push(cold);

  if (
    configuration.generationSmallLemma !== null &&
    configuration.generationMaximalLemma !== null
  ) {
    for (const contextCount of configuration.contextCounts) {
      measurements.push(await benchmarkWarmGeneration(
        libraryPath,
        stemlibPath,
        configuration.generationSmallLemma,
        "small",
        configuration,
        contextCount,
      ));
      measurements.push(await benchmarkWarmGeneration(
        libraryPath,
        stemlibPath,
        configuration.generationMaximalLemma,
        "maximal",
        configuration,
        contextCount,
      ));
    }
    for (const [lemma, label] of [
      [configuration.generationSmallLemma, "small"],
      [configuration.generationMaximalLemma, "maximal"],
    ] as const) {
      const generationCold = await benchmarkColdGeneration(
        libraryPath,
        stemlibPath,
        lemma,
        label,
        configuration.coldSamples,
      );
      if (generationCold) measurements.push(generationCold);
    }
  }

  const report: BenchmarkReport = {
    schemaVersion: 2,
    generatedAt: new Date().toISOString(),
    label: configuration.label,
    platform: Deno.build,
    runtime: Deno.version,
    hardwareConcurrency: navigator.hardwareConcurrency,
    sourceRevision: configuration.sourceRevision ??
      Deno.env.get("MORPHEUS_BENCHMARK_REVISION") ?? null,
    stemlibRevision: configuration.stemlibRevision ??
      Deno.env.get("MORPHEUS_STEMLIB_REVISION") ?? null,
    compiler: configuration.compiler ??
      Deno.env.get("MORPHEUS_BENCHMARK_COMPILER") ?? null,
    corpus: configuration.fixturePath,
    corpusSha256: await corpusSha256(words),
    uniqueWords: words.length,
    iterations: configuration.iterations,
    warmupIterations: configuration.warmupIterations,
    generationSmallLemma: configuration.generationSmallLemma,
    generationMaximalLemma: configuration.generationMaximalLemma,
    generationIndexSha256: configuration.generationSmallLemma === null
      ? null
      : await fileSha256(`${stemlibPath}/gener.index`),
    measurements,
  };
  const comparison = configuration.baselinePath
    ? compareReports(
      JSON.parse(await Deno.readTextFile(configuration.baselinePath)) as BenchmarkReport,
      report,
    )
    : null;
  if (configuration.json) {
    console.log(JSON.stringify(
      comparison === null ? report : { ...report, comparison },
      null,
      2,
    ));
  } else {
    console.table(measurements.map(printable));
    if (comparison) console.table(comparison.map(printableComparison));
  }
}
