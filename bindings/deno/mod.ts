const ABI_VERSION = 1;
const TEXT_CAPACITY = 64;
const DOMAIN_CAPACITY = 24;
const MORPH_FLAG_CAPACITY = 12;
const ALL_MORPH_FLAG_CAPACITY = 14;

const SYMBOLS = {
  morpheus_abi_version: { parameters: [], result: "u32" },
  morpheus_analysis_size: { parameters: [], result: "usize" },
  morpheus_status_message: { parameters: ["u32"], result: "pointer" },
  morpheus_open_path: {
    parameters: ["u32", "buffer", "usize", "u32", "buffer"],
    result: "u32",
  },
  morpheus_close: { parameters: ["pointer"], result: "void" },
  morpheus_analyze: {
    parameters: ["pointer", "buffer", "usize", "u64", "buffer"],
    result: "u32",
    nonblocking: true,
  },
  morpheus_result_count: { parameters: ["pointer"], result: "usize" },
  morpheus_result_copy: {
    parameters: ["pointer", "usize", "buffer", "usize"],
    result: "u32",
  },
  morpheus_result_truncated_fields: {
    parameters: ["pointer", "usize", "buffer"],
    result: "u32",
  },
  morpheus_result_all_morph_flags: {
    parameters: ["pointer", "usize", "buffer", "usize"],
    result: "u32",
  },
  morpheus_result_free: { parameters: ["pointer"], result: "void" },
} as const;

type NativeLibrary = Deno.DynamicLibrary<typeof SYMBOLS>;

export const MorpheusLanguage = {
  Greek: 0,
  Latin: 1,
  Italian: 2,
} as const;
export type MorpheusLanguage =
  typeof MorpheusLanguage[keyof typeof MorpheusLanguage];

export const MorpheusStatus = {
  Ok: 0,
  InvalidArgument: 1,
  AbiMismatch: 2,
  NoMemory: 3,
  InputTooLong: 4,
  OutOfRange: 5,
  InternalError: 6,
  BufferTooSmall: 7,
  StemlibError: 8,
} as const;
export type MorpheusStatus =
  typeof MorpheusStatus[keyof typeof MorpheusStatus];

export const MorpheusOption = {
  StrictCase: 1n << 0n,
  IgnoreAccents: 1n << 1n,
  VerbsOnly: 1n << 2n,
  NoCrasis: 1n << 3n,
  Quick: 1n << 4n,
  HqDictionary: 1n << 5n,
  DialectAttic: 2n << 16n,
  DialectIonic: 8n << 16n,
  DialectAeolic: 16n << 16n,
  DialectLesbian: 32n << 16n,
  DialectHomeric: 64n << 16n,
  DialectDoric: 128n << 16n,
  DialectParadigm: 256n << 16n,
  DialectNonHomericEpic: 1024n << 16n,
  DialectEpic: 1088n << 16n,
  DialectProse: 2048n << 16n,
} as const;

