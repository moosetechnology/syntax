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





/* Constructeur syntaxique de la classe "left corner" de SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 20-12-94 10:58 (pb):		Separation d'avec "csynt"		*/
/************************************************************************/
/* 05-12-94 17:22 (pb):		Ajout de l'option "-lc"			*/
/************************************************************************/
/* 05-05-92 11:50 (pb):		Ajout de l'option "-nd"			*/
/************************************************************************/
/* 06-06-91 18:00 (pb):		Signification differente pour l'option	*/
/*				-[r[h]][la]lr[k]			*/
/************************************************************************/
/* 07-12-90 10:15 (pb):		Ajout de l'option "-time"		*/
/************************************************************************/
/* 26-11-90 10:27 (pb):		Ajout de l'option "-super_arc"		*/
/************************************************************************/
/* 04-09-90 11:36 (pb):		Ajout de l'option "-statistics"		*/
/************************************************************************/
/* 30-08-90 16:35 (pb):		Ajout de l'option -d[0|1|2] a la place	*/
/*				de is_detailed_derivation		*/
/************************************************************************/
/* 05-07-90 10:51 (pb):		Remplacement de l'option is_path	*/
/*				par : is_unique_conflict_per_state	*/
/*				      is_unique_derivation_per_conflict	*/
/*				      is_detailed_derivation		*/
/************************************************************************/
/* 23-10-89 10:10 (pb):		Separation de "options_set" en		*/
/*				"(csynt | optim)_options_set"		*/
/************************************************************************/
/* 06-09-89 10:36 (pb):		Ajout de l'option -[r[h]][la]lr[k]	*/
/************************************************************************/
/* 20-06-89 17:47 (pb):		Ajout de l'option -lrpi			*/
/************************************************************************/
/* 19-04-89 13:25 (pb):		Ajout de l'option -h nnn		*/
/************************************************************************/
/* 09-02-89 14:39 (pb):		Ajout de l'option -rlr			*/
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

#define SX_DFN_EXT_VAR_XCSYNT 

#include "csynt_optim.h"

#include "release.h"
#include "bstr.h"
char WHAT_XCSYNTLEFTCORNER[] = "@(#)SYNTAX - $Id: left_corner.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

extern SXINT	LC0 (void);

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE *sxstdout, *sxstderr, *sxtty;
#endif

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "lc";
static char	*arg;
static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t-fc, -force, -nfc, -noforce,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-a, -automaton, -na, -noautomaton,\n\
\t\t-fe, -floyd_evans, -nfe, -nofloyd_evans,\n\
\t\t-ll, -long_listing, -nll, -nolong_listing,\n\
\t\t-s, -statistics, -ns, -nostatistics,\n\
\t\t-t, -time, -nt, -notime,\n\
\t\t-lc0,\n\n";
static long	csynt_options_set, optim_options_set;

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~(OPTION (opt)))

#define OPT_NB ((sizeof (option_kind)/sizeof (option_kind[0]))-1)

#define UNKNOWN_ARG 		0
#define FORCE 			1
#define VERBOSE 		2
#define AUTOMATON		3
#define FLOYD_EVANS		4
#define LONG_LISTING		5
#define S			6
#define TIME			7
#define K0			8
#define LAST_OPTION		K0

static char	*option_tbl [] = {"",
				      "fc", "force", "nfc", "noforce",
				      "v", "verbose", "nv", "noverbose",
				      "a", "automaton", "na", "noautomaton",
				      "fe", "floyd_evans", "nfe", "nofloyd_evans",
				      "ll", "long_listing", "nll", "nolong_listing", 
				      "s", "statistics", "ns", "nostatistics",
				      "t", "time", "nt", "notime",
				      "lc0",
			      };
static SXINT	option_kind [] = {UNKNOWN_ARG,
				      FORCE, FORCE, -FORCE, -FORCE,
				      VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
				      AUTOMATON, AUTOMATON, -AUTOMATON, -AUTOMATON,
				      FLOYD_EVANS, FLOYD_EVANS, -FLOYD_EVANS, -FLOYD_EVANS,
				      LONG_LISTING, LONG_LISTING, -LONG_LISTING, -LONG_LISTING,
				      S, S, -S, -S,
				      TIME, TIME, -TIME, -TIME,
				      K0,
			      };

#if 0
#define is_digit(c)	c >= '0' && c <= '9'

static SXINT	str_to_nb (void)
{
    SXINT	value;

    value = c - '0';
    c = *arg++;

    while (is_digit (c)) {
	value *= 10;
	value += c - '0';
	c = *arg++;
    }

    return value;
}
#endif /* 0 */



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



