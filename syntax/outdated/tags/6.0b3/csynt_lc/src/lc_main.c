/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe ATOLL				  *
   *                                                      *
   ******************************************************** */





/* Constructeur syntaxique de la classe "left corner" de SYNTAX */
/* A utiliser avec un l'analyseur earley				*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030513 09:00 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 20030319 10:48 (phd):	Utilisation de "sxtimestamp"		*/
/************************************************************************/
/* Wed May 22 14:52:01:		Au boulot				*/
/************************************************************************/

#define SX_DFN_EXT_VAR_CSYNT_LC

#include "lc.h"

#include "release.h"
#include "bstr.h"
char WHAT_LCMAIN[] = "@(#)SYNTAX - $Id: lc_main.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

extern SXINT	LC1 (void);

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE    *sxstdout, *sxstderr;
FILE    *sxtty;
#endif

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "lc1";
static char	*arg;
static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t-v, -verbose, -nv, -noverbose,\n\
\t\t-a, -automaton, -na, -noautomaton,\n\
\t\t-lc, -left_corner\n\
\t\t-elc, -earley_left_corner\n\
\t\t-plr, -predictive_lr\n\
\t\t-t, -time, -nt, -notime,\n";
static SXUINT	options_set;

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~(OPTION (opt)))

#define OPT_NB ((sizeof (option_kind)/sizeof (option_kind[0]))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define AUTOMATON		2
#define LC			3
#define ELC			4
#define PLR			5
#define TIME			6
#define LAST_OPTION		TIME

static char	*option_tbl [] = {"",
				      "v", "verbose", "nv", "noverbose",
				      "a", "automaton", "na", "noautomaton",
				      "lc", "left_corner",
				      "elc", "earley_left_corner",
				      "plr", "predictive_lr",
				      "t", "time", "nt", "notime",
			      };
static SXINT	option_kind [] = {UNKNOWN_ARG,
				      VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				      AUTOMATON, AUTOMATON, -AUTOMATON, -AUTOMATON,
				      LC, LC,
				      ELC, ELC,
				      PLR, PLR,
				      TIME, TIME, -TIME, -TIME,
			      };


static SXINT	option_get_kind (char *option_get_kind_arg)
{
    char	**opt;

    if (*option_get_kind_arg++ != '-')
	return UNKNOWN_ARG;

    for (opt = &(option_tbl [OPT_NB]); opt > option_tbl; opt--) {
	if (strcmp (*opt, option_get_kind_arg) == 0 /* egalite */ )
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




struct bstr	*options_text (/* SXINT kind, */ struct bstr *bstr)
{
    SXINT	i;
    BOOLEAN	is_first = TRUE;

    bstr_raz (bstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		bstr_cat_str (bstr, ", ");

	    bstr_cat_str (bstr, option_get_text (i));
	}

    return bstr;
}



static SXINT	lc1_run (void)
{
    SXINT	code_lc1;

    if (!bnf_read (&bnf_ag, language_name))
	return 2;

    code_lc1 = LC1 ();

    bnf_free (&bnf_ag);
    return code_lc1;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
    int 	argnum;
    static BOOLEAN	has_message [SEVERITIES];

    sxstdout = stdout, sxstderr = stderr;

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	return SEVERITIES;
    }

    sxopentty ();
    sxttycol1p = TRUE;

/* valeurs par defaut */

    options_set = 0;

    is_LC = sxverbosep = print_time = TRUE;
    options_set |= OPTION (LC);
    options_set |= OPTION (TIME);

    is_ELC = is_PLR = is_automaton = FALSE;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (arg = argv [argnum])) {
	case VERBOSE:
	    sxverbosep = TRUE;
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE;
	    break;

	case AUTOMATON:
	    is_automaton = TRUE, options_set |= OPTION (AUTOMATON);
	    break;

	case -AUTOMATON:
	    is_automaton = FALSE, options_set &= noOPTION (AUTOMATON);
	    break;

	case LC:
	    is_LC = TRUE, is_PLR = FALSE, is_ELC = FALSE, options_set &= noOPTION (PLR), options_set &= noOPTION (ELC), options_set |= OPTION (LC);
	    break;

	case ELC:
	    is_LC = FALSE, is_PLR = FALSE, is_ELC = TRUE, options_set &= noOPTION (PLR), options_set &= OPTION (ELC), options_set |= noOPTION (LC);
	    break;

	case PLR:
	    is_LC = FALSE, is_PLR = TRUE, is_ELC = FALSE, options_set &= OPTION (PLR), options_set &= noOPTION (ELC), options_set |= noOPTION (LC);
	    break;

	case TIME:
	    print_time = TRUE, options_set |= OPTION (TIME);
	    break;

	case -TIME:
	    print_time = FALSE, options_set &= noOPTION (TIME);
	    break;

	case UNKNOWN_ARG:
	    if (*arg++ != '-')
		goto run;

	    fprintf (sxstderr, "%s: the unknown option %s is ignored.\n", ME, argv [argnum]);
	    break;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
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

    vstr = varstr_alloc (256);

    do {
	language_name = argv [argnum++];
	has_message [lc1_run ()] = TRUE;
    } while (argnum < argc);

    varstr_free (vstr), vstr = NULL;

    {
	int	severity;

	for (severity = SEVERITIES - 1; severity > 0 && has_message [severity] == FALSE; severity--) {
	}

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}






#if 0
/* Le 20/12/04 devient un module de la librairie  sxtm_mngr.c */
VOID
sxtime (SXINT what, char *str)
{
  if (what == TIME_INIT) {
    sxtimestamp (INIT, NULL);
  }
  else {
    fputs (str, sxtty);

    if (print_time)
      sxtimestamp (ACTION, NULL);
    else
      putc ('\n', sxtty);
  }
}
#endif /* 0 */


VOID
sxvoid (void)
/* procedure ne faisant rien */
{
}

