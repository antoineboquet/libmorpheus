// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_SIZE 64u
#define LEMMA_ENTRY_SIZE 24u
#define BLOCK_ENTRY_SIZE 16u
#define RECORD_ENTRY_SIZE 24u

static uint32_t
get_u32(const unsigned char *input)
{
	uint32_t value = 0;
	int byte;

	for (byte = 3; byte >= 0; byte--)
		value = (value << 8) | input[byte];
	return value;
}

static uint64_t
get_u64(const unsigned char *input)
{
	uint64_t value = 0;
	int byte;

	for (byte = 7; byte >= 0; byte--)
		value = (value << 8) | input[byte];
	return value;
}

static uint64_t
payload_hash(const unsigned char *payload, size_t size)
{
	uint64_t hash = UINT64_C(14695981039346656037);
	size_t byte;

	for (byte = 0; byte != size; byte++)
		hash = (hash ^ payload[byte]) * UINT64_C(1099511628211);
	return hash;
}

static const char *
string_at(const unsigned char *strings, size_t size, uint64_t offset)
{
	const unsigned char *end;

	assert(offset < size);
	end = memchr(strings + offset,0,size - (size_t)offset);
	assert(end);
	return (const char *)(strings + offset);
}

static void
assert_lemma(const unsigned char *entry, const unsigned char *strings,
             size_t string_size, const char *key, uint64_t first_block,
             uint32_t block_count)
{
	assert(!strcmp(string_at(strings,string_size,get_u64(entry)),key));
	assert(get_u64(entry + 8) == first_block);
	assert(get_u32(entry + 16) == block_count);
	assert(get_u32(entry + 20) == 0);
}

static void
assert_block(const unsigned char *entry, uint64_t first_record,
             uint32_t record_count)
{
	assert(get_u64(entry) == first_record);
	assert(get_u32(entry + 8) == record_count);
	assert(get_u32(entry + 12) == 0);
}

static void
assert_record(const unsigned char *entry, const unsigned char *strings,
              size_t string_size, uint32_t kind, const char *stem,
              const char *keys)
{
	assert(get_u32(entry) == kind);
	assert(get_u32(entry + 4) == 0);
	assert(!strcmp(string_at(strings,string_size,get_u64(entry + 8)),stem));
	assert(!strcmp(string_at(strings,string_size,get_u64(entry + 16)),keys));
}

int
main(int argc, char **argv)
{
	FILE *input;
	long file_length;
	unsigned char *file;
	const unsigned char *payload;
	const unsigned char *lemmas;
	const unsigned char *blocks;
	const unsigned char *records;
	const unsigned char *strings;
	uint64_t lemma_count;
	uint64_t block_count;
	uint64_t record_count;
	uint64_t string_size;
	size_t payload_size;

	assert(argc == 2);
	input = fopen(argv[1],"rb");
	assert(input);
	assert(!fseek(input,0,SEEK_END));
	file_length = ftell(input);
	assert(file_length >= (long)HEADER_SIZE);
	assert(!fseek(input,0,SEEK_SET));
	file = malloc((size_t)file_length);
	assert(file);
	assert(fread(file,1,(size_t)file_length,input) == (size_t)file_length);
	assert(!fclose(input));
	assert(!memcmp(file,"MORPHGEN",8));
	assert(get_u32(file + 8) == 1);
	assert(get_u32(file + 12) == 1);
	lemma_count = get_u64(file + 16);
	block_count = get_u64(file + 24);
	record_count = get_u64(file + 32);
	string_size = get_u64(file + 40);
	assert(lemma_count == 3);
	assert(block_count == 4);
	assert(record_count == 5);
	payload = file + HEADER_SIZE;
	payload_size = (size_t)file_length - HEADER_SIZE;
	assert(get_u64(file + 48) == payload_hash(payload,payload_size));
	for (size_t reserved = 56; reserved != HEADER_SIZE; reserved++)
		assert(!file[reserved]);
	assert(payload_size == lemma_count * LEMMA_ENTRY_SIZE +
	                       block_count * BLOCK_ENTRY_SIZE +
	                       record_count * RECORD_ENTRY_SIZE + string_size);
	lemmas = payload;
	blocks = lemmas + lemma_count * LEMMA_ENTRY_SIZE;
	records = blocks + block_count * BLOCK_ENTRY_SIZE;
	strings = records + record_count * RECORD_ENTRY_SIZE;
	assert_lemma(lemmas,strings,(size_t)string_size,"dupe",0,2);
	assert_lemma(lemmas + LEMMA_ENTRY_SIZE,strings,(size_t)string_size,
	             "lo/gos",2,1);
	assert_lemma(lemmas + 2 * LEMMA_ENTRY_SIZE,strings,(size_t)string_size,
	             "zeta",3,1);
	assert_block(blocks,0,1);
	assert_block(blocks + BLOCK_ENTRY_SIZE,1,1);
	assert_block(blocks + 2 * BLOCK_ENTRY_SIZE,2,2);
	assert_block(blocks + 3 * BLOCK_ENTRY_SIZE,4,1);
	assert_record(records,strings,(size_t)string_size,4,"du/pe","indecl");
	assert_record(records + RECORD_ENTRY_SIZE,strings,(size_t)string_size,5,
	              "du/pe","w_stem pres ind act 1st sg");
	assert_record(records + 2 * RECORD_ENTRY_SIZE,strings,
	              (size_t)string_size,1,"log","os_ou masc");
	assert_record(records + 3 * RECORD_ENTRY_SIZE,strings,
	              (size_t)string_size,2,"logik","os_h_on");
	assert_record(records + 4 * RECORD_ENTRY_SIZE,strings,
	              (size_t)string_size,1,"zet","os_ou masc");
	free(file);
	return 0;
}
