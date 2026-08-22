#include <assert.h>
#include <string.h>

#include "../src/greeklib/beta_tolower.proto.h"
#include "../src/greeklib/normucase.proto.h"
#include "../src/greeklib/stripacute.proto.h"
#include "../src/greeklib/stripchar.proto.h"
#include "../src/greeklib/stripzeroend.proto.h"
#include "../src/greeklib/strsqz.proto.h"
#include "../src/greeklib/subchar.proto.h"
#include "../src/greeklib/xstrings.proto.h"
#include "../src/greeklib/isblank.proto.h"
#include "../src/morphlib/beta2smarta.proto.h"
#include "../src/morphlib/nextkey.proto.h"
#include "../src/morphlib/runtime_context.h"
#include "../src/morphlib/runtime_context_internal.h"
#include "../src/morphlib/smk2beta.proto.h"

int main(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;
	char *smarta_entry;
	char *smk_entry;
	char converted[16];
	char roundtrip[16];
	char uppercase_accented[] = {'^',(char)(unsigned char)0213,'\0'};
	char high_byte_word[] = {'*',(char)(unsigned char)0377,'a','\0'};
	char high_byte_key[] = {(char)(unsigned char)0377,' ','a','\0'};
	char key[4];
	char empty[] = "";
	char marker_only[] = "*";
	char multiple_accents[] = "a//b/";
	char normalized[] = "*(/ellhn";
	char remove_chars[] = "a---b-";
	char replace_high_byte[] = {(char)(unsigned char)0377,'a','\0'};
	char squeezed[] = "abcdef";

	_Static_assert(_Generic(Xstrlen(""), size_t: 1, default: 0),
		"Xstrlen must preserve the size_t result of strlen");
	assert(Xstrlen("beta") == strlen("beta"));
	stripzeroend(empty);
	assert(empty[0] == '\0');
	strsqz(NULL,1);
	strsqz(squeezed,0);
	strsqz(squeezed,-1);
	assert(!strcmp(squeezed,"abcdef"));
	strsqz(squeezed,2);
	assert(!strcmp(squeezed,"cdef"));
	strsqz(squeezed,20);
	assert(!squeezed[0]);
	stripchar(remove_chars,'-');
	assert(!strcmp(remove_chars,"ab"));
	stripchar(NULL,'-');
	stripacute(multiple_accents);
	assert(!strcmp(multiple_accents,"ab"));
	stripacute(NULL);
	subchar(NULL,'a','b');
	subchar(replace_high_byte,0377,0200);
	assert((unsigned char)replace_high_byte[0] == 0200);

	assert(context);
	previous = morpheus_runtime_context_activate(context);

	beta2smarta("a/",converted);
	assert((unsigned char)converted[0] == 0213);
	assert(converted[1] == '\0');
	smarta2beta(converted,roundtrip);
	assert(!strcmp(roundtrip,"$a/"));
	smk_entry = context->smk_beta_table[0];
	smarta_entry = context->smarta_beta_table[0];
	assert(init_smk());
	assert(context->smk_beta_table[0] == smk_entry);
	assert(context->smarta_beta_table[0] == smarta_entry);
	smk2beta(converted,roundtrip);
	assert(!strcmp(roundtrip,"a/"));
	smarta2beta(uppercase_accented,roundtrip);
	assert((unsigned char)roundtrip[0] == '$');
	assert((unsigned char)roundtrip[1] == 0200);
	assert((unsigned char)roundtrip[2] == 'a');
	assert(roundtrip[3] == '\0');
	assert(!is_blank(high_byte_key));
	assert(nextkey(high_byte_key,key));
	assert((unsigned char)key[0] == 0377);
	assert(key[1] == '\0');
	assert(!strcmp(high_byte_key,"a"));
	assert(beta_tolower(high_byte_word));
	assert(high_byte_word[0] == 'a');
	assert((unsigned char)high_byte_word[1] == 0377);
	assert(high_byte_word[2] == '\0');
	assert(!beta_tolower(NULL));
	assert(!beta_tolower(marker_only));
	assert(!strcmp(marker_only,"*"));
	assert(normucase(normalized));
	assert(!strcmp(normalized,"E(/llhn"));
	assert(!normucase(NULL));
	assert(!normucase(marker_only));
	assert(!strcmp(marker_only,"*"));

	beta2smarta("i+",converted);
	assert((unsigned char)converted[0] == 0363);
	assert(converted[1] == '\0');

	beta2smarta("r(",converted);
	assert((unsigned char)converted[0] == 0373);
	assert(converted[1] == '\0');

	smk2beta(NULL,roundtrip);
	assert(!roundtrip[0]);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);
	smk2beta("a",NULL);
	assert(morpheus_runtime_context_error(context) ==
	       MORPHEUS_RUNTIME_ERROR_INTERNAL);
	morpheus_runtime_context_clear_error(context);

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(context);
	return 0;
}
