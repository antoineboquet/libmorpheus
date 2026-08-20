#include <assert.h>

#include <gkstring.h>

#include "../src/anal/checkstring.proto.h"
#include "../src/morphlib/runtime_context.h"

int quickflag = 0;

int
main(void)
{
	morpheus_runtime_context *first = morpheus_runtime_context_create();
	morpheus_runtime_context *second = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(first);
	assert(second);
	previous = morpheus_runtime_context_activate(first);
	assert(GetWantDialect() == ALL_DIAL);
	SetWantDialect(ATTIC);
	AddWantDialect(EPIC);
	assert(GetWantDialect() == (ATTIC | EPIC));

	morpheus_runtime_context_activate(second);
	assert(GetWantDialect() == ALL_DIAL);
	SetWantDialect(IONIC);
	assert(GetWantDialect() == IONIC);

	morpheus_runtime_context_activate(first);
	assert(GetWantDialect() == (ATTIC | EPIC));
	ZapWantDialect(EPIC);
	assert(GetWantDialect() == ATTIC);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(first);
	morpheus_runtime_context_destroy(second);
	return(0);
}
