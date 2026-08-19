#ifndef MORPHEUS_RUNTIME_CONTEXT_H
#define MORPHEUS_RUNTIME_CONTEXT_H

typedef struct morpheus_runtime_context morpheus_runtime_context;

/* Active contexts are thread-affine; NULL restores the thread default. */
morpheus_runtime_context *morpheus_runtime_context_create(void);
void morpheus_runtime_context_destroy(morpheus_runtime_context *context);
morpheus_runtime_context *morpheus_runtime_context_activate(
    morpheus_runtime_context *context);
void morpheus_runtime_context_set_language(
    morpheus_runtime_context *context, int language);
int morpheus_runtime_context_language(
    const morpheus_runtime_context *context);

#endif
