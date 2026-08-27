<!-- SPDX-License-Identifier: AGPL-3.0-or-later -->

# Morpheus stem libraries

This short inventory identifies the main stem libraries currently available
online and where `libmorpheus` makes them available locally. It records dataset
scope, not compatibility or linguistic quality; a detailed audit is deferred.

| Source | Languages | Location in this repository | Notes |
| --- | --- | --- | --- |
| [PerseusDL/morpheus](https://github.com/PerseusDL/morpheus) | Ancient Greek, Latin, Italian | Not vendored | Historical upstream dataset; consult the origin repository directly. |
| [alpheios-project/morpheus](https://github.com/alpheios-project/morpheus) | Ancient Greek | [`vendor/alpheios-morpheus`](https://github.com/defense-humanites/libmorpheus/tree/main/vendor/alpheios-morpheus) | Pinned submodule used as the Greek-only reference dataset and by the default container images. |
| [perseids-tools/morpheus](https://github.com/perseids-tools/morpheus) | Ancient Greek, Latin | [`stemlib/`](https://github.com/defense-humanites/libmorpheus/tree/main/stemlib) | Bundled baseline used by the Greek and Latin legacy fixture suite. |

Generation and analysis do not have identical data requirements. Analysis can
read a compiled stemlib directly. The current generation implementation is
Greek-only and additionally reads the validated `gener.index` built at the
stemlib root.
