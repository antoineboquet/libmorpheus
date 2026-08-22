#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <gkstring.h>

#include "../src/greeklib/vaxwords.proto.h"
#include "../src/morphlib/endio.proto.h"
#include "../src/morphlib/morphflags.proto.h"

int main(int argc, char **argv)
{
	FILE *stream;
	gk_string source = {0};
	gk_string result = {0};
	gk_string group_source = {0};
	gk_string group_result = {0};
	word_form form = {0};
	int maxend = 8;
	int stored_maxend = 0;
	int32 truncated_word = 0;
	unsigned char header[8];
	const unsigned char expected_header[8] = {
		0xc1,0x19,0xa6,0x02,0x08,0x00,0x00,0x00
	};

	assert(argc == 2);
	stream = fopen(argv[1],"w+b");
	assert(stream);
	set_voice(form,ACTIVE);
	set_mood(form,INDICATIVE);
	set_tense(form,AORIST);
	set_person(form,2);
	set_number(form,PLURAL);
	forminfo_of(&source) = form;
	stemtype_of(&source) = (Stemtype)01234567;
	derivtype_of(&source) = (Derivtype)07654321;
	dialect_of(&source) = (Dialect)(ATTIC|IONIC);
	geogregion_of(&source) = (GeogRegion)(PHOCIS|LOCRIS);
	morphflags_of(&source)[0] = 0125;
	morphflags_of(&source)[MORPHFLAG_BYTES-1] = 0252;
	strcpy(domains_of(&source),"poetry");
	strcpy(gkstring_of(&source),"luw");
	strcpy(gkstring_of(&group_source),"o(milo");
	add_morphflag(morphflags_of(&group_source),GROUP_NAME);
	assert(Is_group_name(morphflags_of(&group_source)));
	assert(domains_of(&group_source)[0] == 0);
	assert(domains_of(&group_source)[1] == 0);

	assert(set_endheader(stream,maxend) == 1);
	assert(WriteEnding(stream,&source,maxend) == 1);
	assert(WriteEnding(stream,&group_source,maxend) == 1);
	assert(fflush(stream) == 0);
	assert(fseek(stream,0L,SEEK_SET) == 0);
	assert(fread(header,1,sizeof header,stream) == sizeof header);
	assert(!memcmp(header,expected_header,sizeof header));
	assert(fseek(stream,0L,SEEK_SET) == 0);
	assert(get_endheader(stream,&stored_maxend) == 2);
	assert(stored_maxend == maxend);
	assert(ReadEnding(stream,&result,stored_maxend) == 1);
	assert(!memcmp(&source,&result,sizeof source));
	assert(ReadEnding(stream,&group_result,stored_maxend) == 1);
	assert(!memcmp(&group_source,&group_result,sizeof group_source));
	assert(Is_group_name(morphflags_of(&group_result)));
	assert(domains_of(&group_result)[0] == 0);
	assert(domains_of(&group_result)[1] == 0);
	assert(ReadEnding(stream,&result,stored_maxend) == 0);
	fclose(stream);

	stream = fopen(argv[1],"w+b");
	assert(stream);
	assert(fputc(0x44,stream) != EOF);
	assert(fseek(stream,0L,SEEK_SET) == 0);
	assert(vax_fread(&truncated_word,sizeof truncated_word,1,stream) == 0);
	assert(vax_fread(&truncated_word,sizeof truncated_word,-1,stream) == -1);
	fclose(stream);
	assert(remove(argv[1]) == 0);

	return 0;
}
