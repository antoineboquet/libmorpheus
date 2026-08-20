#if !defined(_POSIX_C_SOURCE)
#define _POSIX_C_SOURCE 200809L
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <morpheus/morpheus.h>

#define ARGS "ILalmnbckidsxSVpPeTo:"
#define PATH_SEP '/'

static void trim_line(char *line);
static void trim_digits(char *line);

int
main(int argc, char **argv)
{
  FILE *input=stdin;
  FILE *output=stdout;
  FILE *failed=stderr;
  FILE *stats=stderr;
  morpheus_context *context=NULL;
  morpheus_language language=MORPHEUS_LANGUAGE_GREEK;
  morpheus_compat_flags flags=
      MORPHEUS_COMPAT_PERSEUS_FORMAT|MORPHEUS_COMPAT_STRICT_CASE;
  char output_name[BUFSIZ]={0};
  char failed_name[BUFSIZ]={0};
  char stats_name[BUFSIZ]={0};
  char line[BUFSIZ*4];
  const char *stemlib;
  long words=0;
  long hits=0;
  size_t total_analyses=0;
  size_t total_lemmas=0;
  int time_enabled=1;
  int option;
  int last_count=0;
  clock_t start_time=0;
  clock_t longest_time=0;
  char longest_word[BUFSIZ*4]={0};

  while((option=getopt(argc,argv,ARGS)) != -1) {
    switch(option) {
    case 'a': flags|=MORPHEUS_COMPAT_SHOW_ANAL; break;
    case 'l': flags|=MORPHEUS_COMPAT_SHOW_LEMMA; break;
    case 'm': flags|=MORPHEUS_COMPAT_SHOW_MISSES; break;
    case 'b': flags|=MORPHEUS_COMPAT_BUFFER_ANALYSES; break;
    case 'c': flags|=MORPHEUS_COMPAT_CHECK_PREVERB; break;
    case 'I': language=MORPHEUS_LANGUAGE_ITALIAN; break;
    case 'L': language=MORPHEUS_LANGUAGE_LATIN; break;
    case 'k': flags|=MORPHEUS_COMPAT_KEEP_BETA; break;
    case 'i': flags|=MORPHEUS_COMPAT_SHOW_FULL_INFO; break;
    case 'd':
      flags&=~MORPHEUS_COMPAT_PERSEUS_FORMAT;
      flags&=~MORPHEUS_COMPAT_LEXICON_OUTPUT;
      flags|=MORPHEUS_COMPAT_DATABASE_FORMAT;
      break;
    case 's': flags|=MORPHEUS_COMPAT_DATABASE_SHORT; break;
    case 'n': flags|=MORPHEUS_COMPAT_IGNORE_ACCENTS; break;
    case 'x': flags|=MORPHEUS_COMPAT_LEXICON_OUTPUT; break;
    case 'V': flags|=MORPHEUS_COMPAT_VERBS_ONLY; break;
    case 'S': flags&=~MORPHEUS_COMPAT_STRICT_CASE; break;
    case 'p': flags|=MORPHEUS_COMPAT_PARSE_FORMAT; break;
    case 'P': flags&=~MORPHEUS_COMPAT_PERSEUS_FORMAT; break;
    case 'e': flags|=MORPHEUS_COMPAT_ENDING_INDEX; break;
    case 'T': time_enabled=0; break;
    case 'o':
      if(strcmp(optarg,"-")) {
        if(snprintf(output_name,sizeof output_name,"%s",optarg) >=
               (int)sizeof output_name ||
           snprintf(failed_name,sizeof failed_name,"%s.failed",optarg) >=
               (int)sizeof failed_name ||
           snprintf(stats_name,sizeof stats_name,"%s.stats",optarg) >=
               (int)sizeof stats_name) {
          fprintf(stderr,"output filename is too long\n");
          return(1);
        }
      }
      break;
    default:
      return(1);
    }
  }

  if(optind < argc) {
    char input_name[BUFSIZ];
    const char *base=argv[optind++];
    if(snprintf(input_name,sizeof input_name,"%s.words",base) >=
       (int)sizeof input_name) {
      fprintf(stderr,"input filename is too long\n");
      return(1);
    }
    input=fopen(input_name,"r");
    if(!input) {
      fprintf(stderr,"cannot find [%s]!\n",input_name);
      return(1);
    }
    if(optind < argc) {
      const char *destination=argv[optind];
      if(snprintf(output_name,sizeof output_name,"%s%c%s.morph",
                  destination,PATH_SEP,base) >= (int)sizeof output_name ||
         snprintf(failed_name,sizeof failed_name,"%s%c%s.failed",
                  destination,PATH_SEP,base) >= (int)sizeof failed_name ||
         snprintf(stats_name,sizeof stats_name,"%s%c%s.stats",
                  destination,PATH_SEP,base) >= (int)sizeof stats_name) {
        fprintf(stderr,"destination filename is too long\n");
        return(1);
      }
    } else if(!output_name[0]) {
      if(snprintf(output_name,sizeof output_name,"%s.morph",base) >=
             (int)sizeof output_name ||
         snprintf(failed_name,sizeof failed_name,"%s.failed",base) >=
             (int)sizeof failed_name ||
         snprintf(stats_name,sizeof stats_name,"%s.stats",base) >=
             (int)sizeof stats_name) {
        fprintf(stderr,"output filename is too long\n");
        return(1);
      }
    }
  }

  if(output_name[0]) {
    output=fopen(output_name,"w");
    failed=fopen(failed_name,"w");
    stats=fopen(stats_name,"w");
    if(!output || !failed || !stats) {
      fprintf(stderr,"cannot open output files\n");
      return(1);
    }
  }

  stemlib=getenv("MORPHLIB");
  if(!stemlib) {
    fprintf(stderr,"MORPHLIB not set in your environment!\n");
    return(1);
  }
  {
    morpheus_config config={
      MORPHEUS_ABI_VERSION,sizeof config,stemlib,(uint32_t)language
    };
    morpheus_status status=morpheus_open(&config,&context);
    if(status != MORPHEUS_OK) {
      fprintf(stderr,"%s\n",morpheus_status_message(status));
      return(1);
    }
  }

  while(fgets(line,sizeof line,input)) {
    morpheus_compat_output *formatted=NULL;
    morpheus_status status;
    clock_t word_start=0;
    clock_t elapsed;
    char *space;

    trim_line(line);
    if(!line[0]) continue;
    if(line[0] == '#') {
      fprintf(output,"%s\n",line);
      continue;
    }
    trim_digits(line);
    space=line;
    while(*space && !isspace((unsigned char)*space)) space++;
    *space=0;
    if(!line[0]) continue;

    if(time_enabled) {
      word_start=clock();
      if(!start_time) start_time=word_start;
    }
    status=morpheus_compat_analyze(
        context,(const uint8_t *)line,strlen(line),flags,&formatted);
    if(status != MORPHEUS_OK) {
      fprintf(stderr,"%s: %s\n",line,morpheus_status_message(status));
      morpheus_compat_output_free(formatted);
      continue;
    }
    last_count=(int)morpheus_compat_output_analysis_count(formatted);
    words++;
    if(last_count) {
      hits++;
      total_analyses+=morpheus_compat_output_analysis_count(formatted);
      total_lemmas+=morpheus_compat_output_lemma_count(formatted);
      fputs(morpheus_compat_output_data(formatted),output);
    } else {
      if((flags & MORPHEUS_COMPAT_SHOW_LEMMA) &&
         (flags & MORPHEUS_COMPAT_IGNORE_ACCENTS))
        fputs("form:",failed);
      fprintf(failed,"%s\n",line);
      fflush(failed);
    }
    if(time_enabled) {
      elapsed=clock()-word_start;
      if(elapsed >= longest_time && words > 1 && last_count) {
        longest_time=elapsed;
        snprintf(longest_word,sizeof longest_word,"%s",line);
      }
    }
    morpheus_compat_output_free(formatted);
  }

  if(words) {
    fprintf(stats,"FINAL:  words %ld, analyzed %ld (%0.2f pct), %d\n",
            words,hits,100*((float)hits/(float)words),last_count);
  }
  if(hits) {
    fprintf(stats,":nhits %ld anals %zu anals/hit %0.2f lems %zu lems/hit %0.2f\n",
            hits,total_analyses,(float)total_analyses/(float)hits,
            total_lemmas,(float)total_lemmas/(float)hits);
  }
  if(time_enabled && words) {
    fprintf(stats,":avg time %.2f; long time [%.2f] for [%s]\n",
            (double)(clock()-start_time)/(CLOCKS_PER_SEC*(double)words),
            (double)longest_time/CLOCKS_PER_SEC,longest_word);
  }

  morpheus_close(context);
  if(input != stdin) fclose(input);
  if(output != stdout) fclose(output);
  if(failed != stderr) fclose(failed);
  if(stats != stderr) fclose(stats);
  return(0);
}

static void
trim_line(char *line)
{
  char *start=line;
  char *end;

  while(isspace((unsigned char)*start)) start++;
  if(start != line) memmove(line,start,strlen(start)+1);
  end=line+strlen(line);
  while(end > line && isspace((unsigned char)end[-1])) end--;
  *end=0;
}

static void
trim_digits(char *line)
{
  char *end=line+strlen(line);
  while(end > line && isdigit((unsigned char)end[-1])) end--;
  *end=0;
}