export const MorpheusMorphFlag = {
  SyllAugment: 1,
  CompOnly: 2,
  Enclitic: 3,
  Iterative: 4,
  SuffixAccent: 5,
  StemAccent: 6,
  Contracted: 7,
  PersonName: 8,
  AntepenultAccent: 9,
  IrregularSuperlative: 10,
  IrregularComparative: 11,
  NoComparison: 12,
  ShortPenult: 13,
  LongPenult: 14,
  RecessiveAccent: 15,
  AccentOptional: 16,
  NeedsAccent: 17,
  RhoEtaIotaAlpha: 18,
  NotInComposition: 19,
  HasPreverb: 20,
  Unaugmented: 21,
  Dissimilation: 22,
  Proclitic: 23,
  Apocope: 24,
  IrregularForm: 25,
  HasAugment: 26,
  QuantityMetathesis: 27,
  NuMovable: 28,
  IntervocalicSToH: 29,
  PreverbAugment: 30,
  Poetic: 31,
  UncontractedStem: 32,
  Metathesis: 33,
  ElidedPreverb: 34,
  IndeclinableForm: 35,
  RootPreverb: 36,
  Diminutive: 37,
  Late: 38,
  Rare: 39,
  RawPreverb: 40,
  Early: 41,
  ShortSubjunctive: 42,
  UnaspiratedPreverb: 43,
  Reduplication: 44,
  UncontractedEnding: 45,
  Derivative: 46,
  AtticReduplication: 47,
  NoReduplication: 48,
  NInfix: 49,
  Syncope: 50,
  Impersonal: 51,
  NeedsRoughBreathing: 52,
  NoCircumflex: 53,
  Causal: 54,
  Intransitive: 55,
  Tmesis: 56,
  RawSonant: 57,
  Prodelision: 58,
  Frequentative: 59,
  Later: 60,
  DoubleAugment: 61,
  DoubleReduplication: 62,
  Desiderative: 63,
  PresentReduplication: 64,
  EndsInDigamma: 65,
  GeographicName: 66,
  DoubledConsonant: 67,
  IotaIntensive: 68,
  LostAccent: 69,
  SigmaToCi: 70,
  ShortEis: 71,
  ProsToPoti: 72,
  MetaToPeda: 73,
  ProsToProti: 74,
  UpoToUpai: 75,
  ParaToParai: 76,
  UperToUpeir: 77,
  EnToEni: 78,
  AlphaPrivative: 79,
  AlphaCopulative: 80,
  MetricallyLong: 81,
  DeltaPreverb: 82,
  TauPreverb: 83,
  GroupName: 110,
} as const;
export type MorpheusMorphFlag =
  typeof MorpheusMorphFlag[keyof typeof MorpheusMorphFlag];

export type MorpheusMorphFlagName =
  | "syllabic-augment" | "compound-only" | "enclitic" | "iterative"
  | "suffix-accent" | "stem-accent" | "contracted" | "person-name"
  | "antepenult-accent" | "irregular-superlative" | "irregular-comparative"
  | "no-comparison" | "short-penult" | "long-penult" | "recessive-accent"
  | "accent-optional" | "needs-accent" | "rho-eta-iota-alpha"
  | "not-in-composition" | "has-preverb" | "unaugmented" | "dissimilation"
  | "proclitic" | "apocope" | "irregular-form" | "has-augment"
  | "quantity-metathesis" | "nu-movable" | "intervocalic-s-to-h"
  | "preverb-augment" | "poetic" | "uncontracted-stem" | "metathesis"
  | "elided-preverb" | "indeclinable-form" | "root-preverb" | "diminutive"
  | "late" | "rare" | "raw-preverb" | "early" | "short-subjunctive"
  | "unaspirated-preverb" | "reduplication" | "uncontracted-ending"
  | "derivative" | "attic-reduplication" | "no-reduplication" | "n-infix"
  | "syncope" | "impersonal" | "needs-rough-breathing" | "no-circumflex"
  | "causal" | "intransitive" | "tmesis" | "raw-sonant" | "prodelision"
  | "frequentative" | "later" | "double-augment" | "double-reduplication"
  | "desiderative" | "present-reduplication" | "ends-in-digamma"
  | "geographic-name" | "doubled-consonant" | "iota-intensive"
  | "lost-accent" | "sigma-to-ci" | "short-eis" | "pros-to-poti"
  | "meta-to-peda" | "pros-to-proti" | "upo-to-upai" | "para-to-parai"
  | "uper-to-upeir" | "en-to-eni" | "alpha-privative" | "alpha-copulative"
  | "metrically-long" | "delta-preverb" | "tau-preverb" | "group-name";

