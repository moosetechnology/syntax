/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.(pb&phd)*
   *                                                      *
   ******************************************************** */





/* Constructeur syntaxique vide de SYNTAX utilisable par exemple avec les RCG */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030312 16:43 (phd):	(mixlinkage) sur Alpha			*/
/************************************************************************/
/* Lun  6 Dec 1999 10:33(pb):	Creation				*/
/************************************************************************/

#include "sxunix.h"
#include "release.h"
#include "B_tables.h"
#include "P_tables.h"
char WHAT_DUMMYCSYNT[] = "@(#)SYNTAX - $Id: dummy_csynt.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

/* On sort des parser_tables (presque) vides. */
static struct bnf_ag_item	bnf_ag;
static struct parser_ag_item	parser_ag;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "dummy_csynt";

static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t\t-v, -verbose, -nv, -noverbose\n";
static long	options_set;

BOOLEAN	sxttycol1p, sxverbosep;

#define OPTION(opt)	(1l << (opt - 1))
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
static int	option_kind [] =
	{
	    UNKNOWN_ARG,
	    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
	};



static int	option_get_kind (arg)
    register char	*arg;
{
    register char	**opt;

    if (*arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, arg) == 0 /* egalite */ )
	    break;
    }

    return option_kind [opt - option_tbl];
}


extern BOOLEAN	parser_write () /* (struct parser_ag *parser_ag, char *langname) */ ;


static int	dummy_csynt_run (language_name)
    register char	*language_name;
{
    if (!bnf_read (&bnf_ag, language_name))
	return 2;

    parser_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
    parser_write (&parser_ag, language_name);

    bnf_free (&bnf_ag);

    return 0;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	                argnum;
  static BOOLEAN	has_message [SEVERITIES];

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);

    return SEVERITIES;
  }

  sxopentty ();

  sxttycol1p = TRUE;
  sxverbosep = TRUE;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (argv [argnum])) {
    case VERBOSE:
      sxverbosep = TRUE, options_set |= OPTION (VERBOSE);
      break;

    case -VERBOSE:
      sxverbosep = FALSE, options_set &= noOPTION (VERBOSE);
      break;

    case UNKNOWN_ARG:
      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %d while decoding options.\n", ME, option_get_kind (argv [argnum]));

      break;
    }
  }

  fprintf (sxstderr, "%s: a language name is needed.\n", ME);

  return SEVERITIES;

 run:
  if (sxverbosep) {
    setbuf (stdout, NULL);

    fprintf (sxtty, "%s\n", release_mess);
  }

  do {
    has_message [dummy_csynt_run (argv [argnum++])] = TRUE;
  } while (argnum < argc);

  {
    int	severity;

    for (severity = SEVERITIES - 1; severity > 0 && has_message [severity] == FALSE; severity--) {
    }

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



VOID	sat_mess (name, no, old_size, new_size)
    char	*name;
    int		no, old_size, new_size;
{
    if (!sxttycol1p) {
	fputc ('\n', sxtty);

	sxttycol1p = TRUE;
    }

    fprintf (sxtty, "%s: Array %d of size %d overflowed: reallocation with size %d.\n", name, no, old_size, new_size);
}
