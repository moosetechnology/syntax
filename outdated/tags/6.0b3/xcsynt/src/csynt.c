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
#include "RLR.h"
#include "release.h"
#include "bstr.h"
char WHAT_XCSYNTCSYNT[] = "@(#)SYNTAX - $Id: csynt.c 1209 2008-03-12 15:58:48Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE *sxstdout, *sxstderr, *sxtty;
#endif

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "csynt";
static char	*arg, c;
static BOOLEAN	is_arg_error;
static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t-fc, -force, -nfc, -noforce,\n\
\t\t-v, -verbose, -nv, -noverbose,\n\
\t\t-a, -automaton, -na, -noautomaton,\n\
\t\t-luc, -list_user_conflicts, -nluc, -nolist_user_conflicts,\n\
\t\t-lsc, -list_system_conflicts, -nlsc, -nolist_system_conflicts,\n\
\t\t-lc, -list_conflicts, -nlc, -nolist_conflicts,\n\
\t\t-ab, -abstract, -nab, -noabstract,\n\
\t\t-fe, -floyd_evans, -nfe, -nofloyd_evans,\n\
\t\t-ll, -long_listing, -nll, -nolong_listing,\n\
\t\t-pspe, -partial_single_productions_elimination,\n\
\t\t-tspe, -total_single_productions_elimination,\n\
\t\t-d, -deterministic, -nd, -nondeterministic,\n\
\t\t-nspe, -nosingle_productions_elimination,\n\
\t\t-d[0], -d1, -d2,\n\
\t\t-uc, -unique_conflict, -nuc, -nounique_conflict,\n\
\t\t-ud, -unique_derivation, -nud, -nounique_derivation,\n\
\t\t-[r[h]][la]lr[k],\n\
\t\t-s, -statistics, -ns, -nostatistics,\n\
\t\t-t, -time, -nt, -notime,\n\
\t\t-sa, -super_arc, -nsa, -nosuper_arc,\n\
\t\t-ac, -ambiguity_check, -nac, -noambiguity_check,\n\
\t\t-lc, -left_corner\n";
static long	csynt_options_set, optim_options_set;

#define OPTION(opt)	(1l << (opt - 1))
#define noOPTION(opt)	(~(OPTION (opt)))

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
#define D0			13
#define D1			14
#define D2			15
#define UC			16
#define UD			17
#define S			18
#define SA			19
#define TIME			20
#define DETERMINISTIC		21
#define AMBIGUITY_CHECK		22
#define LAST_OPTION		AMBIGUITY_CHECK

static char	*option_tbl [] = {"", "fc", "force", "nfc", "noforce", "v", "verbose", "nv", "noverbose", "a", "automaton"
     , "na", "noautomaton", "luc", "list_user_conflicts", "nluc", "nolist_user_conflicts", "lsc", "list_system_conflicts",
     "nlsc", "nolist_system_conflicts", "lc", "list_conflicts", "nlc", "nolist_conflicts", "ab", "abstract", "nab",
     "noabstract", "fe", "floyd_evans", "nfe", "nofloyd_evans", "ll", "long_listing", "nll", "nolong_listing", "pspe",
     "partial_single_productions_elimination", "tspe", "total_single_productions_elimination", "nspe",
     "nosingle_productions_elimination",
     "d", "d0", "d1", "d2",
     "uc", "unique_conflict", "nuc", "nounique_conflict",
     "ud", "unique_derivation", "nud", "nounique_derivation",
     "s", "statistics", "ns", "nostatistics",
     "sa", "super_arc", "nsa", "nosuper_arc",
     "t", "time", "nt", "notime",
     "d", "deterministic", "nd", "nondeterministic",				      
     "ac", "ambiguity_check", "nac", "noambiguity_check",
     };
