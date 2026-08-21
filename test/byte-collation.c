#include <assert.h>

#include "../src/morphlib/morphstrcmp.proto.h"
#include "../src/morphlib/runtime_context.h"

int main(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	char lower_byte[] = {(char)(unsigned char)0200,'\0'};
	char higher_byte[] = {(char)(unsigned char)0377,'\0'};

	assert(context);
	previous = morpheus_runtime_context_activate(context);

	assert(morphstrcmp(lower_byte,lower_byte) == 0);
	assert(morphstrcmp(lower_byte,higher_byte) < 0);
	assert(morphstrcmp(higher_byte,lower_byte) > 0);
	assert(morphstrncmp(lower_byte,higher_byte,1) < 0);
	assert(betastrcmp(lower_byte,higher_byte) < 0);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(context);
	return 0;
}
