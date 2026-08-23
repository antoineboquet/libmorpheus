#include <assert.h>

#include <gkstring.h>

#include "../src/anal/checkword.proto.h"
#include "../src/anal/checkcrasis.proto.h"
#include "../src/anal/checkindecl.proto.h"
#include "../src/anal/checknom.proto.h"
#include "../src/anal/checkverb.proto.h"
#include "../src/morphlib/runtime_context.h"

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(!checkword(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!checknom(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!checkregnom(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!checkverb(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!analyzed_verb(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!checkindecl(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!checkcrasis(NULL));
	assert(morpheus_runtime_context_error(first) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(first);
	assert(!get_quickflag());
	set_quickflag(1);
	assert(get_quickflag());

	morpheus_runtime_context_activate(second);
	assert(!get_quickflag());
	set_quickflag(0);
	assert(!get_quickflag());

	morpheus_runtime_context_activate(first);
	assert(get_quickflag());
	set_quickflag(0);
	assert(!get_quickflag());

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