static SXINT	option_kind [] = {UNKNOWN_ARG, FORCE, FORCE, -FORCE, -FORCE, VERBOSE, VERBOSE, -VERBOSE, -VERBOSE,
     AUTOMATON, AUTOMATON, -AUTOMATON, -AUTOMATON, LIST_USER_CONFLICTS, LIST_USER_CONFLICTS, -LIST_USER_CONFLICTS, -
     LIST_USER_CONFLICTS, LIST_SYSTEM_CONFLICTS, LIST_SYSTEM_CONFLICTS, -LIST_SYSTEM_CONFLICTS, -LIST_SYSTEM_CONFLICTS,
     LIST_CONFLICTS, LIST_CONFLICTS, -LIST_CONFLICTS, -LIST_CONFLICTS, ABSTRACT, ABSTRACT, -ABSTRACT, -ABSTRACT,
     FLOYD_EVANS, FLOYD_EVANS, -FLOYD_EVANS, -FLOYD_EVANS, LONG_LISTING, LONG_LISTING, -LONG_LISTING, -LONG_LISTING,
     PARTIAL_SPE, PARTIAL_SPE, TOTAL_SPE, TOTAL_SPE, NO_SPE, NO_SPE,
     D0, D0, D1, D2,
     UC, UC, -UC, -UC,
     UD, UD, -UD, -UD,
     S, S, -S, -S,
     SA, SA, -SA, -SA,
     TIME, TIME, -TIME, -TIME,
     DETERMINISTIC, DETERMINISTIC, -DETERMINISTIC, -DETERMINISTIC,
     AMBIGUITY_CHECK, AMBIGUITY_CHECK, -AMBIGUITY_CHECK, -AMBIGUITY_CHECK,
};

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



