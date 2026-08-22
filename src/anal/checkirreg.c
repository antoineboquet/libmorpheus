#include "anal_internal.h"
#define MAXIRREGS 10
#include "../morphlib/runtime_context_internal.h"

#include "checkirreg.proto.h"


static int
initialize_irregular_buffers(morpheus_runtime_context *context)
{
	int i;

	if (context->analysis_irregular_buffers_initialized) return(1);
	for (i = 0; i < MORPHEUS_IRREGULAR_FORM_COUNT; i++) {
		if (!context->analysis_irregular_forms[i])
			context->analysis_irregular_forms[i] =
				malloc((size_t)MAXWORDSIZE);
		if (!context->analysis_irregular_keys[i])
			context->analysis_irregular_keys[i] =
				malloc((size_t)LONGSTRING);
		if (!context->analysis_irregular_forms[i] ||
		    !context->analysis_irregular_keys[i])
			goto no_memory;
	}
	context->analysis_irregular_buffers_initialized = 1;
	return(1);

no_memory:
	for (i = 0; i < MORPHEUS_IRREGULAR_FORM_COUNT; i++) {
		free(context->analysis_irregular_forms[i]);
		free(context->analysis_irregular_keys[i]);
		context->analysis_irregular_forms[i] = NULL;
		context->analysis_irregular_keys[i] = NULL;
	}
	morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
	return(0);
}

int try_irregvb(gk_word *Gkword)
{
	gk_word Workword;
	char *saveirrform = NULL;
	char *fullpreverb;
	int unasp_prev = 0;

	int rval = 0;
	int i;
	char * irrform;
	char * rawprvb;
	morpheus_runtime_context *context = morpheus_runtime_context_current();
	char **IrrForms = context->analysis_irregular_forms;
	char **IrrKeys = context->analysis_irregular_keys;
	
	saveirrform = (char *)malloc((size_t)MAXWORDSIZE);
	if (!saveirrform || !initialize_irregular_buffers(context)) {
		morpheus_runtime_error_record(MORPHEUS_RUNTIME_ERROR_NO_MEMORY);
		free(saveirrform);
		return(0);
	}
	*saveirrform = 0;
	for(i=0;i<MORPHEUS_IRREGULAR_FORM_COUNT;i++) {
		*IrrForms[i] = 0;
		*IrrKeys[i] = 0;
	}

	Workword = * Gkword;
	rawprvb = rawprvb_of(&Workword);
	fullpreverb = preverb_of(&Workword);
	irrform = stem_of(&Workword);

	set_morphflag(morphflags_of(prvb_gstr_of(&Workword)),0);
	* fullpreverb = *IrrKeys[0] = *IrrKeys[1] = *IrrKeys[2] = 0;

/*
 * start off with some kind of check to make sure that you have a 
 * viable preverb here before you waste time running off to some file
 * on disk
 */
 

	if( *rawprvb ) {
		if( ! is_preverb(rawprvb,fullpreverb,prvb_gstr_of(&Workword)) ) {
			rval = 0;

			goto finish;
		}
		add_morphflag(morphflags_of(stem_gstr_of(&Workword)),HAS_PREVERB);
	}
	
	if( has_morphflag(morphflags_of(prvb_gstr_of(&Workword)), APOCOPE )) {
		if( AndDialect(dialect_of(&Workword),(Dialect)(PROSE)) > 0 )
			goto finish;
	}
/*
	add_morphflags(&Workword,morphflags_of(prvb_gstr_of(&Workword)));
*/


	if( *rawprvb && ! CombPbStem(rawprvb,irrform,dialect_of(&Workword),morphflags_of(prvb_gstr_of(&Workword))) ) {

		rval = 0;
		goto finish;
	}
	add_morphflags(stem_gstr_of(&Workword),morphflags_of(prvb_gstr_of(&Workword)));

/*
 * well let's see if we have a raw irregular verb 
 */
	if( ! *rawprvb || Is_cons(*irrform) || cur_lang() == LATIN ) {
		rval = chckirrvform(irrform,IrrKeys[0]);
		
/*
printf("rval b %d irrform [%s] irkkeys [%s]\n", rval , irrform, IrrKeys[0] );
*/
		if( rval ) {
			rval = ChckIrrLemms(&Workword,irrform,IrrKeys[0]);
			goto finish;
		}
	}

	if( ! Is_vowel(*irrform) )
		goto finish;

	if( getbreath(irrform) != NOBREATH  ) {
		rval = chckirrvform(irrform,IrrKeys[0]);
		if( rval )
			Xstrncpy(IrrForms[0],irrform,MAXWORDSIZE);
		else
			*IrrForms[0] = 0;
	}
/*
 * at this point, we should have no breathing marks (that would only
 * happen if we were at the start of a word and "*irrform" had evaluated to
 * 0 above)
 *
 */
	Xstrncpy(saveirrform,irrform,MAXWORDSIZE);
/*
 * if you don't have an aspirated preverb, try a smooth breathing
 * on the irrform 
 */

	if( ((cur_lang() != LATIN &&
	      (!*rawprvb || !Is_asp(*(rawprvb + Xstrlen(rawprvb) - 1)))) ||
	     mfi_prvb(rawprvb)) && getbreath(irrform) == NOBREATH) {
		addbreath(irrform,SMOOTHBR);
		if( has_morphflag(morphflags_of(stem_gstr_of(&Workword)),UNASP_PREVERB) ) {
			unasp_prev = 1;
			zap_morphflag(morphflags_of(stem_gstr_of(&Workword)),UNASP_PREVERB);
		}
			
		rval += chckirrvform(irrform,IrrKeys[0]);

		if( unasp_prev ) {
			unasp_prev  = 0;
			add_morphflag(morphflags_of(stem_gstr_of(&Workword)),UNASP_PREVERB);
		}
		if( rval )
			Xstrncpy(IrrForms[0],irrform,MAXWORDSIZE);
		else
			*IrrForms[0] = 0;
	}

	Xstrncpy(irrform,saveirrform,MAXWORDSIZE);

	/*
	 * don't go for a smooth breathing if the 
	 * preverb ends in 'p',
	 *	e.g. don't look for "w(=si" if you have
	 *		"a)pw=si" or "e)pw=si"
	 *
	 */

/*
 * grc 7/27/89
 *
 * NO! make sure that you can analyze forms such as ionic a)p-i/hmi
 */
	if( cur_lang() != LATIN && cur_lang() != ITALIAN &&  /* *(rawprvb + Xstrlen(rawprvb) - 1) != 'p'&& */getbreath(irrform) == NOBREATH
/*
 * grc 12/5/90
 *
 * make sure that you don't accept forms such as "e/s" (as equivalent to "e(/s",
 * aor imperat act of i(/hmi
 */
		&& ((!Is_vowel(*irrform)) || (*rawprvb)) ) {
		addbreath(irrform,ROUGHBR);
		rval = chckirrvform(irrform,IrrKeys[1]);

		if( rval ) 
			Xstrncpy(IrrForms[rval],irrform,MAXWORDSIZE);
		else
			*IrrForms[1] = 0;
	}
	Xstrncpy(irrform,saveirrform,MAXWORDSIZE);

	rval = 0;




	if( *IrrForms[1] )
		rval += ChckIrrLemms(&Workword,IrrForms[1],IrrKeys[1]);

	rval += ChckIrrLemms(&Workword,IrrForms[0],IrrKeys[0]);


	finish:
		xFree(saveirrform,"saveirrform");
/*
		xFree(fullpreverb,"fullpreverb");
*/
		saveirrform = fullpreverb = NULL;
		if( rval ) 
			CpGkAnal(Gkword,&Workword);
		return(rval);
}


