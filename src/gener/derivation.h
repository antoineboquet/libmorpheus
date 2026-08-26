/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_GENER_DERIVATION_H
#define MORPHEUS_GENER_DERIVATION_H

#include <stdio.h>

/*
 * Expand one historical principal-part request into one explicit generation
 * record. The rule tables are resolved through the active MORPHLIB context.
 *
 * Returns 1 when a rule produced a record, 0 when no rule matched, and -1 on
 * malformed input, I/O failure, or allocation failure.
 */
int morpheus_gener_expand_derivation(FILE *output, const char *stem,
                                     const char *derivation,
                                     const char *global_keys,
                                     const char *request_keys);

#endif
