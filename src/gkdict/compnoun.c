#include <stdio.h>
#include "gkdict_internal.h"
#include "../morphlib/runtime_context_internal.h"

void
checkforcompnoun(char * curstem,char * endkeys,char * stemkeys)
{
	char *s = curstem;
	char headkeys[BUFSIZ];
	int n = 0;
	char firsth[BUFSIZ];

	if(*s) s++;

	while(*s) {
		if( strlen(s) < 3 ) break;
		if( (Is_vowel(*(s-1))||Is_vowel(*s) ||Is_breath(*(s-1)) ) && is_nomhead(s,headkeys) ) {
			printf("[%s] [%s] [%s]\n", curstem, headkeys , endkeys);
			n = comstemtypes(curstem,headkeys,endkeys);
			Xstrcpy(firsth,curstem);
			firsth[strlen(curstem)-strlen(s)] = 0;
			if(n) {
				char * p;
				p = headkeys;
				while(*p) {
					while(isspace((unsigned char)*p)) p++;
					if(!*p) break;
					printf("%s-", firsth);
					if(*p==':') p++;
					while(*p&&!isspace((unsigned char)*p)) putchar(*p++);
					putchar('\n');
				}
			}
		}
		s++;
	}
}

#define MAXTAILS 10000
#define INITIAL_HEAD_CAPACITY 64

static void
clear_headtab(morpheus_runtime_context *context)
{
	int i;

	for (i = 0; i < context->compound_head_count; i++)
		free(context->compound_head_table[i]);
	free(context->compound_head_table);
	context->compound_head_table = NULL;
	context->compound_head_count = 0;
	context->compound_head_capacity = 0;
	context->compound_head_table_initialized = 0;
}

static int
append_head(morpheus_runtime_context *context, const char *line)
{
	char **new_table;
	char *entry;
	int capacity;
	size_t length = strlen(line);

	if (context->compound_head_count >= MAXTAILS) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	if (context->compound_head_count == context->compound_head_capacity) {
		capacity = context->compound_head_capacity ?
			context->compound_head_capacity * 2 : INITIAL_HEAD_CAPACITY;
		if (capacity > MAXTAILS) capacity = MAXTAILS;
		new_table = realloc(context->compound_head_table,
			(size_t)capacity * sizeof *new_table);
		if (!new_table) {
			morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
			return(0);
		}
		context->compound_head_table = new_table;
		context->compound_head_capacity = capacity;
	}
	entry = malloc(length);
	if (!entry) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
		return(0);
	}
	memcpy(entry,line+1,length);
	context->compound_head_table[context->compound_head_count++] = entry;
	return(1);
}

int
setup_headtab_stream(FILE *fheads)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	char line[1000];
	size_t length;

	if (context->compound_head_table_initialized) return(1);
	while(fgets(line,sizeof line,fheads) ) {
		if( line[0] != '#' ) continue;
		length = strlen(line);
		while (length && (line[length-1] == '\n' || line[length-1] == '\r'))
			line[--length] = 0;
		if (!append_head(context,line)) {
			clear_headtab(context);
			return(0);
		}
	}
	if (ferror(fheads)) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		clear_headtab(context);
		return(0);
	}
	context->compound_head_table_initialized = 1;
	return(1);
}

int
setup_headtab(void)
{
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	FILE * fheads;
	int result;

	if( context->compound_head_table_initialized ) return(1);
	if(!(fheads=fopen(COMPHEADS,"r"))) {
		fprintf(stderr,"could not open [%s]\n", COMPHEADS ); 
		return(0);
	}
	result = setup_headtab_stream(fheads);
	fclose(fheads);
	return(result);
}

int
is_nomhead(char * heads,char * headkeys)
{
	int rval = 0;
	char tmphead[BUFSIZ];
	char tmptab[BUFSIZ];
	char headentry[BUFSIZ];
	int i;
	char *s;
	morpheus_runtime_context *context = morpheus_runtime_context_current();

	if( ! context->compound_head_table_initialized && ! setup_headtab() )
		return(0);
	Xstrcpy(tmphead,heads);
	stripacc(tmphead);
	strcat(tmphead,"\t");

	headkeys[0] = 0;
	for(i=0;i<context->compound_head_count;i++) {
		Xstrcpy(tmptab,context->compound_head_table[i]);
		stripquant(tmptab);
		if( !strncmp(tmptab,tmphead,strlen(tmphead)) ) {
			Xstrcpy(headentry,context->compound_head_table[i]);
			s = headentry+strlen(tmphead)-1;
			while(isspace((unsigned char)*s)) *s++ = ':';	
			while(*s&&!isspace((unsigned char)*s)) s++;
			while(isspace((unsigned char)*s)) *s++ = ':';	
			strcat(headkeys,headentry);
			strcat(headkeys," ");
			rval = 1;
		} 
	}
	return(rval);
}
