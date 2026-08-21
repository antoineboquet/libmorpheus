#include <assert.h>
#include <string.h>

#include "../src/morphlib/beta2smarta.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/smk2beta.proto.h"

int main(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	char converted[16];
	char roundtrip[16];

	assert(context);
	previous = morpheus_runtime_context_activate(context);

	beta2smarta("a/",converted);
	assert((unsigned char)converted[0] == 0213);
	assert(converted[1] == '\0');
	smarta2beta(converted,roundtrip);
	assert(!strcmp(roundtrip,"$a/"));
	smk2beta(converted,roundtrip);
	assert(!strcmp(roundtrip,"a/"));

	beta2smarta("i+",converted);
	assert((unsigned char)converted[0] == 0363);
	assert(converted[1] == '\0');

	beta2smarta("r(",converted);
	assert((unsigned char)converted[0] == 0373);
	assert(converted[1] == '\0');

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(context);
	return 0;
}
