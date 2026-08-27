// SPDX-License-Identifier: AGPL-3.0-or-later

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/api/gener_index.h"

#define HEADER_SIZE 64u
#define LEMMA_ENTRY_SIZE 24u
#define BLOCK_ENTRY_SIZE 16u
#define RECORD_ENTRY_SIZE 24u

static uint64_t
get_u64(const unsigned char *input)
{
	uint64_t value = 0;
	int byte;

	for (byte = 7; byte >= 0; byte--)
		value = (value << 8) | input[byte];
	return value;
}

static void
put_u32(unsigned char *output, uint32_t value)
{
	int byte;

	for (byte = 0; byte != 4; byte++)
		output[byte] = (unsigned char)(value >> (byte * 8));
}

static void
put_u64(unsigned char *output, uint64_t value)
{
	int byte;

	for (byte = 0; byte != 8; byte++)
		output[byte] = (unsigned char)(value >> (byte * 8));
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

static void
refresh_hash(unsigned char *file, size_t size)
{
	put_u64(file + 48,payload_hash(file + HEADER_SIZE,size - HEADER_SIZE));
}

static void
assert_invalid(const unsigned char *file, size_t size)
{
	morpheus_gener_index *index = (morpheus_gener_index *)(uintptr_t)1;

	assert(morpheus_gener_index_open_memory(file,size,&index) ==
	       MORPHEUS_GENER_INDEX_INVALID);
	assert(!index);
}

int
main(int argc, char **argv)
{
	morpheus_gener_index *index = NULL;
	morpheus_gener_lemma_view lemma;
	morpheus_gener_block_view block;
	morpheus_gener_record_view record;
	unsigned char *file;
	unsigned char *changed;
	FILE *input;
	long length;
	size_t size;
	size_t lemma_size;
	size_t block_size;
	size_t record_offset;
	size_t string_offset;

	assert(argc == 2 || argc == 5);
	assert(morpheus_gener_index_open_file("missing-gener-index",&index) ==
	       MORPHEUS_GENER_INDEX_IO_ERROR);
	assert(!index);
	assert(morpheus_gener_index_open_file(argv[1],&index) ==
	       MORPHEUS_GENER_INDEX_OK);
	if (argc == 5) {
		assert(morpheus_gener_index_lemma_count(index) ==
		       (uint64_t)strtoull(argv[2],NULL,10));
		assert(morpheus_gener_index_block_count(index) ==
		       (uint64_t)strtoull(argv[3],NULL,10));
		assert(morpheus_gener_index_record_count(index) ==
		       (uint64_t)strtoull(argv[4],NULL,10));
		assert(morpheus_gener_index_lookup(index,"lo/gos",&lemma) ==
		       MORPHEUS_GENER_INDEX_OK);
		morpheus_gener_index_close(index);
		return 0;
	}
	assert(morpheus_gener_index_lemma_count(index) == 3);
	assert(morpheus_gener_index_block_count(index) == 4);
	assert(morpheus_gener_index_record_count(index) == 5);
	assert(morpheus_gener_index_lookup(index,"dupe",&lemma) ==
	       MORPHEUS_GENER_INDEX_OK);
	assert(lemma.first_block == 0 && lemma.block_count == 2);
	assert(morpheus_gener_index_block(index,lemma.first_block,&block) ==
	       MORPHEUS_GENER_INDEX_OK);
	assert(block.first_record == 0 && block.record_count == 1);
	assert(morpheus_gener_index_record(index,block.first_record,&record) ==
	       MORPHEUS_GENER_INDEX_OK);
	assert(record.kind == MORPHEUS_GENER_RECORD_INDECLINABLE);
	assert(!strcmp(record.stem,"du/pe"));
	assert(!strcmp(record.keys,"indecl"));
	assert(morpheus_gener_index_lookup(index,"lo/gos",&lemma) ==
	       MORPHEUS_GENER_INDEX_OK);
	assert(lemma.first_block == 2 && lemma.block_count == 1);
	assert(morpheus_gener_index_lookup(index,"missing",&lemma) ==
	       MORPHEUS_GENER_INDEX_NOT_FOUND);
	assert(morpheus_gener_index_block(index,4,&block) ==
	       MORPHEUS_GENER_INDEX_NOT_FOUND);
	assert(morpheus_gener_index_record(index,5,&record) ==
	       MORPHEUS_GENER_INDEX_NOT_FOUND);
	morpheus_gener_index_close(index);

	input = fopen(argv[1],"rb");
	assert(input);
	assert(!fseek(input,0,SEEK_END));
	length = ftell(input);
	assert(length > (long)HEADER_SIZE);
	assert(!fseek(input,0,SEEK_SET));
	size = (size_t)length;
	file = malloc(size);
	changed = malloc(size);
	assert(file && changed);
	assert(fread(file,1,size,input) == size);
	assert(!fclose(input));
	assert(morpheus_gener_index_open_memory(file,size,&index) ==
	       MORPHEUS_GENER_INDEX_OK);
	morpheus_gener_index_close(index);
	assert_invalid(file,size - 1);

	memcpy(changed,file,size);
	changed[0] ^= 1;
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	put_u32(changed + 8,2);
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	changed[56] = 1;
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	changed[HEADER_SIZE] ^= 1;
	assert_invalid(changed,size);

	lemma_size = (size_t)get_u64(file + 16) * LEMMA_ENTRY_SIZE;
	block_size = (size_t)get_u64(file + 24) * BLOCK_ENTRY_SIZE;
	record_offset = HEADER_SIZE + lemma_size + block_size;
	string_offset = record_offset +
	                (size_t)get_u64(file + 32) * RECORD_ENTRY_SIZE;
	memcpy(changed,file,size);
	changed[HEADER_SIZE + 20] = 1;
	refresh_hash(changed,size);
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	put_u64(changed + HEADER_SIZE,get_u64(file + 40));
	refresh_hash(changed,size);
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	put_u32(changed + HEADER_SIZE + lemma_size + 8,0);
	refresh_hash(changed,size);
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	put_u32(changed + record_offset,0);
	refresh_hash(changed,size);
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	changed[size - 1] = 'x';
	refresh_hash(changed,size);
	assert_invalid(changed,size);
	memcpy(changed,file,size);
	changed[string_offset + (size_t)get_u64(changed + HEADER_SIZE)] = 'z';
	refresh_hash(changed,size);
	assert_invalid(changed,size);

	free(changed);
	free(file);
	return 0;
}
