#define _POSIX_C_SOURCE 200809L

// SPDX-License-Identifier: MPL-2.0

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gkstring.h>

#include "../src/gener/derivation.h"

#define LINE_CAPACITY 8192u

typedef enum {
	BASE_NONE,
	BASE_DIRECT,
	BASE_DERIVATION
} base_kind;

typedef struct {
	char *data;
	size_t size;
	size_t capacity;
} prepared_buffer;

typedef struct {
	prepared_buffer *output;
	char *base_prefix;
	char derivation_stem[MAXWORDSIZE];
	char derivation_type[MAXWORDSIZE];
	char derivation_keys[LONGSTRING];
	char saved_request[LONGSTRING];
	base_kind base;
	int have_lemma;
	int have_explicit_request;
} prepare_state;

static int append_bytes(prepared_buffer *output, const char *data,
                        size_t size);
static void report_errno(const char *operation, const char *path);

static int
copy_text(char *destination, size_t capacity, const char *source)
{
	int length = snprintf(destination,capacity,"%s",source);

	return length >= 0 && (size_t)length < capacity;
}

static int
take_token(const char **cursor, char *token, size_t capacity)
{
	const char *start = *cursor;
	size_t length;

	while (*start == ' ' || *start == '\t')
		start++;
	length = 0;
	while (start[length] && start[length] != ' ' && start[length] != '\t')
		length++;
	if (!length || length >= capacity)
		return 0;
	memcpy(token,start,length);
	token[length] = 0;
	*cursor = start + length;
	return 1;
}

static int
valid_derivation_name(const char *name)
{
	const unsigned char *cursor = (const unsigned char *)name;

	while (*cursor) {
		if (!((*cursor >= 'a' && *cursor <= 'z') ||
		      (*cursor >= 'A' && *cursor <= 'Z') ||
		      (*cursor >= '0' && *cursor <= '9') || *cursor == '_'))
			return 0;
		cursor++;
	}
	return cursor != (const unsigned char *)name;
}

static int
expand_request(prepare_state *state, const char *request, const char *path,
               unsigned long long line_number)
{
	FILE *stream;
	char *record = NULL;
	size_t size = 0;
	int expanded;

	stream = open_memstream(&record,&size);
	if (!stream) {
		fprintf(stderr,"morpheus-gener-prepare: out of memory\n");
		return 0;
	}
	expanded = morpheus_gener_expand_derivation(
	    stream,state->derivation_stem,state->derivation_type,
	    state->derivation_keys,request);
	if (fclose(stream)) {
		free(record);
		report_errno("cannot finalize derivation",path);
		return 0;
	}
	if (expanded != 1) {
		fprintf(stderr,
		        "morpheus-gener-prepare: %s:%llu: derivation %s produced no record\n",
		        path,line_number,state->derivation_type);
		free(record);
		return 0;
	}
	if (!append_bytes(state->output,record,size)) {
		fprintf(stderr,"morpheus-gener-prepare: out of memory\n");
		free(record);
		return 0;
	}
	free(record);
	return 1;
}

static int
flush_derivation(prepare_state *state, const char *path,
                 unsigned long long line_number)
{
	if (state->base != BASE_DERIVATION || state->have_explicit_request)
		return 1;
	state->have_explicit_request = 1;
	return expand_request(state,"pr",path,line_number);
}

static void
report_errno(const char *operation, const char *path)
{
	fprintf(stderr,"morpheus-gener-prepare: %s %s: %s\n",
	        operation,path,strerror(errno));
}

static char *
trim(char *value)
{
	char *end;

	while (*value == ' ' || *value == '\t')
		value++;
	end = value + strlen(value);
	while (end != value && (end[-1] == ' ' || end[-1] == '\t' ||
	                        end[-1] == '\r' || end[-1] == '\n'))
		*--end = 0;
	return value;
}

static int
is_generation_record(const char *line)
{
	static const char *const prefixes[] = {
		":no:", ":aj:", ":vs:", ":wd:", ":vb:"
	};
	size_t prefix;

	for (prefix = 0; prefix != sizeof prefixes / sizeof prefixes[0];
	     prefix++) {
		if (!strncmp(line,prefixes[prefix],4))
			return 1;
	}
	return 0;
}