const MORPH_FLAG_NAMES = new Map<number, MorpheusMorphFlagName>([
  [1, "syllabic-augment"], [2, "compound-only"], [3, "enclitic"],
  [4, "iterative"], [5, "suffix-accent"], [6, "stem-accent"],
  [7, "contracted"], [8, "person-name"], [9, "antepenult-accent"],
  [10, "irregular-superlative"], [11, "irregular-comparative"],
  [12, "no-comparison"], [13, "short-penult"], [14, "long-penult"],
  [15, "recessive-accent"], [16, "accent-optional"], [17, "needs-accent"],
  [18, "rho-eta-iota-alpha"], [19, "not-in-composition"], [20, "has-preverb"],
  [21, "unaugmented"], [22, "dissimilation"], [23, "proclitic"],
  [24, "apocope"], [25, "irregular-form"], [26, "has-augment"],
  [27, "quantity-metathesis"], [28, "nu-movable"],
  [29, "intervocalic-s-to-h"], [30, "preverb-augment"], [31, "poetic"],
  [32, "uncontracted-stem"], [33, "metathesis"], [34, "elided-preverb"],
  [35, "indeclinable-form"], [36, "root-preverb"], [37, "diminutive"],
  [38, "late"], [39, "rare"], [40, "raw-preverb"], [41, "early"],
  [42, "short-subjunctive"], [43, "unaspirated-preverb"],
  [44, "reduplication"], [45, "uncontracted-ending"], [46, "derivative"],
  [47, "attic-reduplication"], [48, "no-reduplication"], [49, "n-infix"],
  [50, "syncope"], [51, "impersonal"], [52, "needs-rough-breathing"],
  [53, "no-circumflex"], [54, "causal"], [55, "intransitive"],
  [56, "tmesis"], [57, "raw-sonant"], [58, "prodelision"],
  [59, "frequentative"], [60, "later"], [61, "double-augment"],
  [62, "double-reduplication"], [63, "desiderative"],
  [64, "present-reduplication"], [65, "ends-in-digamma"],
  [66, "geographic-name"], [67, "doubled-consonant"],
  [68, "iota-intensive"], [69, "lost-accent"], [70, "sigma-to-ci"],
  [71, "short-eis"], [72, "pros-to-poti"], [73, "meta-to-peda"],
  [74, "pros-to-proti"], [75, "upo-to-upai"], [76, "para-to-parai"],
  [77, "uper-to-upeir"], [78, "en-to-eni"], [79, "alpha-privative"],
  [80, "alpha-copulative"], [81, "metrically-long"],
  [82, "delta-preverb"], [83, "tau-preverb"], [110, "group-name"],
]);

export function hasMorpheusMorphFlag(
  analysis:
    | Pick<MorpheusRawAnalysis, "allMorphFlags">
    | Pick<MorpheusAnalysis, "morphFlags">,
  flag: number | MorpheusMorphFlagName,
): boolean {
  if (!("allMorphFlags" in analysis)) {
    const name = typeof flag === "number" ? MORPH_FLAG_NAMES.get(flag) : flag;
    return name !== undefined && analysis.morphFlags.includes(name);
  }
  if (typeof flag !== "number") {
    for (const [code, name] of MORPH_FLAG_NAMES) {
      if (name === flag) return hasMorpheusMorphFlag(analysis, code);
    }
    return false;
  }
  if (flag < 1) return false;
  const index = Math.floor((flag - 1) / 8);
  return index < analysis.allMorphFlags.length &&
    (analysis.allMorphFlags[index] & (1 << ((flag - 1) % 8))) !== 0;
}

export const MorpheusPartOfSpeech = {
  Unknown: 0,
  Noun: 1,
  Verb: 2,
  Adjective: 3,
} as const;

export const MorpheusPerson = {
  None: 0,
  First: 1,
  Second: 2,
  Third: 4,
} as const;

export const MorpheusNumber = {
  None: 0,
  Singular: 1,
  Dual: 2,
  Plural: 4,
} as const;

export const MorpheusGender = {
  None: 0,
  Masculine: 1,
  Feminine: 2,
  Neuter: 4,
  Adverbial: 8,
} as const;

