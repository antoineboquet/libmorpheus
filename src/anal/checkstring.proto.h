
/* checkstring.c */
int checkstring(char *, PrntFlags, FILE *);
int checkstring1(gk_word *);
int checkstring2(gk_word *);
int checkstring3(gk_word *);
int has_cun(char *);
int checkapostr(gk_word *);
int has_tt(char *);
void SetWantDialect(Dialect);
void AddWantDialect(Dialect);
void ZapWantDialect(Dialect);
Dialect GetWantDialect(void);
int updateDialect(Dialect);
int u2v(char *);
void setepic(void);
void setatticprose(void);
