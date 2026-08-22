import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
  MorpheusPartOfSpeech,
  MorpheusTruncatedField,
} from "./mod.ts";

function assert(condition: unknown, message: string): asserts condition {
  if (!condition) throw new Error(message);
}

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
});
