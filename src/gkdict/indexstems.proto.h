
/* SPDX-License-Identifier: MPL-2.0 */

/* indexstems.c */
int zstrcmp(const void *p1, const void *p2);
int index_hqdict(int wantstem, int wantirrverb, int wantindecl);
int index_noms(int wantstem, int wantirrverb, int wantindecl);
int index_vbs(int wantstem, int wantirrverb, int wantindecl);
int index_stems(int wantstem, int wantirrverb, int wantindecl, char *wlist, char *indexlist, int indfreq);
int add_newstemkey(char *s);
int do_curstem(char *curstem, char *curlemma, char *curline, char *prefix);
int dumpaccstem(char *prefix, char *curstem, char *markedstem, char *curlemma, gk_string *gstr, gk_string *avoidgstr, int syllnum, char *preverb);
int dump_curstem(char *prefix, char *curstem, char *markedstem, char *curlemma, gk_string *gstr, gk_string *avoidgstr, char *preverb);
void clear_globs(char *s);
int is_presredupl(char *s);
int huh(void);
void *zogalloc(size_t count, size_t size);