export const MorpheusCase = {
  None: 0,
  Nominative: 1,
  Genitive: 2,
  Dative: 4,
  Accusative: 8,
  Vocative: 16,
  Ablative: 32,
} as const;

export const MorpheusTense = {
  None: 0,
  Present: 1,
  Imperfect: 10,
  Future: 3,
  Aorist: 12,
  Perfect: 5,
  Pluperfect: 6,
  FuturePerfect: 15,
  PastAbsolute: 8,
} as const;

export const MorpheusMood = {
  None: 0,
  Indicative: 1,
  Subjunctive: 2,
  Optative: 3,
  Imperative: 4,
  Infinitive: 5,
  Participle: 6,
  Gerundive: 7,
  Supine: 8,
  Conditional: 9,
} as const;

export const MorpheusVoice = {
  None: 0,
  Active: 1,
  Middle: 2,
  Passive: 4,
  MedioPassive: 6,
  Deponent: 3,
} as const;

export const MorpheusDegree = {
  Positive: 0,
  Comparative: 1,
  Superlative: 2,
} as const;

export const MorpheusDialect = {
  All: 0,
  Attic: 2,
  Ionic: 8,
  Aeolic: 16,
  Lesbian: 32,
  Homeric: 64,
  Doric: 128,
  Paradigm: 256,
  NonHomericEpic: 1024,
  Epic: 1088,
  Prose: 2048,
} as const;

export const MorpheusGeographicRegion = {
  None: 0,
  Phocis: 1,
  Locris: 2,
  Elis: 4,
  Laconia: 16,
  Heraclea: 32,
  Megarid: 64,
  Argolid: 128,
  Rhodes: 256,
  Cos: 512,
  Thera: 1024,
  Cyrene: 2048,
  Crete: 4096,
  Arcadia: 8192,
  Cyprus: 16384,
  Boeotia: 32768,
} as const;

export const MorpheusTruncatedField = {
  None: 0,
  Raw: 1 << 0,
  Workword: 1 << 1,
  Lemma: 1 << 2,
  Preverb: 1 << 3,
  Augment: 1 << 4,
  Stem: 1 << 5,
  Suffix: 1 << 6,
  Ending: 1 << 7,
  Crasis: 1 << 8,
  DictionaryForm: 1 << 9,
  EnglishForm: 1 << 10,
  RawPreverb: 1 << 11,
  Domains: 1 << 12,
} as const;

export interface MorpheusRawAnalysis {
  readonly structSize: number;
  readonly partOfSpeech: number;
  readonly stemType: number;
  readonly derivationType: number;
  readonly dialect: number;
  readonly geographicRegion: number;
  readonly person: number;
  readonly number: number;
  readonly gender: number;
  readonly grammaticalCase: number;
  readonly tense: number;
  readonly mood: number;
  readonly voice: number;
  readonly degree: number;
  readonly raw: string;
  readonly workword: string;
  readonly lemma: string;
  readonly preverb: string;
  readonly augment: string;
  readonly stem: string;
  readonly suffix: string;
  readonly ending: string;
  readonly crasis: string;
  readonly dictionaryForm: string;
  readonly englishForm: string;
  readonly rawPreverb: string;
  readonly domains: string;
  readonly morphFlags: Uint8Array;
  readonly allMorphFlags: Uint8Array;
  readonly truncatedFields: number;
}

export type MorpheusPartOfSpeechName = "unknown" | "noun" | "verb" | "adjective";
export type MorpheusPersonName = "first" | "second" | "third";
export type MorpheusNumberName = "singular" | "dual" | "plural";
export type MorpheusGenderName = "masculine" | "feminine" | "neuter" | "adverbial";
export type MorpheusCaseName =
  | "nominative" | "genitive" | "dative" | "accusative" | "vocative" | "ablative";
