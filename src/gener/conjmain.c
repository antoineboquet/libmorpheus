/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "conjugation_internal.h"
#include "../morphlib/setlang.proto.h"

static const char *owned_output, *owned_odd;
static int completed;
static void cleanup(void)
{
    if (!completed) {
        if (owned_output) remove(owned_output);
        if (owned_odd) remove(owned_odd);
    }
}

int main(int argc, char **argv)
{
    int option, full = 0, ok;
    FILE *input, *output;
    while ((option = getopt(argc, argv, "ILf")) != -1) {
        switch (option) {
        case 'I': set_lang(ITALIAN); break;
        case 'L': set_lang(LATIN); break;
        case 'f': full = 1; break;
        default: return EXIT_FAILURE;
        }
    }
    if (argc - optind != 3 || !strcmp(argv[optind], argv[optind + 1])) {
        fprintf(stderr, "usage: %s [-I|-L] [-f] INPUT OUTPUT ODD_OUTPUT\n", argv[0]);
        return EXIT_FAILURE;
    }
    input = fopen(argv[optind], "r");
    if (!input) return EXIT_FAILURE;
    output = fopen(argv[optind + 1], "wx");
    if (!output) { fclose(input); return EXIT_FAILURE; }
    owned_output = argv[optind + 1];
    atexit(cleanup);
    morpheus_conj_odd_output = fopen(argv[optind + 2], "wx");
    if (!morpheus_conj_odd_output) conj_fail("cannot create odd-key output");
    owned_odd = argv[optind + 2];
    ok = GenConjForms(input, output, full);
    if (fclose(morpheus_conj_odd_output) != 0) ok = 0;
    if (fclose(input) != 0) ok = 0;
    if (fclose(output) != 0) ok = 0;
    completed = ok;
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}

#include <stdarg.h>
_Noreturn void conj_fail(const char *message)
{
    fprintf(stderr, "do_conj: %s\n", message);
    exit(EXIT_FAILURE);
}
void conj_copy(char *destination, const char *source, size_t capacity)
{
    if (!Xstrncpy(destination, source, capacity)) conj_fail("oversized field");
}
void conj_append(char *destination, const char *source, size_t capacity)
{
    if (!Xstrncat(destination, source, capacity)) conj_fail("oversized concatenation");
}
void conj_format(char *destination, size_t capacity, const char *format, ...)
{
    va_list arguments;
    int length;
    va_start(arguments, format);
    length = vsnprintf(destination, capacity, format, arguments);
    va_end(arguments);
    if (length < 0 || (size_t)length >= capacity) conj_fail("oversized formatted field");
}
void conj_key(char *input, char *output, size_t capacity)
{
    char *start = input, *end;
    size_t length;
    while (isspace((unsigned char)*start)) start++;
    end = start;
    while (*end && !isspace((unsigned char)*end)) end++;
    length = (size_t)(end - start);
    if (length >= capacity) conj_fail("oversized key");
    memcpy(output, start, length);
    output[length] = 0;
    while (isspace((unsigned char)*end)) end++;
    memmove(input, end, strlen(end) + 1);
}
