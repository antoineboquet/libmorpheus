/* SPDX-License-Identifier: MPL-2.0 */

#include "conjugation_internal.h"
#include <contract.h>

gk_string * ppass_table = NULL;
static int nppass = 0;

void makeppass(char * origstem, gk_string * gstr)
{
    char newstem[MAXWORDSIZE];
    char stemname[MAXWORDSIZE];



    if( ! get_ppasstype(origstem,newstem,stemname) ) return;
    conj_copy(origstem, newstem, MAXWORDSIZE);
    set_stemtype(gstr,GetStemNum(stemname));
}

int get_ppasstype(char * stem, char * newstem, char * stemname)
{
    int  i;
    char * p;

    if( Is_vowel(*(lastn(stem,1)))  )
        return(0);
    conj_copy(newstem, stem, MAXWORDSIZE);
    conj_copy(stemname, "perfp_vow", MAXWORDSIZE);
    if( ! ppass_table ) {
        ppass_table = load_euph_tab(PPASSLIST,&nppass,NO);

        if (!ppass_table) conj_fail("missing passive-stem rules");
    }
    for(i=0;i<nppass;i++) {
        p = gkstring_of(ppass_table+i);
        if( ends_in(stem,p) ) {
            *(lastn(newstem,strlen(p))) = 0;
            conj_copy(stemname, p+MAXSUBSTRING, MAXWORDSIZE);

            return(1);
        }

    }
    return(0);
}


