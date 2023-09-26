#include "sxunix.h"

#define MAX_LINE_LENGTH 16777216

static char	ME [] = "dico_consult";

extern struct sxdfa_comb dico;
static struct sxdfa_comb *dico_ptr = &(dico);


/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] [filename]\n\
options=\n\
\t\t--h, --help,\n\
";

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define HELP 		        ((SXINT)16)

static char	*option_tbl [] = {
  "",
  "-h", "-help",
};
static SXINT	option_kind [] = {
  UNKNOWN_ARG,
  HELP, HELP,
};



static SXINT	option_get_kind (char *arg)
{
    char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}



static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}

static FILE *input_file;
static char l[MAX_LINE_LENGTH], sentence[MAX_LINE_LENGTH], *line;
static SXINT id, lgth;
extern char *t2string[];

static SXVOID
dico_consult (void)
{
  char *c;

  while (fgets (l, MAX_LINE_LENGTH, input_file)) {
    line = l;
    lgth = strlen (line) - 1;
    c = line + lgth;
    if (*c == '\n')
      *c = 0;
    id = sxdfa_comb_get (dico_ptr, line, lgth);
    fprintf (sxstderr, "%s\t%s\n", line, t2string [id]);
  }
}

static SXVOID
dico_consult_run (char *pathname)
{
  
  sxerr_mngr (SXBEGIN);

  if (pathname != NULL) {
    if (input_file = fopen (pathname, "r"))
      ;
    else {
      fprintf (stderr, "Could not open file %s\n", pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }
  } else {
    input_file = stdin;
  }

  dico_consult ();

  sxerr_mngr (SXEND);
}


int
main (int argc, char *argv[])
{
    sxopentty ();

    {
      /* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
      int	argnum;
      SXBOOLEAN   has_unknown_arg = SXFALSE;

      for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {

	case HELP:
	  fprintf (sxstderr, "%s", Usage);
	  return EXIT_SUCCESS;	  

	case UNKNOWN_ARG:
	  has_unknown_arg = SXTRUE;

	  if (argnum+1 == argc && argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
	    dico_consult_run (NULL);
	  }
	  else {
	    if (argv [argnum] [0] == '-')
	      fprintf (sxstderr, "A file name cannot start with an \"-\": ignored.\n");
	    else
	      dico_consult_run (argv [argnum]);
	  }

	  break;

	default:
	  sxtrap (ME, "main");
	  break;
	}
      }

      if (!has_unknown_arg)
	dico_consult_run (NULL);
    }

    return EXIT_SUCCESS;
}
