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





/* Constructeur syntaxique de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 14-12-88 10:06 (pb):		Ajout de l'option -lr1 et changement	*/
/*				de -lalr1 en -automaton			*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 18-04-88 17:00 (pb):		Ajout de l'option is_paths		*/
/************************************************************************/
/* 18-03-88 10:20 (pb):		On raccourcit des noms, merci Apollo	*/
/************************************************************************/
/* 22-01-88 08:44 (pb):		Options "tspe", "pspe" et "nspe"	*/
/************************************************************************/
/* 28-11-87 08:28 (pb):		Options "ll"				*/
/************************************************************************/
/* 31-07-87 16:22 (phd):	Utilisation de "sxopentty"		*/
/************************************************************************/
/* 04-02-87 14:07 (phd):	Amelioration des messages au terminal  	*/
/************************************************************************/
/* 28-01-87 08:28 (pb):		Options "luc", "lsc" et "lc"		*/
/************************************************************************/
/* 30-10-86 15:31 (pb&phd):	Reecriture complete			*/
/************************************************************************/

#define SX_DFN_EXT_VAR_CSYNT
#include "csynt_optim.h"

#include "release.h"
#include "bstr.h"
char WHAT_CSYNT[] = "@(#)SYNTAX - $Id: csynt.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
/* On lit a priori sur stdin, et cetera */
FILE    *sxstdout, *sxstderr;
FILE    *sxtty;
#endif

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
    BOOLEAN	is_first = TRUE;

    options_text_options_set = kind == 1 ? &csynt_options_set : &optim_options_set;
    bstr_raz (bstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (*options_text_options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		bstr_cat_str (bstr, ", ");

	    bstr_cat_str (bstr, option_get_text (i));
	}

    return bstr;
}


#if 0
VARSTR	options_text (VARSTR vstr)
{
    SXINT	i;
    BOOLEAN	is_first = TRUE;

    varstr_raz (vstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		vstr = varstr_catenate (vstr, ", ");

	    vstr = varstr_catenate (vstr, option_get_text (i));
	}

    return vstr;
}
#endif


static SXINT	csynt_run (char *language_name)
{
    SXINT	code_lalr1, code_optim;

    if (!bnf_read (&bnf_ag, language_name))
	return 2;

    switch (code_lalr1 = LALR1 (language_name)) {
    case 0:
	break;

    case 1:
	if (is_force) {
	    break;
	}

    default:
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);

	    sxttycol1p = TRUE;
	}

	fputs ("Optimization phase skipped, no tables generated.\n", sxstderr);

	bnf_free (&bnf_ag);

	return 2;
    }

    code_optim = OPTIM (language_name);

    bnf_free (&bnf_ag);

    return code_optim > code_lalr1 ? code_optim : code_lalr1;
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

    csynt_options_set = OPTION (FORCE) | OPTION (VERBOSE) | OPTION (LIST_SYSTEM_CONFLICTS);
    optim_options_set = OPTION (PARTIAL_SPE);

    is_force = sxverbosep = is_list_system_conflicts = is_pspe = TRUE;

    is_automaton = is_list_user_conflicts = is_list_conflicts = is_abstract = is_floyd_evans = is_long_listing = is_tspe = is_nspe = is_path = is_lr1_constructor = FALSE;