static char	*option_get_text (SXINT	kind)
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
    char H [12], K [12];

    if (is_rlr && h > 0)
	sprintf (H, "R (%ld) ", (long)h);

    if (k >= 0)
	sprintf (K, " (%ld)", (long)k);

    sprintf (string, "%s%sLR%s", (is_rlr ? (h == 0 ? "R (oxo) " : H) : ""),
	     (is_lalr ? "LA" : ""), (k >= 0 ? K : ""));

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

    switch (code_lalr1 = LALR1 ()) {
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

  csynt_options_set = OPTION (LIST_SYSTEM_CONFLICTS) | OPTION (D0) | OPTION (UD) | OPTION (UC);
  optim_options_set = OPTION (PARTIAL_SPE);
  is_left_corner = FALSE;
  conflict_derivation_kind = 0;
  is_force = sxverbosep = is_list_system_conflicts = is_pspe =
    is_unique_derivation_per_conflict = is_unique_conflict_per_state = TRUE;
  is_automaton = is_list_user_conflicts = is_list_conflicts = is_abstract =
    is_floyd_evans = is_long_listing = is_tspe =
    is_nspe = is_lr_constructor = is_rlr_constructor = 
    is_ambiguity_check = is_statistics = is_super_arc = print_time =
    is_non_deterministic_automaton = FALSE;
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
      is_long_listing = TRUE, csynt_options_set |= OPTION (LONG_LISTING), optim_options_set |= OPTION (LONG_LISTING);
      break;

    case -LONG_LISTING:
      is_long_listing = FALSE, csynt_options_set &= noOPTION (LONG_LISTING), optim_options_set &= noOPTION (LONG_LISTING);
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
      is_pspe = FALSE, optim_options_set &= noOPTION (PARTIAL_SPE);
      is_tspe = FALSE, optim_options_set &= noOPTION (TOTAL_SPE);
      is_nspe = TRUE, optim_options_set |= OPTION (NO_SPE);
      break;

    case D0:
      /* Pas de derivation. */
      conflict_derivation_kind = 0, csynt_options_set &= noOPTION (D1) & noOPTION (D2) & noOPTION (UC) & noOPTION (UD), csynt_options_set |= OPTION (D0);
      is_unique_conflict_per_state = FALSE;
      is_unique_derivation_per_conflict = FALSE;
      break;

    case D1:
      /* Derivations abregees. */
      conflict_derivation_kind = 1, csynt_options_set &= noOPTION (D0) & noOPTION (D2), csynt_options_set |= OPTION (D1);
      break;

    case D2:
      /* Derivations detaillees. */
      conflict_derivation_kind = 2, csynt_options_set &= noOPTION (D0) & noOPTION (D1), csynt_options_set |= OPTION (D2);
      break;

    case UC:
      is_unique_conflict_per_state = TRUE, csynt_options_set |= OPTION (UC);
      break;

    case -UC:
      is_unique_conflict_per_state = FALSE, csynt_options_set &= noOPTION (UC);
      break;

    case UD:
      is_unique_derivation_per_conflict = TRUE, csynt_options_set |= OPTION (UD);
      break;

    case -UD:
      is_unique_derivation_per_conflict = FALSE, csynt_options_set &= noOPTION (UD);
      break;

    case S:
      is_statistics = TRUE, csynt_options_set |= OPTION (S);
      break;

    case -S:
      is_statistics = FALSE, csynt_options_set &= noOPTION (S);
      break;

    case SA:
      is_super_arc = TRUE, csynt_options_set |= OPTION (SA);
      break;

    case -SA:
      is_super_arc = FALSE, csynt_options_set &= noOPTION (SA);
      break;

    case TIME:
      print_time = TRUE, csynt_options_set |= OPTION (TIME);
      break;

    case -TIME:
      print_time = FALSE, csynt_options_set &= noOPTION (TIME);
      break;

    case DETERMINISTIC:
      is_non_deterministic_automaton = FALSE, csynt_options_set |= OPTION (DETERMINISTIC);
      break;

    case -DETERMINISTIC:
      is_non_deterministic_automaton = TRUE, csynt_options_set &= noOPTION (DETERMINISTIC);
      break;

    case AMBIGUITY_CHECK:
      is_ambiguity_check = TRUE, csynt_options_set |= OPTION (AMBIGUITY_CHECK);
      break;

    case -AMBIGUITY_CHECK:
      is_ambiguity_check = FALSE, csynt_options_set &= noOPTION (AMBIGUITY_CHECK);
      break;

    case UNKNOWN_ARG:
      if (*arg++ != '-')
	goto run;

      c = *arg++;

      if (c != 'r' && c != 'l') {
	fprintf (sxstderr, "%s: the unknown option %s is ignored.\n", ME, argv [argnum]);
	break;
      }

      /* Test de [r[h]][la]lr[k] */
      /* Le k de r[h][la]lrk a la signification suivante :
	 un etat d'un [X]ARC est considere comme conflictuel (pour limiter sa taille) si aucune de
	 ses chaines de look-ahead associees est de taille inferieure a k. */
      /* r[h][la]lr 	=> on prend la valeur de k par defaut.
	 r[h][la]lr0 	=> on prend pour k une valeur infinie.
	 r[h][la]lrk 	=> valeur de k. */
      /* initialisation values */

      is_lr_constructor = TRUE;
      is_arg_error = FALSE;

      if (c == 'r') {
	is_rlr_constructor = TRUE;
	k_value = 10; /* pour l'instant. */
	c = *arg++;
	h_value = is_digit (c) ? str_to_nb () : 0;
      }
      else
	h_value = -1;

      if (c == 'l') {
	c = *arg++;

	if (c == 'a') {
	  is_lr_constructor = FALSE;

	  if ((c = *arg++) == 'l')
	    c = *arg++;
	  else
	    is_arg_error = TRUE;
	}

	if (c == 'r') {
	  c = *arg++;

	  if (is_digit (c))
	    k_value = str_to_nb ();
	}
	else
	  is_arg_error = TRUE;
      }
      else
	is_arg_error = TRUE;
	    
      if (!is_arg_error) {
	if (c != NUL)
	  is_arg_error = TRUE;
	else {
	  /* On verifie la compatibilite des valeurs de h et k. */


	  /*
	    |	h	k
	    -------------------------------------
	    lr    rlr   |	0	>=0
	    lr   !rlr   |	-1	>0
	    !lr    rlr	|	>=0	>=0
	    !lr   !rlr	|	-1	>0
	  */

	  if (is_lr_constructor && is_rlr_constructor) {
	    if (h_value != 0)
	      is_arg_error = TRUE;
	  }
	  else
	    if (!is_rlr_constructor && k_value == 0)
	      is_arg_error = TRUE;
	}
      }

	    
      if (is_arg_error) {
	fprintf (sxstderr,
		 "%s: The string \"%s\" should match the pattern \n\
\t\"r[0]lr[k1] | r[h]lalr[k1] | lr[k] | lalr[k]\",\n\
 with h, k1 >= 0 and k > 0, default values (i.e. \"lalr1\") are retained.\n", ME,
		 argv [argnum]);
	is_rlr_constructor = is_lr_constructor = FALSE;
	h_value = k_value = 1;
      }

      break;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
      break;
    }
  }

  if (is_rlr_constructor && is_lr_constructor)
    h_value = 0;

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
