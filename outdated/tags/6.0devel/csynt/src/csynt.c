/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/









/* Constructeur syntaxique de SYNTAX */



#define SX_DFN_EXT_VAR_CSYNT
#include "sxversion.h"
#include "sxcommon.h"
#include "csynt_optim.h"
#include "bstr.h"
char WHAT_CSYNT[] = "@(#)SYNTAX - $Id: csynt.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "csynt";
static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t-fc, -force, -nfc, -noforce,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-a, -automaton, -na, -noautomaton,\n\
\t\t-luc, -list_user_conflicts, -nluc, -nolist_user_conflicts,\n\
\t\t-lsc, -list_system_conflicts, -nlsc, -nolist_system_conflicts,\n\
\t\t-lc, -list_conflicts, -nlc, -nolist_conflicts,\n\
\t\t-ab, -abstract, -nab, -noabstract,\n\
\t\t-fe, -floyd_evans, -nfe, -nofloyd_evans\n\
\t\t-ll, -long_listing, -nll, -nolong_listing\n\
\t\t-pspe, -partial_single_productions_elimination\n\
\t\t-tspe, -total_single_productions_elimination\n\
\t\t-nspe, -no_single_productions_elimination\n\
\t\t-p, -path, -np, -nopath\n\
\t\t-lr1, -nlr1, -nolr1,\n";
static long	csynt_options_set, optim_options_set;
static long	options_set;

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~OPTION (opt))

#define OPT_NB ((sizeof (option_kind)/sizeof (option_kind[0]))-1)

#define UNKNOWN_ARG 		0
#define FORCE 			1
#define VERBOSE 		2
#define AUTOMATON		3
#define LIST_USER_CONFLICTS 	4
#define LIST_SYSTEM_CONFLICTS 	5
#define LIST_CONFLICTS	        6
#define ABSTRACT	 	7
#define FLOYD_EVANS		8
#define LONG_LISTING		9
#define PARTIAL_SPE		10
#define TOTAL_SPE		11
#define NO_SPE			12
#define PATH			13
#define LR1_CONSTRUCTOR		14
#define LAST_OPTION		LR1_CONSTRUCTOR

static char	*option_tbl [] =
	{
	    "",
	    "fc", "force", "nfc", "noforce",
	    "v", "verbose", "nv", "noverbose",
	    "a", "automaton", "na", "noautomaton",
	    "luc", "list_user_conflicts", "nluc", "nolist_user_conflicts",
	    "lsc", "list_system_conflicts", "nlsc", "nolist_system_conflicts",
	    "lc", "list_conflicts", "nlc", "nolist_conflicts",
	    "ab", "abstract", "nab", "noabstract",
	    "fe", "floyd_evans", "nfe", "nofloyd_evans",
	    "ll", "long_listing", "nll", "nolong_listing",
	    "pspe", "partial_single_productions_elimination",
	    "tspe", "total_single_productions_elimination",
	    "nspe", "no_single_productions_elimination",
	    "p", "path", "np", "nopath",
	    "lr1", "nlr1", "nolr1"
	};
