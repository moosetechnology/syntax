/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXMAIN[] = "@(#)SYNTAX - $Id: amlgm_cmpd_main.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

static char ME [] = "amlgm_cmpd.c";

extern SXTABLES	sxtables;

bool           is_static;
char              *LANGUAGE_NAME;

/*---------------*/
/*    options    */
/*---------------*/

static char	Usage [] = "\
Usage:\t%s [options] [files]\n\
options=\n\
\t\t-e, -extern,\n\
\t\t--h, --help,\n\
";

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		((SXINT)0)
#define EXTERN 		        ((SXINT)1)
#define HELP 		        ((SXINT)2)

static char	*option_tbl [] = {"", "e", "extern", "-h", "-help"};
static SXINT	option_kind [] = {UNKNOWN_ARG, EXTERN, EXTERN, HELP, HELP};



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


#if 0
static char	*option_get_text (SXINT kind)
{
    SXINT	i;

    for (i = OPT_NB; i > 0; i--) {
	if (option_kind [i] == kind)
	    break;
    }

    return option_tbl [i];
}
#endif

static	void
amlgm_cmpd_run (char *pathname)
{
  FILE	 *infile;
    
  syntax (SXINIT, &sxtables, false /* no includes */) /* Initialisation de SYNTAX (mono-langage) */ ;

  if (pathname == NULL) {
    if (sxverbosep) {
      fputs ("\"stdin\":\n", sxtty);
    }

    LANGUAGE_NAME = "<stdin>";

    syntax (SXBEGIN, &sxtables, stdin, "");
    syntax (SXACTION, &sxtables);
    syntax (SXEND, &sxtables);
    /* clearerr (stdin); si utilisation repetee de "stdin" */
  }
  else {
    if ((infile = sxfopen (pathname, "r")) == NULL) {
      fprintf (sxstderr, "%s: Cannot open (read) ", ME);
      sxperror (pathname);
      sxerrmngr.nbmess [2]++;
      return;
    }

    if (sxverbosep) {
      fprintf (sxtty, "%s:\n", pathname);
    }
    
    LANGUAGE_NAME = pathname;

    syntax (SXBEGIN, &sxtables, infile, pathname);
    syntax (SXACTION, &sxtables);
    syntax (SXEND, &sxtables);
    fclose (infile);
  }

  syntax (SXFINAL, &sxtables, true);
}


int main (int argc, char *argv[])
{
    sxopentty ();

    /* Valeurs par defaut */
    is_static = true;

    {
      /* Tous les arguments sont des noms de fichiers, "-" signifiant stdin */
      int	argnum;
      bool   has_unknown_arg = false;

      for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case EXTERN:
	  is_static = false;
	  break;

	case HELP:
	  fprintf (sxstderr, "%s", Usage);
	  return EXIT_SUCCESS;	  

	case UNKNOWN_ARG:
	  has_unknown_arg = true;

	  if (argnum+1 == argc && argv [argnum] [0] == '-' && argv [argnum] [1] == '\0') {
	    amlgm_cmpd_run (NULL);
	  }
	  else {
	    if (argv [argnum] [0] == '-')
	      fprintf (sxstderr, "A file name cannot start with an \"-\": ignored.\n");
	    else
	      amlgm_cmpd_run (argv [argnum]);
	  }

	  break;
	default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	  sxtrap("make_a_dico/amlgm_cmpd_main","unknown switch case #1");
#endif
	  break;
	}
      }

      if (!has_unknown_arg)
	amlgm_cmpd_run (NULL);
    }

    return EXIT_SUCCESS;
}

