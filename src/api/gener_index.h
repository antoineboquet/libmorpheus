// SPDX-License-Identifier: AGPL-3.0-or-later

#ifndef MORPHEUS_GENER_INDEX_H
#define MORPHEUS_GENER_INDEX_H

#include <stddef.h>
#include <stdint.h>

typedef struct morpheus_gener_index morpheus_gener_index;

typedef enum {
	MORPHEUS_GENER_INDEX_OK = 0,
	MORPHEUS_GENER_INDEX_INVALID = 1,
	MORPHEUS_GENER_INDEX_IO_ERROR = 2,
	MORPHEUS_GENER_INDEX_NO_MEMORY = 3,
	MORPHEUS_GENER_INDEX_NOT_FOUND = 4
} morpheus_gener_index_status;

typedef enum {
	MORPHEUS_GENER_RECORD_NOUN = 1,
	MORPHEUS_GENER_RECORD_ADJECTIVE = 2,
	MORPHEUS_GENER_RECORD_VERB_STEM = 3,
	MORPHEUS_GENER_RECORD_INDECLINABLE = 4,
	MORPHEUS_GENER_RECORD_IRREGULAR_VERB = 5
} morpheus_gener_record_kind;

typedef struct {
	uint64_t first_block;
	uint32_t block_count;
} morpheus_gener_lemma_view;

typedef struct {
	uint64_t first_record;
	uint32_t record_count;
} morpheus_gener_block_view;

typedef struct {
	morpheus_gener_record_kind kind;
	const char *stem;
	const char *keys;
} morpheus_gener_record_view;

morpheus_gener_index_status morpheus_gener_index_open_file(
    const char *path, morpheus_gener_index **index);
morpheus_gener_index_status morpheus_gener_index_open_memory(
    const void *data, size_t size, morpheus_gener_index **index);
void morpheus_gener_index_close(morpheus_gener_index *index);

morpheus_gener_index_status morpheus_gener_index_lookup(
    const morpheus_gener_index *index, const char *canonical_lemma,
    morpheus_gener_lemma_view *lemma);
morpheus_gener_index_status morpheus_gener_index_block(
    const morpheus_gener_index *index, uint64_t block,
    morpheus_gener_block_view *view);
morpheus_gener_index_status morpheus_gener_index_record(
    const morpheus_gener_index *index, uint64_t record,
    morpheus_gener_record_view *view);

uint64_t morpheus_gener_index_lemma_count(const morpheus_gener_index *index);
uint64_t morpheus_gener_index_block_count(const morpheus_gener_index *index);
uint64_t morpheus_gener_index_record_count(const morpheus_gener_index *index);

#endif
