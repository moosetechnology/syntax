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

/* Constructeur syntaxique vide de SYNTAX utilisable par exemple avec les RCG */

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "P_tables.h"

char WHAT_DUMMYCSYNT[] = "@(#)SYNTAX - $Id: dummy_csynt.c 3651 2023-12-24 09:37:54Z garavel $" WHAT_DEBUG;

/* On sort des parser_tables (presque) vides. */
static struct bnf_ag_item	bnf_ag;
static struct parser_ag_item	parser_ag;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "dummy_csynt";

static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t\t-v, -verbose, -nv, -noverbose\n";
static SXINT	options_set;

bool	sxttycol1p;

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof (option_kind)/sizeof (option_kind[0]))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define LAST_OPTION		VERBOSE

static char	*option_tbl [] =
	{
	    "",
	    "v", "verbose", "nv", "noverbose",
	};
static SXINT	option_kind [] =
	{
	    UNKNOWN_ARG,
	    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
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


extern bool	parser_write (struct parser_ag_item *parser_ag_ptr, char *langname);


static SXINT	dummy_csynt_run (char *ln)
{
    if (!bnf_read (&bnf_ag, ln))
	return 2;

    parser_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
    parser_write (&parser_ag, ln);

    bnf_free (&bnf_ag);

    return 0;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	                argnum;
  static bool	has_message [SXSEVERITIES];

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);

    return SXSEVERITIES;
  }

  sxttycol1p = true;
  sxverbosep = false;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = true, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = false, options_set &= noOPTION (VERBOSE);
      break;

    case UNKNOWN_ARG:
      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, option_get_kind (argv [argnum]));

      break;
    }
  }

  fprintf (sxstderr, "%s: a language name is needed.\n", ME);

  return SXSEVERITIES;

 run:
  if (sxverbosep) {
    setbuf (stdout, NULL);

    fprintf (sxtty, "%s\n", release_mess);
  }

  do {
    has_message [dummy_csynt_run (argv [argnum++])] = true;
  } while (argnum < argc);

  {
    int	severity;

    for (severity = SXSEVERITIES - 1; severity > 0 && has_message [severity] == false; severity--) {
    }

    sxexit (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



void	sat_mess (char *name, SXINT no, SXINT old_size, SXINT new_size)
{
#if EBUG
    if (!sxttycol1p) {
	fputc ('\n', sxtty);

	sxttycol1p = true;
    }
    fprintf (sxtty, "%s: Array %ld of size %ld overflowed: reallocation with size %ld.\n", name, (SXINT) no, (SXINT) old_size, (SXINT) new_size);
#else
    sxuse (name);
    sxuse (no);
    sxuse (old_size);
    sxuse (new_size);
#endif
}
