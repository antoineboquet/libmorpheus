/* SPDX-License-Identifier: MPL-2.0 */

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libfiles.h>

#include "compostypes.h"
#include "gkends_internal.h"
#include "expendtable.proto.h"

static void usage(const char *program)
{
	fprintf(stderr,"usage: %s [-I|-L] {all|DERIVATION}\n",program);
}

static int build_all_derivations(void)
{
	FILE *input;
	char path[MAXPATHNAME];
	char line[LONGSTRING];
	char derivation[LONGSTRING];
	int status=0;

	Xstrcpy(path,DERIVTYPES);
	input=MorphFopen(path,"r");
	if(!input) {
		fprintf(stderr,"could not open [%s]\n",path);
		return -1;
	}
	while(fgets(line,sizeof line,input)) {
		if(!isalpha((unsigned char)line[0]))
			continue;
		nextkey(line,derivation);
		if(expendtables(derivation,1,DODERIV)<0) {
			status=-1;
			break;
		}
	}
	if(ferror(input))
		status=-1;
	if(fclose(input)==EOF)
		status=-1;
	return status;
}

int main(int argc, char *argv[])
{
	char *derivation;
	int option;
	int status;

	while((option=getopt(argc,argv,"IL"))!=-1) {
		switch(option) {
		case 'I':
			set_lang(ITALIAN);
			break;
		case 'L':
			set_lang(LATIN);
			break;
		default:
			usage(argv[0]);
			return EXIT_FAILURE;
		}
	}
	if(optind+1!=argc) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	derivation=argv[optind];
	status=!strcmp(derivation,"all") ? build_all_derivations()
	                                      : expendtables(derivation,1,DODERIV);
	return status<0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
