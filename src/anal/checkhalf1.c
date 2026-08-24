#include "anal_internal.h"
#include "../morphlib/runtime_context_internal.h"

#include "checkhalf1.proto.h"

extern int verbose;

static int
valid_stem_argument(const void *argument)
{
	if (argument) return(1);
	morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
	return(0);
}

/*
 * this routine gets handed possible stems that have been stripped
 * of preverbs. it checks to see if we have a stem beginning in a vowel
 * but lacking a breathing. if so, it tries both a smooth and a rough
 * breathing.
 */
int checkhalf1(gk_word *Gkword, char *endkeys)
{
	int rval = 0;
	char * stem;
	char savestem[MAXWORDSIZE];
	int unasp_prev = 0;

	if (!valid_stem_argument(Gkword) ||
	    !valid_stem_argument(endkeys))
		return(0);
	stem = stem_of(Gkword);

/*
printf("half1 stem preverb [%s] stem [%s] end [%s]\n", preverb_of(Gkword) , stem_of(Gkword), endstring_of(Gkword));
*/
	Xstrcpy(savestem,stem);
	if( *stem == 'r'  && getbreath(stem) == NOBREATH && cur_lang() == GREEK) {
		char tmp[MAXWORDSIZE];

		Xstrncpy(tmp,"r(",(int)sizeof tmp);
		Xstrncat(tmp,stem+1,(int)sizeof tmp);
		Xstrncpy(stem,tmp,MAXWORDSIZE);
		rval=checkhalf2(Gkword,endkeys);
		return(rval);
	}


	if( cur_lang() == GREEK && Is_vowel(*stem) && getbreath(stem) == NOBREATH  ) {

		Xstrncpy(savestem,stem_of(Gkword),(int)sizeof savestem );
/*
 * check for rough breathing
 */

		addbreath(stem,ROUGHBR);


		rval+=checkhalf2(Gkword,endkeys);
/*
 * if that fails, try for smooth 
 *
 * no! -- don't quit:  otherwise, dielw= gets analyzed as di+aire/w (e(lw= is a late
 * attic form), and we don't get the common attic analysis that derives this from 
 * dielau/nw.  grc 5/20/89
 */
		set_stem(Gkword,savestem);
		addbreath(stem,SMOOTHBR);

		if( has_morphflag(morphflags_of(prvb_gstr_of(Gkword)),UNASP_PREVERB) ) {
			unasp_prev = 1;
			zap_morphflag(morphflags_of(prvb_gstr_of(Gkword)),UNASP_PREVERB);
		}
			
		rval += checkhalf2(Gkword,endkeys);
		if( unasp_prev ) {
			unasp_prev  = 0;
			add_morphflag(morphflags_of(prvb_gstr_of(Gkword)),UNASP_PREVERB);
		}
/*
 *  grc 6/9/89 -- ok now check to see if it begins with a diphthong and, if so, whether
 *  putting a diaeresis in the diphth helps things. we need to do this because otherwise
 *  when the parser sees "a)mf-ai/ssomai" it looks for ai)ss-, but not a)iss- (which is 
 *  the proper form
 */
	 	if( ! rval && starts_w_diphth(stem) ) {
	 		char diaerstem[MAXWORDSIZE];
	 		
	 		
		strncpy(diaerstem,savestem,2);
		diaerstem[2] = 0;
		stripdiaer(diaerstem);
		if (!Xstrncat(diaerstem,"+",sizeof diaerstem) ||
		    !Xstrncat(diaerstem,savestem+2,sizeof diaerstem)) {
			morpheus_runtime_error_record(
				MORPHEUS_RUNTIME_ERROR_INTERNAL);
			set_stem(Gkword,savestem);
			return(rval);
		}
			set_stem(Gkword,diaerstem);
	/*
	 * check for rough breathing
	 */
			addbreath(stem,ROUGHBR);
		
			rval+=checkhalf2(Gkword,endkeys);

			set_stem(Gkword,diaerstem);
	/*
	 * check for smooth breathing
	 */
			if( has_morphflag(morphflags_of(prvb_gstr_of(Gkword)),UNASP_PREVERB) ) {
				unasp_prev = 1;
				zap_morphflag(morphflags_of(prvb_gstr_of(Gkword)),UNASP_PREVERB);
			}
			
			addbreath(stem,SMOOTHBR);
			rval+=checkhalf2(Gkword,endkeys);
		
			if( unasp_prev ) {
				unasp_prev  = 0;
				add_morphflag(morphflags_of(prvb_gstr_of(Gkword)),UNASP_PREVERB);
			}
			set_stem(Gkword,savestem);
		}
 		
		return(rval);
	} else if( (rval=checkhalf2(Gkword,endkeys))) {
		return(rval);
	} else if( cur_lang() != LATIN && rawprvb_of(Gkword)[0] && starts_w_diphth(stem) ) {
		int cbreath;
	 	char diaerstem[MAXWORDSIZE];
	/*
	 * grc 6/26/89
	 *
	 * check to see if you have a poorly manufactured breathing mark on
	 * a verb with a preverb, e.g. a)pai/ssw is guessed as a)p+ai)/ssw, rather
	 * than as a)p-a)i/ssw
	 */
	 	cbreath = getbreath(stem);
	 	stripbreath(stem);
	 	diaerstem[0] = *stem;
	 	diaerstem[1] = DIAERESIS;
	 	Xstrcpy(diaerstem+2,stem+1);
	 	addbreath(diaerstem,cbreath);
	 	set_stem(Gkword,diaerstem);
		rval+=checkhalf2(Gkword,endkeys);
		set_stem(Gkword,savestem);
	 		
	}
	if( ! rval && preverb_of(Gkword)[0] && getbreath(savestem)==SMOOTHBR && 
		(AndDialect(dialect_of(Gkword),(Dialect)IONIC)>=0) &&
		 cur_lang() != LATIN  ) {
			add_morphflag(morphflags_of(Gkword),UNASP_PREVERB);
			Xstrcpy(stem,savestem);
			stripbreath(stem);
			addbreath(stem,ROUGHBR);
			rval+=checkhalf2(Gkword,endkeys);
			set_stem(Gkword,savestem);
			zap_morphflag(morphflags_of(Gkword),UNASP_PREVERB);
	}
	return(rval);
}