static char *
base_prefix(const char *line, int *complete)
{
	const char *cursor = line;
	const char *second;
	size_t length;
	char *result;

	*complete = 0;
	while (*cursor && *cursor != ' ' && *cursor != '\t')
		cursor++;
	if (!*cursor)
		return NULL;
	while (*cursor == ' ' || *cursor == '\t')
		cursor++;
	second = cursor;
	while (*cursor && *cursor != ' ' && *cursor != '\t')
		cursor++;
	if (cursor == second)
		return NULL;
	*complete = 1;
	length = (size_t)(cursor - line);
	result = malloc(length + 1);
	if (!result)
		return NULL;
	memcpy(result,line,length);
	result[length] = 0;
	return result;
}

static int
append_bytes(prepared_buffer *output, const char *data, size_t size)
{
	if (size > SIZE_MAX - output->size)
		return 0;
	if (output->size + size > output->capacity) {
		size_t required = output->size + size;
		size_t capacity = output->capacity ? output->capacity : 16384;
		char *grown;

		while (capacity < required) {
			if (capacity > SIZE_MAX / 2)
				capacity = required;
			else
				capacity *= 2;
		}
		grown = realloc(output->data,capacity);
		if (!grown)
			return 0;
		output->data = grown;
		output->capacity = capacity;
	}
	memcpy(output->data + output->size,data,size);
	output->size += size;
	return 1;
}

static int
write_line(prepared_buffer *output, const char *line)
{
	return append_bytes(output,line,strlen(line)) &&
	       append_bytes(output,"\n",1);
}

