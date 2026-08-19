#ifndef MORPHEUS_RUNTIME_CONTEXT_INTERNAL_H
#define MORPHEUS_RUNTIME_CONTEXT_INTERNAL_H

#include "runtime_context.h"

struct morpheus_runtime_context {
	int language;
	int heap_allocated;
	char comparison_table[128];
	char beta_table[128];
	int comparison_table_initialized;
	int beta_table_initialized;
};

morpheus_runtime_context *morpheus_runtime_context_current(void);

#endif