char *get_constructor_name (char *string, 
			    BOOLEAN is_lalr, 
			    BOOLEAN is_rlr, 
			    SXINT h, 
			    SXINT k)
{
    sxuse(is_lalr); /* pour faire taire gcc -Wunused */
    sxuse(is_rlr); /* pour faire taire gcc -Wunused */
    sxuse(h); /* pour faire taire gcc -Wunused */
    sprintf (string, "%sLC (%ld)", k > 0 ? "LA" : "", (long)k);

    return string;
}


struct bstr	*options_text (SXINT kind, struct bstr *bstr)
{
    long	*options_set;
    SXINT	i;
    BOOLEAN	is_first = TRUE;

    options_set = kind == 1 ? &csynt_options_set : &optim_options_set;
    bstr_raz (bstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (*options_set & OPTION (i)) {
	    if (is_first)
		is_first = FALSE;
	    else
		bstr_cat_str (bstr, ", ");

	    bstr_cat_str (bstr, option_get_text (i));
	}

    return bstr;
}



static SXINT	csynt_run (void)
{
    SXINT	code_lalr1, code_optim;

    if (!bnf_read (&bnf_ag, language_name))
	return 2;

    switch (code_lalr1 = LC0 ()) {
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

    code_optim = OPTIM ();
    bnf_free (&bnf_ag);
    return code_optim > code_lalr1 ? code_optim : code_lalr1;
}



/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  int	                argnum;
  static BOOLEAN	has_message [SEVERITIES];

  sxstdout = stdout;
  sxstderr = stderr;

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    return SEVERITIES;
  }

  sxopentty ();
  sxttycol1p = TRUE;

  /* valeurs par defaut */

  is_left_corner = TRUE;

  csynt_options_set = 0;
  optim_options_set = 0;
  conflict_derivation_kind = 0;

  is_force = sxverbosep = is_nspe = is_non_deterministic_automaton = TRUE;

  is_automaton = is_list_user_conflicts = is_list_conflicts = is_abstract =
    is_floyd_evans = is_long_listing = is_tspe =
    is_pspe = is_lr_constructor = is_rlr_constructor = 
    is_ambiguity_check = is_statistics = is_super_arc = print_time =
    is_unique_derivation_per_conflict = is_unique_conflict_per_state =
    is_list_system_conflicts = FALSE;

  h_value = k_value = 1;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (arg = argv [argnum])) {
    case FORCE:
      is_force = TRUE;
      break;

    case -FORCE:
      is_force = FALSE;
      break;

    case VERBOSE:
      sxverbosep = TRUE;
      break;

    case -VERBOSE:
      sxverbosep = FALSE;
      break;

    case AUTOMATON:
      is_automaton = TRUE, csynt_options_set |= OPTION (AUTOMATON);
      break;

    case -AUTOMATON:
      is_automaton = FALSE, csynt_options_set &= noOPTION (AUTOMATON);
      break;

    case FLOYD_EVANS:
      is_floyd_evans = TRUE, optim_options_set |= OPTION (FLOYD_EVANS);
      break;

    case -FLOYD_EVANS:
      is_floyd_evans = FALSE, optim_options_set &= noOPTION (FLOYD_EVANS);
      break;

    case LONG_LISTING:
      is_long_listing = TRUE, csynt_options_set |= OPTION (LONG_LISTING), optim_options_set |= OPTION (LONG_LISTING);
      break;

    case -LONG_LISTING:
      is_long_listing = FALSE, csynt_options_set &= noOPTION (LONG_LISTING), optim_options_set &= noOPTION (LONG_LISTING);
      break;

    case S:
      is_statistics = TRUE, csynt_options_set |= OPTION (S);
      break;

    case -S:
      is_statistics = FALSE, csynt_options_set &= noOPTION (S);
      break;

    case TIME:
      print_time = TRUE, csynt_options_set |= OPTION (TIME);
      break;

    case -TIME:
      print_time = FALSE, csynt_options_set &= noOPTION (TIME);
      break;

    case K0:
      k_value = 0, csynt_options_set |= OPTION (K0);
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

  get_constructor_name (constructor_name, !is_lr_constructor, is_rlr_constructor, h_value, k_value);
  vstr = varstr_alloc (256);

  do {
    language_name = argv [argnum++];
    has_message [csynt_run ()] = TRUE;
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
    fprintf (statistics_file, "%s: Array %ld of size %ld overflowed: reallocation with size %ld.\n", name, (long)no, (long)old_size, (long)new_size);
#else
    sxuse (name);
    sxuse (no);
    sxuse (old_size);
    sxuse (new_size);
#endif
}
