#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <gkstring.h>

#include "../src/greeklib/keyio.proto.h"
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
	morpheus_stemlib_offset key_offset = 0;
	unsigned short truncated_short = 0;
	char key[KEYLEN] = {0};
	unsigned char header[8];
	unsigned char stored_form[4];
	const unsigned char expected_header[8] = {
		0xc1,0x19,0xa6,0x02,0x08,0x00,0x00,0x00
	};
	const unsigned char expected_form[4] = {0x09,0x16,0x01,0x00};

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
	assert(fseek(stream,(long)(sizeof header+(size_t)maxend),SEEK_SET) == 0);
	assert(fread(stored_form,1,sizeof stored_form,stream) == sizeof stored_form);
	assert(!memcmp(stored_form,expected_form,sizeof stored_form));
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
	assert(WriteEnding(NULL,&source,maxend) == -1);
	assert(WriteEnding(stream,NULL,maxend) == -1);
	assert(WriteEnding(stream,&source,0) == -1);
	assert(WriteEnding(stream,&source,MAXWORDSIZE+1) == -1);
	assert(ReadEnding(NULL,&result,maxend) == -1);
	assert(ReadEnding(stream,NULL,maxend) == -1);
	assert(ReadEnding(stream,&result,0) == -1);
	assert(set_endheader(NULL,maxend) == -1);
	assert(set_endheader(stream,0) == -1);
	stored_maxend = 7;
	assert(get_endheader(NULL,&stored_maxend) == -1);
	assert(stored_maxend == 0);
	assert(get_endheader(stream,NULL) == -1);
	fclose(stream);

	stream = fopen(argv[1],"w+b");
	assert(stream);
	assert(fputc(0x44,stream) != EOF);
	assert(fseek(stream,0L,SEEK_SET) == 0);
	strcpy(gkstring_of(&result),"unchanged");
	assert(ReadEnding(stream,&result,maxend) < 1);
	assert(!strcmp(gkstring_of(&result),"unchanged"));
	assert(fseek(stream,0L,SEEK_SET) == 0);
	assert(vax_fread(&truncated_word,sizeof truncated_word,1,stream) == 0);
	truncated_word = UINT32_C(0xdeadbeef);
	assert(fseek(stream,0L,SEEK_SET) == 0);
	assert(get_int32(&truncated_word,stream) == 0);
	assert(truncated_word == UINT32_C(0xdeadbeef));
	truncated_short = 0xbeefU;
	assert(fseek(stream,0L,SEEK_SET) == 0);
	assert(get_short(&truncated_short,stream) == 0);
	assert(truncated_short == 0xbeefU);
	assert(vax_fread(&truncated_word,sizeof truncated_word,-1,stream) == -1);
	assert(vax_fread(NULL,1,0,NULL) == 0);
	assert(vax_fwrite(NULL,1,0,NULL) == 0);
	assert(vax_fread(NULL,1,1,stream) == -1);
	assert(vax_fread(&truncated_word,1,1,NULL) == -1);
	assert(vax_fwrite(NULL,1,1,stream) == -1);
	assert(vax_fwrite(&truncated_word,1,1,NULL) == -1);
	assert(!get_int32(NULL,stream));
	assert(!get_int32(&truncated_word,NULL));
	assert(!put_int32(NULL,stream));
	assert(!put_int32(&truncated_word,NULL));
	assert(!get_short(NULL,stream));
	assert(!get_short(&truncated_short,NULL));
	assert(!put_short(NULL,stream));
	assert(!put_short(&truncated_short,NULL));
	assert(WriteKey(NULL,&key_offset,stream) == -1);
	assert(WriteKey(key,NULL,stream) == -1);
	assert(WriteKey(key,&key_offset,NULL) == -1);
	assert(ReadKey(NULL,&key_offset,stream) == -1);
	assert(ReadKey(key,NULL,stream) == -1);
	assert(ReadKey(key,&key_offset,NULL) == -1);
	fclose(stream);
	assert(remove(argv[1]) == 0);

	return 0;
}
