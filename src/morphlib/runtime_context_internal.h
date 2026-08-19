#ifndef MORPHEUS_RUNTIME_CONTEXT_INTERNAL_H
#define MORPHEUS_RUNTIME_CONTEXT_INTERNAL_H

#include <gkstring.h>
#include "morphargs.h"

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
	char *smk_beta_table[257];
	char *smarta_beta_table[257];
	char **active_inverse_conversion_table;
	char inverse_smarta_characters[257];
	int inverse_conversion_tables_initialized;
	int inverse_conversion_from_smk;
	int inverse_conversion_current_font;
	Morph_args *stem_type_arguments;
	Morph_args *derivation_type_arguments;
	Morph_args *domain_arguments;
	Morph_args **morph_key_table;
	int morph_key_stem_count;
	int morph_key_derivation_count;
	int morph_key_domain_count;
	int morph_key_count;
	int morph_keys_initialized;
	int morph_key_language;
};

morpheus_runtime_context *morpheus_runtime_context_current(void);

#endif
