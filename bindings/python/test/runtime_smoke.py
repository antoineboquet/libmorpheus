# SPDX-License-Identifier: AGPL-3.0-or-later
"""Exercise the Python package against a compiled runtime and real stem data."""

from __future__ import annotations

import json
from pathlib import Path
import sys

from libmorpheus import Language, Library


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("usage: runtime_smoke.py LIBRARY STEMLIB")
    native_library = Path(sys.argv[1]).resolve()
    stemlib = Path(sys.argv[2]).resolve()
    results: dict[str, int] = {}

    with Library(native_library) as library:
        with library.context(stemlib, Language.GREEK) as greek:
            analyses = greek.analyze("a)/nqrwpos")
            assert analyses, "Greek analysis returned no result"
            assert any(item.part_of_speech == "noun" for item in analyses)

            forms = greek.generate("lo/gos")
            duals = [
                form for form in forms
                if form.grammatical_number == "dual"
            ]
            assert len(forms) == 18, "generation lost its fixture count"
            assert len(duals) == 3, "generation lost dual forms"
            assert len({form.surface for form in forms}) == 17, (
                "generation lost multiple morphological interpretations"
            )
            results["greek_analyses"] = len(analyses)
            results["generated_forms"] = len(forms)
            results["generated_duals"] = len(duals)

        with library.context(stemlib, Language.LATIN) as latin:
            analyses = latin.analyze("est")
            assert len(analyses) >= 2, "Latin analysis lost interpretations"
            assert any(item.lemma == "sum#1" for item in analyses), (
                "Latin analysis lost the expected sum#1 lemma"
            )
            results["latin_analyses"] = len(analyses)

    print(json.dumps(results, sort_keys=True))


if __name__ == "__main__":
    main()
