/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "gkends_internal.h"
#include "indexendtables.proto.h"

static void usage(const char *program)
{
	fprintf(stderr,"usage: %s [-I|-L]\n",program);
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
	if(optind!=argc) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	return indexendtables(0,1)<0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
