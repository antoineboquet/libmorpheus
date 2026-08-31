// SPDX-License-Identifier: AGPL-3.0-or-later

import assert from "node:assert/strict";
import test from "node:test";

import {
  MorpheusDialect,
  MorpheusLanguage,
  MorpheusLibrary,
  MorpheusNumber,
  MorpheusOption,
} from "../index.js";

const nativeLibrary = process.env.MORPHEUS_LIBRARY;
const stemlib = process.env.MORPHEUS_STEMLIB;

test("analyzes Greek and preserves multiple semantic records", async () => {
  assert.ok(nativeLibrary);
  assert.ok(stemlib);
  const library = new MorpheusLibrary(nativeLibrary);
  const context = library.createContext(stemlib, MorpheusLanguage.Greek);
  try {
    const analyses = await context.analyze(
      "bi/ou",
      MorpheusOption.StrictCase,
    );
    assert.equal(analyses.length, 4);
    assert.ok(analyses.every((analysis) => analysis.lemma.length > 0));
    assert.ok(analyses.some((analysis) => analysis.partOfSpeech === "noun"));
  } finally {
    await context.close();
    library.close();
  }
});

test("serializes calls and preserves experimental dual generation", async () => {
  const library = new MorpheusLibrary(nativeLibrary);
  const context = library.createContext(stemlib, MorpheusLanguage.Greek);
  try {
    const [analysis, generations] = await Promise.all([
      context.analyze("du/o", MorpheusOption.StrictCase),
      context.generate("lo/gos"),
    ]);
    assert.ok(analysis.length > 0);
    assert.equal(generations.length, 18);
    assert.equal(
      generations.filter((item) => item.grammaticalNumber === "dual").length,
      3,
    );
    assert.equal(
      (await context.generate("lo/gos", {
        number: MorpheusNumber.Dual,
      })).length,
      3,
    );
    assert.equal(
      (await context.generate("multiple", {
        dialect: MorpheusDialect.Attic,
        resultLimit: 1,
      }))[0].surface,
      "prw=ton",
    );
  } finally {
    await context.close();
    library.close();
  }
});

test("analyzes Latin without collapsing interpretations", async () => {
  const library = new MorpheusLibrary(nativeLibrary);
  const context = library.createContext(stemlib, MorpheusLanguage.Latin);
  try {
    const analyses = await context.analyze("est");
    assert.ok(analyses.length >= 2);
    assert.ok(analyses.some((analysis) => analysis.lemma === "sum#1"));
  } finally {
    await context.close();
    library.close();
  }
});
