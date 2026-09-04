/* SPDX-License-Identifier: MPL-2.0 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "gkends_internal.h"
#include "indexendtables.proto.h"

static void usage(const char *program)
{
	fprintf(stderr,"usage: %s [-I|-L] [-f TABLE_LIST]\n",program);
}

int main(int argc, char *argv[])
{
	int option;
	char *table_list=NULL;

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
	if(optind!=argc) {
		usage(argv[0]);
		return EXIT_FAILURE;
	}
	return (table_list ? indexendtables_from_list(0,1,table_list)
	                   : indexendtables(0,1)) < 0
	       ? EXIT_FAILURE : EXIT_SUCCESS;
}
