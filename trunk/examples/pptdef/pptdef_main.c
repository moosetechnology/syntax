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

/* paragrapheur de TDEF */

#include "sxunix.h"

char WHAT_TDEFPPTDEFMAIN[] = "@(#)SYNTAX - $Id: pptdef_main.c 3327 2023-06-04 15:18:00Z garavel $" WHAT_DEBUG;

extern struct sxtables	pptdef_tables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "pptdef";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof (option_tbl) / sizeof (*option_tbl)) - 1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define LINE_LENGTH	 	2
#define TABS			3
#define EVERY			4
#define LAST_OPTION		EVERY

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose", "ll", "line_length", "tabs", "notabs", "every"};
static SXINT	option_kind [] = {UNKNOWN_ARG, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE, LINE_LENGTH, LINE_LENGTH, TABS, -
     TABS, EVERY};



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



static SXVOID	pptdef_run (char *pathname)
{
    FILE	*infile;
    char		un_nom [20];

    if (pathname == NULL) {
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	infile = stdin;
	syntax (SXBEGIN, &pptdef_tables, infile, "");
	sxstdout = stdout; /* H. Garavel: this instruction seems useless */
    }
    else {
	if ((infile = sxfopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    if ((sxstdout = sxnewfile (strcpy (un_nom, "pptdefXXXXXX"))) == NULL) {
		fprintf (sxstderr, "%s: Cannot open (write) ", ME);
		sxperror (un_nom);
		sxerrmngr.nbmess [2]++;
		return;
	    }

	    if (sxverbosep) {
		fprintf (sxtty, "%s:\n", pathname);
	    }

	    syntax (SXBEGIN, &pptdef_tables, infile, pathname);
	}
    }

    syntax (SXACTION, &pptdef_tables);

    if (infile != stdin) {
	fclose (sxstdout);
	fclose (infile);

	if (sxppvariables.is_error || !sxfile_copy (un_nom, pathname, sxppvariables.char_count, ME)) {
	    fprintf (sxstderr, "%s: Le resultat du paragraphage se trouve dans %s.\n", ME, un_nom);
	    (sxerrmngr.nbmess [2])++;
	}
	else {
	    unlink (un_nom);
	}
    }

    syntax (SXEND, &pptdef_tables);
}


/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int   argnum;
  SXINT	val;

  sxopentty ();

  /* valeurs par defaut */

  sxverbosep = SXFALSE;
  sxppvariables.kw_case = SXCAPITALISED_INITIAL /* How should keywords be written */ ;
  sxppvariables.terminal_case = NULL;
  sxppvariables.kw_dark = SXFALSE /* keywords are not artificially darkened */ ;
  sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
  sxppvariables.no_tabs = SXFALSE /* optimize spaces into tabs */ ;
  sxppvariables.block_margin = SXFALSE /* preserve structure when deeply nested */ ;
  sxppvariables.line_length = 80 /* What it says */ ;
  sxppvariables.max_margin = 60 /* Do not indent lines further than that */ ;
  sxppvariables.tabs_interval = SXTAB_INTERVAL /* number of columns between two tab positions */ ;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = SXTRUE;
      break;

    case -VERBOSE:
      sxverbosep = SXFALSE;
      break;

    case LINE_LENGTH:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &val))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-ll\", %ld value is retained.\n", ME,
		 sxppvariables.line_length);
      else {
	sxppvariables.line_length = val;
      }

      break;

    case TABS:
      sxppvariables.no_tabs = SXFALSE;
      break;

    case -TABS:
      sxppvariables.no_tabs = SXTRUE;
      break;

    case EVERY:
      if (++argnum >= argc || !sxnumarg (argv [argnum], &val))
	fprintf (sxstderr, "%s: a numeric argument must follow \"-every\", %ld value is retained.\n", ME,
		 sxppvariables.tabs_interval);
      else {
	sxppvariables.tabs_interval = val;
      }

      break;

    case UNKNOWN_ARG:
      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
      sxexit (3);
    }
  }

 run:
  if (sxverbosep) {
    setbuf (stdout, NULL);
  }

  syntax (SXINIT, &pptdef_tables, SXFALSE /* no include */);

  if (argnum == argc) {
    pptdef_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      pptdef_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &pptdef_tables, SXTRUE);

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
