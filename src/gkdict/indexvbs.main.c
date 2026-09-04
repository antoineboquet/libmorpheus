/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include <gkstring.h>

#include "indexstems.proto.h"
#include "../morphlib/setlang.proto.h"

int verbose=0;

static void usage(const char *program)
{
	fprintf(stderr,"usage: %s [-I|-L] INPUT OUTPUT\n",program);
}

int main(int argc, char *argv[])
{
	int option;

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
	if(optind+2!=argc) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	return index_stems(3,0,0,argv[optind],argv[optind+1],10)<0
	       ? EXIT_FAILURE : EXIT_SUCCESS;
}
