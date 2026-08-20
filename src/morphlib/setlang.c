#include "morphlib_internal.h"

static _Thread_local morpheus_runtime_context default_context = { 0, 0 };
static _Thread_local morpheus_runtime_context *active_context;

morpheus_runtime_context *
morpheus_runtime_context_current(void)
{
	return(active_context ? active_context : &default_context);
}

morpheus_runtime_context *
morpheus_runtime_context_create(void)
{
	morpheus_runtime_context *context = calloc(1,sizeof *context);

	if (context) context->heap_allocated = 1;
	return(context);
}

void
morpheus_runtime_context_destroy(morpheus_runtime_context *context)
{
	size_t i;

	if (!context || !context->heap_allocated) return;
	if (active_context == context) active_context = NULL;
	free(context->hidden_morphflag_table);
	free(context->preverb_morphflag_table);
	if (context->raw_preverb_table)
		FreeGkString(context->raw_preverb_table);
	if (context->vowel_contraction_table)
		FreeGkString(context->vowel_contraction_table);
	if (context->consonant_euphony_table)
		FreeGkString(context->consonant_euphony_table);
	for (i = 0; i < sizeof context->smk_beta_table /
				 sizeof context->smk_beta_table[0]; i++) {
		free(context->smk_beta_table[i]);
		free(context->smarta_beta_table[i]);
	}
	free(context->morph_key_table);
	free(context->stem_type_arguments);
	free(context->derivation_type_arguments);
	free(context->domain_arguments);
	free(context);
}

morpheus_runtime_context *
morpheus_runtime_context_activate(morpheus_runtime_context *context)
{
	morpheus_runtime_context *previous = morpheus_runtime_context_current();

	active_context = context;
	return(previous);
}

void
morpheus_runtime_context_set_language(
	morpheus_runtime_context *context, int language)
{
	if (context) context->language = language;
}

int
morpheus_runtime_context_language(const morpheus_runtime_context *context)
{
	return(context ? context->language : default_context.language);
}

void set_lang(int n)
{
	morpheus_runtime_context_current()->language = n;
}

int cur_lang(void)
{
	return(morpheus_runtime_context_current()->language);
}
