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
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 



/* Constructeur syntaxique de SYNTAX */



#define SX_DFN_EXT_VAR_XCSYNT 

#include "sxdynam_parser.h"
#include "sxversion.h"
char WHAT_XCSYNTCSYNTDMAIN[] = "@(#)SYNTAX - $Id: csyntd_main.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

/*---------------*/
/*    options    */
/*---------------*/

static char	ME [] = "csyntd";
static char	Usage [] = "\
Usage:\t%s [options] language...\n\
options=\t-v, -verbose, -nv, -noverbose,\n";
static long	csyntd_options_set;
static char	*arg, c;

static SXINT	*state_stack;

char		*language_name;


#define OPTION(opt)	(1 << (opt - 1))
#define noOPTION(opt)	(~(OPTION (opt)))

#define OPT_NB ((sizeof (option_kind)/sizeof (option_kind[0]))-1)

#define UNKNOWN_ARG 		0
#define VERBOSE 		1
#define LAST_OPTION		VERBOSE

static char	*option_tbl [] = {"", "v", "verbose", "nv", "noverbose"
				      };
static SXINT	option_kind [] = {UNKNOWN_ARG,
				      VERBOSE, VERBOSE, -VERBOSE, -VERBOSE
				  };

#define is_digit(c)	c >= '0' && c <= '9'



static SXINT	option_get_kind (arg)
    char	*arg;
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



