#include <gkstring.h>
#include "gkends_internal.h"
#include "../morphlib/runtime_context_internal.h"
#include <libfiles.h>
static int OpenDerivFile(morpheus_runtime_context *);

char * 
NextSuffTable(char *entry)
{
		morpheus_runtime_context *context = morpheus_runtime_context_current();
		size_t length;

		if( ! context->suffix_table_file ) {
			
			if( context->suffix_table_unavailable )
				return(NULL);
			if( ! OpenDerivFile(context) )
				return(NULL);
		}
		if( ! fgets(entry,MAXPATHNAME,context->suffix_table_file) ) {
			fclose(context->suffix_table_file);
			context->suffix_table_file = NULL;
			return(NULL);
		}
		length = strlen(entry);
		if (length && entry[length-1] == '\n') entry[length-1] = 0;
		return(entry);
}

static int
OpenDerivFile(morpheus_runtime_context *context)
{
	context->suffix_table_file = MorphFopen(DERIVTYPES,"r");
	if( ! context->suffix_table_file )
		context->suffix_table_unavailable = 1;
	return( context->suffix_table_unavailable == 0 );
}
