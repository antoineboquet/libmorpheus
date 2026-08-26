// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gkstring.h>
#include <modes.h>

#include "../src/gener/gener_internal.h"
#include "../src/morphlib/runtime_context.h"

typedef struct {
	char *name;
	char *kind;
	char *lemma;
	char *stem;
	char *keys;
	int irregular;
	int mode;
	size_t expected_count;
	size_t expected_surfaces;
	size_t expected_nondual_count;
	size_t expected_nondual_surfaces;
	uint64_t expected_fingerprint;
} gener_fixture;

static uint64_t
hash_byte(uint64_t hash, unsigned char byte)
{
	return (hash ^ byte) * UINT64_C(1099511628211);
}

static uint64_t
hash_u32(uint64_t hash, uint32_t value)
{
	int shift;

	for (shift = 0; shift != 32; shift += 8)
		hash = hash_byte(hash,(unsigned char)(value >> shift));
	return hash;
}

static uint64_t
hash_string(uint64_t hash, const char *value)
{
	const unsigned char *cursor = (const unsigned char *)value;

	while (*cursor)
		hash = hash_byte(hash,*cursor++);
	return hash_byte(hash,0);
}

static uint64_t
hash_form(uint64_t hash, const gk_word *form)
{
	word_form info = forminfo_of(form);
	int flag;

	hash = hash_string(hash,workword_of(form));
	hash = hash_string(hash,lemma_of(form));
	hash = hash_string(hash,preverb_of(form));
	hash = hash_string(hash,aug1_of(form));
	hash = hash_string(hash,stem_of(form));
	hash = hash_string(hash,suffix_of(form));
	hash = hash_string(hash,endstring_of(form));
	hash = hash_u32(hash,voice_of(info));
	hash = hash_u32(hash,mood_of(info));
	hash = hash_u32(hash,tense_of(info));
	hash = hash_u32(hash,person_of(info));
	hash = hash_u32(hash,number_of(info));
	hash = hash_u32(hash,case_of(info));
	hash = hash_u32(hash,degree_of(info));
	hash = hash_u32(hash,gender_of(info));
	hash = hash_u32(hash,stemtype_of(form));
	hash = hash_u32(hash,derivtype_of(form));
	hash = hash_u32(hash,(uint32_t)(unsigned short)dialect_of(form));
	hash = hash_u32(hash,(uint32_t)geogregion_of(form));
	for (flag = 0; flag != MORPHFLAG_STORAGE_BYTES; flag++)
		hash = hash_byte(hash,morphflags_of(form)[flag]);
	return hash;
}

static size_t
count_unique_surfaces(const gk_word *forms, size_t count, int omit_dual)
{
	size_t current;
	size_t previous;
	size_t unique = 0;

	for (current = 0; current != count; current++) {
		if (omit_dual &&
		    number_of(forminfo_of(forms + current)) == DUAL)
			continue;
		for (previous = 0; previous != current; previous++) {
			if (omit_dual &&
			    number_of(forminfo_of(forms + previous)) == DUAL)
				continue;
			if (!strcmp(workword_of(forms + current),
			            workword_of(forms + previous)))
				break;
		}
		if (previous == current)
			unique++;
	}
	return unique;
}

static void
run_fixture(const gener_fixture *fixture)
{
	gk_word input = { 0 };
	gk_word *forms;
	uint64_t fingerprint = UINT64_C(14695981039346656037);
	size_t count;
	size_t nondual_count = 0;
	size_t unique;
	size_t nondual_unique;

	set_lemma(&input,fixture->lemma);
	set_stem(&input,fixture->stem);
	forms = fixture->irregular
	        ? GenIrregForm(&input,(char *)fixture->keys,fixture->mode)
	        : GenStemForms(&input,(char *)fixture->keys,fixture->mode);
	assert(forms);
	for (count = 0; workword_of(forms + count)[0]; count++)
		fingerprint = hash_form(fingerprint,forms + count);
	unique = count_unique_surfaces(forms,count,0);
	for (size_t form = 0; form != count; form++) {
		if (number_of(forminfo_of(forms + form)) != DUAL)
			nondual_count++;
	}
	nondual_unique = count_unique_surfaces(forms,count,1);
	if (count != fixture->expected_count ||
	    unique != fixture->expected_surfaces ||
	    nondual_count != fixture->expected_nondual_count ||
	    nondual_unique != fixture->expected_nondual_surfaces ||
	    fingerprint != fixture->expected_fingerprint) {
		fprintf(stderr,
		        "%s: got %zu/%zu/%zu/%zu/%016" PRIx64 "\n",
		        fixture->name,count,unique,nondual_count,nondual_unique,
		        fingerprint);
	}
	assert(count == fixture->expected_count);
	assert(unique == fixture->expected_surfaces);
	assert(nondual_count == fixture->expected_nondual_count);
	assert(nondual_unique == fixture->expected_nondual_surfaces);
	assert(fingerprint == fixture->expected_fingerprint);
	oddkeys_of(forms) = NULL;
	FreeGkword(forms);
	free(oddkeys_of(&input));
}

static int
split_fields(char *line, char **fields, int capacity)
{
	int count = 0;
	char *cursor = line;

	while (count != capacity) {
		char *tab;

		fields[count++] = cursor;
		tab = strchr(cursor,'\t');
		if (!tab)
			break;
		*tab = 0;
		cursor = tab + 1;
	}
	return count;
}

static size_t
parse_size(const char *value)
{
	char *end;
	uintmax_t parsed = strtoumax(value,&end,10);

	assert(*value && !*end && parsed <= SIZE_MAX);
	return (size_t)parsed;
}

static void
parse_fixture(char *line, gener_fixture *fixture)
{
	char *fields[10];
	char *end;
	uintmax_t fingerprint;

	assert(split_fields(line,fields,10) == 10);
	fixture->name = fields[0];
	fixture->kind = fields[1];
	fixture->lemma = fields[2];
	fixture->stem = fields[3];
	fixture->keys = fields[4];
	fixture->irregular = strcmp(fields[1],"regular") != 0;
	fixture->mode = !strcmp(fields[1],"indeclinable") ? INDECL : 0;
	assert(!strcmp(fields[1],"regular") ||
	       !strcmp(fields[1],"irregular") ||
	       !strcmp(fields[1],"indeclinable"));
	fixture->expected_count = parse_size(fields[5]);
	fixture->expected_surfaces = parse_size(fields[6]);
	fixture->expected_nondual_count = parse_size(fields[7]);
	fixture->expected_nondual_surfaces = parse_size(fields[8]);
	fingerprint = strtoumax(fields[9],&end,16);
	assert(*fields[9] && !*end && fingerprint <= UINT64_MAX);
	fixture->expected_fingerprint = (uint64_t)fingerprint;
}

int
main(int argc, char **argv)
{
	morpheus_runtime_context *context = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	FILE *input;
	char line[4096];
	size_t fixture_count = 0;

	assert(argc == 2);
	input = fopen(argv[1],"r");
	assert(input);
	assert(context);
	previous = morpheus_runtime_context_activate(context);
	while (fgets(line,sizeof line,input)) {
		gener_fixture fixture = { 0 };
		size_t length = strlen(line);

		assert(length && line[length - 1] == '\n');
		line[length - 1] = 0;
		if (!line[0] || line[0] == '#')
			continue;
		parse_fixture(line,&fixture);
		run_fixture(&fixture);
		fixture_count++;
	}
	assert(!ferror(input));
	assert(fixture_count == 5);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_NONE);
	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(context);
	assert(!fclose(input));
	return 0;
}
