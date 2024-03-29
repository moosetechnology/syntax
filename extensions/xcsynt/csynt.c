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

/* Constructeur syntaxique de SYNTAX */

#define SX_DFN_EXT_VAR_XCSYNT 

#include "sxversion.h"
#include "csynt_optim.h"
#include "RLR.h"
#include "bstr.h"
#include "put_edit.h"

char WHAT_XCSYNTCSYNT[] = "@(#)SYNTAX - $Id: csynt.c 3702 2024-02-07 18:51:13Z garavel $" WHAT_DEBUG;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "csynt";
static char	*arg, c;
static bool	is_arg_error;
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

static SXINT	csynt_options_set, optim_options_set;

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
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
			    bool is_lalr,
			    bool is_rlr,
			    SXINT h,
			    SXINT k)
{
    char H [28], K [28];

    if (is_rlr && h > 0)
	sprintf (H, "R (%ld) ", (SXINT) h);

    if (k >= 0)
	sprintf (K, " (%ld)", (SXINT) k);

    sprintf (string, "%s%sLR%s", (is_rlr ? (h == 0 ? "R (oxo) " : H) : ""),
	     (is_lalr ? "LA" : ""), (k >= 0 ? K : ""));

    return string;
}


struct bstr	*options_text (SXINT kind, struct bstr *bstr)
{
    SXINT	*options_set;
    SXINT	i;
    bool	is_first = true;

    options_set = kind == 1 ? &csynt_options_set : &optim_options_set;
    bstr_raz (bstr);

    for (i = 1; i <= LAST_OPTION; i++)
	if (*options_set & OPTION (i)) {
	    if (is_first)
		is_first = false;
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
        /* FALLTHROUGH */

    default:
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = true;
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
  static bool	has_message [SXSEVERITIES];

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    return SXSEVERITIES;
  }

  sxttycol1p = true;

  /* valeurs par defaut */

