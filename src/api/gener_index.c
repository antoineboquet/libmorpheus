// SPDX-License-Identifier: AGPL-3.0-or-later

#include "gener_index.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEADER_SIZE 64u
#define LEMMA_ENTRY_SIZE 24u
#define BLOCK_ENTRY_SIZE 16u
#define RECORD_ENTRY_SIZE 24u
#define FORMAT_VERSION 1u
#define LANGUAGE_GREEK 1u

struct morpheus_gener_index {
	unsigned char *file;
	size_t file_size;
	const unsigned char *lemmas;
	const unsigned char *blocks;
	const unsigned char *records;
	const unsigned char *strings;
	size_t string_size;
	uint64_t lemma_count;
	uint64_t block_count;
	uint64_t record_count;
};

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

static int
add_size(size_t *total, size_t amount)
{
	if (amount > SIZE_MAX - *total)
		return 0;
	*total += amount;
	return 1;
}

static int
table_size(uint64_t count, size_t entry_size, size_t *result)
{
	if (count > (uint64_t)SIZE_MAX ||
	    (count && entry_size > SIZE_MAX / (size_t)count))
		return 0;
	*result = (size_t)count * entry_size;
	return 1;
}

static const char *
string_at(const morpheus_gener_index *index, uint64_t offset)
{
	if (offset >= index->string_size ||
	    !memchr(index->strings + (size_t)offset,0,
	            index->string_size - (size_t)offset))
		return NULL;
	return (const char *)(index->strings + (size_t)offset);
}

static int
valid_layout(morpheus_gener_index *index)
{
	const unsigned char *header = index->file;
	size_t lemma_size, block_size, record_size;
	size_t expected_size = HEADER_SIZE;
	uint64_t expected_block = 0;
	uint64_t expected_record = 0;
	const char *previous_key = NULL;
	uint64_t item;

	if (index->file_size < HEADER_SIZE ||
	    memcmp(header,"MORPHGEN",8) ||
	    get_u32(header + 8) != FORMAT_VERSION ||
	    get_u32(header + 12) != LANGUAGE_GREEK ||
	    get_u64(header + 56))
		return 0;
	index->lemma_count = get_u64(header + 16);
	index->block_count = get_u64(header + 24);
	index->record_count = get_u64(header + 32);
	if (get_u64(header + 40) > SIZE_MAX)
		return 0;
	index->string_size = (size_t)get_u64(header + 40);
	if (!table_size(index->lemma_count,LEMMA_ENTRY_SIZE,&lemma_size) ||
	    !table_size(index->block_count,BLOCK_ENTRY_SIZE,&block_size) ||
	    !table_size(index->record_count,RECORD_ENTRY_SIZE,&record_size) ||
	    !add_size(&expected_size,lemma_size) ||
	    !add_size(&expected_size,block_size) ||
	    !add_size(&expected_size,record_size) ||
	    !add_size(&expected_size,index->string_size) ||
	    expected_size != index->file_size ||
	    get_u64(header + 48) !=
	        payload_hash(header + HEADER_SIZE,index->file_size - HEADER_SIZE))
		return 0;
	index->lemmas = header + HEADER_SIZE;
	index->blocks = index->lemmas + lemma_size;
	index->records = index->blocks + block_size;
	index->strings = index->records + record_size;

	for (item = 0; item != index->lemma_count; item++) {
		const unsigned char *entry =
		    index->lemmas + (size_t)item * LEMMA_ENTRY_SIZE;
		const char *key = string_at(index,get_u64(entry));
		uint64_t first_block = get_u64(entry + 8);
		uint32_t count = get_u32(entry + 16);

		if (!key || !*key || get_u32(entry + 20) || !count ||
		    first_block != expected_block ||
		    expected_block > index->block_count ||
		    count > index->block_count - expected_block ||
		    (previous_key && strcmp(previous_key,key) >= 0))
			return 0;
		expected_block += count;
		previous_key = key;
	}
	if (expected_block != index->block_count)
		return 0;

	for (item = 0; item != index->block_count; item++) {
		const unsigned char *entry =
		    index->blocks + (size_t)item * BLOCK_ENTRY_SIZE;
		uint64_t first_record = get_u64(entry);
		uint32_t count = get_u32(entry + 8);

		if (get_u32(entry + 12) || !count ||
		    first_record != expected_record ||
		    expected_record > index->record_count ||
		    count > index->record_count - expected_record)
			return 0;
		expected_record += count;
	}
	if (expected_record != index->record_count)
		return 0;

	for (item = 0; item != index->record_count; item++) {
		const unsigned char *entry =
		    index->records + (size_t)item * RECORD_ENTRY_SIZE;
		uint32_t kind = get_u32(entry);
		const char *stem = string_at(index,get_u64(entry + 8));
		const char *keys = string_at(index,get_u64(entry + 16));

		if (kind < MORPHEUS_GENER_RECORD_NOUN ||
		    kind > MORPHEUS_GENER_RECORD_IRREGULAR_VERB ||
		    get_u32(entry + 4) || !stem || !*stem || !keys || !*keys)
			return 0;
	}
	return 1;
}

