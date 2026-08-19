
/* indkeys.c */
int index_list(char *, char *, int);
#ifdef DECALPHA
void prockeyline(char *, int, int, FILE *);
#else
void prockeyline(char *, int, long, FILE *);
#endif
