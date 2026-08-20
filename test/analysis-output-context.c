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

int
main(void)
{
	morpheus_runtime_context *greek = morpheus_runtime_context_create();
	morpheus_runtime_context *latin = morpheus_runtime_context_create();
	morpheus_runtime_context *previous;

	assert(greek);
	assert(latin);
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
