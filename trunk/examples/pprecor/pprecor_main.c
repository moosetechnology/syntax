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

/* paragrapheur de RECOR */

#include "sxunix.h"
char WHAT_PPRECOR_MAIN[] = "@(#)SYNTAX - $Id: pprecor_main.c 4173 2024-08-26 11:56:20Z garavel $" WHAT_DEBUG;

extern SXTABLES	sxtables;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "pprecor";

#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof(option_tbl)/sizeof(*option_tbl))-1)

#define UNKNOWN_ARG 		0
#define KWUC 			1
#define KWLC 			2
#define KWCI 			3
#define KWDK 			4
#define VERBOSE 		5
#define LINE_LENGTH	 	6
#define TABS			7
#define EVERY			8
#define LAST_OPTION		EVERY

static char	*option_tbl [] = {"", "kwuc", "keywords_in_upper_case", "kwlc", "keywords_in_lower_case", "kwci",
     "keywords_capitalised_initial", "kwdk", "keywords_in_dark", "nkwdk", "no_keywords_in_dark", "v", "verbose", "nv",
     "noverbose", "ll", "line_length", "tabs", "notabs", "every"};
static SXINT	option_kind [] = {UNKNOWN_ARG, KWUC, KWUC, KWLC, KWLC, KWCI, KWCI, KWDK, KWDK, -KWDK, -KWDK, VERBOSE,
     VERBOSE, -VERBOSE, -VERBOSE, LINE_LENGTH, LINE_LENGTH, TABS, -TABS, EVERY};



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



static	void pprecor_run (char *pathname)
{
    FILE	*infile;
    char	un_nom [20];

    if (pathname == NULL) {
	if (sxverbosep) {
	    fputs ("\"stdin\":\n", sxtty);
	}

	infile = stdin;
	syntax (SXBEGIN, &sxtables, infile, "");
	sxstdout = stdout;
    }
    else {
	if ((infile = sxfopen (pathname, "r")) == NULL) {
	    fprintf (sxstderr, "%s: Cannot open (read) ", ME);
	    sxperror (pathname);
	    sxerrmngr.nbmess [2]++;
	    return;
	}
	else {
	    if ((sxstdout = sxnewfile (strcpy (un_nom, "pprecorXXXXXX"))) == NULL) {
		fprintf (sxstderr, "%s: Cannot open (write) ", ME);
		sxperror (un_nom);
		sxerrmngr.nbmess [2]++;
		return;
	    }

	    if (sxverbosep) fprintf (sxtty, "%s:\n", pathname);
	    syntax (SXBEGIN, &sxtables, infile, pathname);
	}
    }

    syntax (SXACTION, &sxtables);

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

    syntax (SXEND, &sxtables);
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	argnum;
  SXINT		val;

  sxopentty ();

  /* valeurs par defaut */

  sxppvariables.kw_case = SXCAPITALISED_INITIAL /* How should keywords be written */ ;
  sxppvariables.terminal_case = NULL;
  sxppvariables.kw_dark = false /* keywords are not artificially darkened */ ;
  sxppvariables.terminal_dark = NULL /* Same as kw_dark, but for each type of terminal */ ;
  sxppvariables.no_tabs = false /* optimize spaces into tabs */ ;
  sxppvariables.block_margin = false /* preserve structure when deeply nested */ ;
  sxppvariables.line_length = 80 /* What it says */ ;
  sxppvariables.max_margin = 60 /* Do not indent lines further than that */ ;
  sxppvariables.tabs_interval = SXTAB_INTERVAL /* number of columns between two tab positions */ ;


  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case KWUC:
      sxppvariables.kw_case = SXUPPER_CASE;
      break;

    case KWLC:
      sxppvariables.kw_case = SXLOWER_CASE;
      break;

    case KWCI:
      sxppvariables.kw_case = SXCAPITALISED_INITIAL;
      break;

    case KWDK:
      sxppvariables.kw_dark = true;
      break;

    case -KWDK:
      sxppvariables.kw_dark = false;
      break;

    case VERBOSE:
      sxverbosep = true;
      break;

    case -VERBOSE:
      sxverbosep = false;
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
      sxppvariables.no_tabs = false;
      break;

    case -TABS:
      sxppvariables.no_tabs = true;
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

  syntax (SXINIT, &sxtables, false /* no includes */);

  if (argnum == argc) {
    pprecor_run ((char*)NULL);
  }
  else {
    do {
      int	severity = sxerrmngr.nbmess [2];

      sxerrmngr.nbmess [2] = 0;
      pprecor_run (argv [argnum++]);
      sxerrmngr.nbmess [2] += severity;
    } while (argnum < argc);
  }

  syntax (SXFINAL, &sxtables, true);

  sxexit (sxerr_max_severity ());
  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

