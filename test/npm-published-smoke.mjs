// SPDX-License-Identifier: AGPL-3.0-or-later

import {
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusOption,
} from "@libmorpheus/node";

function assert(condition, message) {
  if (!condition) throw new Error(message);
}

const [nativeLibrary, alpheiosRoot, perseidsRoot] = process.argv.slice(2);
assert(nativeLibrary, "native library path is required");
assert(alpheiosRoot, "Alpheios installation path is required");
assert(perseidsRoot, "Perseids installation path is required");

const library = new MorpheusLibrary(nativeLibrary);
const results = {};
try {
  const alpheios = library.createContext(
    alpheiosRoot,
    MorpheusLanguage.Greek,
  );
  try {
    const analyses = await alpheios.analyze(
      "a)/nqrwpos",
      MorpheusOption.StrictCase,
    );
    assert(analyses.length > 0, "Alpheios Greek analysis returned no result");
    assert(
      analyses.some((analysis) => analysis.partOfSpeech === "noun"),
      "Alpheios Greek analysis lost the expected noun interpretation",
    );

    const forms = await alpheios.generate("lo/gos");
    const duals = forms.filter((form) => form.grammaticalNumber === "dual");
    assert(forms.length === 18, "published generation lost its fixture count");
    assert(duals.length === 3, "published generation lost dual forms");
    assert(
      new Set(forms.map((form) => form.surface)).size === 17,
      "published generation lost multiple morphological interpretations",
    );
    results.alpheiosAnalyses = analyses.length;
    results.generatedForms = forms.length;
    results.generatedDuals = duals.length;
  } finally {
    await alpheios.close();
  }

  const perseidsGreek = library.createContext(
    perseidsRoot,
    MorpheusLanguage.Greek,
  );
  try {
    const analyses = await perseidsGreek.analyze("a)/nqrwpos");
    assert(analyses.length > 0, "Perseids Greek analysis returned no result");
    results.perseidsGreekAnalyses = analyses.length;
  } finally {
    await perseidsGreek.close();
  }

  const perseidsLatin = library.createContext(
    perseidsRoot,
    MorpheusLanguage.Latin,
  );
  try {
    const analyses = await perseidsLatin.analyze("est");
    assert(analyses.length >= 2, "Perseids Latin analysis lost interpretations");
    assert(
      analyses.some((analysis) => analysis.lemma === "sum#1"),
      "Perseids Latin analysis lost the expected sum#1 lemma",
    );
    results.perseidsLatinAnalyses = analyses.length;
  } finally {
    await perseidsLatin.close();
  }
} finally {
  library.close();
}

console.log(JSON.stringify(results));
