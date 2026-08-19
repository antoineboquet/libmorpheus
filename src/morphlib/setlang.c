#include "morphlib_internal.h"

struct morpheus_runtime_context {
	int language;
	int heap_allocated;
};

static _Thread_local morpheus_runtime_context default_context = { 0, 0 };
static _Thread_local morpheus_runtime_context *active_context;

static morpheus_runtime_context *
current_context(void)
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
	if (!context || !context->heap_allocated) return;
	if (active_context == context) active_context = NULL;
	free(context);
}

morpheus_runtime_context *
morpheus_runtime_context_activate(morpheus_runtime_context *context)
{
	morpheus_runtime_context *previous = current_context();

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
	current_context()->language = n;
}

int cur_lang(void)
{
	return(current_context()->language);
}