static SXINT	option_kind [] =
	{
	    UNKNOWN_ARG,
	    FORCE, FORCE, -FORCE, -FORCE,
	    VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
	    AUTOMATON, AUTOMATON, -AUTOMATON, -AUTOMATON,
	    LIST_USER_CONFLICTS, LIST_USER_CONFLICTS, -LIST_USER_CONFLICTS, -LIST_USER_CONFLICTS,
	    LIST_SYSTEM_CONFLICTS, LIST_SYSTEM_CONFLICTS, -LIST_SYSTEM_CONFLICTS, -LIST_SYSTEM_CONFLICTS,
	    LIST_CONFLICTS, LIST_CONFLICTS, -LIST_CONFLICTS, -LIST_CONFLICTS,
	    ABSTRACT, ABSTRACT, -ABSTRACT, -ABSTRACT,
	    FLOYD_EVANS, FLOYD_EVANS, -FLOYD_EVANS, -FLOYD_EVANS,
	    LONG_LISTING, LONG_LISTING, -LONG_LISTING, -LONG_LISTING,
	    PARTIAL_SPE, PARTIAL_SPE,
	    TOTAL_SPE, TOTAL_SPE,
	    NO_SPE, NO_SPE,
	    PATH, PATH, -PATH, -PATH,
	    LR1_CONSTRUCTOR, -LR1_CONSTRUCTOR, -LR1_CONSTRUCTOR
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



struct bstr	*options_text (SXINT kind, struct bstr *bstr)
{
    long	*options_text_options_set;
    SXINT	i;
    SXBOOLEAN	is_first = SXTRUE;

    options_text_options_set = kind == 1 ? &csynt_options_set : &optim_options_set;
    bstr_raz (bstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (*options_text_options_set & OPTION (i)) {
	    if (is_first)
		is_first = SXFALSE;
	    else
		bstr_cat_str (bstr, ", ");

	    bstr_cat_str (bstr, option_get_text (i));
	}

    return bstr;
}




static SXINT	csynt_run (char *ln)
{
    SXINT	code_lalr1, code_optim;

    if (!bnf_read (&bnf_ag, ln))
	return 2;

    switch (code_lalr1 = LALR1 (ln)) {
    case 0:
	break;

    case 1:
	if (is_force) {
	    break;
	}

    default:
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);

	    sxttycol1p = SXTRUE;
	}

	fputs ("Optimization phase skipped, no tables generated.\n", sxstderr);

	bnf_free (&bnf_ag);

	return 2;
    }

    code_optim = OPTIM (ln);

    bnf_free (&bnf_ag);

    return code_optim > code_lalr1 ? code_optim : code_lalr1;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main(int argc, char *argv[])
{
    int 	argnum;
    static SXBOOLEAN	has_message [SXSEVERITIES];

    sxopentty ();

    if (argc == 1) {
	fprintf (sxstderr, Usage, ME);

	return SXSEVERITIES;
    }

    sxttycol1p = SXTRUE;


/* valeurs par defaut */

    csynt_options_set = OPTION (FORCE) | OPTION (VERBOSE) | OPTION (LIST_SYSTEM_CONFLICTS);
    optim_options_set = OPTION (PARTIAL_SPE);

    sxverbosep = SXFALSE;
    is_force = is_list_system_conflicts = is_pspe = SXTRUE;

    is_automaton = is_list_user_conflicts = is_list_conflicts = is_abstract = is_floyd_evans = is_long_listing = is_tspe = is_nspe = is_path = is_lr1_constructor = SXFALSE;


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case FORCE:
	    is_force = SXTRUE, csynt_options_set |= OPTION (FORCE);
	    break;

	case -FORCE:
	    is_force = SXFALSE, csynt_options_set &= noOPTION (FORCE);
	    break;

	case VERBOSE:
	    sxverbosep = SXTRUE, csynt_options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = SXFALSE, csynt_options_set &= noOPTION (VERBOSE);
	    break;

	case AUTOMATON:
	    is_automaton = SXTRUE, csynt_options_set |= OPTION (AUTOMATON);
	    break;

	case -AUTOMATON:
	    is_automaton = SXFALSE, csynt_options_set &= noOPTION (AUTOMATON);
	    break;

	case LIST_USER_CONFLICTS:
	    is_list_user_conflicts = SXTRUE, csynt_options_set |= OPTION (LIST_USER_CONFLICTS);
	    break;

	case -LIST_USER_CONFLICTS:
	    is_list_user_conflicts = SXFALSE, csynt_options_set &= noOPTION (LIST_USER_CONFLICTS);
	    break;

	case LIST_SYSTEM_CONFLICTS:
	    is_list_system_conflicts = SXTRUE, csynt_options_set |= OPTION (LIST_SYSTEM_CONFLICTS);
	    break;

	case -LIST_SYSTEM_CONFLICTS:
	    is_list_system_conflicts = SXFALSE, csynt_options_set &= noOPTION (LIST_SYSTEM_CONFLICTS);
	    break;

	case LIST_CONFLICTS:
	    is_list_system_conflicts = is_list_user_conflicts = SXTRUE, csynt_options_set |= OPTION (LIST_CONFLICTS);
	    break;

	case -LIST_CONFLICTS:
	    is_list_system_conflicts = is_list_user_conflicts = SXFALSE, csynt_options_set &= noOPTION (LIST_CONFLICTS);
	    break;

	case ABSTRACT:
	    is_abstract = SXTRUE, optim_options_set |= OPTION (ABSTRACT);
	    break;

	case -ABSTRACT:
	    is_abstract = SXFALSE, optim_options_set &= noOPTION (ABSTRACT);
	    break;

	case FLOYD_EVANS:
	    is_floyd_evans = SXTRUE, optim_options_set |= OPTION (FLOYD_EVANS);
	    break;

	case -FLOYD_EVANS:
	    is_floyd_evans = SXFALSE, optim_options_set &= noOPTION (FLOYD_EVANS);
	    break;

	case LONG_LISTING:
	    is_long_listing = SXTRUE, csynt_options_set |= OPTION (LONG_LISTING);
	    break;

	case -LONG_LISTING:
	    is_long_listing = SXFALSE, csynt_options_set &= noOPTION (LONG_LISTING);
	    break;

	case PARTIAL_SPE:
	    is_pspe = SXTRUE, optim_options_set |= OPTION (PARTIAL_SPE);
	    is_tspe = SXFALSE, optim_options_set &= noOPTION (TOTAL_SPE);
	    is_nspe = SXFALSE, optim_options_set &= noOPTION (NO_SPE);
	    break;

	case TOTAL_SPE:
	    is_pspe = SXFALSE, optim_options_set &= noOPTION (PARTIAL_SPE);
	    is_tspe = SXTRUE, optim_options_set |= OPTION (TOTAL_SPE);
	    is_nspe = SXFALSE, optim_options_set &= noOPTION (NO_SPE);
	    break;

	case NO_SPE:
	    is_pspe = SXFALSE, options_set &= noOPTION (PARTIAL_SPE);
	    is_tspe = SXFALSE, options_set &= noOPTION (TOTAL_SPE);
	    is_nspe = SXTRUE, options_set |= OPTION (NO_SPE);
	    break;

	case PATH:
	    is_path = SXTRUE, csynt_options_set |= OPTION (PATH);
	    break;

	case -PATH:
	    is_path = SXFALSE, csynt_options_set &= noOPTION (PATH);
	    break;

	case LR1_CONSTRUCTOR:
	    is_lr1_constructor = SXTRUE, csynt_options_set |= OPTION (LR1_CONSTRUCTOR);
	    break;

	case -LR1_CONSTRUCTOR:
	    is_lr1_constructor = SXFALSE, csynt_options_set &= noOPTION (LR1_CONSTRUCTOR);
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

    vstr = varstr_alloc (256);

    do {
	has_message [csynt_run (argv [argnum++])] = SXTRUE;
    } while (argnum < argc);

    varstr_free (vstr), vstr = NULL;

    {
	int	severity;

	for (severity = SXSEVERITIES - 1; severity > 0 && has_message [severity] == SXFALSE; severity--) {
	}

	SXEXIT (severity);
    }

    return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}



SXVOID	sat_mess (char *name, SXINT no, SXINT old_size, SXINT new_size)
{
#if EBUG
    if (!sxttycol1p) {
	fputc ('\n', sxtty);

	sxttycol1p = SXTRUE;
    }

    fprintf (sxtty, "%s: Array %ld of size %ld overflowed: reallocation with size %ld.\n", name, (long)no, (long)old_size, (long)new_size);
#else
    sxuse (name);
    sxuse (no);
    sxuse (old_size);
    sxuse (new_size);
#endif
}
