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

function decodeAnalysis(bytes: Uint8Array): MorpheusAnalysis {
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
        analyses.push(decodeAnalysis(bytes));
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