static int
prepare_file(prepare_state *state, const char *path)
{
	FILE *input = fopen(path,"r");
	char line[LINE_CAPACITY];
	unsigned long long line_number = 0;
	int result = 0;

	if (!input) {
		report_errno("cannot open",path);
		return 0;
	}
	free(state->base_prefix);
	state->base_prefix = NULL;
	state->base = BASE_NONE;
	state->have_lemma = 0;
	state->have_explicit_request = 0;

	while (fgets(line,sizeof line,input)) {
		char *content;
		size_t length;

		line_number++;
		length = strlen(line);
		if (length == sizeof line - 1 && line[length - 1] != '\n') {
			fprintf(stderr,
			        "morpheus-gener-prepare: %s:%llu: line too long\n",
			        path,line_number);
			goto finish;
		}
		content = trim(line);
		if (!*content || *content == '#' || *content == '?')
			continue;

		if (!strncmp(content,":le:",4)) {
			char *lemma = trim(content + 4);

			if (!flush_derivation(state,path,line_number))
				goto finish;
			if (!*lemma) {
				fprintf(stderr,
				        "morpheus-gener-prepare: %s:%llu: invalid lemma\n",
				        path,line_number);
				goto finish;
			}
			free(state->base_prefix);
			state->base_prefix = NULL;
			state->base = BASE_NONE;
			state->have_lemma = 1;
			state->have_explicit_request = 0;
			if (!write_line(state->output,content))
				goto write_error;
			continue;
		}

		if (is_generation_record(content)) {
			char *prefix;
			int complete;

			if (!state->have_lemma) {
				fprintf(stderr,
				        "morpheus-gener-prepare: %s:%llu: record before lemma\n",
				        path,line_number);
				goto finish;
			}
			state->base = BASE_NONE;
			state->have_explicit_request = 0;
			prefix = base_prefix(content,&complete);
			if (!prefix) {
				if (complete)
					fprintf(stderr,"morpheus-gener-prepare: out of memory\n");
				else
					fprintf(stderr,
					        "morpheus-gener-prepare: %s:%llu: incomplete record\n",
					        path,line_number);
				goto finish;
			}
			free(state->base_prefix);
			state->base_prefix = prefix;
			state->base = BASE_DIRECT;
			if (!write_line(state->output,content))
				goto write_error;
			continue;
		}

		if (*content == '@') {
			if (!state->have_lemma || state->base == BASE_NONE) {
				fprintf(stderr,
				        "morpheus-gener-prepare: %s:%llu: continuation without active record\n",
				        path,line_number);
				goto finish;
			}
			if (state->base == BASE_DIRECT) {
				if (!append_bytes(state->output,state->base_prefix,
				                  strlen(state->base_prefix)) ||
				    !write_line(state->output,content + 1))
					goto write_error;
			} else {
				char request[LONGSTRING * 2];
				char *continuation = trim(content + 1);
				int length;

				if (!*state->saved_request) {
					fprintf(stderr,
					        "morpheus-gener-prepare: %s:%llu: derivation continuation without principal part\n",
					        path,line_number);
					goto finish;
				}
				length = snprintf(request,sizeof request,"%s%s%s",
				                  state->saved_request,
				                  *continuation ? " " : "",continuation);
				if (length < 0 || (size_t)length >= sizeof request ||
				    !expand_request(state,request,path,line_number))
					goto finish;
			}
			continue;
		}

		if (!strncmp(content,":de:",4)) {
			const char *cursor = content + 4;
			char *global;

			if (!state->have_lemma || *cursor == ' ' || *cursor == '\t' ||
			    !flush_derivation(state,path,line_number) ||
			    !take_token(&cursor,state->derivation_stem,
			                sizeof state->derivation_stem) ||
			    !take_token(&cursor,state->derivation_type,
			                sizeof state->derivation_type) ||
			    !valid_derivation_name(state->derivation_type)) {
				fprintf(stderr,
				        "morpheus-gener-prepare: %s:%llu: invalid derivation record\n",
				        path,line_number);
				goto finish;
			}
			global = trim((char *)cursor);
			if (!copy_text(state->derivation_keys,
			               sizeof state->derivation_keys,global))
				goto write_error;
			state->saved_request[0] = 0;
			state->have_explicit_request = 0;
			state->base = BASE_DERIVATION;
			continue;
		}

		if (*content == ';') {
			const char *cursor = content + 1;
			char request[LONGSTRING];

			if (state->base != BASE_DERIVATION ||
			    !take_token(&cursor,state->saved_request,
			                sizeof state->saved_request) ||
			    !copy_text(request,sizeof request,trim(content + 1))) {
				fprintf(stderr,
				        "morpheus-gener-prepare: %s:%llu: invalid principal-part request\n",
				        path,line_number);
				goto finish;
			}
			state->have_explicit_request = 1;
			if (!expand_request(state,request,path,line_number))
				goto finish;
			continue;
		}
	}
	if (ferror(input)) {
		report_errno("cannot read",path);
		goto finish;
	}
	if (!flush_derivation(state,path,line_number + 1))
		goto finish;
	result = 1;
	goto finish;

write_error:
	fprintf(stderr,"morpheus-gener-prepare: out of memory\n");

finish:
	if (fclose(input) && result) {
		report_errno("cannot close",path);
		result = 0;
	}
	return result;
}

static int
write_output(const prepared_buffer *prepared, const char *path)
{
	FILE *output;
	int result = 0;

	output = fopen(path,"wb");
	if (!output) {
		report_errno("cannot create",path);
		return 0;
	}
	if (prepared->size &&
	    fwrite(prepared->data,1,prepared->size,output) != prepared->size) {
		report_errno("cannot write",path);
		goto finish;
	}
	result = 1;

finish:
	if (fclose(output) && result) {
		report_errno("cannot close",path);
		result = 0;
	}
	if (!result)
		remove(path);
	return result;
}

int
main(int argc, char **argv)
{
	prepared_buffer prepared = { 0 };
	prepare_state state = { 0 };
	int input;
	int result = EXIT_FAILURE;

	if (argc < 3) {
		fprintf(stderr,
		        "usage: morpheus_gener_source_preparer OUTPUT INPUT...\n");
		return EXIT_FAILURE;
	}
	state.output = &prepared;
	for (input = 2; input != argc; input++) {
		if (!prepare_file(&state,argv[input]))
			goto finish;
	}
	if (!write_output(&prepared,argv[1]))
		goto finish;
	result = EXIT_SUCCESS;

finish:
	free(state.base_prefix);
	free(prepared.data);
	return result;
}
