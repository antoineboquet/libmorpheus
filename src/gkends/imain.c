/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gkends_internal.h"
#include "indexendtables.proto.h"

static void usage(const char *program)
{
	fprintf(stderr,
	        "usage: %s [-I|-L] [-f TABLE_LIST] {nom|verb|STEMTYPE}\n",
	        program);
}

int main(int argc, char *argv[])
{
	gk_string parsed={0};
	gk_word word={0};
	char *type;
	char *table_list=NULL;
	int option;
	Stemtype stype;

	while((option=getopt(argc,argv,"ILf:"))!=-1) {
		switch(option) {
		case 'I':
			set_lang(ITALIAN);
			break;
		case 'L':
			set_lang(LATIN);
			break;
		case 'f':
			if(table_list) {
				usage(argv[0]);
				return EXIT_FAILURE;
			}
			table_list=optarg;
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
	type=argv[optind];
	if(!strcmp(type,"nom"))
		stype=NOUNSTEM|ADJSTEM;
	else if(!strcmp(type,"verb"))
		stype=PPARTMASK;
	else {
		if(ScanAsciiKeys(type,&word,&parsed,NULL)<0) {
			fprintf(stderr,"invalid stem type: %s\n",type);
			return EXIT_FAILURE;
		}
		stype=stemtype_of(&parsed);
		if(stype&(NOUNSTEM|ADJSTEM))
			stype=NOUNSTEM|ADJSTEM;
		else if(stype&PPARTMASK)
			stype=PPARTMASK;
		else {
			fprintf(stderr,"unsupported stem type: %s\n",type);
			return EXIT_FAILURE;
		}
	}
	return (table_list ? indexendtables_from_list(stype,0,table_list)
	                   : indexendtables(stype,0)) < 0
	       ? EXIT_FAILURE : EXIT_SUCCESS;
}