export type MorpheusTenseName =
  | "present" | "imperfect" | "future" | "aorist" | "perfect"
  | "pluperfect" | "future-perfect" | "past-absolute";
export type MorpheusMoodName =
  | "indicative" | "subjunctive" | "optative" | "imperative" | "infinitive"
  | "participle" | "gerundive" | "supine" | "conditional";
export type MorpheusVoiceName =
  | "active" | "middle" | "passive" | "medio-passive" | "deponent";
export type MorpheusDegreeName = "positive" | "comparative" | "superlative";
export type MorpheusDialectName =
  | "attic" | "ionic" | "aeolic" | "lesbian" | "homeric" | "doric"
  | "paradigm" | "non-homeric-epic" | "epic" | "prose";
export type MorpheusGeographicRegionName =
  | "phocis" | "locris" | "elis" | "laconia" | "heraclea" | "megarid"
  | "argolid" | "rhodes" | "cos" | "thera" | "cyrene" | "crete"
  | "arcadia" | "cyprus" | "boeotia";
export type MorpheusTruncatedFieldName =
  | "raw" | "workword" | "lemma" | "preverb" | "augment" | "stem"
  | "suffix" | "ending" | "crasis" | "dictionaryForm" | "englishForm"
  | "rawPreverb" | "domains";

/** A stemlib-specific identifier whose meaning is intentionally opaque in ABI 1. */
export interface MorpheusOpaqueCode {
  readonly code: number;
}

export interface MorpheusAnalysis {
  readonly partOfSpeech: MorpheusPartOfSpeechName;
  readonly stemType: MorpheusOpaqueCode;
  readonly derivationType: MorpheusOpaqueCode | null;
  readonly dialects: readonly MorpheusDialectName[];
  readonly geographicRegions: readonly MorpheusGeographicRegionName[];
  readonly person: MorpheusPersonName | null;
  readonly grammaticalNumber: MorpheusNumberName | null;
  readonly genders: readonly MorpheusGenderName[];
  readonly grammaticalCases: readonly MorpheusCaseName[];
  readonly tense: MorpheusTenseName | null;
  readonly mood: MorpheusMoodName | null;
  readonly voices: readonly MorpheusVoiceName[];
  readonly degree: MorpheusDegreeName;
  readonly raw: string;
  readonly workword: string;
  readonly lemma: string;
  readonly preverb: string;
  readonly augment: string;
  readonly stem: string;
  readonly suffix: string;
  readonly ending: string;
  readonly crasis: string;
  readonly dictionaryForm: string;
  readonly englishForm: string;
  readonly rawPreverb: string;
  readonly domains: string;
  readonly morphFlags: readonly MorpheusMorphFlagName[];
  readonly truncatedFields: readonly MorpheusTruncatedFieldName[];
}

export class MorpheusError extends Error {
  constructor(readonly status: number, message: string) {
    super(message);
    this.name = "MorpheusError";
  }
}

const encoder = new TextEncoder();
const decoder = new TextDecoder();
const littleEndian = new Uint8Array(new Uint16Array([1]).buffer)[0] === 1;

function pointerFromSlot(slot: BigUint64Array): Deno.PointerObject {
  const pointer = Deno.UnsafePointer.create(slot[0]);
  if (pointer === null) throw new Error("Morpheus returned a null pointer");
  return pointer;
}

function cString(bytes: Uint8Array, offset: number, capacity: number): string {
  const field = bytes.subarray(offset, offset + capacity);
  const terminator = field.indexOf(0);
  return decoder.decode(terminator < 0 ? field : field.subarray(0, terminator));
}

