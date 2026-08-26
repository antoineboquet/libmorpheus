// SPDX-License-Identifier: MPL-2.0

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_CAPACITY 8192u

typedef enum {
	BASE_NONE,
	BASE_DIRECT
} base_kind;

typedef struct {
	char *data;
	size_t size;
	size_t capacity;
} prepared_buffer;

typedef struct {
	prepared_buffer *output;
	char *base_prefix;
	base_kind base;
	int have_lemma;
} prepare_state;

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
			if (!state->have_lemma || state->base != BASE_DIRECT) {
				fprintf(stderr,
				        "morpheus-gener-prepare: %s:%llu: continuation without active record\n",
				        path,line_number);
				goto finish;
			}
			if (!append_bytes(state->output,state->base_prefix,
			                  strlen(state->base_prefix)) ||
			    !write_line(state->output,content + 1))
				goto write_error;
			continue;
		}

		if (!strncmp(content,":de:",4) || *content == ';') {
			fprintf(stderr,
			        "morpheus-gener-prepare: %s:%llu: unexpanded derivation record\n",
			        path,line_number);
			goto finish;
		}
	}
	if (ferror(input)) {
		report_errno("cannot read",path);
		goto finish;
	}
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
