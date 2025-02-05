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

/* Constructeur syntaxique de la classe "left corner" de SYNTAX */
/* A utiliser avec un l'analyseur earley				*/

#define SX_DFN_EXT_VAR_CSYNT_LC
#include "lc.h"
#include "sxversion.h"
#include "bstr.h"
#include "put_edit.h"
char WHAT_LCMAIN[] = "@(#)SYNTAX - $Id: lc_main.c 3652 2023-12-24 09:43:15Z garavel $" WHAT_DEBUG;

extern SXINT	LC1 (void);

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

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
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
    bool	is_first = true;

    bstr_raz (bstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = false;
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
    static bool	has_message [SXSEVERITIES];

    sxopentty ();

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);
	return SXSEVERITIES;
    }

    sxttycol1p = true;

/* valeurs par defaut */

    options_set = 0;

    sxverbosep = false;
    is_LC = print_time = true;
    options_set |= OPTION (LC);
    options_set |= OPTION (TIME);

    is_ELC = is_PLR = is_automaton = false;

/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (arg = argv [argnum])) {
	case VERBOSE:
	    sxverbosep = true;
	    break;

	case -VERBOSE:
	    sxverbosep = false;
	    break;

	case AUTOMATON:
	    is_automaton = true, options_set |= OPTION (AUTOMATON);
	    break;

	case -AUTOMATON:
	    is_automaton = false, options_set &= noOPTION (AUTOMATON);
	    break;

	case LC:
	    is_LC = true, is_PLR = false, is_ELC = false, options_set &= noOPTION (PLR), options_set &= noOPTION (ELC), options_set |= OPTION (LC);
	    break;

	case ELC:
	    is_LC = false, is_PLR = false, is_ELC = true, options_set &= noOPTION (PLR), options_set &= OPTION (ELC), options_set |= noOPTION (LC);
	    break;

	case PLR:
	    is_LC = false, is_PLR = true, is_ELC = false, options_set &= OPTION (PLR), options_set &= noOPTION (ELC), options_set |= noOPTION (LC);
	    break;

	case TIME:
	    print_time = true, options_set |= OPTION (TIME);
	    break;

	case -TIME:
	    print_time = false, options_set &= noOPTION (TIME);
	    break;

	case UNKNOWN_ARG:
	    if (*arg++ != '-')
		goto run;

	    fprintf (sxstderr, "%s: the unknown option %s is ignored.\n", ME, argv [argnum]);
	    break;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
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

    vstr = varstr_alloc (256);

    do {
	language_name = argv [argnum++];
	has_message [lc1_run ()] = true;
    } while (argnum < argc);

    varstr_free (vstr), vstr = NULL;

    {
	int	severity;

	for (severity = SXSEVERITIES - 1; severity > 0 && has_message [severity] == false; severity--) {
	}

	sxexit (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

