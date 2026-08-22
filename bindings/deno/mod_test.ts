import {
  hasMorpheusMorphFlag,
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
  MorpheusMorphFlag,
  MorpheusNumber,
  MorpheusPartOfSpeech,
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
  assert(analyses[0].morphFlags.length === 12, "morph flags must be copied");
  assert(
    analyses[0].allMorphFlags.length === 14,
    "complete morph flags must be copied",
  );
  assert(
    typeof hasMorpheusMorphFlag(
      analyses[0],
      MorpheusMorphFlag.GroupName,
    ) === "boolean",
    "named morph flags must be testable",
  );
  assert(
    analyses[0].partOfSpeech !== MorpheusPartOfSpeech.Unknown,
    "part of speech must use a documented code",
  );
  assert(
    analyses[0].truncatedFields === MorpheusTruncatedField.None,
    "fixture fields must not be truncated",
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

  const groupAnalyses = await context.analyze(
    "*dio/skoroi",
    MorpheusOption.StrictCase,
  );
  const group = groupAnalyses.find((analysis) =>
    hasMorpheusMorphFlag(analysis, MorpheusMorphFlag.GroupName)
  );
  assert(group, "Dioscuri must preserve the group-name stem flag");
  assert(
    (group.number & MorpheusNumber.Plural) !== 0,
    "group-name analysis must retain the inferred plural number",
  );
});
