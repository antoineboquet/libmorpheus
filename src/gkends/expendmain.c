/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compostypes.h"
#include "gkends_internal.h"
#include "expendtable.proto.h"
#include "../morphlib/morphkeys.proto.h"

static void usage(const char *program)
{
	fprintf(stderr,"usage: %s [-I|-L] {all|nom|verb|TABLE}\n",program);
}

int main(int argc, char *argv[])
{
	char *curtable;
	char *table;
	int index=0;
	int option;
	Stemtype stype=0;

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
	table=argv[optind];
	if(!strcmp(table,"all"))
		stype=NOUNSTEM|ADJSTEM|PPARTMASK;
	else if(!strcmp(table,"nom"))
		stype=NOUNSTEM|ADJSTEM;
	else if(!strcmp(table,"verb"))
		stype=PPARTMASK;
	else
		return expendtables(table,strcmp(table,"formulaX")!=0,DOEND)<0
		       ? EXIT_FAILURE : EXIT_SUCCESS;

	while((curtable=NextEndTable(&index,stype))!=NULL) {
		if(expendtables(curtable,1,DOEND)<0)
			return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
