
/* vaxwords.c */
int get_int32(int32 *, FILE *);
int put_int32(const int32 *, FILE *);
int get_short(unsigned short *, FILE *);
int put_short(const unsigned short *, FILE *);
int vax_fread(void *, size_t, int, FILE *);
int vax_fwrite(const void *, size_t, int, FILE *);
