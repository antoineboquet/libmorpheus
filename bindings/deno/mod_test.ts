import {
  hasMorpheusMorphFlag,
  MorpheusError,
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
  MorpheusMorphFlag,
  MorpheusPartOfSpeech,
  MorpheusStatus,
  MorpheusTruncatedField,
} from "./mod.ts";

function assert(condition: unknown, message: string): asserts condition {
  if (!condition) throw new Error(message);
}

Deno.test("tests one-based morphology flag bits", () => {
  const allMorphFlags = new Uint8Array(14);
  allMorphFlags[0] = 1;
  allMorphFlags[13] = 32;
  assert(
    hasMorpheusMorphFlag({ allMorphFlags }, MorpheusMorphFlag.SyllAugment),
    "flag 1 must use the low bit of byte 0",
  );
  assert(
    hasMorpheusMorphFlag({ allMorphFlags }, MorpheusMorphFlag.GroupName),
    "flag 110 must use bit 5 of byte 13",
  );
  assert(
    !hasMorpheusMorphFlag({ allMorphFlags }, MorpheusMorphFlag.Poetic),
    "unset flags must remain false",
  );
});

Deno.test("analyzes Greek through the native ABI", async () => {
  const libraryPath = Deno.env.get("MORPHEUS_LIBRARY");
  const stemlibPath = Deno.env.get("MORPHEUS_STEMLIB");
  assert(libraryPath, "MORPHEUS_LIBRARY is required");
  assert(stemlibPath, "MORPHEUS_STEMLIB is required");

  using library = new MorpheusLibrary(libraryPath);
  let rejectedMissingStemlib = false;
  try {
    library.createContext(`${stemlibPath}/missing`, MorpheusLanguage.Greek);
  } catch (error) {
    assert(error instanceof MorpheusError, "stemlib failure must be typed");
    assert(
      error.status === MorpheusStatus.StemlibError,
      "missing stemlib must have a stable status",
    );
    rejectedMissingStemlib = true;
  }
  assert(rejectedMissingStemlib, "missing stemlib must be rejected at open");
  await using context = library.createContext(
    stemlibPath,
    MorpheusLanguage.Greek,
  );

  const analyses = await context.analyze(
    "bi/ou",
    MorpheusOption.StrictCase,
  );
  assert(analyses.length === 4, "bi/ou must retain the CLI fixture count");
  assert(analyses[0].raw.length > 0, "raw form must be populated");
  assert(analyses[0].lemma.length > 0, "lemma must be populated");
  assert(Array.isArray(analyses[0].morphFlags), "morph flags must be named");
  assert(
    typeof hasMorpheusMorphFlag(
      analyses[0],
      MorpheusMorphFlag.GroupName,
    ) === "boolean",
    "named morph flags must be testable",
  );
  assert(
    analyses[0].partOfSpeech !== "unknown",
    "part of speech must use a documented name",
  );
  assert(
    analyses[0].truncatedFields.length === 0,
    "fixture fields must expose no truncation names",
  );

  const rawAnalyses = await context.analyzeRaw(
    "bi/ou",
    MorpheusOption.StrictCase,
  );
  assert(rawAnalyses.length === analyses.length, "raw analysis count must match");
  assert(rawAnalyses[0].structSize >= 860, "raw ABI size must be preserved");
  assert(rawAnalyses[0].morphFlags.length === 12, "raw flags must be copied");
  assert(rawAnalyses[0].allMorphFlags.length === 14, "all raw flags must be copied");
  assert(
    rawAnalyses[0].partOfSpeech !== MorpheusPartOfSpeech.Unknown,
    "raw part of speech must preserve its numeric code",
  );
  assert(
    rawAnalyses[0].truncatedFields === MorpheusTruncatedField.None,
    "raw truncation mask must be preserved",
  );

  const [first, second] = await Promise.all([
    context.analyze("tou=", MorpheusOption.StrictCase),
    context.analyze(
      "anqrwpos",
      MorpheusOption.StrictCase | MorpheusOption.IgnoreAccents,
    ),
  ]);
  assert(first.length === 3, "tou= must retain the CLI fixture count");
  assert(second.length > 0, "breathing fallback must be available through FFI");
  assert(second[0].partOfSpeech === "noun", "part of speech must be readable");
  assert(
    second[0].grammaticalNumber === "singular",
    "grammatical number must be readable",
  );
  assert(
    second[0].genders.includes("masculine"),
    "gender masks must be decoded",
  );
  assert(
    second[0].grammaticalCases.includes("nominative"),
    "case masks must be decoded",
  );
  assert(second[0].stemType.code > 0, "opaque stem type code must be retained");
  assert(second[0].derivationType === null, "absent derivation must be null");

  const scoped = await context.analyze(
    "a)/nqrwpos",
    MorpheusOption.StrictCase |
      MorpheusOption.NoCrasis |
      MorpheusOption.Quick |
      MorpheusOption.HqDictionary |
      MorpheusOption.DialectAttic,
  );
  assert(Array.isArray(scoped), "advanced request options must be accepted");
  const afterScoped = await context.analyze(
    "a)/nqrwpos",
    MorpheusOption.StrictCase,
  );
  assert(afterScoped.length > 0, "request options must not leak to later calls");

  const personAnalyses = await context.analyze(
    "*)arta/chs",
    MorpheusOption.StrictCase,
  );
  const person = personAnalyses.find((analysis) =>
    hasMorpheusMorphFlag(analysis, "person-name")
  );
  assert(person, "Artaches must preserve the person-name stem flag");
});
