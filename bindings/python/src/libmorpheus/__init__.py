# SPDX-License-Identifier: AGPL-3.0-or-later
"""Typed Python binding for the libmorpheus native runtime."""

from ._library import Context, Library, MorpheusError
from ._types import (
    Analysis,
    Case,
    Degree,
    Dialect,
    Gender,
    Generation,
    GenerationOptions,
    GeographicRegion,
    Language,
    Mood,
    Number,
    Option,
    PartOfSpeech,
    Person,
    RawAnalysis,
    RawGeneration,
    Status,
    Tense,
    TruncatedField,
    Voice,
    has_morph_flag,
)

__version__ = "0.1.0"

__all__ = [
    "Analysis",
    "Case",
    "Context",
    "Degree",
    "Dialect",
    "Gender",
    "Generation",
    "GenerationOptions",
    "GeographicRegion",
    "Language",
    "Library",
    "Mood",
    "MorpheusError",
    "Number",
    "Option",
    "PartOfSpeech",
    "Person",
    "RawAnalysis",
    "RawGeneration",
    "Status",
    "Tense",
    "TruncatedField",
    "Voice",
    "__version__",
    "has_morph_flag",
]
