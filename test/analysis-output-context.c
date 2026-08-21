#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <gkstring.h>
#include <prntflags.h>

#include "../src/anal/prntanal.proto.h"
#include "../src/morphlib/runtime_context.h"

static void
render_empty_analysis(const char *word)
{
	gk_analysis analysis = { 0 };
	gk_word candidate = { 0 };

	set_analysis(&candidate,&analysis);
	set_rawword(&candidate,word);
	set_stem(&candidate,word);
	assert(!PrntAnalyses(&candidate,KEEP_BETA,stdout));
}

static void
assert_analysis_sort_is_stable(void)
{
	gk_analysis analyses[4] = { 0 };

	set_lemma(&analyses[0],"zeta");
	set_rawword(&analyses[0],"zeta-first");
	set_lemma(&analyses[1],"alpha");
	set_rawword(&analyses[1],"alpha-first");
	set_lemma(&analyses[2],"zeta");
	set_rawword(&analyses[2],"zeta-second");
	set_lemma(&analyses[3],"alpha");
	set_rawword(&analyses[3],"alpha-second");

	SortAnals(analyses,4);

	assert(!strcmp(rawword_of(&analyses[0]),"alpha-first"));
	assert(!strcmp(rawword_of(&analyses[1]),"alpha-second"));
	assert(!strcmp(rawword_of(&analyses[2]),"zeta-first"));
	assert(!strcmp(rawword_of(&analyses[3]),"zeta-second"));
}

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(greek);
	assert(latin);
	assert_analysis_sort_is_stable();
	previous = morpheus_runtime_context_activate(greek);
	assert(!anal_buf());
	render_empty_analysis("alpha");
	assert(strstr(anal_buf(),"alpha"));

	morpheus_runtime_context_activate(latin);
	assert(!anal_buf());
	render_empty_analysis("beta");
	assert(strstr(anal_buf(),"beta"));
	assert(!strstr(anal_buf(),"alpha"));

	morpheus_runtime_context_activate(greek);
	assert(strstr(anal_buf(),"alpha"));
	assert(!strstr(anal_buf(),"beta"));

	morpheus_runtime_context_activate(previous);
	morpheus_runtime_context_destroy(greek);
	morpheus_runtime_context_destroy(latin);
	return(0);
}