function decodeAnalysis(
  bytes: Uint8Array,
  truncatedFields: number,
  allMorphFlags: Uint8Array,
): MorpheusRawAnalysis {
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const numbers: number[] = [];
  for (let offset = 0; offset < 56; offset += 4) {
    numbers.push(view.getUint32(offset, littleEndian));
  }
  let offset = 56;
  const text = () => {
    const value = cString(bytes, offset, TEXT_CAPACITY);
    offset += TEXT_CAPACITY;
    return value;
  };
  const raw = text();
  const workword = text();
  const lemma = text();
  const preverb = text();
  const augment = text();
  const stem = text();
  const suffix = text();
  const ending = text();
  const crasis = text();
  const dictionaryForm = text();
  const englishForm = text();
  const rawPreverb = text();
  const domains = cString(bytes, offset, DOMAIN_CAPACITY);
  offset += DOMAIN_CAPACITY;
  const morphFlags = bytes.slice(offset, offset + MORPH_FLAG_CAPACITY);

  return {
    structSize: numbers[0],
    partOfSpeech: numbers[1],
    stemType: numbers[2],
    derivationType: numbers[3],
    dialect: numbers[4],
    geographicRegion: numbers[5],
    person: numbers[6],
    number: numbers[7],
    gender: numbers[8],
    grammaticalCase: numbers[9],
    tense: numbers[10],
    mood: numbers[11],
    voice: numbers[12],
    degree: numbers[13],
    raw,
    workword,
    lemma,
    preverb,
    augment,
    stem,
    suffix,
    ending,
    crasis,
    dictionaryForm,
    englishForm,
    rawPreverb,
    domains,
    morphFlags,
    allMorphFlags,
    truncatedFields,
  };
}

function exactName<T extends string>(
  value: number,
  entries: readonly (readonly [number, T])[],
): T | null {
  return entries.find(([code]) => code === value)?.[1] ?? null;
}

function maskNames<T extends string>(
  value: number,
  entries: readonly (readonly [number, T])[],
  exactEntries: readonly (readonly [number, T])[] = [],
): readonly T[] {
  const exact = exactName(value, exactEntries);
  if (exact !== null) return [exact];
  return entries.filter(([bit]) => (value & bit) === bit).map(([, name]) => name);
}

const PART_OF_SPEECH_NAMES = [[0, "unknown"], [1, "noun"], [2, "verb"], [3, "adjective"]] as const;
const PERSON_NAMES = [[1, "first"], [2, "second"], [4, "third"]] as const;
const NUMBER_NAMES = [[1, "singular"], [2, "dual"], [4, "plural"]] as const;
const GENDER_NAMES = [[1, "masculine"], [2, "feminine"], [4, "neuter"], [8, "adverbial"]] as const;
const CASE_NAMES = [[1, "nominative"], [2, "genitive"], [4, "dative"], [8, "accusative"], [16, "vocative"], [32, "ablative"]] as const;
const TENSE_NAMES = [[1, "present"], [10, "imperfect"], [3, "future"], [12, "aorist"], [5, "perfect"], [6, "pluperfect"], [15, "future-perfect"], [8, "past-absolute"]] as const;
const MOOD_NAMES = [[1, "indicative"], [2, "subjunctive"], [3, "optative"], [4, "imperative"], [5, "infinitive"], [6, "participle"], [7, "gerundive"], [8, "supine"], [9, "conditional"]] as const;
const VOICE_NAMES = [[1, "active"], [2, "middle"], [4, "passive"]] as const;
const VOICE_EXACT_NAMES = [[6, "medio-passive"], [3, "deponent"]] as const;
const DEGREE_NAMES = [[0, "positive"], [1, "comparative"], [2, "superlative"]] as const;
const DIALECT_NAMES = [[2, "attic"], [8, "ionic"], [16, "aeolic"], [32, "lesbian"], [64, "homeric"], [128, "doric"], [256, "paradigm"], [1024, "non-homeric-epic"], [2048, "prose"]] as const;
const DIALECT_EXACT_NAMES = [[1088, "epic"]] as const;
const REGION_NAMES = [[1, "phocis"], [2, "locris"], [4, "elis"], [16, "laconia"], [32, "heraclea"], [64, "megarid"], [128, "argolid"], [256, "rhodes"], [512, "cos"], [1024, "thera"], [2048, "cyrene"], [4096, "crete"], [8192, "arcadia"], [16384, "cyprus"], [32768, "boeotia"]] as const;
const TRUNCATED_FIELD_NAMES = [[1 << 0, "raw"], [1 << 1, "workword"], [1 << 2, "lemma"], [1 << 3, "preverb"], [1 << 4, "augment"], [1 << 5, "stem"], [1 << 6, "suffix"], [1 << 7, "ending"], [1 << 8, "crasis"], [1 << 9, "dictionaryForm"], [1 << 10, "englishForm"], [1 << 11, "rawPreverb"], [1 << 12, "domains"]] as const;

