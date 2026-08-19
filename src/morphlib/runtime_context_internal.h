#ifndef MORPHEUS_RUNTIME_CONTEXT_INTERNAL_H
#define MORPHEUS_RUNTIME_CONTEXT_INTERNAL_H

#include <gkstring.h>

#include "runtime_context.h"

struct morpheus_runtime_context {
	int language;
	int heap_allocated;
	char comparison_table[128];
	char beta_table[128];
	int comparison_table_initialized;
	int beta_table_initialized;
	char *hidden_morphflag_table;
	char *preverb_morphflag_table;
	gk_string *raw_preverb_table;
	int raw_preverb_count;
	int raw_preverb_language;
	int smarta_greek_table[256];
	int smarta_accent_table[256];
	int smarta_tables_initialized;
	int smarta_current_font;
	int smarta_character_style;
};

morpheus_runtime_context *morpheus_runtime_context_current(void);

#endif