static morpheus_gener_index_status
open_owned(unsigned char *data, size_t size, morpheus_gener_index **index)
{
	morpheus_gener_index *created;

	created = calloc(1,sizeof *created);
	if (!created) {
		free(data);
		return MORPHEUS_GENER_INDEX_NO_MEMORY;
	}
	created->file = data;
	created->file_size = size;
	if (!valid_layout(created)) {
		morpheus_gener_index_close(created);
		return MORPHEUS_GENER_INDEX_INVALID;
	}
	*index = created;
	return MORPHEUS_GENER_INDEX_OK;
}

morpheus_gener_index_status
morpheus_gener_index_open_memory(const void *data, size_t size,
                                 morpheus_gener_index **index)
{
	unsigned char *copy;

	if (!data || !size || !index)
		return MORPHEUS_GENER_INDEX_INVALID;
	*index = NULL;
	copy = malloc(size);
	if (!copy)
		return MORPHEUS_GENER_INDEX_NO_MEMORY;
	memcpy(copy,data,size);
	return open_owned(copy,size,index);
}

morpheus_gener_index_status
morpheus_gener_index_open_file(const char *path, morpheus_gener_index **index)
{
	unsigned char *data;
	FILE *input;
	long length;
	int close_result;
	size_t read_size;
	morpheus_gener_index_status status;

	if (!path || !*path || !index)
		return MORPHEUS_GENER_INDEX_INVALID;
	*index = NULL;
	input = fopen(path,"rb");
	if (!input)
		return MORPHEUS_GENER_INDEX_IO_ERROR;
	if (fseek(input,0,SEEK_END) || (length = ftell(input)) <= 0 ||
	    fseek(input,0,SEEK_SET)) {
		fclose(input);
		return MORPHEUS_GENER_INDEX_IO_ERROR;
	}
	data = malloc((size_t)length);
	if (!data) {
		fclose(input);
		return MORPHEUS_GENER_INDEX_NO_MEMORY;
	}
	read_size = fread(data,1,(size_t)length,input);
	close_result = fclose(input);
	if (read_size != (size_t)length || close_result) {
		free(data);
		return MORPHEUS_GENER_INDEX_IO_ERROR;
	}
	status = open_owned(data,(size_t)length,index);
	return status;
}

void
morpheus_gener_index_close(morpheus_gener_index *index)
{
	if (!index)
		return;
	free(index->file);
	free(index);
}

morpheus_gener_index_status
morpheus_gener_index_lookup(const morpheus_gener_index *index,
                            const char *canonical_lemma,
                            morpheus_gener_lemma_view *lemma)
{
	uint64_t low = 0;
	uint64_t high;

	if (!index || !canonical_lemma || !*canonical_lemma || !lemma)
		return MORPHEUS_GENER_INDEX_INVALID;
	high = index->lemma_count;
	while (low != high) {
		uint64_t middle = low + (high - low) / 2;
		const unsigned char *entry =
		    index->lemmas + (size_t)middle * LEMMA_ENTRY_SIZE;
		const char *key = string_at(index,get_u64(entry));
		int order = strcmp(canonical_lemma,key);

		if (order < 0)
			high = middle;
		else if (order > 0)
			low = middle + 1;
		else {
			lemma->first_block = get_u64(entry + 8);
			lemma->block_count = get_u32(entry + 16);
			return MORPHEUS_GENER_INDEX_OK;
		}
	}
	return MORPHEUS_GENER_INDEX_NOT_FOUND;
}

morpheus_gener_index_status
morpheus_gener_index_block(const morpheus_gener_index *index, uint64_t block,
                           morpheus_gener_block_view *view)
{
	const unsigned char *entry;

	if (!index || !view)
		return MORPHEUS_GENER_INDEX_INVALID;
	if (block >= index->block_count)
		return MORPHEUS_GENER_INDEX_NOT_FOUND;
	entry = index->blocks + (size_t)block * BLOCK_ENTRY_SIZE;
	view->first_record = get_u64(entry);
	view->record_count = get_u32(entry + 8);
	return MORPHEUS_GENER_INDEX_OK;
}

morpheus_gener_index_status
morpheus_gener_index_record(const morpheus_gener_index *index, uint64_t record,
                            morpheus_gener_record_view *view)
{
	const unsigned char *entry;

	if (!index || !view)
		return MORPHEUS_GENER_INDEX_INVALID;
	if (record >= index->record_count)
		return MORPHEUS_GENER_INDEX_NOT_FOUND;
	entry = index->records + (size_t)record * RECORD_ENTRY_SIZE;
	view->kind = (morpheus_gener_record_kind)get_u32(entry);
	view->stem = string_at(index,get_u64(entry + 8));
	view->keys = string_at(index,get_u64(entry + 16));
	return MORPHEUS_GENER_INDEX_OK;
}

uint64_t
morpheus_gener_index_lemma_count(const morpheus_gener_index *index)
{
	return index ? index->lemma_count : 0;
}

uint64_t
morpheus_gener_index_block_count(const morpheus_gener_index *index)
{
	return index ? index->block_count : 0;
}

uint64_t
morpheus_gener_index_record_count(const morpheus_gener_index *index)
{
	return index ? index->record_count : 0;
}
