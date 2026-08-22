const ABI_VERSION = 1;
const TEXT_CAPACITY = 64;
const DOMAIN_CAPACITY = 24;
const MORPH_FLAG_CAPACITY = 12;

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

export interface MorpheusAnalysis {
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
  readonly truncatedFields: number;
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
): MorpheusAnalysis {
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
    truncatedFields,
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
  ): Promise<readonly MorpheusAnalysis[]> {
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
      const analyses: MorpheusAnalysis[] = [];
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
        analyses.push(decodeAnalysis(bytes, truncatedFields[0]));
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
