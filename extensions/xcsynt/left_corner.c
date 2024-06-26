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

#define SX_DFN_EXT_VAR_XCSYNT 

#include "sxversion.h"
#include "sxcommon.h"
#include "csynt_optim.h"
#include "bstr.h"
#include "put_edit.h"

char WHAT_XCSYNTLEFTCORNER[] = "@(#)SYNTAX - $Id: left_corner.c 3697 2024-02-07 17:57:37Z garavel $" WHAT_DEBUG;

extern SXINT	LC0 (void);

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

static SXINT	csynt_options_set, optim_options_set;

#define OPTION(opt)	((SXINT) 1 << (opt - 1))
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
			    bool is_lalr, 
			    bool is_rlr, 
			    SXINT h, 
			    SXINT k)
{
    sxuse(is_lalr); /* pour faire taire gcc -Wunused */
    sxuse(is_rlr); /* pour faire taire gcc -Wunused */
    sxuse(h); /* pour faire taire gcc -Wunused */
    sprintf (string, "%sLC (%ld)", k > 0 ? "LA" : "", (SXINT) k);

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

    switch (code_lalr1 = LC0 ()) {
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

  is_left_corner = true;

  csynt_options_set = 0;
  optim_options_set = 0;
  conflict_derivation_kind = 0;

  sxverbosep = false;
  is_force = is_nspe = is_non_deterministic_automaton = true;

  is_automaton = is_list_user_conflicts = is_list_conflicts = is_abstract =
    is_floyd_evans = is_long_listing = is_tspe =
    is_pspe = is_lr_constructor = is_rlr_constructor = 
    is_ambiguity_check = is_statistics = is_super_arc = print_time =
    is_unique_derivation_per_conflict = is_unique_conflict_per_state =
    is_list_system_conflicts = false;

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

    case S:
      is_statistics = true, csynt_options_set |= OPTION (S);
      break;

    case -S:
      is_statistics = false, csynt_options_set &= noOPTION (S);
      break;

    case TIME:
      print_time = true, csynt_options_set |= OPTION (TIME);
      break;

    case -TIME:
      print_time = false, csynt_options_set &= noOPTION (TIME);
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