function semanticAnalysis(raw: MorpheusRawAnalysis): MorpheusAnalysis {
  const morphFlags: MorpheusMorphFlagName[] = [];
  for (const [code, name] of MORPH_FLAG_NAMES) {
    if (hasMorpheusMorphFlag(raw, code)) morphFlags.push(name);
  }
  return {
    partOfSpeech: exactName(raw.partOfSpeech, PART_OF_SPEECH_NAMES) ?? "unknown",
    stemType: { code: raw.stemType },
    derivationType: raw.derivationType === 0 ? null : { code: raw.derivationType },
    dialects: maskNames<MorpheusDialectName>(
      raw.dialect,
      DIALECT_NAMES,
      DIALECT_EXACT_NAMES,
    ),
    geographicRegions: maskNames(raw.geographicRegion, REGION_NAMES),
    person: exactName(raw.person, PERSON_NAMES),
    grammaticalNumber: exactName(raw.number, NUMBER_NAMES),
    genders: maskNames(raw.gender, GENDER_NAMES),
    grammaticalCases: maskNames(raw.grammaticalCase, CASE_NAMES),
    tense: exactName(raw.tense, TENSE_NAMES),
    mood: exactName(raw.mood, MOOD_NAMES),
    voices: maskNames<MorpheusVoiceName>(
      raw.voice,
      VOICE_NAMES,
      VOICE_EXACT_NAMES,
    ),
    degree: exactName(raw.degree, DEGREE_NAMES) ?? "positive",
    raw: raw.raw, workword: raw.workword, lemma: raw.lemma,
    preverb: raw.preverb, augment: raw.augment, stem: raw.stem,
    suffix: raw.suffix, ending: raw.ending, crasis: raw.crasis,
    dictionaryForm: raw.dictionaryForm, englishForm: raw.englishForm,
    rawPreverb: raw.rawPreverb, domains: raw.domains,
    morphFlags,
    truncatedFields: maskNames(raw.truncatedFields, TRUNCATED_FIELD_NAMES),
  };
}

export class MorpheusLibrary {
  readonly #native: NativeLibrary;
  readonly #analysisSize: number;
  #contexts = 0;
  #closed = false;

  constructor(path: string | URL) {
    if (!["x86_64", "aarch64"].includes(Deno.build.arch)) {
      throw new Error(`Unsupported pointer width for ${Deno.build.arch}`);
    }
    this.#native = Deno.dlopen(path, SYMBOLS);
    if (this.#native.symbols.morpheus_abi_version() !== ABI_VERSION) {
      this.#native.close();
      throw new Error("Unsupported libmorpheus ABI version");
    }
    this.#analysisSize =
      Number(this.#native.symbols.morpheus_analysis_size());
    if (this.#analysisSize < 860) {
      this.#native.close();
      throw new Error("libmorpheus analysis record is smaller than ABI version 1");
    }
  }

  createContext(stemlibPath: string, language: MorpheusLanguage) {
    if (this.#closed) throw new Error("Morpheus library is closed");
    const path = encoder.encode(stemlibPath);
    const output = new BigUint64Array(1);
    const status = this.#native.symbols.morpheus_open_path(
      ABI_VERSION,
      path,
      BigInt(path.byteLength),
      language,
      output,
    );
    this.#throwOnError(status);
    this.#contexts++;
    return new MorpheusContext(
      this.#native,
      pointerFromSlot(output),
      this.#analysisSize,
      () => this.#contexts--,
    );
  }

  close(): void {
    if (this.#closed) return;
    if (this.#contexts) {
      throw new Error("Close all Morpheus contexts before the library");
    }
    this.#native.close();
    this.#closed = true;
  }

  [Symbol.dispose](): void {
    this.close();
  }

  #throwOnError(status: number): void {
    if (status === 0) return;
    const pointer = this.#native.symbols.morpheus_status_message(status);
    const message = pointer === null
      ? `Morpheus status ${status}`
      : new Deno.UnsafePointerView(pointer).getCString();
    throw new MorpheusError(status, message);
  }
}

export class MorpheusContext {
  #tail: Promise<void> = Promise.resolve();
  #closed = false;

