/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "compostypes.h"
#include "gkends_internal.h"
#include "expendtable.proto.h"

static void usage(const char *program)
{
	fprintf(stderr,"usage: %s [-I|-L] < input > output\n",program);
}

int main(int argc, char *argv[])
{
	int option;
	char line[BUFSIZ];

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
	if(optind!=argc) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	if(!InitGstrMem()) {
		fprintf(stderr,"could not allocate ending storage\n");
		return EXIT_FAILURE;
	}

	while(fgets(line,sizeof line,stdin)) {
		if(!strncmp(line,":le:",4)) {
			PrntNewGstrings(stdout,0);
			ResetGstrBuf();
			if(fputs("\n",stdout)==EOF || fputs(line,stdout)==EOF)
				return EXIT_FAILURE;
			continue;
		}
		if(line[0]==':') {
			if(fputs(line,stdout)==EOF)
				return EXIT_FAILURE;
			continue;
		}
		if(is_blank(line) || Is_comment(line))
			continue;
		if(AddEndLine(line,"indeclform")<0)
			return EXIT_FAILURE;
	}
	if(ferror(stdin))
		return EXIT_FAILURE;
	PrntNewGstrings(stdout,0);
	ResetGstrBuf();
	return ferror(stdout) ? EXIT_FAILURE : EXIT_SUCCESS;
}