static int
initialize_possible_stems(morpheus_runtime_context *context)
{
	int i;

	if (context->analysis_possible_stems_initialized) return(1);
	for (i = 0; i < MORPHEUS_POSSIBLE_STEM_COUNT; i++) {
		if (!context->analysis_possible_stems[i])
			context->analysis_possible_stems[i] = CreatGkString(1);
		if (!context->analysis_possible_keys[i])
			context->analysis_possible_keys[i] = malloc((size_t)LONGSTRING);
		if (!context->analysis_possible_stems[i] ||
		    !context->analysis_possible_keys[i])
			goto no_memory;
	}
	context->analysis_possible_stems_initialized = 1;
	return(1);

no_memory:
	for (i = 0; i < MORPHEUS_POSSIBLE_STEM_COUNT; i++) {
		if (context->analysis_possible_stems[i])
			FreeGkString(context->analysis_possible_stems[i]);
		free(context->analysis_possible_keys[i]);
		context->analysis_possible_stems[i] = NULL;
		context->analysis_possible_keys[i] = NULL;
	}
	morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
	return(0);
}

int checkhalf2(gk_word *Gkword, char *endkeys)
{
	int i;
	int rval = 0;
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	gk_string **poss_stems = context->analysis_possible_stems;
	char **poss_keys = context->analysis_possible_keys;

	if (!valid_stem_argument(Gkword) ||
	    !valid_stem_argument(endkeys))
		return(0);

	if (!initialize_possible_stems(context)) return(0);

	for(i=0;i<MORPHEUS_POSSIBLE_STEM_COUNT;i++) {
		ClearGkstring(poss_stems[i]);
/*
		set_gkstring(poss_stems[i],"");
*/
		*poss_keys[i] = 0;
	}
	rval = checkstem(stem_of(Gkword),endkeys,poss_stems,poss_keys,
		MORPHEUS_POSSIBLE_STEM_COUNT-1);

/*
fprintf(stderr,"rval %d for pb [%s] stem [%s] endkeys [%s]\n", rval, preverb_of(Gkword), stem_of(Gkword) , endkeys );
*/
	if( rval ) {
		if((rval=StemsWork(Gkword,poss_stems,poss_keys,rval)))
			goto finish;
	}
	finish:
		if( rval ) return(1);
		else return(0);
}

int StemsWork(gk_word *Gkword, gk_string *poss_stems[], char *poss_keys[], int stem_num)
{
	char savestem[MAXWORDSIZE];
	int i, rval, result;
	result = 0;
	if (!valid_stem_argument(Gkword) ||
	    !valid_stem_argument(poss_stems) ||
	    !valid_stem_argument(poss_keys) || stem_num < 0) {
		if (stem_num < 0)
			morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_INTERNAL);
		return(0);
	}
	for (i = 0; i < stem_num; i++) {
		if (!valid_stem_argument(poss_stems[i]) ||
		    !valid_stem_argument(poss_keys[i]))
			return(0);
	}
	
	Xstrncpy(savestem,stem_of(Gkword),(int)sizeof savestem);

/*
fprintf(stderr,"stem_num [%d]\n", stem_num );
*/
	for(i=0;i<stem_num;i++) {
		if ((rval = StemWorks(Gkword,poss_keys[i],poss_stems[i])) != 0)
			result += rval;
	}
	if( ! result )
		set_stem(Gkword,savestem);

	return(result);
}

int StemWorks(gk_word *Gkword, char *posskey, gk_string *possstem)
{
	int rval = 0;
	int curval = 0;
	char *workkey = NULL;
	char *curkey = NULL;

	if (!valid_stem_argument(Gkword) ||
	    !valid_stem_argument(posskey) ||
	    !valid_stem_argument(possstem))
		return(0);
	

	workkey = (char *)malloc((size_t)BUFSIZ*2);
	curkey = (char *)malloc((size_t)BUFSIZ*2);
	if (!workkey || !curkey) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
		goto finish;
	}
	Xstrncpy(workkey,posskey, BUFSIZ*2);

	while(nextkey(workkey,curkey) ) {
/*
		set_stem(Gkword,possstem);
*/
/*fprintf(stderr,"possstem [%s] stemtype [%s] key {%s]\n", possstem,  curkey );*/
		curval = checkdict(Gkword,possstem,curkey);
		rval += curval;

	}
	finish:
		if (workkey) xFree(workkey,"workkey");
		if (curkey) xFree(curkey,"curkey");
		curkey = workkey = NULL;
	return(rval);
}