/*
 * ok, given an irregular verb stem, go check the various lemmas
 * associated with it (e.g., "ei)=mi" and "ei)mi" for "ei)si")
 */


int ChckIrrLemms(gk_word *Gkword, char *irrform, char *irrkey)
{
	register char * sp;
	char stemkeys[LONGSTRING];
	char curlemma[LONGSTRING];
	char tmpword[LONGSTRING];
	char * curkeys;
	gk_word TmpGkword;
	int rval = 0;
	int curval = 0;


	while( nextkey(irrkey,curlemma) ) {
		sp = curlemma;
		TmpGkword = * Gkword;


		sp = parsefield(sp,tmpword,':',LONGSTRING);
		if( tmpword[0] ) {
			Xstrncpy(irrform,tmpword,LONGSTRING);
		} else {
			Xstrncpy(irrform,stem_of(Gkword),MAXWORDSIZE);
			stripacc(irrform);
			stripquant(irrform);
			stripdiaer(irrform);
		}
		sp = parsefield(sp,lemma_of(&TmpGkword),':',LONGSTRING);
		sp = parsefield(sp,stemkeys,' ',LONGSTRING);
		subchar(stemkeys,':',' ');

		curval += CheckIrregForm(&TmpGkword,irrform,stemkeys);

		if( curval ) {
			CpGkAnal(Gkword,&TmpGkword);
		}
		rval += curval;
	}
	return(rval);
}

int CheckIrregForm(gk_word *Gkword, char *stem, char *stemkeys)
{
	gk_word * Forms = NULL;
	gk_word StemForms;
	gk_string Gstr;
	char * prevb = preverb_of(Gkword);
	char * pbptr;
	int rval = 0;

	StemForms = (*Gkword);
	set_stem(&StemForms,stem);

	if( * prevb ) {
		pbptr =is_substring(stemkeys,"pb:");
		if( pbptr == NULL )  {
			Xstrncat(stemkeys," pb:",LONGSTRING);
			Xstrncat(stemkeys,prevb,LONGSTRING);
		}
		/*
		 * if this stem already has a preverb and it differs
		 * from the one that you assume here, don't bother going
		 * any further.
		 */
		else if(Xstrncmp(pbptr+3,prevb,Xstrlen(prevb)))
			return(0);
	
	}

	Forms = GenIrregForm(&StemForms,stemkeys,0);
	
	set_morphflags(&Gstr,morphflags_of(Gkword));
	if( Forms ) {
		rval = CheckGenWords(Gkword,Forms);
		FreeGkString((gk_string *)Forms);
	}
	set_gwmorphflags(Gkword,morphflags_of(&Gstr));
	return(rval);
}

int chckirrvform(char *form, char *keys)
{
	char tmpform[MAXWORDSIZE];
	int rval;
	int curacc, cursyll;
	
/*
	if( (rval=chckirrverb(form,keys)) )
			return(rval);

	stripacc(form);
*/
	rval=chckirrverb(form,keys);
	return(rval);

}

int mfi_prvb(char *rawprvb)
{
	size_t slen = Xstrlen(rawprvb);
	
	if (slen < 2) return(0);
	if( rawprvb[slen-1] == 'f' && rawprvb[slen-2] == 'm' ) return(1);
	return(0);
}