static SXINT	csyntd_run ()
{
    sxfiledesc_t	file_descr	/* file descriptor */ ;
    char	file_name [128];
    SXBOOLEAN	is_prio;
    SXINT		state, transition, next_state;

    sxdp_alloc (200 /* rule_nb */);

    {
      /* Cas de la lecture dynamique de la grammaire */

      /* "lit" la grammaire initiale. */
      SXINT symbols [50];		/* TRES TRES LAID !!! */
      SXINT	rule, i, item, symbol, new_rule, action_nb, nt, xnt, t, xt, prio, assoc;

#include "B_tables.h"

      extern SXBOOLEAN	prio_read ();
      extern SXVOID	prio_free ();

      struct bnf_ag_item	bnf_ag;
      struct priority	*t_priorities, *r_priorities;
	
      if (!bnf_read (&bnf_ag, language_name))
	sxexit ();

      if (prio_read (bnf_ag.WS_TBL_SIZE.xtmax + 1,
		     bnf_ag.WS_TBL_SIZE.xnbpro + 1,
		     &t_priorities,
		     &r_priorities,
		     language_name))
	{
	  SXDG.r_priorities = (struct priority*) sxalloc (XxY_size (SXDG.P) + 1,
							  sizeof (struct priority));

	  SXDG.t_priorities = (struct priority*) sxalloc (XxY_size (SXDG.xt) + 1,
							  sizeof (struct priority));
	}
      else
	SXDG.t_priorities = SXDG.r_priorities = NULL;

      for (nt = 1; nt <= bnf_ag.WS_TBL_SIZE.ntmax; nt++)
	{
	  sxdp_new_xnt (nt, -1, &xnt);
	}

      for (nt = bnf_ag.WS_TBL_SIZE.ntmax + 1; nt <= bnf_ag.WS_TBL_SIZE.ntmax; nt++)
	{
	  sxdp_new_xnt (nt,
			bnf_ag.XNT_TO_NT_PRDCT [nt - bnf_ag.WS_TBL_SIZE.ntmax].prdct_no,
			&xnt);
	}

      prio = assoc = 0;

      for (t = -1; t >= bnf_ag.WS_TBL_SIZE.tmax; t--)
	{
	  if (SXDG.t_priorities != NULL)
	    {
	      prio = t_priorities [-t].value * 100;
	      assoc = t_priorities [-t].assoc;
	    }

	  sxdp_new_xt (t, -1, &xt, prio, assoc);
	}

      for (t = bnf_ag.WS_TBL_SIZE.tmax - 1; -t <= bnf_ag.WS_TBL_SIZE.xtmax; t--)
	{
	  if (SXDG.t_priorities != NULL)
	    {
	      prio = t_priorities [-t].value * 100;
	      assoc = t_priorities [-t].assoc;
	    }

	  sxdp_new_xt (t,
		       bnf_ag.XT_TO_T_PRDCT [bnf_ag.WS_TBL_SIZE.tmax - t].prdct_no,
		       &xt,
		       prio,
		       assoc);
	}


      /* Pour avoir les memes numeros que ds bnf! */
      {
	SXINT	rhs, item, next_item;

	SXDG.eof_code = bnf_ag.WS_TBL_SIZE.tmax;
	SXDG.start_symbol = 1;
	SXDG.super_start_symbol = 0;
	SXDG.super_rule = 0;
	XxY_set (&SXDG.rhs, SXDG.eof_code, 0, &rhs);
	XxY_set (&SXDG.rhs, SXDG.start_symbol, rhs, &rhs);
	XxY_set (&SXDG.rhs, SXDG.eof_code, rhs, &rhs);
	/* On ne met pas la super-regle ds SXDG.P */
	/* XxY_set (&SXDG.P, SXDG.super_start_symbol, rhs, &rule); */

	SXDG.rule_to_rhs_lgth [SXDG.super_rule] = 3;
	XxY_set (&SXDG.items, SXDG.super_rule, rhs, &item);
	SXDG.rule_to_first_item [SXDG.super_rule] = item;

	while (rhs != 0)
	  {
	    rhs = XxY_Y (SXDG.rhs, rhs);
	    XxY_set (&SXDG.items, SXDG.super_rule, rhs, &next_item);
	    SXDG.item_to_next_item [item] = next_item;
	    item = next_item;
	  }

	SXDG.item_to_next_item [item] = 0;
      }

      for (rule = 1; rule <= bnf_ag.WS_TBL_SIZE.xnbpro; rule++)
	{
	  symbols [i = 0] = bnf_ag.WS_NBPRO [rule].reduc;
	  item = bnf_ag.WS_NBPRO [rule].prolon;

	  while ((symbol = bnf_ag.WS_INDPRO [item++].lispro) != 0)
	    symbols [++i] = symbol;

	  /* Les parsact sont codees en negatif, ca permet de les reconnaitre
	     facilement tout en permettant de les creer dynamiquement au milieu
	     de regles normales. */
	  action_nb = bnf_ag.WS_NBPRO [rule].action;

	  if (rule > bnf_ag.WS_TBL_SIZE.nbpro && rule <= bnf_ag.WS_TBL_SIZE.actpro)
	    action_nb = -action_nb;
	    
	  if (SXDG.r_priorities != NULL)
	    {
	      prio = r_priorities [rule].value * 100;
	      assoc = r_priorities [rule].assoc;
	    }

	  if (sxdp_rule_set (i, symbols, &new_rule, action_nb, prio, assoc))
	    sxdp_rule_activate (new_rule);

	}

      prio_free (&t_priorities, &r_priorities);
      bnf_free (&bnf_ag);

      sxdp_init_state ();
    }

    state_stack = SS_alloc (50);

    /* calcul de l'automate LR (0) */
    SS_push (state_stack, SXDG.init_state);

    while (!SS_is_empty (state_stack)) {
	state = SS_pop (state_stack);

	XxY_Xforeach (SXDG.transitions, state, transition)
	{
	    if (XxY_Y (SXDG.transitions, transition) != 0)
	    {
		/* Pas reduce */
		if (sxdp_add_new_state (transition, &next_state))
		    SS_push (state_stack, next_state);
	    }
	}
    }


    if ((file_descr = creat (strcat (strcpy (file_name, language_name), ".dp"), 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot create ", ME);
	sxperror (file_name);
	return 2;
    }

    if (!sxdp_write (file_descr)) {
	fprintf (sxstderr, "%s: cannot write ", ME);
	sxperror (file_name);
	return 2;
    }

    close (file_descr);

    sxdp_free ();
    SS_free (state_stack);

    return 0;
}



/************************************************************************/
/* main function
/************************************************************************/
int main (int argc, char *argv[])
{
  int	                argnum;
  static SXBOOLEAN	has_message [SXSEVERITIES];

  sxopentty ();

  if (argc == 1) {
    fprintf (sxstderr, Usage, ME);
    return SXSEVERITIES;
  }

  /* valeurs par defaut */

  csyntd_options_set = 0;
  sxverbosep = SXFALSE;

  /* Decodage des options */

  for (argnum = 1; argnum < argc; argnum++) {
    switch (option_get_kind (arg = argv [argnum])) {
    case VERBOSE:
      sxverbosep = SXTRUE;
      break;

    case -VERBOSE:
      sxverbosep = SXFALSE;
      break;

    case UNKNOWN_ARG:
      goto run;

    default:
      fprintf (sxstderr, "%s: internal error %ld while decoding options.\n", ME, (long)option_get_kind (argv [argnum]));
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
    has_message [csyntd_run (language_name = argv [argnum++])] = SXTRUE;
  } while (argnum < argc);

  {
    SXINT	severity;

    for (severity = SXSEVERITIES - 1;
	 severity > 0 && has_message [severity] == SXFALSE;
	 severity--) {
    }

    SXEXIT (severity);
  }

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}

