/* SPDX-License-Identifier: MPL-2.0 */

#ifndef MORPHEUS_SETLANG_PROTO_H
#define MORPHEUS_SETLANG_PROTO_H

#include <stddef.h>

void set_lang(int language);
int cur_lang(void);
int morpheus_runtime_string_append(char *destination, const char *source,
                                   size_t capacity);

#endif