/* Decodage des options */

    for (argnum = 1; argnum < argc; argnum++) {
	switch (option_get_kind (argv [argnum])) {
	case FORCE:
	    is_force = TRUE, csynt_options_set |= OPTION (FORCE);
	    break;

	case -FORCE:
	    is_force = FALSE, csynt_options_set &= noOPTION (FORCE);
	    break;

	case VERBOSE:
	    sxverbosep = TRUE, csynt_options_set |= OPTION (VERBOSE);
	    break;

	case -VERBOSE:
	    sxverbosep = FALSE, csynt_options_set &= noOPTION (VERBOSE);
	    break;

	case AUTOMATON:
	    is_automaton = TRUE, csynt_options_set |= OPTION (AUTOMATON);
	    break;

	case -AUTOMATON:
	    is_automaton = FALSE, csynt_options_set &= noOPTION (AUTOMATON);
	    break;

	case LIST_USER_CONFLICTS:
	    is_list_user_conflicts = TRUE, csynt_options_set |= OPTION (LIST_USER_CONFLICTS);
	    break;

	case -LIST_USER_CONFLICTS:
	    is_list_user_conflicts = FALSE, csynt_options_set &= noOPTION (LIST_USER_CONFLICTS);
	    break;

	case LIST_SYSTEM_CONFLICTS:
	    is_list_system_conflicts = TRUE, csynt_options_set |= OPTION (LIST_SYSTEM_CONFLICTS);
	    break;

	case -LIST_SYSTEM_CONFLICTS:
	    is_list_system_conflicts = FALSE, csynt_options_set &= noOPTION (LIST_SYSTEM_CONFLICTS);
	    break;

	case LIST_CONFLICTS:
	    is_list_system_conflicts = is_list_user_conflicts = TRUE, csynt_options_set |= OPTION (LIST_CONFLICTS);
	    break;

	case -LIST_CONFLICTS:
	    is_list_system_conflicts = is_list_user_conflicts = FALSE, csynt_options_set &= noOPTION (LIST_CONFLICTS);
	    break;

	case ABSTRACT:
	    is_abstract = TRUE, optim_options_set |= OPTION (ABSTRACT);
	    break;

	case -ABSTRACT:
	    is_abstract = FALSE, optim_options_set &= noOPTION (ABSTRACT);
	    break;

	case FLOYD_EVANS:
	    is_floyd_evans = TRUE, optim_options_set |= OPTION (FLOYD_EVANS);
	    break;

	case -FLOYD_EVANS:
	    is_floyd_evans = FALSE, optim_options_set &= noOPTION (FLOYD_EVANS);
	    break;

	case LONG_LISTING:
	    is_long_listing = TRUE, csynt_options_set |= OPTION (LONG_LISTING);
	    break;

	case -LONG_LISTING:
	    is_long_listing = FALSE, csynt_options_set &= noOPTION (LONG_LISTING);
	    break;

	case PARTIAL_SPE:
	    is_pspe = TRUE, optim_options_set |= OPTION (PARTIAL_SPE);
	    is_tspe = FALSE, optim_options_set &= noOPTION (TOTAL_SPE);
	    is_nspe = FALSE, optim_options_set &= noOPTION (NO_SPE);
	    break;

	case TOTAL_SPE:
	    is_pspe = FALSE, optim_options_set &= noOPTION (PARTIAL_SPE);
	    is_tspe = TRUE, optim_options_set |= OPTION (TOTAL_SPE);
	    is_nspe = FALSE, optim_options_set &= noOPTION (NO_SPE);
	    break;

	case NO_SPE:
	    is_pspe = FALSE, options_set &= noOPTION (PARTIAL_SPE);
	    is_tspe = FALSE, options_set &= noOPTION (TOTAL_SPE);
	    is_nspe = TRUE, options_set |= OPTION (NO_SPE);
	    break;

	case PATH:
	    is_path = TRUE, csynt_options_set |= OPTION (PATH);
	    break;

	case -PATH:
	    is_path = FALSE, csynt_options_set &= noOPTION (PATH);
	    break;

	case LR1_CONSTRUCTOR:
	    is_lr1_constructor = TRUE, csynt_options_set |= OPTION (LR1_CONSTRUCTOR);
	    break;

	case -LR1_CONSTRUCTOR:
	    is_lr1_constructor = FALSE, csynt_options_set &= noOPTION (LR1_CONSTRUCTOR);
	    break;

	case UNKNOWN_ARG:
	    goto run;

	default:
	    fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, option_get_kind (argv [argnum]));

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
	has_message [csynt_run (argv [argnum++])] = TRUE;
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



VOID	sat_mess (char *name, SXINT no, SXINT old_size, SXINT new_size)
{
#if EBUG
    if (!sxttycol1p) {
	fputc ('\n', sxtty);

	sxttycol1p = TRUE;
    }

    fprintf (sxtty, "%s: Array %ld of size %ld overflowed: reallocation with size %ld.\n", name, (long)no, (long)old_size, (long)new_size);
#else
    sxuse (name);
    sxuse (no);
    sxuse (old_size);
    sxuse (new_size);
#endif
}