  csynt_options_set = OPTION (LIST_SYSTEM_CONFLICTS) | OPTION (D0) | OPTION (UD) | OPTION (UC);
  optim_options_set = OPTION (PARTIAL_SPE);
  is_left_corner = false;
  conflict_derivation_kind = 0;
  is_force = sxverbosep = is_list_system_conflicts = is_pspe =
    is_unique_derivation_per_conflict = is_unique_conflict_per_state = true;
  is_automaton = is_list_user_conflicts = is_list_conflicts = is_abstract =
    is_floyd_evans = is_long_listing = is_tspe =
    is_nspe = is_lr_constructor = is_rlr_constructor = 
    is_ambiguity_check = is_statistics = is_super_arc = print_time =
    is_non_deterministic_automaton = false;
  h_value = k_value = 1;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (arg = argv [argnum])) {
    case FORCE:
      is_force = true;
      break;

    case -FORCE:
      is_force = false;
      break;

    case VERBOSE:
      sxverbosep = true;
      break;

    case -VERBOSE:
      sxverbosep = false;
      break;

    case AUTOMATON:
      is_automaton = true, csynt_options_set |= OPTION (AUTOMATON);
      break;

    case -AUTOMATON:
      is_automaton = false, csynt_options_set &= noOPTION (AUTOMATON);
      break;

    case LIST_USER_CONFLICTS:
      is_list_user_conflicts = true, csynt_options_set |= OPTION (LIST_USER_CONFLICTS);
      break;

    case -LIST_USER_CONFLICTS:
      is_list_user_conflicts = false, csynt_options_set &= noOPTION (LIST_USER_CONFLICTS);
      break;

    case LIST_SYSTEM_CONFLICTS:
      is_list_system_conflicts = true, csynt_options_set |= OPTION (LIST_SYSTEM_CONFLICTS);
      break;

    case -LIST_SYSTEM_CONFLICTS:
      is_list_system_conflicts = false, csynt_options_set &= noOPTION (LIST_SYSTEM_CONFLICTS);
      break;

    case LIST_CONFLICTS:
      is_list_system_conflicts = is_list_user_conflicts = true, csynt_options_set |= OPTION (LIST_CONFLICTS);
      break;

    case -LIST_CONFLICTS:
      is_list_system_conflicts = is_list_user_conflicts = false, csynt_options_set &= noOPTION (LIST_CONFLICTS);
      break;

    case ABSTRACT:
      is_abstract = true, optim_options_set |= OPTION (ABSTRACT);
      break;

    case -ABSTRACT:
      is_abstract = false, optim_options_set &= noOPTION (ABSTRACT);
      break;

    case FLOYD_EVANS:
      is_floyd_evans = true, optim_options_set |= OPTION (FLOYD_EVANS);
      break;

    case -FLOYD_EVANS:
      is_floyd_evans = false, optim_options_set &= noOPTION (FLOYD_EVANS);
      break;

    case LONG_LISTING:
      is_long_listing = true, csynt_options_set |= OPTION (LONG_LISTING), optim_options_set |= OPTION (LONG_LISTING);
      break;

    case -LONG_LISTING:
      is_long_listing = false, csynt_options_set &= noOPTION (LONG_LISTING), optim_options_set &= noOPTION (LONG_LISTING);
      break;

    case PARTIAL_SPE:
      is_pspe = true, optim_options_set |= OPTION (PARTIAL_SPE);
      is_tspe = false, optim_options_set &= noOPTION (TOTAL_SPE);
      is_nspe = false, optim_options_set &= noOPTION (NO_SPE);
      break;

    case TOTAL_SPE:
      is_pspe = false, optim_options_set &= noOPTION (PARTIAL_SPE);
      is_tspe = true, optim_options_set |= OPTION (TOTAL_SPE);
      is_nspe = false, optim_options_set &= noOPTION (NO_SPE);
      break;

    case NO_SPE:
      is_pspe = false, optim_options_set &= noOPTION (PARTIAL_SPE);
      is_tspe = false, optim_options_set &= noOPTION (TOTAL_SPE);
      is_nspe = true, optim_options_set |= OPTION (NO_SPE);
      break;

    case D0:
      /* Pas de derivation. */
      conflict_derivation_kind = 0, csynt_options_set &= noOPTION (D1) & noOPTION (D2) & noOPTION (UC) & noOPTION (UD), csynt_options_set |= OPTION (D0);
      is_unique_conflict_per_state = false;
      is_unique_derivation_per_conflict = false;
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
      is_unique_conflict_per_state = true, csynt_options_set |= OPTION (UC);
      break;

    case -UC:
      is_unique_conflict_per_state = false, csynt_options_set &= noOPTION (UC);
      break;

    case UD:
      is_unique_derivation_per_conflict = true, csynt_options_set |= OPTION (UD);
      break;

    case -UD:
      is_unique_derivation_per_conflict = false, csynt_options_set &= noOPTION (UD);
      break;

    case S:
      is_statistics = true, csynt_options_set |= OPTION (S);
      break;

    case -S:
      is_statistics = false, csynt_options_set &= noOPTION (S);
      break;

    case SA:
      is_super_arc = true, csynt_options_set |= OPTION (SA);
      break;

    case -SA:
      is_super_arc = false, csynt_options_set &= noOPTION (SA);
      break;

    case TIME:
      print_time = true, csynt_options_set |= OPTION (TIME);
      break;

    case -TIME:
      print_time = false, csynt_options_set &= noOPTION (TIME);
      break;

    case DETERMINISTIC:
      is_non_deterministic_automaton = false, csynt_options_set |= OPTION (DETERMINISTIC);
      break;

    case -DETERMINISTIC:
      is_non_deterministic_automaton = true, csynt_options_set &= noOPTION (DETERMINISTIC);
      break;

    case AMBIGUITY_CHECK:
      is_ambiguity_check = true, csynt_options_set |= OPTION (AMBIGUITY_CHECK);
      break;

    case -AMBIGUITY_CHECK:
      is_ambiguity_check = false, csynt_options_set &= noOPTION (AMBIGUITY_CHECK);
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

      is_lr_constructor = true;
      is_arg_error = false;

      if (c == 'r') {
	is_rlr_constructor = true;
	k_value = 10; /* pour l'instant. */
	c = *arg++;
	h_value = is_digit (c) ? str_to_nb () : 0;
      }
      else
	h_value = -1;

      if (c == 'l') {
	c = *arg++;

	if (c == 'a') {
	  is_lr_constructor = false;

	  if ((c = *arg++) == 'l')
	    c = *arg++;
	  else
	    is_arg_error = true;
	}

	if (c == 'r') {
	  c = *arg++;

	  if (is_digit (c))
	    k_value = str_to_nb ();
	}
	else
	  is_arg_error = true;
      }
      else
	is_arg_error = true;
	    
      if (!is_arg_error) {
	if (c != SXNUL)
	  is_arg_error = true;
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
	      is_arg_error = true;
	  }
	  else
	    if (!is_rlr_constructor && k_value == 0)
	      is_arg_error = true;
	}
      }

	    
      if (is_arg_error) {
	fprintf (sxstderr,
		 "%s: The string \"%s\" should match the pattern \n\
\t\"r[0]lr[k1] | r[h]lalr[k1] | lr[k] | lalr[k]\",\n\
 with h, k1 >= 0 and k > 0, default values (i.e. \"lalr1\") are retained.\n", ME,
		 argv [argnum]);
	is_rlr_constructor = is_lr_constructor = false;
	h_value = k_value = 1;
      }

      break;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (SXINT) option_get_kind (argv [argnum]));
      break;
    }
  }

  if (is_rlr_constructor && is_lr_constructor)
    h_value = 0;

  fprintf (sxstderr, "%s: a language name is needed.\n", ME);
  return SXSEVERITIES;

 run:
  if (sxverbosep) {
    setbuf (stdout, NULL);
    fprintf (sxtty, "%s\n", release_mess);
  }

  get_constructor_name (constructor_name, (bool) (!is_lr_constructor), is_rlr_constructor, h_value, k_value);
  vstr = varstr_alloc (256);

  do {
    language_name = argv [argnum++];
    has_message [csynt_run ()] = true;
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



void	sat_mess (char *name, SXINT no, SXINT old_size, SXINT new_size)
{
#if EBUG
    fprintf (statistics_file, "%s: Array %ld of size %ld overflowed: reallocation with size %ld.\n", name, (SXINT) no, (SXINT) old_size, (SXINT) new_size);
#else
    sxuse (name);
    sxuse (no);
    sxuse (old_size);
    sxuse (new_size);
#endif
}