  constructor(
    private readonly native: NativeLibrary,
    private readonly pointer: Deno.PointerObject,
    private readonly analysisSize: number,
    private readonly onClose: () => void,
  ) {}

  analyze(
    betaCode: string,
    options: bigint = 0n,
  ): Promise<readonly MorpheusAnalysis[]> {
    return this.analyzeRaw(betaCode, options).then((analyses) =>
      analyses.map(semanticAnalysis)
    );
  }

  analyzeRaw(
    betaCode: string,
    options: bigint = 0n,
  ): Promise<readonly MorpheusRawAnalysis[]> {
    if (this.#closed) return Promise.reject(new Error("Morpheus context is closed"));
    const run = this.#tail.then(() => this.#analyze(betaCode, options));
    this.#tail = run.then(() => undefined, () => undefined);
    return run;
  }

  async close(): Promise<void> {
    if (this.#closed) return;
    this.#closed = true;
    await this.#tail;
    this.native.symbols.morpheus_close(this.pointer);
    this.onClose();
  }

  async [Symbol.asyncDispose](): Promise<void> {
    await this.close();
  }

  async #analyze(
    betaCode: string,
    options: bigint,
  ): Promise<readonly MorpheusRawAnalysis[]> {
    const input = encoder.encode(betaCode);
    const output = new BigUint64Array(1);
    const status = await this.native.symbols.morpheus_analyze(
      this.pointer,
      input,
      BigInt(input.byteLength),
      options,
      output,
    );
    this.#throwOnError(status);
    const result = pointerFromSlot(output);
    try {
      const count = Number(this.native.symbols.morpheus_result_count(result));
      const analyses: MorpheusRawAnalysis[] = [];
      for (let index = 0; index < count; index++) {
        const bytes = new Uint8Array(this.analysisSize);
        this.#throwOnError(this.native.symbols.morpheus_result_copy(
          result,
          BigInt(index),
          bytes,
          BigInt(bytes.byteLength),
        ));
        const truncatedFields = new Uint32Array(1);
        this.#throwOnError(this.native.symbols.morpheus_result_truncated_fields(
          result,
          BigInt(index),
          truncatedFields,
        ));
        const allMorphFlags = new Uint8Array(ALL_MORPH_FLAG_CAPACITY);
        this.#throwOnError(this.native.symbols.morpheus_result_all_morph_flags(
          result,
          BigInt(index),
          allMorphFlags,
          BigInt(allMorphFlags.byteLength),
        ));
        analyses.push(decodeAnalysis(
          bytes,
          truncatedFields[0],
          allMorphFlags,
        ));
      }
      return analyses;
    } finally {
      this.native.symbols.morpheus_result_free(result);
    }
  }

  #throwOnError(status: number): void {
    if (status === 0) return;
    const pointer = this.native.symbols.morpheus_status_message(status);
    const message = pointer === null
      ? `Morpheus status ${status}`
      : new Deno.UnsafePointerView(pointer).getCString();
    throw new MorpheusError(status, message);
  }
}
