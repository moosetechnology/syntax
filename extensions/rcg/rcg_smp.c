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

/* ********************************************************************
   *  This program has been generated from rcg.at                     *
   *  on Tue Jul 21 17:50:11 1998                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

/* Genere a partir d'une Range Concatenation Grammar
   - le parser en C correspondant
   - une bnf bidon qui permet d'utiliser SYNTAX pour le scanner
 */

static char	ME [] = "rcg_smp";

/*   I N C L U D E S   */

#define SXNODE struct rcg_node
#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "put_edit.h"
#include "rcg.h"
#include "rcg_td.h"
#include "varstr.h"
#include "SS.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

char WHAT_RCGSMP[] = "@(#)SYNTAX - $Id: rcg_smp.c 4191 2024-08-29 10:29:33Z garavel $" WHAT_DEBUG;

SXINT       clause2lhs_nt (SXINT clause);
extern void gen_clause_comment (FILE *parser_file, SXINT clause);
extern void lrbnf (void), 
            tig_form (void), 
            rcg_gen_parser (void), 
            gen_bnf (void), 
            one_rcg (void), 
            two_var_form (void), 
            lrfsa (void), 
            rcg2bnf (bool);

/*  N O D E   N A M E S  */
#define ERROR_n 1
#define ARGUMENT_S_n 2
#define CLAUSE_n 3
#define CLAUSE_S_n 4
#define ETC_n 5
#define NOT_n 6
#define PREDICATE_n 7
#define PREDICATE_S_n 8
#define STRING_S_n 9
#define VOID_n 10
#define external_action_n 11
#define external_predicate_n 12
#define internal_predicate_n 13
#define terminal_n 14
#define variable_n 15
/*
E N D   N O D E   N A M E S
*/

#define		ml	(max_prdct_nb/rule_nb)
#define		mp	3
#define		ms	3


/* Le 19/12/03 suppression de is_empty_arg car 
   A(X) --> . 
   est une clause qui marche avec un arg vide!! */
static bool		is_rhs, has_non_range_arg /*, is_empty_arg */, must_be_checked, is_false_clause;
static SXINT		cur_var, lhs_var, lhs_prdct, cur_prdct;
static SXINT		max_rhs_arity = 1; /* prudence */
static SXINT		etc_nb, log_max_prdct_nb;
static SXINT		cur_param, cur_arity, rhs_arity, lhs_max_ovar_nb, ovar_nb;
static SXINT		lhs_adr;
static SXINT		rhs_arg_nb, lhs_arg_nb;
static SXINT		cur_rhs, cur_clause, cur_nt, cur_t, ste;
static SXINT		*clause2rhsnb;
static SXINT		prev_var, prev_symb;
static bool		is_first_pass;
static SXBA		lhs_var_ste_set, rhs_var_ste_set, ste_set;
static SXBA             lhs_pos_set, lhs_pos_hd_set;
static SXINT            *lhs_pos2ste, max_lhs_pos, max_lhs_pos_nb;
static SXINT		*ste_stack;
static XxY_header	var_couples;

static SXBA		next_rhs_set;
static SXINT		*next_var2cur;

static SXBA		multiple_var_set, var_set, param_set;
static SXINT		*state_stack, *equal_stack, *multiple_var2eq;
static SXBA		*equal, *plus1, *moins1, *less, *less_eq;	

static SXBA		productive_clause_set, nt_set, *cf_skeleton, *rhsnt2clause /*, *multiple_rhsnt2clause */;
static SXINT		*nt_stack;

static SXBA		clause_set;
static SXINT		*clause_stack;
#if 0
static SXINT		*U_stack, *NV_stack, *V_stack, *param_stack;
static SXBA		GU_set, NV_set, V_set, lhs_var_set, *U_set;
static SXBA             rhs_var_set, productive_eclause_set, completed_clause_set, nt2empty_vector_is_set, wbvect;
static SXINT		**var_stacks_disp, *var_stacks;
#endif /* 0 */

/* static XxY_header	empty; 
   Remplace le 5/2/2002 par nt2empty_vector
   Chaque nt a un seul vecteur caracteristique qui lui est associe
   (nt2empty_vector[nt], pos) == 1 <=> l'arg en position pos de nt ne peut pas deriver ds le vide [0 <= pos < arity(nt)] */

static SXBA             *nt2empty_vector, *clause2empty_vector, pos_set;

static SXINT		*first_Ak_stack, *last_Ak_stack, *Ak_stack;
static SXBA		first_Ak_set, last_Ak_set;
static XxY_header	first_rel, last_rel;
static XH_header	first_Ak_list, last_Ak_list;

static SXINT		tooth_pos, clause;

static XxY_header	memel;
static SXINT              *memel2val;

static void
memel_oflw (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    memel2val = (SXINT*) sxrealloc (memel2val, new_size+1, sizeof (SXINT));
}


bool
IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXINT	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	if (*lhs_bits_ptr-- & *rhs_bits_ptr--)
	    return true;
    }

    return false;
}



static bool
str2int (char *str, SXINT *val)
{
    char 	*str2 = (char *) NULL;

    *val = (SXINT) strtol (str, &str2, 10);

    return (*str2 == SXNUL);
}

static bool
is_monotone (SXINT is_monotone_ste)
{
    SXINT		couple;
    bool	ret_val;

    if (SXBA_bit_is_reset_set (ste_set, is_monotone_ste)) {
	ret_val = true;

	XxY_Xforeach (var_couples, is_monotone_ste, couple) {
	    if (!is_monotone (XxY_Y (var_couples, couple))) {
		ret_val = false;
		break;
	    }
	}

	SXBA_0_bit (ste_set, is_monotone_ste);
    }
    else
	ret_val = false;

    return ret_val;
}



#if 0
/* Remplace' le 12/02/04 pour que la numerotation des variables ne depende que de la clause courante */
/* Attention : l'attribution des codes des variables de depend pas que de la clause en cours
   de traitement.  Ces codes dependent aussi des clauses precedentes a cause des ste qui dependent
   du contexte !!  C'est un peu desagreable */
static bool
check_monotonicity (void)
{
    /* On verifie que les var de la LHS sont monotones et on affecte les codes dans tous les cas */
    SXINT		ste, ste1, couple;
    bool	ret_val = true;
    

    /* Pas de boucle ds var_couples */
    ste = 0;

    while ((ste = sxba_scan (hd_var_ste_set, ste)) > 0) {
	if (!is_monotone (ste)) {
	    /* detection d'un cycle */
	    ret_val = false;
	    break;
	}
    }

    /* Dans tous les cas on donne des codes aux variables */
    max_var = 0;

    /* var_ste_set contient les ste de toutes les variables de la LHS */
    /* hd_var_ste_set contient les ste des tetes des args de la LHS */

    if (ret_val) {
	/* tri topologique */
	/* Une variable qui n'a pas de predecesseur (ou dont tous les predecesseurs sont
	   calcules) prend le code ++max_var */
	ste = 0;

	/* Attention : l'attribution des codes des variables de depend pas que de la clause en cours
	   de traitement.  Ces codes dependent aussi des clauses precedentes a cause des ste qui dependent
	   du contexte !!  C'est un peu desagreable */
	while ((ste = sxba_scan (hd_var_ste_set, ste)) > 0) {
	    XxY_Yforeach (var_couples, ste, couple) {
		break;
	    }

	    if (couple == 0) {
		/* pas de predecesseur */
		ste2var [ste] = ++max_var;
		var2ste [max_var] = ste;
		SXBA_0_bit (var_ste_set, ste);
	    }
	    else
		SXBA_0_bit (hd_var_ste_set, ste);
	}

	/* Les var de hd_var_ste_set ont des codes */
	/* Les var de var_ste_set n'ont pas de codes et ont des predecesseurs */
	while ((ste = sxba_scan (var_ste_set, 0)) > 0) {
	    do {
		XxY_Yforeach (var_couples, ste, couple) {
		    ste1 = XxY_X (var_couples, couple);

		    if (!SXBA_bit_is_set (hd_var_ste_set, ste1))
			break;
		}

		if (couple == 0) {
		    /* Tous ses predecesseurs sont calcules */
		    ste2var [ste] = ++max_var;
		    var2ste [max_var] = ste;
		    SXBA_0_bit (var_ste_set, ste);
		    SXBA_1_bit (hd_var_ste_set, ste);
		}
	    } while ((ste = sxba_scan (var_ste_set, ste)) > 0);
	}
    }
    else {
	ste = 0;

	while ((ste = sxba_scan_reset (var_ste_set, ste)) > 0) {
	    ste2var [ste] = ++max_var;
	    var2ste [max_var] = ste;
	}
    }

    sxba_empty (var_ste_set);
    sxba_empty (hd_var_ste_set);
    XxY_clear (&var_couples);

    return ret_val;
}
#endif /* 0 */

static bool
check_monotonicity (void)
{
  /* On verifie que les var de la LHS sont monotones et on affecte les codes dans tous les cas */
  SXINT		check_monotonicity_ste, ste1, couple, lhs_pos;
  bool	ret_val = true;
    

  /* Pas de boucle ds var_couples */
  lhs_pos = 0;

  while ((lhs_pos = sxba_scan (lhs_pos_hd_set, lhs_pos)) > 0) {
    check_monotonicity_ste = lhs_pos2ste [lhs_pos];

    if (!is_monotone (check_monotonicity_ste)) {
      /* detection d'un cycle */
      ret_val = false;
      break;
    }
  }

  /* Dans tous les cas on donne des codes aux variables */
  max_var = 0;
  lhs_pos = -1;

  if (ret_val) {
    /* On est sur qu'au moins une variable de tete n'a pas de predecesseur */
    /* tri topologique */
    /* Une variable qui n'a pas de predecesseur (ou dont tous les predecesseurs sont
       calcules) prend le code ++max_var */
    while ((lhs_pos = sxba_scan_reset (lhs_pos_hd_set, lhs_pos)) > 0) {
      check_monotonicity_ste = lhs_pos2ste [lhs_pos];

      XxY_Yforeach (var_couples, check_monotonicity_ste, couple) {
	break;
      }

      if (couple == 0) {
	/* Variable de "tete" sans predecesseur */
	if (ste2var [check_monotonicity_ste] == 0) {
	  /* 1ere occurrence */
	  ste2var [check_monotonicity_ste] = ++max_var;
	  var2ste [max_var] = check_monotonicity_ste;
	}

	SXBA_0_bit (lhs_pos_set, lhs_pos);
      }
    }

    /* Les variables associees aux pos de lhs_pos_set n'ont pas de codes et ont des predecesseurs */
    /* lhs_pos == -1; */
    while ((lhs_pos = sxba_scan (lhs_pos_set, lhs_pos)) > 0) {
      do {
	check_monotonicity_ste = lhs_pos2ste [lhs_pos];

	if (ste2var [check_monotonicity_ste] == 0) {
	  /* pas calcule */
	  XxY_Yforeach (var_couples, check_monotonicity_ste, couple) {
	    ste1 = XxY_X (var_couples, couple);

	    if (ste2var [ste1] == 0)
	      /* pas calcule */
	      break;
	  }

	  if (couple == 0) {
	    /* Tous ses predecesseurs sont calcules */
	    /* on peut donc le calculer */
	    ste2var [check_monotonicity_ste] = ++max_var;
	    var2ste [max_var] = check_monotonicity_ste;
	    SXBA_0_bit (lhs_pos_set, lhs_pos);
	  }
	}
	else {
	  /* calcule' */
	  SXBA_0_bit (lhs_pos_set, lhs_pos);
	}
      } while ((lhs_pos = sxba_scan (lhs_pos_set, lhs_pos)) > 0);
      /* lhs_pos == -1; */
    }
  }
  else {
    /* cyclique */
    sxba_empty (lhs_pos_hd_set);

    while ((lhs_pos = sxba_scan_reset (lhs_pos_set, lhs_pos)) > 0) {
      check_monotonicity_ste = lhs_pos2ste [lhs_pos];
	
      if (ste2var [check_monotonicity_ste] == 0) {
	ste2var [check_monotonicity_ste] = ++max_var;
	var2ste [max_var] = check_monotonicity_ste;
      }
    }
  }

  XxY_clear (&var_couples);

  max_lhs_pos = 0;

  return ret_val;
}



static bool  is_bottom_up_erasing_clause, is_top_down_erasing_clause, is_loop_clause, is_combinatorial_clause;
static bool  is_proper_clause, is_simple_clause;

static void
rcg_pi (void)
{
    /*
       I N H E R I T E D
       */

    switch (SXVISITED->father->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :		/* SXVISITED->name = STRING_S_n */
	prev_var = 0;

	if (!is_first_pass) {
	    prev_symb = -1;	/* pas de prev_symb */
	    etc_nb = 0;
	}
	break;

    case CLAUSE_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = PREDICATE_n */
	    is_rhs = false;
		
	    if (is_first_pass) {
		sxba_empty (lhs_var_ste_set);
		sxba_empty (rhs_var_ste_set);
	    }
	    else {
		XH_push (rcg_predicates, 0); /* predicat en lhs d'une clause, toujours positif */
		lhs_adr = 0;
		ovar_nb = 0;
	    }
	    break;

	case 2 :		/* SXVISITED->name = PREDICATE_S_n */
	    is_rhs = true;

	    if (is_first_pass) {
		/* On verifie que les arguments de la LHS forment une suite monotone */
		if (!check_monotonicity ())
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sThe LHS arguments are not monotonic.",
			     sxtab_ptr->err_titles [2]+1);
	    }
	    else {
		if (SXVISITED->degree > 0 || lhs_arg_nb != 1)
		    is_a_complete_terminal_grammar = false;

		if (ovar_nb > lhs_max_ovar_nb)
		    lhs_max_ovar_nb = ovar_nb;

		if (lhs_adr > max_lhs_adr)
		    max_lhs_adr = lhs_adr;

		lhs_var = max_var;
		has_non_range_arg = false;
		tooth_pos = 0;
		rhs_arg_nb = 0;
		rhs_arity = 0;
		component_nb = 0; /* Taille cumulee des composants des args de la RHS */
	    }
	    break;

	case 3 :		/* SXVISITED->name = {VOID_n, external_action_n} */
	    if (!is_first_pass) {
		if (SXVISITED->name == VOID_n)
		    clause2action [cur_clause] = SXEMPTY_STE;
		else {
		    has_sem_act = true;
		    clause2action [cur_clause] = SXVISITED->token.string_table_entry;
		}
	    }
	    break;

	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #1");
#endif
	  break;
	}
	break;

    case CLAUSE_S_n :		/* SXVISITED->name = CLAUSE_n */
	is_first_pass = true;
	clause = SXVISITED->position;
	is_false_clause = false;
	is_proper_clause = is_simple_clause = true;
	is_bottom_up_erasing_clause = is_top_down_erasing_clause = is_loop_clause = is_combinatorial_clause = false;
	break;

    case NOT_n :		/* SXVISITED->name = {PREDICATE_n} */
	break;

    case PREDICATE_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {VOID_n, external_predicate_n, internal_predicate_n} */
	    
	    break;

	case 2 :		/* SXVISITED->name = ARGUMENT_S_n */
	    break;

	default:
#if EBUG
	  sxtrap(ME,"unknown switch case #2");
#endif
	  break;
	}

	break;

    case PREDICATE_S_n :	/* SXVISITED->name = {NOT_n, PREDICATE_n} */
	if (!is_first_pass) {
	    if (SXVISITED->name == NOT_n) {
		SXINT lahead = SXVISITED->son->token.lahead;

		switch (lahead) {
		case first_t:
		case last_t:
		case false_t:
		case cut_t:
		case true_t:
		case lex_t:
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [1][0],
			     "%sThis negative predefined predicate call is ignored",
			     sxtab_ptr->err_titles [1]+1);
		    XH_push (rcg_predicates, 0);
		    break;

		default:
		    XH_push (rcg_predicates, 1); /* valid negative predicate call */
		    break;
		}
	    }
	    else
		XH_push (rcg_predicates, 0);
	}
	break;

    case STRING_S_n :		/* SXVISITED->name = {ETC_n, terminal_n, variable_n} */
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end rcg_pi */
}


/*  1 => ds T*
   -1 => entier seul
    0 => sinon
*/
SXINT
is_in_Tstar (SXINT bot3, SXINT top3)
{
  SXINT		cur3;

  if ((top3-bot3 == 1) && (XH_list_elem (rcg_parameters, bot3) < -max_ste)) {
    /* nb entier (longueur) */
    return -1;
  }

  for (cur3 = bot3; cur3 < top3; cur3++) {
    if (XH_list_elem (rcg_parameters, cur3) >= 0)
      /* Si 0 c'est un "..." et donc pas un terminal */
      return 0;
  }

  return 1;
}

SXINT
is_arg_in_Tstar (SXINT param)
{
    return is_in_Tstar (XH_X (rcg_parameters, param), XH_X (rcg_parameters, param+1));
}


static SXINT
is_a_sub_param (SXINT param, SXINT bot3, SXINT top3)
{
  SXINT		cur3, arg_lgth, lhs_bot3, lhs_top3, lhs_cur3, i;

  if (param > 0) {
    lhs_bot3 = lhs_cur3 = XH_X (rcg_parameters, param);
    lhs_top3 = XH_X (rcg_parameters, param+1);
    arg_lgth = top3-bot3;

    while (lhs_top3 - lhs_cur3 >= arg_lgth) {
      for (i = 0, cur3 = bot3; cur3 < top3; i++, cur3++) {
	if (XH_list_elem (rcg_parameters, lhs_cur3+i) !=
	    XH_list_elem (rcg_parameters, cur3))
	  break;
      }

      if (cur3 == top3)
	return lhs_cur3-lhs_bot3; /* c'est une sous_chaine a l'indice retourne' (occurrence unique) */

      lhs_cur3++;
    }
  }

  return -1; /* Ce n'est pas une sous_chaine */
}

static bool
is_a_sub_arg (SXINT is_a_sub_arg_lhs_prdct, SXINT bot3, SXINT top3)
{
  /* On regarde si l'argument courant est une sous-chaine de l'un des args de la LHS */
  /* Il doit contenir au moins une variable */
  SXINT		lhs_bot2, lhs_top2, lhs_cur2, param;

  if (is_in_Tstar (bot3, top3) == 0) {
    /* \not \in T^* */
    lhs_bot2 = XH_X (rcg_predicates, is_a_sub_arg_lhs_prdct);
    lhs_top2 = XH_X (rcg_predicates, is_a_sub_arg_lhs_prdct+1);
    
    for (lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_cur2++) {
      param = XH_list_elem (rcg_predicates, lhs_cur2);

      if (is_a_sub_param (param, bot3, top3) >= 0)
	return true;
    }
  }

  return false;
}


bool
rhs_arg2sub_arg (SXINT rhs_arg2sub_arg_lhs_prdct, SXINT arg)
{
    /* Si l'arg est de la forme ...u...v...w..., on decoupe en u, v et w */
    SXINT bot3, cur3, top3, symb;

    bot3 = XH_X (rcg_parameters, arg);
    top3 = XH_X (rcg_parameters, arg+1);
		    
    for (cur3 = bot3; cur3 < top3; cur3++) {
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb == 0) {
	    /* C'est une limite */
	    if (bot3 < cur3) {
		/* superieure */
		if (!is_a_sub_arg (rhs_arg2sub_arg_lhs_prdct, bot3, cur3))
		    return false;
	    }

	    /* et inferieure */
	    bot3 = cur3+1;
	}
    }

    if (bot3 < cur3)
	return is_a_sub_arg (rhs_arg2sub_arg_lhs_prdct, bot3, cur3);

    return true;
}


/* Le 19/05/04 */
static void
fill_clause2attr_disp (SXINT bot3, SXINT cur3, SXINT rhs_pos)
{
  SXINT  fill_clause2attr_disp_lhs_prdct, lhs_bot2, lhs_top2, lhs_cur2, lhs_pos;
  SXBA fill_clause2attr_disp_lhs_pos_set;

  fill_clause2attr_disp_lhs_prdct = XxY_X (rcg_clauses, cur_clause) & lhs_prdct_filter;
  lhs_bot2 = XH_X (rcg_predicates, fill_clause2attr_disp_lhs_prdct);
  lhs_top2 = XH_X (rcg_predicates, fill_clause2attr_disp_lhs_prdct+1);

  fill_clause2attr_disp_lhs_pos_set = clause2attr_disp [cur_clause] [rhs_pos];

  for (lhs_pos = 0, lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_pos++, lhs_cur2++) {
    if (is_a_sub_param (XH_list_elem (rcg_predicates, lhs_cur2), bot3, cur3) >= 0) {
      SXBA_1_bit (fill_clause2attr_disp_lhs_pos_set, lhs_pos);
    }
  }
}

static void
set_clause_attrs (void)
{
  SXINT set_clause_attrs_lhs_prdct, lhs_bot2, lhs_nt, lhs_arity, rhs, top, bot, cur, rhs_pos, prdct, bot2, cur2, top2;
  SXINT nt, k, param, bot3, top3, cur3, symb;

  clause2attr_disp [cur_clause] = sxbm_calloc (rhs_arg_nb+1, lhs_arg_nb+1);

  /* A chaque rhs_pos de la RHS de cur_clause, on fait correspondre l'ensemble des lhs_pos de la LHS telles que
     l'arg en rhs_pos est une sous-chaine de l'arg en lhs_pos */

  set_clause_attrs_lhs_prdct = XxY_X (rcg_clauses, cur_clause) & lhs_prdct_filter;
  lhs_bot2 = XH_X (rcg_predicates, set_clause_attrs_lhs_prdct);
  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
  lhs_arity = nt2arity [lhs_nt];

  if (lhs_arity) {
    rhs = XxY_Y (rcg_clauses, cur_clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
    rhs_pos = -1;

    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      top2 = XH_X (rcg_predicates, prdct+1);
      nt = XH_list_elem (rcg_predicates, bot2+1);

      if (nt > 0 || nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic) {
	/* Appels positifs ou negatifs */

	for (k = 0, cur2 = bot2+2; cur2 < top2; k++, cur2++) {
	  rhs_pos++;

	  if (k != 0 || nt != STRLEN_ic) {
	    param = XH_list_elem (rcg_predicates, cur2);
    
	    /* Si l'arg est de la forme ...u...v...w..., on decoupe en u, v et w */
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);
		    
	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb == 0) {
		/* "..." est une limite */
		if (bot3 < cur3) {
		  /* superieure */
		  fill_clause2attr_disp (bot3, cur3, rhs_pos);
		}

		/* et inferieure */
		bot3 = cur3+1;
	      }
	    }

	    if (bot3 < cur3)
	      fill_clause2attr_disp (bot3, cur3, rhs_pos);
	  }
	}
      }
      else
	rhs_pos += top2-bot2-2;
    }
  }
}

static SXINT *var2param;

static void
rcg_pd (void)
{
    /* D E R I V E D */

    SXINT		x, lahead;
    bool	is_old;

    switch (SXVISITED->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :
	if (!is_first_pass) {
	    XH_set (&rcg_predicates, &cur_prdct);

	    if (is_rhs) {
		SXINT		bot, arg1, arg2, bot3, X;
		bool		is_pos_call;

		bot = XH_X (rcg_predicates, cur_prdct);
		is_pos_call = XH_list_elem (rcg_predicates, bot) == 0;

		if (XH_list_elem (rcg_predicates, bot+1) < 0) {
		    /* predicat predefini */
		    arg1 = XH_list_elem (rcg_predicates, bot+2);
		    arg2 = XH_list_elem (rcg_predicates, bot+3);

		    switch (SXVISITED->father->son->token.lahead) {
		    case streq_t:
			if (SXVISITED->degree != 2) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &StrEq must be two.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}
			else {
			    if (arg1 == arg2) {
				sxerror (SXVISITED->token.source_index,
					 sxtab_ptr->err_titles [1][0],
					 "%sUseless identical arguments.",
					 sxtab_ptr->err_titles [1]+1);
				cur_prdct = 0;
			    }
			    else {
				if (is_drcg && is_pos_call) {
				    bot3 = XH_X (rcg_parameters, arg1);

				    if (XH_X (rcg_parameters, arg1+1) == bot3+1 &&
					XH_list_elem (rcg_parameters, bot3) < 0) {
					bot3 = XH_X (rcg_parameters, arg2);

					if (XH_X (rcg_parameters, arg2+1) == bot3+1) {
					    /* &StrEq ("a", X) */
					    /* Au cas ou la semantique serait definie par une DRCG, on etablit
					       la correspondance entre &StrEq ("a", X) et la variable X de la partie
					       definite en notant pour chaque clause la liste de couple (k, ste)
					       ou k est la pos de X (l'index de olb ou oub) et ste la
					       string_table_entry de X */
					    if (da_top+2 > da_size)
						clause2da = (SXINT*) sxrealloc (clause2da, (da_size *=2) + 1,
									      sizeof (SXINT));

					    clause2da [da_top++] = ++tooth_pos; /* Numero du fils */
					    X = XH_list_elem (rcg_parameters, bot3);
					    clause2da [da_top++] = var2ste [X];
					}
				    }
				}
			    }
			}

			break;

		    case streqlen_t:
			if (SXVISITED->degree != 2) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &StrEqLen must be two.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}
			else {
			    if (arg1 == arg2) {
				sxerror (SXVISITED->token.source_index,
					 sxtab_ptr->err_titles [1][0],
					 "%sUseless identical arguments.",
					 sxtab_ptr->err_titles [1]+1);
				cur_prdct = 0;
			    }
			}

			break;

		    case strlen_t:
			if (SXVISITED->degree != 2) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &StrLen must be two.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}

			/* Le test de trucs du type &StrLen(1,X) &StrLen(2,X) ou + complique est
			   fait dynamiquement */
			break;

		    case false_t:
			if (SXVISITED->degree != 0) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &False must be nul.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}
			break;

		    case first_t:
			if (SXVISITED->degree != 2) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &First must be two.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}

			has_first_last_pp = true;
			break;

		    case last_t:
			if (SXVISITED->degree != 2) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &Last must be two.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}

			has_first_last_pp = true;
			break;

		    case cut_t:
			/* L'arite de cut est quelconque (meme nulle), ses args servent uniquement a
			   determiner le moment de son execution (quand ils sont tous calcules et de
			   gauche a droite */
			break;

		    case true_t:
			/* Retourne toujours vrai, argument unique qui est decompte ds le calcul des positions
			   permet de faire du mapping identitaire entre une RCG et sa couverture 1-RCG */
			if (SXVISITED->degree != 1) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &True must be one.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}
			break;

		    case lex_t:
			/* Evaluation statique (toujours vrai).  Son unique argument est une chaine de terminaux
			   qui doivent se trouver ds le source pour que la clause courante puisse etre validee
			   (si is_lex ou is_lex2).  En particulier, ces terminaux peuvent tres bien ne PAS appartenir
			   au langage de la clause courante.  Ca permet par exemple ds les 1-RCG de n'essayer une
			   clause que si les terminaux de la clause soeur sont ds le source */
			if (SXVISITED->degree != 1) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [2][0],
				     "%sThe arity of the predefined predicate &Lex must be one.",
				     sxtab_ptr->err_titles [2]+1);
			    cur_prdct = 0;
			}
			break;

			

		    default:
			sxtrap (ME, "rcg_pd");
		    }
		}
		else if (is_pos_call)
		    tooth_pos++;

		if (cur_prdct) {
		    /* On verifie que ce predicat n'est pas un duplicat ds la RHS courante */
		    for (x = XH_X (rcg_rhs, XH_top (rcg_rhs)); x < XH_list_top (rcg_rhs); x++) {
			if (XH_list_elem (rcg_rhs, x) == cur_prdct) {
			    sxerror (SXVISITED->token.source_index,
				     sxtab_ptr->err_titles [1][0],
				     "%sThis duplicated predicate is erased.",
				     sxtab_ptr->err_titles [1]+1);

			    cur_prdct = 0;
			}
		    }
		}
	    }
	    else {
		/* A FAIRE : verifier que l'"environment" terminal de chaque variable de la LHS (ca suffit)
		   est coherent.  Par exemple pas de ... a b X ... et ... b b X ...
		   Si c'est vrai en LHS, ca le sera en RHS car chaque arg est une sous-chaine d'un arg de
		   la LHS. */
		/* En fait c'est complique car on peut avoir ... a b X ... et ... b b U X ... avec qqpart
		   qqchose qui dit que |U|==0 ! */
	    }

	    if (is_rhs) {
		if (cur_prdct) {
		    XH_push (rcg_rhs, cur_prdct);
		    rhs_arg_nb += SXVISITED->degree;
		}

		rhs_arity += SXVISITED->degree;
	    }
	    else {
		lhs_arg_nb = SXVISITED->degree;
		lhs_prdct = cur_prdct;
	    }
	}

	break;

    case CLAUSE_n :
	if (is_first_pass) {
	    is_first_pass = false;
	    /* On reboucle sur la LHS */
	    sxat_snv (SXINHERITED, SXVISITED->son);
	}
	else {
	    if (is_drcg)
		clause2da_disp [clause+1] = da_top;

#if 0
	    /* Deplace' le 25/04/08 ds check_linearity () */
	    if (!is_false_clause) {
	      if (!is_left_linear_clause) {
		if (is_proper || is_simple)
		  sxerror (SXVISITED->token.source_index,
			   sxtab_ptr->err_titles [2][0],
			   "%sThis clause is not left linear.",
			   sxtab_ptr->err_titles [2]+1);
	      }

	      if (!is_right_linear_clause) {
		if (is_proper || is_simple)
		  sxerror (SXVISITED->token.source_index,
			   sxtab_ptr->err_titles [2][0],
			   "%sThis clause is not right linear.",
			   sxtab_ptr->err_titles [2]+1);
	      }
	    }
#endif /* 0 */

	    /* Suppression des variables de la clause */
	    for (cur_var = 1; cur_var <= max_var; cur_var++) {
		ste = var2ste [cur_var];
		ste2var [ste] = var2ste [cur_var] = 0;
		var2param [cur_var] = 0;
	    }

	    max_var = 0;
	}
	break;

    case CLAUSE_S_n :
	break;

    case ETC_n :
	if (!is_first_pass) {
	    /* On est obligatoirement dans un arg de la RHS */
	    if (prev_symb == 0) {
		/* 2 "..." qui se suivent */
		sxerror (SXVISITED->token.source_index,
			 sxtab_ptr->err_titles [1][0],
			 "%sMultiple \"...\", ignored.",
			 sxtab_ptr->err_titles [1]+1);
	    }
	    else {
		etc_nb++;
		prev_symb = 0;
		XH_push (rcg_parameters, 0);
	    }
	}
	break;

    case NOT_n :
	break;

    case PREDICATE_n :
	break;

    case PREDICATE_S_n :
	if (!is_first_pass) {
	    if (SXVISITED->degree == 0)
		cur_rhs = 0;
	    else {
		XH_set (&rcg_rhs, &cur_rhs);

		if (SXVISITED->degree > max_rhs_prdct)
		    max_rhs_prdct = SXVISITED->degree;
	    }

	    is_old = XxY_set (&rcg_clauses, lhs_prdct, cur_rhs, &cur_clause);

	    if (is_old && check_set)
		sxerror (SXVISITED->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sClause already defined at line #%ld.",
			 sxtab_ptr->err_titles [2]+1,
			 clause2visited [cur_clause]->token.source_index.line);

	    if (is_old) {
		/* On "force" la mise en table de la clause courante */
		SXINT main_clause = cur_clause;

		x = 0;

		while (is_old) {
		    x++;
		    is_old = XxY_set (&rcg_clauses, (x<<log_max_prdct_nb)+lhs_prdct, cur_rhs, &cur_clause);
		}

		has_identical_clauses = true;
		SXBA_1_bit (has_an_identical_clause_set, cur_clause);
		clause2identical [cur_clause] = main_clause;
	    }
	    else {
		clause2identical [cur_clause] = cur_clause;
	    }

	    /* last_clause+1 == cur_clause */
	    last_clause = cur_clause;
	    clause2visited [cur_clause] = SXVISITED;
	    clause2lhs_var [cur_clause] = lhs_var;
	    clause2max_var [cur_clause] = max_var;

	    if (is_bottom_up_erasing_clause)
		SXBA_1_bit (clause2bottom_up_erasing, cur_clause);

	    if (has_non_range_arg)
		SXBA_1_bit (clause2non_range_arg, cur_clause);

#if 0
	    /* Suppression des variables de la partie droite courante */
	    for (cur_var = lhs_var+1; cur_var <= max_var;cur_var++) {
		ste = var2ste [cur_var];
		ste2var [ste] = var2ste [cur_var] = 0;
	    }
#endif

	    if (!is_false_clause) {
	      if (clause2identical [cur_clause] == cur_clause)
		set_clause_attrs (); /* Le 19/05/04 */

	      /* Le Ven  8 Jun 2001 11:40:03 */
	      gsize += lhs_arg_nb+rhs_arg_nb;

	      if (max_var > max_gvar)
		max_gvar = max_var;

	      if (rhs_arg_nb > max_rhs_arg_nb)
		max_rhs_arg_nb = rhs_arg_nb;

	      if (lhs_arg_nb+rhs_arg_nb > max_clause_arg_nb)
		max_clause_arg_nb = lhs_arg_nb+rhs_arg_nb;

	      if (rhs_arity > max_rhs_arity)
		max_rhs_arity = rhs_arity;

	      if (component_nb > max_gcomponent_nb)
		max_gcomponent_nb = component_nb;
	    }

	    max_var = lhs_var;
	}
	break;

    case STRING_S_n :
	if (!is_first_pass) {
	    /* Rien au cours de la 1ere passe */
	    if (SXVISITED->degree == 0) {
		cur_param = 0;
		/* Le 19/12/03 is_empty_arg = true; */
	    }
	    else {
		XH_set (&rcg_parameters, &cur_param);
	    }
	
	    if (is_rhs) {
	      if (!is_false_clause) { 
		if (SXVISITED->father->father->son->name != VOID_n) {
		  /* On n'est pas ds un predicat predefini */
		  if (SXVISITED->degree > max_arg_size)
		    max_arg_size = SXVISITED->degree;
		}
	      }

	      lahead = SXVISITED->father->father->son->token.lahead;

	      if (SXVISITED->position > 1 || (lahead != strlen_t && lahead != first_t && lahead != last_t && lahead != lex_t)) {
		/* On verifie que l'arg courant n'est pas un element de T* et est
		   une sous-chaine d'un arg de la LHS. */
		/* Modif du 2/9/98 */
		SXINT bot, top, cur_param_kind;
#if 0
		SXINT X, param;
#endif

		bot = XH_X (rcg_parameters, cur_param);
		top = XH_X (rcg_parameters, cur_param+1);

		cur_param_kind = is_in_Tstar (bot, top);

		if (cur_param_kind == 1 /* Ds T* */) {
		  /* Seul le 1er arg de StrEq ou (a la rigueur) de StrEqLen peut etre ds T^* */
		  if (SXVISITED->position == 1 && (lahead == streq_t || lahead == streqlen_t)) {
		  }
		  else
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sIn this context, terminal strings can not be used as argument.",
			     sxtab_ptr->err_titles [2]+1);
			    
		}
		else {
		  if (cur_param_kind == 0) {
		    /* vrai arg qui contient une variable (pas longueur) */
#if 0
		    /* Deplace le 25/04/08 ds check_linearity () */
		    /* On regarde si la grammaire est overlapping */
		    while (bot < top) {
		      X = XH_list_elem (rcg_parameters, bot);

		      if (X > 0) {
			param = var2param [X];

			if (param == 0)
			  var2param [X] = cur_param;
			else {
			  if (param != cur_param)
			    is_overlapping_clause = is_overlapping_grammar = true;
			}
		      }

		      bot++;
		    }
#endif /* 0 */

		    if (SXVISITED->degree != 1) {
		      /* Les predicats predefinis doivent peut etre avoir un statut particulier ? */
		      is_combinatorial_grammar = is_combinatorial_clause = true;
		      is_proper_grammar = is_simple_grammar = is_proper_clause = is_simple_clause = false;

		      if (!is_combinatorial || is_proper || is_simple) {
			sxerror (SXVISITED->token.source_index,
				 sxtab_ptr->err_titles [2][0],
				 "%sThis argument is combinatorial.",
				 sxtab_ptr->err_titles [2]+1);
		      }
		    }

		    /* Le 09/04/08 Je differe ce test car si cur_param est un compteur, il peut ne pas etre
		       une sous-chaine d'un arg de la lhs, les variables qu'il contient peuvent meme ne pas etre "consecutives" */
#if 0
		    if (!rhs_arg2sub_arg (lhs_prdct, cur_param)) {
#if 0
		      /* Le 24/08/04 comme clause2non_range_arg n'est pas utilise', je vire c,a !! */
		      if (lahead == streq_t || lahead == streqlen_t || lahead == strlen_t) {
			if (lahead == streq_t || SXVISITED->father->father->father->name == NOT_n)
			  /* Il y a du genere sur les appels negatifs */
			  has_non_range_arg = true;
		      }
		      else
#endif /* 0 */

			sxerror (SXVISITED->token.source_index,
				 sxtab_ptr->err_titles [2][0],
				 "%sMust be a sub-string of some LHS argument.",
				 sxtab_ptr->err_titles [2]+1);
		    }
#endif /* 0 */
		  }
		}

#if 0
		if (!rhs_arg2sub_arg (lhs_prdct, cur_param)) {
		  if (lahead == streq_t &&
		      SXVISITED->position == 1 &&
		      is_in_Tstar (XH_X (rcg_parameters, cur_param), XH_X (rcg_parameters, cur_param+1))) {
		    /* Le premier param de &StrEq peut etre une chaine terminale */
		  }
		  else
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sMust be a sub-string of some LHS argument.",
			     sxtab_ptr->err_titles [2]+1);
		}
#endif

		/* On calcule le nombre de composants de l'argument */
		if (etc_nb) {
		  /* Il y a des "..." */
		  if (etc_nb == 1)
		    /* "..." ou "...u" ou "u..." */
		    component_nb++;
		  else
		    component_nb += XH_list_lgth (rcg_parameters, cur_param) - etc_nb;
		}
	      }
	      else {
		/* On est sur le premier arg de &strlen, &first ou &last ou &lex */
		if (lahead == first_t || lahead == last_t || lahead == lex_t) {
		  SXNODE	*p;

		  for (p = SXVISITED->son; p != NULL; p = p->brother) {
		    if (p->name == variable_n)
		      sxerror (p->token.source_index,
			       sxtab_ptr->err_titles [2][0],
			       "%sOnly terminal symbols are allowed.",
			       sxtab_ptr->err_titles [2]+1);
		  }
		}
	      }
	    }
	    else {
	      /* On est en lhs */
	      if (!is_false_clause && SXVISITED->degree > max_arg_size)
		max_arg_size = SXVISITED->degree;
	
	      lhs_adr += SXVISITED->degree+1;

	      {
		SXINT bot, top;

		bot = XH_X (rcg_parameters, cur_param);
		top = XH_X (rcg_parameters, cur_param+1);

		if (top-bot != 1 || XH_list_elem (rcg_parameters, bot) > 0 /* var */)
		  is_a_complete_terminal_grammar = false;
	      }
	    }

	    XH_push (rcg_predicates, cur_param);
	}
	break;

    case VOID_n :
	if (SXVISITED->father->name == PREDICATE_n) {
	    if (is_first_pass) {
		if (SXVISITED->token.lahead == false_t)
		    is_false_clause = true;
	    }
	    else {
		switch (SXVISITED->token.lahead) {
		case streq_t:
		    XH_push (rcg_predicates, STREQ_ic);
		    break;

		case streqlen_t:
		    XH_push (rcg_predicates, STREQLEN_ic);
		    break;

		case strlen_t:
		    XH_push (rcg_predicates, STRLEN_ic);
		    break;

		case false_t:
		    XH_push (rcg_predicates, FALSE_ic);
	    
		    if (SXBA_bit_is_reset_set (false_clause_set, cur_clause+1))
		      PUSH (new_false_clause_stack, cur_clause+1);

		    break;

		case first_t:
		    XH_push (rcg_predicates, FIRST_ic);
		    break;

		case last_t:
		    XH_push (rcg_predicates, LAST_ic);
		    break;

		case cut_t:
		    XH_push (rcg_predicates, CUT_ic);
		    break;

		case true_t:
		    XH_push (rcg_predicates, TRUE_ic);
		    break;

		case lex_t:
		    XH_push (rcg_predicates, LEX_ic);
		    break;

		default:
		    sxtrap (ME, "rcg_pd");
		}
	    }
	}
	break;

    case external_action_n :
	break;

    case external_predicate_n :
    case internal_predicate_n :
	if (!is_first_pass) {
	    SXINT arg_nb;

	    ste = SXVISITED->token.string_table_entry;
	    cur_nt = ste2nt [ste];
	    arg_nb = SXVISITED->brother->degree;

	    if (!is_rhs && arg_nb == 1 && SXVISITED->brother->son->degree == 0)
		/* cas de A() en lhs */
		arg_nb = 0;

	    if (cur_nt == 0) {
		cur_nt = ++max_nt;
		ste2nt [ste] = max_nt;
		nt2ste [max_nt] = ste;

		if (SXVISITED->name == internal_predicate_n)
		    SXBA_1_bit (is_nt_internal, cur_nt);
		else
		    SXBA_1_bit (is_nt_external, cur_nt);

		if (cur_nt == 1 && SXVISITED->name == internal_predicate_n) {
		    /* On est en lhs de la 1ere clause */
		    if (arg_nb == 0)
			arg_nb = 1; /* erreur possible ds les autres cas */

		    cur_arity = nt2arity [cur_nt] = 1; /* "vrai" axiome */
		}
		else {
		    cur_arity = nt2arity [cur_nt] = arg_nb; /* pas d'erreur */

		    if (!is_rhs && arg_nb == 0)
			/* La premiere occur de A a la forme A() et a lieu en lhs */
			nt2arity [cur_nt] = -1;
		}

#if 0
		/* Restriction supprimee le 5/2/2002, merci Philippe... */
		/* Les bvect de "empty" sont stockes sur des long */
		if (cur_arity >= SXBITS_PER_LONG)
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sImplementation restriction: the maximum arity is limited to #%ld.",
			     sxtab_ptr->err_titles [2]+1,
			     SXBITS_PER_LONG-1);
#endif

		if (cur_arity > max_garity)
		    max_garity = cur_arity;
	    }
	    else {
		/* On verifie la coherence des arites */
		cur_arity = nt2arity [cur_nt];

		if (cur_arity == -1) {
		    /* L'arite determinee jusqu'ici est inconnue, c'est 0 ou 1 */
		    if (arg_nb == 0) {
			if (is_rhs)
			    nt2arity [cur_nt] = 0;

			cur_arity = 0;
		    }
		    else {
			/* On la force a 1, message d'erreur si arg_nb > 1 */
			nt2arity [cur_nt] = 1;
			cur_arity = 1;
		    }
		}
		else {
		    if (cur_arity == 1 && !is_rhs && arg_nb == 0)
			/* chaine vide comme seul arg en lhs d'un prdct unaire */
			arg_nb = 1; /* OK */
		}
	    }

	    if (cur_arity != arg_nb)
		sxerror (SXVISITED->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sThe arity must be %ld.",
			 sxtab_ptr->err_titles [2]+1,
			 cur_arity);

	    if (is_rhs)
		SXBA_1_bit (is_rhs_nt, cur_nt);
	    else
		SXBA_1_bit (is_lhs_nt, cur_nt);

	    if (SXVISITED->name == internal_predicate_n) {
		if (SXBA_bit_is_set (is_nt_external, cur_nt)) {
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [1][0],
			     "%sExternal class is assumed.",
			     sxtab_ptr->err_titles [1]+1);
		}
		else
		    SXBA_1_bit (is_nt_internal, cur_nt);
	    }
	    else {
		if (SXBA_bit_is_set (is_nt_internal, cur_nt)) {
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [1][0],
			     "%sInternal class is assumed.",
			     sxtab_ptr->err_titles [1]+1);
		}
		else
		    SXBA_1_bit (is_nt_external, cur_nt);
	    }

	    XH_push (rcg_predicates, cur_nt);
	}
	break;

    case terminal_n :
	if (!is_first_pass) {
	  /* On ne fait rien sur les terminaux des arg au cours de la 1ere passe */
	  char *str;

	  ste = SXVISITED->token.string_table_entry;
	  str = sxstrget (ste);

	  if (*str == '"' && sxstrlen (ste) == 1) {
	    SXNODE *ancestor = SXVISITED->father->father->father;

	    if (ancestor->name == PREDICATE_n
		&& (ancestor->son->token.lahead == first_t || ancestor->son->token.lahead == last_t)) {
	      ste2t [ste] = cur_t = 0;
	      t2ste [0] = ste;
	    }
	    else
	      sxerror (SXVISITED->token.source_index,
		       sxtab_ptr->err_titles [2][0],
		       "%sThe empty terminal string is only allowed in the first argument of &First or &Last.",
		       sxtab_ptr->err_titles [2]+1);
	  }
	  else {
	    SXINT t = SXVISITED->father->father->father->son->token.lahead;

	    if (str2int (str, &cur_t)) {
	      /* L'arg est contient un entier */
	      if (t == strlen_t) {
		if (SXVISITED->father->degree == 1 &&
		  SXVISITED->father->position == 1 &&
		    SXVISITED->father->father->father->name == PREDICATE_n) {
		  /* ... 1er arg de &StrLen */
		  if (cur_t > max_radius)
		    max_radius = cur_t;

		  cur_t = -cur_t;
		}
		else {
		  sxerror (SXVISITED->token.source_index,
			   sxtab_ptr->err_titles [2][0],
			   "%sThe first argument of &StrLen must be an integer number.",
			   sxtab_ptr->err_titles [2]+1);
		  cur_t = 0;
		}
	      }
	      else {
		if (t == streqlen_t) {
		  /* t == streqlen */
		  /* &StrEqLen (5, X) est suspect Merci Philippe */
		  if (SXVISITED->father->position == 1 &&
		      SXVISITED->father->father->father->name == PREDICATE_n) {
		  sxerror (SXVISITED->token.source_index,
			   sxtab_ptr->err_titles [1][0],
			   "%sThe first argument of &StrEqLen is an integer number, do you really mean it?",
			   sxtab_ptr->err_titles [1]+1);
		  }
		  else {
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [1][0],
			     "%sThe first argument of &StrEqLen is an integer number, do you really mean it?",
			     sxtab_ptr->err_titles [1]+1);
		  }

		  cur_t = 0;
		}
		else {
		  /* Nouveaute' si un arg contient un entier c'est un compteur */
		  cur_t += max_ste+1; /* Pour le reconnaitre */
		}
	      }
	    }
	    else {
	      if (SXVISITED->father->position == 1 &&
		  SXVISITED->father->father->father->name == PREDICATE_n &&
		  t == strlen_t) {
		sxerror (SXVISITED->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sThe first argument of &StrLen must be an integer number.",
			 sxtab_ptr->err_titles [2]+1);
		cur_t = 0;
	      }
	      else {
		cur_t = ste2t [ste];

		if (cur_t == 0) {
		  cur_t = ++max_t;
		  ste2t [ste] = max_t;
		  t2ste [max_t] = ste;
		}
	      }
	    }


#if 0
	    if (SXVISITED->position == 1 &&
		SXVISITED->father->degree == 1 &&
		SXVISITED->father->position == 1 &&
		SXVISITED->father->father->father->name == PREDICATE_n &&
		(t == strlen_t || t == streqlen_t)) {
	      bool is_int = str2int (str, &cur_t);

	      if (t == strlen_t) {
		if (is_int) {
		  if (cur_t > max_radius)
		    max_radius = cur_t;

		  cur_t = -cur_t;
		}
		else {
		  sxerror (SXVISITED->token.source_index,
			   sxtab_ptr->err_titles [2][0],
			   "%sThe first argument of &StrLen must be an integer number.",
			   sxtab_ptr->err_titles [2]+1);
		  cur_t = 0;
		}
	      }
	      else {
		/* t == streqlen */
		/* &StrEqLen (5, X) est suspect Merci Philippe */
		if (is_int) {
		  sxerror (SXVISITED->token.source_index,
			   sxtab_ptr->err_titles [1][0],
			   "%sThe first argument of &StrEqLen is an integer number, do you really mean it?",
			   sxtab_ptr->err_titles [1]+1);
		}
	      }
	    }
	    else {
	      cur_t = ste2t [ste];

	      if (cur_t == 0) {
		cur_t = ++max_t;
		ste2t [ste] = max_t;
		t2ste [max_t] = ste;
	      }
	    }
#endif /* 0 */
	  }

	  prev_symb = 1; /* Variable ou terminal */
	  XH_push (rcg_parameters, -cur_t);
	}
	break;

    case variable_n :
	ste = SXVISITED->token.string_table_entry;

	if (is_first_pass) {
	    /* La 1ere passe sur les arg de la clause permet de verifier qu'ils sont monotones et de leur affecter
	       le code correspondant */
	    /* Un arg est monotone ssi il existe une bijection entre ses variables et des nb entiers t.q.
	       telle que ces nb forment une sequence monotone.  Bien sur cette notion s'etend d' arg
	       a un predicat a une clause et a une grammaire. */
	    SXINT	couple;

	    if (is_rhs) {
		lahead = SXVISITED->father->father->father->son->token.lahead;

		if (lahead == internal_user_predicate_t || lahead == external_user_predicate_t) {
		    /* On ne tient pas compte des args des predefinis pour decider la linearite a droite */
		    if (!SXBA_bit_is_reset_set (rhs_var_ste_set, ste)) {
			is_proper_grammar = is_simple_grammar = is_proper_clause = is_simple_clause = false;
		    }
		}

		if (!SXBA_bit_is_set (lhs_var_ste_set, ste)) {
		    /* Variable de la RHS non deja definie en LHS */
		    is_bottom_up_erasing_grammar = is_bottom_up_erasing_clause = true;
		    is_simple_grammar = is_simple_clause = false;
		    sxerror (SXVISITED->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sUndefined RHS variable.",
			     sxtab_ptr->err_titles [2]+1);

		    /* On lui donne quand meme un code */
		    if (ste2var [ste] == 0) {
			ste2var [ste] = ++max_var;
			var2ste [max_var] = ste;
		    }
		}
	    }
	    else {
	      max_lhs_pos++;

	      if (prev_var != 0) {
		/* ce n'est pas la 1ere variable */
		XxY_set (&var_couples, prev_var, ste, &couple);
	      }
	      else
		/* On note les tetes des args */
		SXBA_1_bit (lhs_pos_hd_set, max_lhs_pos);

	      SXBA_1_bit (lhs_pos_set, max_lhs_pos);
	      lhs_pos2ste [max_lhs_pos] = ste;

	      if (!SXBA_bit_is_reset_set (lhs_var_ste_set, ste)) {
		is_proper_grammar = is_simple_grammar = is_proper_clause = is_simple_clause = false;
	      }

	      prev_var = ste;
	    }
	}
	else {
	    ovar_nb++;
	    cur_var = ste2var [ste];

	    if (is_rhs) {
		prev_var = cur_var;
		prev_symb = 1; /* Variable ou terminal */
	    }
	    else {
		if (!SXBA_bit_is_set (rhs_var_ste_set, ste)) {
		    /* Variable de la LHS non deja definie en RHS */
		    is_top_down_erasing_grammar = is_top_down_erasing_clause = true;
		    is_simple_grammar = is_simple_clause = false;

		    if (is_proper || is_simple) {
		      sxerror (SXVISITED->token.source_index,
			       sxtab_ptr->err_titles [2][0],
			       "%sThis LHS variable must occur in RHS.",
			       sxtab_ptr->err_titles [2]+1);
		    }
		}
	    }

	    XH_push (rcg_parameters, cur_var);
	}

	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end rcg_pd */
}

static SXINT		*nt2clause_disp, *nt2clause;


/* Si toutes les A-clauses sont fausses => A est false ainsi que toutes les clauses ds lesquelles A
   apparait en rhs */
static void
false_clause_propagate (void)
{
  SXINT false_clause_propagate_clause, A, A_clause, bot, top;
  bool done;
  SXBA A_clause_set;

  if (!IS_EMPTY (new_false_clause_stack)) {
    while (!IS_EMPTY (new_false_clause_stack)) {
      false_clause_propagate_clause = POP (new_false_clause_stack); /* ...est &False */
      A = clause2lhs_nt (false_clause_propagate_clause);
	
      if (!SXBA_bit_is_set (is_nt_external, A)) {
	A_clause_set = lhsnt2clause [A];

	A_clause = 0;

	while ((A_clause = sxba_scan (A_clause_set, A_clause)) > 0) {
	  if (!SXBA_bit_is_set (false_clause_set, A_clause))
	    break;
	}

	if (A_clause < 0) {
	  /* Toutes les A_clauses sont &False */
	  SXBA_1_bit (false_A_set, A);

	  if (A == 1) {
	    /* Tout est False ds la grammaire!! */
	    sxtmsg (sxsrcmngr.source_coord.file_name,
		    "%sThis RCG is empty (false).",
		    sxtab_ptr->err_titles [2]+1);
	    break;
	  }

	  top = nt2clause_disp [A+1];
	  bot = nt2clause_disp [A];

	  while (bot < top) {
	    false_clause_propagate_clause = nt2clause [bot];

	    if (SXBA_bit_is_reset_set (false_clause_set, false_clause_propagate_clause)) {
	      PUSH (new_false_clause_stack, false_clause_propagate_clause);
#if 0
	      fprintf (sxtty, "Induced false clause #%ld\n", false_clause_propagate_clause);
#endif
	    }

	    bot++;
	  }
	}
      }
    }

    done = SXBA_bit_is_set (false_A_set, 1);

    /* Algo un peu grossier qui "rate" par exemple A --> A (toutes les autres A-clauses sont false) */
    /* On laisse la suite s'occuper de ce cas (non_instantiable) */
    while (!done) {
      done = true;

      /* On saute l'axiome (appele depuis le main, donc pas faux) */
      A = 1;

      while ((A = sxba_0_lrscan (false_A_set, A)) > 0) {
	/* A n'est pas [encore] false */
	if (SXBA_bit_is_set (is_lhs_nt, A) && !SXBA_bit_is_set (is_nt_external, A)) {
	  top = nt2clause_disp [A+1];
	  bot = nt2clause_disp [A];

	  if (bot < top) {
	    do {
	      false_clause_propagate_clause = nt2clause [bot];

	      if (!SXBA_bit_is_set (false_clause_set, false_clause_propagate_clause))
		break;
	    } while (++bot < top);

	    if (bot == top) {
	      /* Toutes les utilisations de A sont false,
		 A ne sert donc a rien => on met les A_clauses a false */
	      SXBA_1_bit (false_A_set, A);
	    
	      A_clause_set = lhsnt2clause [A];

	      A_clause = 0;

	      while ((A_clause = sxba_scan (A_clause_set, A_clause)) > 0) {
		if (SXBA_bit_is_reset_set (false_clause_set, A_clause)) {
		  done = false;
#if 0
		  fprintf (sxtty, "Induced false clause #%ld\n", A_clause);
#endif
		}
	      }
	    }
	  }
	}
      }
    }
  }
}


static SXINT
equiv_prdct (SXINT cur_x, SXINT next_x, SXINT equiv_prdct_prev_var, SXINT equiv_prdct_lhs_var)
{
    SXINT		equiv_prdct_cur_prdct, cur_prdct_bot, cur_op, equiv_prdct_cur_nt, next_prdct, next_prdct_bot, next_op, next_nt;
    SXINT		arity, next_var, z, var;
    SXINT		equiv_prdct_cur_param, next_param, cur_param_bot, next_param_bot, param_lgth, l, cur_symb, next_symb;

    equiv_prdct_cur_prdct = XH_list_elem (rcg_rhs, cur_x);
    cur_prdct_bot = XH_X (rcg_predicates, equiv_prdct_cur_prdct);
    cur_op = XH_list_elem (rcg_predicates, cur_prdct_bot++);
    equiv_prdct_cur_nt = XH_list_elem (rcg_predicates, cur_prdct_bot);
    next_prdct = XH_list_elem (rcg_rhs, next_x);
    next_prdct_bot = XH_X (rcg_predicates, next_prdct);
    next_op = XH_list_elem (rcg_predicates, next_prdct_bot++);
    next_nt = XH_list_elem (rcg_predicates, next_prdct_bot);

    if (equiv_prdct_cur_nt != next_nt || cur_op != next_op)
	return 0;

    if (equiv_prdct_cur_nt > 0)
	arity = nt2arity [equiv_prdct_cur_nt];
    else {
	switch (equiv_prdct_cur_nt) {
	case STREQ_ic:
	case STREQLEN_ic:
	case STRLEN_ic:
	case FIRST_ic:
	case LAST_ic:
	    arity = 2;
	    break;

	case TRUE_ic:
	case LEX_ic:
	    arity = 1;
	    break;

	case FALSE_ic:
	    arity = 0;
	    break;

	case CUT_ic:
	    arity = XH_X (rcg_predicates, equiv_prdct_cur_prdct+1)-cur_prdct_bot-1;

	    if (arity != XH_X (rcg_predicates, next_prdct+1)-next_prdct_bot-1)
		/* 2 cut ayant un nb different d'args */
		return 0;

	    break;

	default:
	    sxtrap (ME, "equiv_prdct");
	    sxinitialise(arity); /* pour faire taire gcc -Wuninitialized */
	}
    }

    next_var = equiv_prdct_prev_var;

    z = 1;

    while (z <= arity) {
	/* On compare les parametres */
	equiv_prdct_cur_param = XH_list_elem (rcg_predicates, cur_prdct_bot+z);
	next_param = XH_list_elem (rcg_predicates, next_prdct_bot+z);
	cur_param_bot = XH_X (rcg_parameters, equiv_prdct_cur_param);
	next_param_bot = XH_X (rcg_parameters, next_param);
	param_lgth = XH_X (rcg_parameters, equiv_prdct_cur_param+1) - cur_param_bot;

	if (param_lgth != XH_X (rcg_parameters, next_param+1) - next_param_bot)
	    /* Longueurs differentes */
	    break;

	if (z == 1 && (equiv_prdct_cur_nt == STRLEN_ic || equiv_prdct_cur_nt == FIRST_ic || equiv_prdct_cur_nt == LAST_ic || equiv_prdct_cur_nt == LEX_ic)) {
	    cur_symb = XH_list_elem (rcg_parameters, cur_param_bot);
	    next_symb = XH_list_elem (rcg_parameters, next_param_bot);

	    if (cur_symb != next_symb)
		    /* non egalite des 1er params */
		break;
	}
	else {
	    l = 0;
			
	    while (l < param_lgth) {
		/* On compare les symboles */
		cur_symb = XH_list_elem (rcg_parameters, cur_param_bot+l);
		next_symb = XH_list_elem (rcg_parameters, next_param_bot+l);

		if (cur_symb > equiv_prdct_lhs_var && next_symb > equiv_prdct_lhs_var) {
		    if ((var = next_var2cur [next_symb]) == 0)
			var = next_var2cur [next_symb] = ++next_var;

		    if (cur_symb != var)
			/* non equivalence des params */
			break;
		}
		else if (cur_symb != next_symb)
		    /* non equivalence des params */
		    break;

		l++;
	    }

	    if (l < param_lgth)
		/* non equivalence des params */
		break;
	}

	z++;
    }

    if (z > arity)
	return next_var;

    return 0;
}

static bool
is_equiv_prdct (SXINT cur_x, 
		SXINT next_clause_bot, 
		SXINT delta, 
		SXINT rhs_lgth, 
		SXINT is_equiv_prdct_lhs_var, 
		SXINT is_equiv_prdct_prev_var, 
		SXINT is_equiv_prdct_max_var, 
		SXINT n)
{
    SXINT		next_x, next_var, next_delta, y;

    next_x = next_clause_bot + delta;

    if ((next_var = equiv_prdct (cur_x, next_x, is_equiv_prdct_prev_var, is_equiv_prdct_lhs_var)) > 0) {

	if (n == 0)
	    return true;

	SXBA_1_bit (next_rhs_set, delta);

	for (next_delta = 0; next_delta < rhs_lgth; next_delta++) {
	    if (!SXBA_bit_is_set (next_rhs_set, next_delta)) {
		if (is_equiv_prdct (cur_x+1, next_clause_bot, next_delta, rhs_lgth, is_equiv_prdct_lhs_var, next_var, is_equiv_prdct_max_var, n-1))
		    return true;
	    }
	}

	SXBA_0_bit (next_rhs_set, delta);
    }

    /* non equivalence des params : on remet en etat */
    for (y = is_equiv_prdct_max_var; y > is_equiv_prdct_lhs_var; y--) {
	if (next_var2cur [y] > is_equiv_prdct_prev_var)
	    next_var2cur [y] = 0;
    }

    return false;
}
    

static bool
equiv_rhs (SXINT equiv_rhs_cur_clause, SXINT next_clause)
{
    SXINT		cur_clause_rhs, cur_clause_bot, next_clause_rhs, next_clause_bot, rhs_lgth, equiv_rhs_lhs_var, equiv_rhs_max_var;
    SXINT		delta, y;

    cur_clause_rhs = XxY_Y (rcg_clauses, equiv_rhs_cur_clause);
    cur_clause_bot = XH_X (rcg_rhs, cur_clause_rhs);
    next_clause_rhs = XxY_Y (rcg_clauses, next_clause);
    next_clause_bot = XH_X (rcg_rhs, next_clause_rhs);

    rhs_lgth = XH_X (rcg_rhs, cur_clause_rhs+1) - cur_clause_bot;

    equiv_rhs_lhs_var = clause2lhs_var [equiv_rhs_cur_clause];
    equiv_rhs_max_var = clause2max_var [equiv_rhs_cur_clause];

    for (delta = 0; delta < rhs_lgth; delta++) {
	if (is_equiv_prdct (cur_clause_bot, next_clause_bot, delta, rhs_lgth, equiv_rhs_lhs_var, equiv_rhs_lhs_var, equiv_rhs_max_var, rhs_lgth-1))
	    break;
    }

    sxba_empty (next_rhs_set);

    if (delta == rhs_lgth)
	/* next_var2cur est RAZe */
	return false;

    for (y = equiv_rhs_max_var; y > equiv_rhs_lhs_var; y--) {
	next_var2cur [y] = 0;
    }

    return true;
}



static void
is_a_set (void)
{
    /* We check that clauses are all different after permutation of their RHSs */
    SXINT		next_clause, next_rhs, rhs_lgth, cur_bot, cur_x, is_a_set_cur_prdct, next_prdct;
    bool	is_cur_bottom_up_erasing, is_next_bottom_up_erasing;
    SXINT		*ne_couples, *e_couples;
    SXBA	tbp;

    S1_alloc (ne_couples, last_clause+1);
    S1_alloc (e_couples, last_clause+1);
    tbp = sxba_calloc (last_clause+1);
    

    XxY_foreach (rcg_clauses, cur_clause) {
	if (clause2identical [cur_clause] == cur_clause && !SXBA_bit_is_set (false_clause_set, cur_clause)) {
	    lhs_prdct = XxY_X (rcg_clauses, cur_clause);
	    cur_rhs = XxY_Y (rcg_clauses, cur_clause);

	    XxY_Xforeach (rcg_clauses, lhs_prdct, next_clause) {
		if (next_clause > cur_clause) {
		    /* Chaque paire n'est examinee qu'une fois */
		    next_rhs = XxY_Y (rcg_clauses, next_clause);
		    /* On est sur que les 2 rhs ne sont pas vides */

		    if (cur_rhs > 0 && next_rhs > 0 &&
			(rhs_lgth = XH_list_lgth (rcg_rhs, cur_rhs)) == XH_list_lgth (rcg_rhs, next_rhs)) {
			/* Longueur des parties droites identiques */
			is_cur_bottom_up_erasing = SXBA_bit_is_set (clause2bottom_up_erasing, cur_clause);
			is_next_bottom_up_erasing = SXBA_bit_is_set (clause2bottom_up_erasing, next_clause);

			if ((is_cur_bottom_up_erasing && is_next_bottom_up_erasing) ||
			    (!is_cur_bottom_up_erasing && !is_next_bottom_up_erasing)) {
			    if (is_cur_bottom_up_erasing &&
				clause2max_var [cur_clause] == clause2max_var [next_clause]) {
				/* Il y a de nouvelles variables en RHS (le meme nombre), les codes internes
				   dependent donc de l'ordre des predicats */
				S1_push (e_couples, cur_clause);
				S1_push (e_couples, next_clause);
			    }
			    else {
				/* Il n'y a pas de nouvelles variables en RHS, les codes internes ne
				   dependent donc pas de l'ordre des predicats */
				S1_push (ne_couples, cur_clause);
				S1_push (ne_couples, next_clause);
				SXBA_1_bit (tbp, cur_clause);
				SXBA_1_bit (tbp, next_clause);
			    }
			}
		    }
		}
	    }
	}
    }

    if (!S1_is_empty (ne_couples)) {
	/* On fabrique une forme normale des RHS : les predicats sont classes par leur code interne
	   (2 occur ne peuvent pas avoir le meme code interne) */
	SXINT		min_prdct, prdct, prdct_nb;
	XH_header	rhs_nf;
	SXINT		*clause2rhs_nf;

	XH_alloc (&rhs_nf, "rhs_nf", sxba_cardinal (tbp) + 1, 1, ml, NULL, NULL);
	clause2rhs_nf = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));

	cur_clause = 0;

	while ((cur_clause = sxba_scan (tbp, cur_clause)) > 0) {
	    cur_rhs = XxY_Y (rcg_clauses, cur_clause);
	    cur_bot = XH_X (rcg_rhs, cur_rhs);
	    rhs_lgth = XH_X (rcg_rhs, cur_rhs+1) - cur_bot;

	    if (rhs_lgth > 2) {
		min_prdct = 0;
		prdct_nb = 0;

		do {
		    prdct = XH_top (rcg_predicates);

		    for (cur_x = cur_bot + rhs_lgth - 1; cur_x >= cur_bot; cur_x--) {
			is_a_set_cur_prdct = XH_list_elem (rcg_rhs, cur_x);

			if (is_a_set_cur_prdct > min_prdct && is_a_set_cur_prdct < prdct)
			    prdct = is_a_set_cur_prdct;
		    }

		    XH_push (rhs_nf, prdct);
		    min_prdct = prdct;
		} while (++prdct_nb < rhs_lgth);
	    }
	    else if (rhs_lgth == 2) {
		is_a_set_cur_prdct = XH_list_elem (rcg_rhs, cur_bot);
		next_prdct = XH_list_elem (rcg_rhs, cur_bot+1);

		if (is_a_set_cur_prdct < next_prdct) {
		    XH_push (rhs_nf, is_a_set_cur_prdct);
		    XH_push (rhs_nf, next_prdct);
		}
		else {
		    XH_push (rhs_nf, next_prdct);
		    XH_push (rhs_nf, is_a_set_cur_prdct);
		}
	    }
	    else {
		/* rhs_lgth == 1 */
		is_a_set_cur_prdct = XH_list_elem (rcg_rhs, cur_bot);
		XH_push (rhs_nf, is_a_set_cur_prdct);
	    }
	    
	    XH_set (&rhs_nf, &clause2rhs_nf [cur_clause]);
	}

	do {
	    next_clause = S1_pop (ne_couples);
	    cur_clause = S1_pop (ne_couples);

	    if (clause2rhs_nf [cur_clause] == clause2rhs_nf [next_clause]) {
	      if (check_set) {
		sxerror (clause2visited [cur_clause]->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sEquivalent with the clause defined at line #%ld.",
			 sxtab_ptr->err_titles [2]+1,
			 clause2visited [next_clause]->token.source_index.line);

		/* XxY_erase (rcg_clauses, next_clause); */ /* Le 09/04/04 */
	      }

	      has_identical_clauses = true;
	      SXBA_1_bit (has_an_identical_clause_set, next_clause);
	      clause2identical [next_clause] = cur_clause;
	    }
	} while (!S1_is_empty (ne_couples));

	XH_free (&rhs_nf);
	sxfree (clause2rhs_nf);
    }

    S1_free (ne_couples);
    sxfree (tbp);

    if (!S1_is_empty (e_couples)) {
	do {
	    next_clause = S1_pop (e_couples);
	    cur_clause = S1_pop (e_couples);

	    if (equiv_rhs (cur_clause, next_clause)) {
	      if (check_set) {
		sxerror (clause2visited [cur_clause]->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sEquivalent with the clause defined at line #%ld.",
			 sxtab_ptr->err_titles [2]+1,
			 clause2visited [next_clause]->token.source_index.line);

		/* XxY_erase (rcg_clauses, next_clause); */ /* Le 09/04/04 */
	      }

	      has_identical_clauses = true;
	      SXBA_1_bit (has_an_identical_clause_set, next_clause);
	      clause2identical [next_clause] = cur_clause;
	    }
	} while (!S1_is_empty (e_couples));
    }

    S1_free (e_couples);
}


static void
extract_params (SXINT prdct)
{
    SXINT prdct_top, x, param, bot, top, y, symb;

    for (prdct_top = XH_X (rcg_predicates, prdct+1), x = XH_X (rcg_predicates, prdct)+2; x < prdct_top; x++) {
	param = XH_list_elem (rcg_predicates, x);

	if (!SXBA_bit_is_set (param_set, param)) {
	    bot = XH_X (rcg_parameters, param);
	    top = XH_X (rcg_parameters, param+1);

	    if (top > bot+1) {
		/* longueur du param > 1 */
		for (y = bot; y < top; y++) {
		    symb = XH_list_elem (rcg_parameters, y);

		    if (symb > 0) {
			/* variable */
			SXBA_1_bit (param_set, param);

			if (!SXBA_bit_is_reset_set (var_set, symb)) {
			    /* Occurrence multiple de la variable symb */
			    must_be_checked = true;
			    SXBA_1_bit (multiple_var_set, symb);
			}
		    }
		}
	    }
	}
    }
}


static SXINT max_i;

static void
set_equal (SXINT i, SXINT j)
{
    if (i == j) return;

    if (SXBA_bit_is_reset_set (equal [i], j)) {
	SXBA_1_bit (equal [j], i);
	S1_push (equal_stack, i);
	S1_push (equal_stack, j);
    }
}



static bool
is_instantiable_clause (void)
{
    /* On ne considere que les parametres ayant une variable de multiple_var_set */
    SXINT		param, bot, top, y, symb, equiv_class, i, j, k, l, symbi, symbj;
    SXBA	less_i, less_j, less_eqj, less_eqi, equali, equalj, plus1i, plus1j, moins1i, moins1j;

    param = 0;

    while ((param = sxba_scan (param_set, param)) > 0) {
	bot = XH_X (rcg_parameters, param);
	top = XH_X (rcg_parameters, param+1);

	for (y = bot; y < top; y++) {
	    symb = XH_list_elem (rcg_parameters, y);

	    if (symb > 0 && SXBA_bit_is_set (multiple_var_set, symb))
		break;
	}

	if (y < top) {
	    /* C'est un candidat, on l'epluche */
	    for (y = bot; y < top; y++) {
		PUSH (state_stack, y);
		i = state_stack [0];
		j = i+1;
		symb = XH_list_elem (rcg_parameters, y);

		if (symb >= 0) {
		    SXBA_1_bit (less_eq [i], j);

		    if (symb > 0) {
			if ((equiv_class = multiple_var2eq [symb]) == 0)
			    multiple_var2eq [symb] = i;
			else {
			    set_equal (equiv_class, i);
			    set_equal (equiv_class+1, j);
			}
		    }
		}
		else {
		    SXBA_1_bit (less [i], j);
		    SXBA_1_bit (plus1 [i], j);
		    SXBA_1_bit (moins1 [j], i);
		}
	    }

	    PUSH (state_stack, 0); /* rien pour top */
	}
    }

    /* On cherche le plus petit ensemble equal t.q.
          equal = equal0 U equal equal U plus1 equal moins1
    */

    while (!S1_is_empty (equal_stack)) {
	j = S1_pop (equal_stack);
	i = S1_pop (equal_stack);

	equali = equal [i];
	equalj = equal [j];

	k = 0;

	while ((k = sxba_scan (equalj, k)) > 0) {
	    set_equal (i, k);
	}

	l = 0;

	while ((l = sxba_scan (equali, l)) > 0) {
	    set_equal (l, j);
	}

	plus1i = plus1 [i];
	plus1j = plus1 [j];

	l = 0;

	while ((l = sxba_scan (plus1i, l)) > 0) {
	    k = 0;

	    while ((k = sxba_scan (plus1j, k)) > 0) {
		set_equal (l, k);
	    }
	}

	moins1i = moins1 [i];
	moins1j = moins1 [j];

	l = 0;

	while ((l = sxba_scan (moins1i, l)) > 0) {
	    k = 0;

	    while ((k = sxba_scan (moins1j, k)) > 0) {
		set_equal (l, k);
	    }
	}
    }

    max_i = state_stack [0];

    /* On calcule less_equal = (less_equal U equal)^* */
    for (i = 1; i <= max_i; i++)
	sxba_or (less_eq [i], equal [i]);

    fermer (less_eq, max_i + 1);

    /* On calcule less = [less_equal] (less [less_equal])^* */

    for (i = 1; i <= max_i; i++) {
	j = 0;
	less_i = less [i];

	while ((j = sxba_scan (less_i, j)) > 0) {
	    k = 0;
	    less_eqj = less_eq [j];
	    
	    while ((k = sxba_scan (less_eqj, k)) > 0)
		SXBA_1_bit (less_i, k);
	}
    }

    fermer (less, max_i + 1);

    for (i = 1; i <= max_i; i++) {
	j = 0;
	less_eqi = less_eq [i];

	while ((j = sxba_scan (less_eqi, j)) > 0) {
	    k = 0;
	    less_j = less [j];
	    
	    while ((k = sxba_scan (less_j, k)) > 0)
		SXBA_1_bit (less [i], k);
	}
    }

    /* On cherche les incompatibilites :
          i less j & j less_equal i
	  exists i,j ai,aj \in T et ai != aj et i equal j
    */

    for (i = 1; i <= max_i; i++) {
	j = 0;
	less_i = less [i];

	while ((j = sxba_scan (less_i, j)) > 0) {
	    if (SXBA_bit_is_set (less_eq [j], i))
		return false;
	}

	j = i;
	equali = equal [i];

	while ((j = sxba_scan (equali, j)) > 0) {
	    symbi = XH_list_elem (rcg_parameters, state_stack [i]);
	    symbj = XH_list_elem (rcg_parameters, state_stack [j]);

	    if (symbi < 0 && symbj < 0 && symbi != symbj)
		return false;
	}
    }

    return true;
}


/* Remplit, pour chaque Ak l'element counter_Ak_se qui indique si l'arg k de A est un compteur */
/* Attention une variable peut designer a la fois (ds la meme clause) un range et un compteur */
/* Inversement un compteur ne peut pas devenir un range */
/* Si l'arg Ak contient une cste => c'est un compteur */
/* Si l'arg Ak est un compteur Ak ne peut pas etre vide ni contenir de terminaux */
/* De plus Si Ak est en lhs et s'il contient la variable X, tout arg de la RHS qui contient X est un compteur */
static void
check_counter_args (void)
{
  SXINT     top, param, size, symb, prdct_bot, prdct_code, Ak, k, Bh, prdct_top, x, y, rhs, prdct, A, rhs_bot, rhs_top, param_bot, param_top;
  SXBA      has_t_param_set, counter_param_set, variable_set, *prdct2clause_set, local_clause_set;
  SXINT     *Ak_stack_check_counter_args;
  SXNODE    *prdct_ptr, *param_ptr;
  bool has_var;



  top = XH_top (rcg_parameters);

  has_t_param_set = sxba_calloc (top+1);
  counter_param_set = sxba_calloc (top+1);
  variable_set = sxba_calloc (max_ste+1);

  Ak_stack_check_counter_args = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT)), RAZ (Ak_stack_check_counter_args);

  prdct2clause_set = sxbm_calloc (max_nt+1, XxY_top (rcg_clauses)+1);
  
  for (param = 1; param < top; param++) {
    param_bot = XH_X (rcg_parameters, param);
    param_top = XH_X (rcg_parameters, param+1);
    size = param_top-param_bot;

    if (size > 0) {
      while (param_bot < param_top) {
	symb = XH_list_elem (rcg_parameters, param_bot);

	if (symb < -max_ste) {
	  /* param contient une constante entiere */
	  SXBA_1_bit (counter_param_set, param);
	}
	else {
	  if (symb < 0)
	    /* param contient un terminal */
	    SXBA_1_bit (has_t_param_set, param);
	}

	param_bot++;
      }
    }
  }

  XxY_foreach (rcg_clauses, cur_clause) {
    if (clause2identical [cur_clause] == cur_clause && !SXBA_bit_is_set (false_clause_set, cur_clause)) {
      lhs_prdct = XxY_X (rcg_clauses, cur_clause) & lhs_prdct_filter;
      prdct_bot = XH_X (rcg_predicates, lhs_prdct);
      prdct_code = XH_list_elem (rcg_predicates, prdct_bot+1);

      SXBA_1_bit (prdct2clause_set [prdct_code], cur_clause); /* en lhs */
      Ak = A_k2Ak (prdct_code, 0);
      k = 0;
      prdct_ptr = clause2visited [cur_clause]->father->son; /* lhs predicat */
      param_ptr = prdct_ptr->son->brother->son; /* 1er arg */
	  
      for (prdct_top = XH_X (rcg_predicates, lhs_prdct+1), x = prdct_bot+2; x < prdct_top; x++, Ak++, k++) {
	param = XH_list_elem (rcg_predicates, x);

	if (SXBA_bit_is_set (counter_param_set, param) && SXBA_bit_is_set (has_t_param_set, param)) {
	  /* Terminal et constante */
	  sxerror (param_ptr->token.source_index,
		   sxtab_ptr->err_titles [2][0],
		   "%sThe %ld%s parameter of predicate <%s> is erroneous since it contains both a terminal symbol and an integer constant.",
		   sxtab_ptr->err_titles [2]+1,
		   k+1,
		   k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
		   sxstrget (nt2ste [prdct_code]));
	}
	else {
	  if (SXBA_bit_is_set (counter_param_set, param)) {
	    /* Ak doit etre un compteur (ainsi que les variables qui le composent) */
	    if (SXBA_bit_is_reset_set (counter_Ak_set, Ak))
	      PUSH (Ak_stack_check_counter_args, Ak);
	  }
	}
	      
	param_ptr = param_ptr->brother;
      }

      rhs = XxY_Y (rcg_clauses, cur_clause);

      if (rhs > 0) {
	prdct_ptr = prdct_ptr->brother->son;
	
	for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	  prdct = XH_list_elem (rcg_rhs, x);
	  prdct_bot = XH_X (rcg_predicates, prdct);
	  prdct_code = XH_list_elem (rcg_predicates, prdct_bot+1);

	  if (prdct_code > 0) {
	    /* Pas predefini */
	    Ak = A_k2Ak (prdct_code, 0);
	    k = 0;
	    param_ptr = prdct_ptr->son->brother->son; /* 1er arg */
	  
	    for (prdct_top = XH_X (rcg_predicates, prdct+1), y = prdct_bot+2; y < prdct_top; y++, Ak++, k++) {
	      param = XH_list_elem (rcg_predicates, y);

	      if (SXBA_bit_is_set (counter_param_set, param) && SXBA_bit_is_set (has_t_param_set, param)) {
		sxerror (param_ptr->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			"%sThe %ld%s parameter of predicate <%s> is erroneous since it contains both a terminal symbol and an integer constant.",
			 sxtab_ptr->err_titles [2]+1,
			 k+1,
			 k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
			 sxstrget (nt2ste [prdct_code]));
	      }
	      else {
		if (SXBA_bit_is_set (counter_param_set, param)) {
		  /* Ak doit etre un compteur (ainsi que les variables qui le composent) */
		  if (SXBA_bit_is_reset_set (counter_Ak_set, Ak))
		    PUSH (Ak_stack_check_counter_args, Ak);
		}
	      }

	      param_ptr = param_ptr->brother;
	    }
	  }

	  prdct_ptr = prdct_ptr->brother;
	}
      }
    }
  }

  while (!IS_EMPTY (Ak_stack_check_counter_args)) {
    Ak = POP (Ak_stack_check_counter_args);
    A = Ak2A (Ak);
    k = Ak_A2k(Ak, A);
    local_clause_set = prdct2clause_set [A];

    cur_clause = 0;

    while ((cur_clause = sxba_scan (local_clause_set, cur_clause)) > 0) {
      /* A occurre ds cur_clause en lhs */
      lhs_prdct = XxY_X (rcg_clauses, cur_clause) & lhs_prdct_filter;
      prdct_bot = XH_X (rcg_predicates, lhs_prdct);
      has_var = false;

      x = prdct_bot+2+k;
      param = XH_list_elem (rcg_predicates, x);
	
      param_bot = XH_X (rcg_parameters, param);
      param_top = XH_X (rcg_parameters, param+1);
      size = param_top-param_bot;

      if (size > 0) {
	while (param_bot < param_top) {
	  symb = XH_list_elem (rcg_parameters, param_bot);

	  if (symb > 0) {
	    SXBA_1_bit (variable_set, symb);
	    has_var = true;
	  }

	  param_bot++;
	}
      }

      if (has_var) {
	rhs = XxY_Y (rcg_clauses, cur_clause);

	if (rhs > 0) {
	  rhs_bot = XH_X (rcg_rhs, rhs);
	  rhs_top = XH_X (rcg_rhs, rhs+1);

	  for (x = rhs_bot; x < rhs_top; x++) {
	    prdct = XH_list_elem (rcg_rhs, x);
	    prdct_bot = XH_X (rcg_predicates, prdct);
	    prdct_code = XH_list_elem (rcg_predicates, prdct_bot+1);

	    if (prdct_code > 0) {
	      /* Pas predefini */
	      Bh = A_k2Ak (prdct_code, 0);
	  
	      for (prdct_top = XH_X (rcg_predicates, prdct+1), y = prdct_bot+2; y < prdct_top; y++, Bh++) {
		param = XH_list_elem (rcg_predicates, y);
	
		param_bot = XH_X (rcg_parameters, param);
		param_top = XH_X (rcg_parameters, param+1);
		size = param_top-param_bot;

		if (size > 0) {
		  while (param_bot < param_top) {
		    symb = XH_list_elem (rcg_parameters, param_bot);

		    if (symb > 0 && SXBA_bit_is_set (variable_set, symb)) {
		      if (SXBA_bit_is_reset_set (counter_Ak_set, Bh))
			PUSH (Ak_stack_check_counter_args, Bh);

		      break;
		    }

		    param_bot++;
		  }
		}
	      }
	    }
	  }
	}

	sxba_empty (variable_set);
      }
    }
  }

#if EBUG
  for (Ak = 1; Ak <= max_Ak; Ak++) {
    if (SXBA_bit_is_set (counter_Ak_set, Ak)) {
      A = Ak2A (Ak);
      k = Ak_A2k (Ak, A);
    
      fprintf (sxtty, "The %ld%s parameter of predicate <%s> denotes a counter.\n",
	    k+1,
	    k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
	    sxstrget (nt2ste [A]));
    }
  }
#endif /* EBUG */

  sxfree (has_t_param_set);
  sxfree (counter_param_set);
  sxfree (variable_set);
  sxfree (Ak_stack_check_counter_args);
  sxbm_free (prdct2clause_set);
  /* On "exporte" counter_Ak_set */
}

/* On regarde si la grammaire est lineaire (on ne tient pas compte des compteurs) */
static void
check_linearity (void)
{
  SXINT   cur_clause_check_linearity, lhs_prdct_check_linearity, prdct_bot, prdct_code, k, Ak, prdct_top, x, y, param, bot3, cur3, top3, symb, rhs, top, prdct, nb, left_Ak, max_symb;
  SXNODE  *prdct_ptr, *param_ptr;
  SXBA    lhs_var_set, rhs_var_set, *derive_Ak, nl_Ak_set, non_right_linear_Ak_set;
  SXINT   *var2Ak, *var2param_check_linearity, *Ak_stack_check_linearity, **symb2Ak_stack, *symb_Ak_stack;

  lhs_var_set = sxba_calloc (max_gvar+1);
  rhs_var_set = sxba_calloc (max_gvar+1);
  var2Ak = (SXINT *) sxalloc (max_gvar+1, sizeof (SXINT));
  var2param_check_linearity = (SXINT *) sxalloc (max_gvar+1, sizeof (SXINT));
  /* Soit A0() -> ... Ai() ... une clause. Ai_h est ds derive_Ak(A0_k) ssi le hieme arg de Ai et le kieme arg de A0 partage une variable X */
  derive_Ak = sxbm_calloc (max_Ak+1, max_Ak+1);
  Ak_stack_check_linearity = (SXINT *) sxcalloc (max_gvar*(max_garity+1)+1, sizeof (SXINT)), TOP(Ak_stack_check_linearity) = 0;
  symb2Ak_stack = (SXINT **) sxalloc (max_gvar+1, sizeof (SXINT*)), symb2Ak_stack [0] = NULL;
  nl_Ak_set = sxba_calloc (max_Ak+1);
  non_linear_Ak_set = sxba_calloc (max_Ak+1);

  clause2non_right_linear_Ak_set = sxbm_calloc (last_clause+1, max_Ak+1);
  non_right_linear_clause_set = sxba_calloc (last_clause+1);

  symb_Ak_stack = Ak_stack_check_linearity;

  for (symb = 1; symb <= max_gvar; symb++) {
    symb2Ak_stack [symb] = symb_Ak_stack;
    symb_Ak_stack += max_garity+1;
  }

  is_left_linear_grammar = true;
  is_right_linear_grammar = true;
  is_overlapping_grammar = false;

  XxY_foreach (rcg_clauses, cur_clause_check_linearity) {
    if (clause2identical [cur_clause_check_linearity] == cur_clause_check_linearity && !SXBA_bit_is_set (false_clause_set, cur_clause_check_linearity)) {
      sxba_empty (lhs_var_set);

      lhs_prdct_check_linearity = XxY_X (rcg_clauses, cur_clause_check_linearity) & lhs_prdct_filter;
      prdct_bot = XH_X (rcg_predicates, lhs_prdct_check_linearity);
      prdct_code = XH_list_elem (rcg_predicates, prdct_bot+1);

      max_symb = 0;
      k = 0;
      prdct_ptr = clause2visited [cur_clause_check_linearity]->father->son; /* lhs predicat */
      param_ptr = prdct_ptr->son->brother->son; /* 1er arg */
	  
      for (Ak = A_k2Ak (prdct_code, 0), prdct_top = XH_X (rcg_predicates, lhs_prdct_check_linearity+1), x = prdct_bot+2; x < prdct_top; x++, Ak++, k++) {
	if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
	  /* Pas un compteur */
	  param = XH_list_elem (rcg_predicates, x);

	  if (param > 0) {
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);
		    
	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      symb = XH_list_elem (rcg_parameters, cur3);

	      if (symb > 0) {
		if (!SXBA_bit_is_reset_set (lhs_var_set, symb)) {
		  /* Occur multiple de symb en LHS */
		  is_left_linear_grammar = false;

		  if (is_proper || is_simple)
		    sxerror (param_ptr->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sThe %ld%s parameter of the lhs predicate <%s> in clause #%ld makes this RCG non left linear.",
			     sxtab_ptr->err_titles [2]+1,
			     k+1,
			     k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
			     sxstrget (nt2ste [prdct_code]),
			     clause);
		}

		PUSH (symb2Ak_stack [symb], Ak);

		if (symb > max_symb)
		  max_symb = symb;
	      }
	    }
	  }
	}
	      
	param_ptr = param_ptr->brother;
      }

      rhs = XxY_Y (rcg_clauses, cur_clause_check_linearity);

      if (rhs > 0) {
	non_right_linear_Ak_set = clause2non_right_linear_Ak_set [cur_clause_check_linearity];
	sxba_empty (rhs_var_set);
	prdct_ptr = prdct_ptr->brother->son;
	
	for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	  prdct = XH_list_elem (rcg_rhs, x);
	  prdct_bot = XH_X (rcg_predicates, prdct);
	  prdct_code = XH_list_elem (rcg_predicates, prdct_bot+1);

	  if (prdct_code > 0) {
	    /* Pas predefini */
	    k = 0;
	    param_ptr = prdct_ptr->son->brother->son; /* 1er arg */
	  
	    for (Ak = A_k2Ak (prdct_code, 0), prdct_top = XH_X (rcg_predicates, prdct+1), y = prdct_bot+2; y < prdct_top; y++, Ak++, k++) {
	      if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
		/* Pas un compteur */
		param = XH_list_elem (rcg_predicates, y);

		if (param > 0) {
		  bot3 = XH_X (rcg_parameters, param);
		  top3 = XH_X (rcg_parameters, param+1);
		    
		  for (cur3 = bot3; cur3 < top3; cur3++) {
		    symb = XH_list_elem (rcg_parameters, cur3);

		    if (symb > 0) {
		      if (SXBA_bit_is_reset_set (rhs_var_set, symb)) {
			/* 1ere fois qu'on trouve symb en rhs ... */
			/* On note ds quel arg */
			var2Ak [symb] = Ak;
			var2param_check_linearity [symb] = param;
		      }
		      else {
			left_Ak = var2Ak [symb];
			/* Tant pis affectation multiple possible */
			SXBA_1_bit (non_right_linear_clause_set, cur_clause_check_linearity);
			SXBA_1_bit (non_right_linear_Ak_set, left_Ak);
			SXBA_1_bit (non_right_linear_Ak_set, Ak);

			SXBA_1_bit (nl_Ak_set, left_Ak);
			SXBA_1_bit (nl_Ak_set, Ak);

			is_right_linear_grammar = false;

			if (is_proper || is_simple)
			  sxerror (param_ptr->token.source_index,
				   sxtab_ptr->err_titles [2][0],
				   "%sThe %ld%s parameter of the rhs predicate <%s> in clause #%ld makes this RCG non right linear.",
				   sxtab_ptr->err_titles [2]+1,
				   k+1,
				   k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
				   sxstrget (nt2ste [prdct_code]),
				   clause);

			if (param != var2param_check_linearity [symb])
			  /* La meme variable figure dans 2 args differents => overlapping */
			  is_overlapping_grammar = true;
		      }
		    
		      symb_Ak_stack = symb2Ak_stack [symb];

		      nb = *symb_Ak_stack;

		      while (nb > 0) {
			left_Ak = symb_Ak_stack [nb];
			SXBA_1_bit (derive_Ak [left_Ak], Ak);
			SXBA_1_bit (nl_Ak_set, left_Ak);
			nb--;
		      }
		    }
		  }
		}
	      }

	      param_ptr = param_ptr->brother;
	    }
	  }

	  prdct_ptr = prdct_ptr->brother;
	}
      }

      for (symb = 1; symb <= max_symb; symb++) {
	symb2Ak_stack [symb] [0] = 0;
      }
    }
  }

  /* On ferme derive_Ak pour avoir les de'pendances */
  fermer2 (derive_Ak, max_Ak+1);
  /* Si dans une clause c Ak et Bh sont la cause d'une non-linearite, il faudra calculer les automates associes aux Cl derives des Ak et Bh
     pour pouvoir, sur la clause c, calculer les intersections */
  /* Pb, un module non principal peut avoir besoin de calculer des automates pour certains Ak detectes comme lineaires ds le module (la non-linearite
     provenant de modules appelant) on suppose qu'on a un moyen de le dire : soit a la compilation du module RCG ex arg -Non_Linear "A k B h"
     Soit en permettant a un arg de commencer par les 2 variables "N N" et signifiant "arg non lineaire ayant besoin d'un FSA associe'" 
     Soit dynamiquement a` partir de derive_Ak
  */
  Ak = 0;

  sxba_copy (non_linear_Ak_set, nl_Ak_set);

  while ((Ak = sxba_scan (nl_Ak_set, Ak)) > 0) {
    sxba_or (non_linear_Ak_set, derive_Ak [Ak]);
  }

  if (is_left_linear_grammar == false || is_right_linear_grammar == false)
    is_proper_grammar = is_simple_grammar = false;

  sxfree (lhs_var_set);
  sxfree (rhs_var_set);
  sxfree (var2Ak);
  sxfree (var2param_check_linearity);
  sxbm_free (derive_Ak);
  sxfree (Ak_stack_check_linearity);
  sxfree (symb2Ak_stack);
  sxfree (nl_Ak_set);

  /* On "exporte" non_linear_Ak_set */
}


#if 0
/* Supprime' le 15/04/08 si un arg de la rhs n'est pas une sous-chaine d'un arg de la lhs, ce doit etre un compteur */
static void
check_rhs_args_as_substrings (void)
{
  SXINT   cur_clause, rhs, lhs_prdct, top, x, y, prdct, prdct_bot, prdct_code, Ak, k, prdct_top, param;
  SXNODE  *prdct_ptr, *param_ptr;

  XxY_foreach (rcg_clauses, cur_clause) {
    if (clause2identical [cur_clause] == cur_clause && !SXBA_bit_is_set (false_clause_set, cur_clause)) {
      rhs = XxY_Y (rcg_clauses, cur_clause);

      if (rhs > 0) {
	lhs_prdct = XxY_X (rcg_clauses, cur_clause) & lhs_prdct_filter;
	prdct_ptr = clause2visited [cur_clause]->son; /* 1er predicat */

	for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	  prdct = XH_list_elem (rcg_rhs, x);
	  prdct_bot = XH_X (rcg_predicates, prdct);
	  prdct_code = XH_list_elem (rcg_predicates, prdct_bot+1);

	  if (prdct_code > 0) {
	    /* Pas predefini */
	    Ak = A_k2Ak (prdct_code, 0);
	    param_ptr = prdct_ptr->son->brother->son; /* 1er arg */
	  
	    for (prdct_top = XH_X (rcg_predicates, prdct+1), y = prdct_bot+2; y < prdct_top; y++, Ak++) {
	      if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
		/* Cet arg n'est pas un compteur */
		param = XH_list_elem (rcg_predicates, y);
		    
		if (!rhs_arg2sub_arg (lhs_prdct, param)) {
		  sxerror (param_ptr->token.source_index,
			   sxtab_ptr->err_titles [2][0],
			   "%sMust be a sub-string of some LHS argument.",
			   sxtab_ptr->err_titles [2]+1);
		}
	      }

	      param_ptr = param_ptr->brother;
	    }
	  }
	  else {
	    if (prdct_code == STREQ_ic || prdct_code == STREQLEN_ic) {
	      param_ptr = prdct_ptr->son->brother->son; /* 1er arg */
	  
	      for (prdct_top = XH_X (rcg_predicates, prdct+1), y = prdct_bot+2; y < prdct_top; y++) {
		/* Cet arg n'est pas un compteur */
		param = XH_list_elem (rcg_predicates, y);
		    
		if (!rhs_arg2sub_arg (lhs_prdct, param)) {
		  sxerror (param_ptr->token.source_index,
			   sxtab_ptr->err_titles [2][0],
			   "%sMust be a sub-string of some LHS argument.",
			   sxtab_ptr->err_titles [2]+1);
		}

		param_ptr = param_ptr->brother;
	      }
	    }
	  }

	  prdct_ptr = prdct_ptr->brother;
	}
      }
    }
  }
}
#endif /* 0 */


static void
is_instantiable (void)
{
    /* On teste si chaque clause est instantiable */
    /* Seuls les parametres dont la longueur est superieure a 1 et qui contiennent
       des variables utilisees dans les parametres precedents non unitaires sont consideres */
    /* A FAIRE :
       Il faudrait verifier que par exemple une RHS qui contient
          &StrEq (X, Y) &StrLen (1, X) &StrLen (2, Y)
       n'est pas instantiable */

    SXINT is_istantiable_cur_clause, is_istantiable_lhs_prdct, rhs, top, x, var, i;

    XxY_foreach (rcg_clauses, is_istantiable_cur_clause) {
	if (clause2identical [is_istantiable_cur_clause] == is_istantiable_cur_clause && !SXBA_bit_is_set (false_clause_set, is_istantiable_cur_clause)) {
	    is_istantiable_lhs_prdct = XxY_X (rcg_clauses, is_istantiable_cur_clause) & lhs_prdct_filter;
	    extract_params (is_istantiable_lhs_prdct);
	    rhs = XxY_Y (rcg_clauses, is_istantiable_cur_clause);

	    for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
		extract_params (XH_list_elem (rcg_rhs, x));
	    }

	    if (must_be_checked && !is_instantiable_clause ()) {
	      if (check_instantiable_clause)
		sxerror (clause2visited [is_istantiable_cur_clause]->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sNon instantiable clause.",
			 sxtab_ptr->err_titles [2]+1);

	      /* XxY_erase (rcg_clauses, is_istantiable_cur_clause); */
	      if (SXBA_bit_is_reset_set (false_clause_set, is_istantiable_cur_clause))
		PUSH (new_false_clause_stack, is_istantiable_cur_clause); /* Le 09/04/04 */
	    }

	    sxba_empty (param_set);
	    sxba_empty (var_set);
	    sxba_empty (multiple_var_set);

	    for (var = max_gvar; var > 0; var--)
		multiple_var2eq [var] = 0;
	
	    state_stack [0] = 0;

	    for (i=1; i <= max_i; i++)
		sxba_empty (equal [i]), sxba_empty (plus1 [i]), sxba_empty (moins1 [i]),
		sxba_empty (less [i]), sxba_empty (less_eq [i]);

	    must_be_checked = false;
	}
    }
}



static void
Xref (void)
{
  SXINT		Xref_clause, nt, nt_nb, rhs, top, x, y, prdct;
  SXBA	undefined_nt_set;
  VARSTR vstr;
  /* On commence par regarder s'il y a des predicats internes (declares) et non definis */
  undefined_nt_set = sxba_calloc (max_nt+1);

  sxba_copy (undefined_nt_set, is_lhs_nt);
  sxba_not (undefined_nt_set);
  sxba_and (undefined_nt_set, is_rhs_nt);
  sxba_and (undefined_nt_set, is_nt_internal);

  if ((nt = sxba_scan (undefined_nt_set, 0)) > 0) {
    vstr = varstr_alloc (64);
    nt_nb = 1;

    vstr = varstr_catenate (vstr, sxstrget (nt2ste [nt]));

    while ((nt = sxba_scan (undefined_nt_set, nt)) > 0) {
      nt_nb++;
      vstr = varstr_catenate (vstr, ", ");
      vstr = varstr_catenate (vstr, sxstrget (nt2ste [nt]));
    }

    fputc (SXNEWLINE, sxstderr);
    sxtmsg (sxsrcmngr.source_coord.file_name,
	    "%sThe following predicate%s undefined:\n{%s}\n",
	    sxtab_ptr->err_titles [2]+1,
	    nt_nb > 1 ? "s are" : " is",
	    varstr_tostr (vstr));

    varstr_free (vstr);
  }

  sxfree (undefined_nt_set);

  /* On extrait le squelette CF et les productions vides */
  /* On verifie que
     - tout predicat interne (autre que l'axiome) defini en LHS est utilise en RHS
     - tout predicat interne utilise en RHS est defini en LHS */

  XxY_foreach (rcg_clauses, Xref_clause) {
    if (clause2identical [Xref_clause] == Xref_clause && !SXBA_bit_is_set (false_clause_set, Xref_clause)) { /* Le 13/02/04 */
      /* Les 2 lignes suivantes ajoutees le 10/02/04 */
      prdct = XxY_X (rcg_clauses, Xref_clause) & lhs_prdct_filter;
      nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, prdct)+1);

      if (nt > 1 && SXBA_bit_is_set (is_nt_internal, nt) && !SXBA_bit_is_set (is_rhs_nt, nt)) {
	sxerror (clause2visited [Xref_clause]->token.source_index,
		 sxtab_ptr->err_titles [2][0],
		 "%sThe LHS predicate %s is unused.",
		 sxtab_ptr->err_titles [2]+1,
		 sxstrget (nt2ste [nt]));
      }

      rhs = XxY_Y (rcg_clauses, Xref_clause);
      nt_nb = 0;

      if (rhs > 0) {
	for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	  prdct = XH_list_elem (rcg_rhs, x);
	  y = XH_X (rcg_predicates, prdct);
	  nt = XH_list_elem (rcg_predicates, y+1);

	  if (nt > 0) {
	    if (SXBA_bit_is_set (is_nt_internal, nt) || SXBA_bit_is_set (is_lhs_nt, nt)) {
	      /* Ce n'est ni un predicat predefini ni un predicat externe defini ds une autre grammaire */
	      if (SXBA_bit_is_set (is_nt_internal, nt) && !SXBA_bit_is_set (is_lhs_nt, nt)) {
		/* predicat interne dont il manque la definition */
		sxerror (clause2visited [Xref_clause]->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sThe RHS predicate %s is undefined.",
			 sxtab_ptr->err_titles [2]+1,
			 sxstrget (nt2ste [nt]));
	      } 

	      if (XH_list_elem (rcg_predicates, y) == 1) {
		is_negative = true;
		SXBA_1_bit (negative_nt_set, nt);
	      }

	      if (SXBA_bit_is_reset_set (cf_skeleton [Xref_clause], nt)) {
		nt_nb++;
		SXBA_1_bit (rhsnt2clause [nt], Xref_clause);
	
#if 0	
		/* Le 19/12/03 */
		if (!SXBA_bit_is_reset_set (rhsnt2clause [nt], Xref_clause))
		  SXBA_1_bit (multiple_rhsnt2clause [nt], Xref_clause);
#endif /* 0 */
	      }
	    }

#if 0
	    if (is_robust && XH_list_elem (rcg_predicates, y) == 1) {
	      /* Dans le cas -robust, on ne traite que les PRCG... */
	      sxerror (clause2visited [Xref_clause]->token.source_index,
		       sxtab_ptr->err_titles [2][0],
		       "%sNegative clause and robust grammar are incompatible.",
		       sxtab_ptr->err_titles [2]+1);
	    }
#endif
	  }
	}
      }

      clause2rhsnb [Xref_clause] = nt_nb;
    }
  }
}


static void
has_loops (void)
{
  /* Mer 16 Mai 2001 14:19:56 */
  /* Detecte les productions de la forme A(\vec{\alpha}) --> A(\vec{\alpha}) &Lex(...) . */
  SXINT has_loops_clause, rhs, has_loops_lhs_prdct, rhs_prdct, x, top, nt;
  SXINT	cur2, bot2, top2, param,cur3, bot3, top3;
  bool	has_lhs_prdct;

  XxY_foreach (rcg_clauses, has_loops_clause) {
    if (clause2identical [has_loops_clause] == has_loops_clause && !SXBA_bit_is_set (false_clause_set, has_loops_clause)) { /* Le 10/02/04 */
      rhs = XxY_Y (rcg_clauses, has_loops_clause);

      if (rhs > 0) {
	has_loops_lhs_prdct = XxY_X (rcg_clauses, has_loops_clause) & lhs_prdct_filter;
	has_lhs_prdct = false;

	for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	  rhs_prdct = XH_list_elem (rcg_rhs, x);
	  nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, rhs_prdct)+1);

	  if (rhs_prdct == has_loops_lhs_prdct)
	    has_lhs_prdct = true;
	  else
	    if (nt != LEX_ic)
	      break;
	}

	if (has_lhs_prdct && x == top) {
	  /* De +, il faut tester que \vec{\alpha} ne contient pas de terminaux */
	  bot2 = XH_X (rcg_predicates, has_loops_lhs_prdct);
	  top2 = XH_X (rcg_predicates, has_loops_lhs_prdct+1);

	  for (cur2 = bot2+2; cur2 < top2; cur2++) {
	    param = XH_list_elem (rcg_predicates, cur2);

	    if (param != 0) {
	      bot3 = XH_X (rcg_parameters, param);
	      top3 = XH_X (rcg_parameters, param+1);

	      for (cur3 = bot3; cur3 < top3; cur3++) {
		if (XH_list_elem (rcg_parameters, cur3) < 0)
		  break;
	      }

	      if (cur3 < top3)
		break;
	    }
	  }
		
	  if (cur2 == top2) {
	    SXBA_1_bit (loop_clause_set, has_loops_clause);
	    nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, has_loops_lhs_prdct)+1);
	    SXBA_1_bit (loop_nt_set, nt);
	    is_loop_grammar = true;
	  }
	}
      }
    }
  }
}



static void
is_productive (void)
{
  /* On teste si chaque is_productive_clause est productive (condition necessaire) */
  SXINT		is_productive_clause, is_productive_lhs_prdct, nt, nt_nb, rhs, rhs_prdct, x, top;
  SXBA	        ntline;

  XxY_foreach (rcg_clauses, is_productive_clause) {
    if (clause2identical [is_productive_clause] == is_productive_clause && !SXBA_bit_is_set (false_clause_set, is_productive_clause)) { /* Le 10/02/04 */
      nt_nb = clause2rhsnb [is_productive_clause];

      if (nt_nb) {
	/* Partie droite non vide */
	/* Ajoute le 10/02/04 et le 17/05/04 */
	/* On ne tient compte que des appels positifs des predicats utilisateurs definis ds ce module */
	rhs = XxY_Y (rcg_clauses, is_productive_clause);
	
	for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	  rhs_prdct = XH_list_elem (rcg_rhs, x);

	  if (XH_list_elem (rcg_predicates, XH_X (rcg_predicates, rhs_prdct)) == 0 /* Appel positif */
	      && (nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, rhs_prdct)+1)) > 0 /* prdct utilisateur */
	      && SXBA_bit_is_set (is_lhs_nt, nt) /* defini ds ce module */) {
	    /* Appel d'un predicat externe non defini ds ce module */
	    break;
	  }
	}

	if (x == top)
	  nt_nb = 0;
      }

      if (nt_nb == 0) {
	/* Partie droite vide (generalisee) */
	SXBA_1_bit (empty_clause_set, is_productive_clause);
	SXBA_1_bit (productive_clause_set, is_productive_clause);
	is_productive_lhs_prdct = XxY_X (rcg_clauses, is_productive_clause) & lhs_prdct_filter;
	nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, is_productive_lhs_prdct)+1);

	if (SXBA_bit_is_reset_set (nt_set, nt)) {
	  PUSH (nt_stack, nt);
	}
      }
    }
  }

  /* Les nt A ayant une A_clause vide sont instantiables ...
     ... ainsi que les nt externes non definis ds ce module */
  if ((nt = sxba_scan (is_nt_external, 1)) > 0 && sxba_0_lrscan (is_lhs_nt, 1) > 0) {
    do {
      if (!SXBA_bit_is_set (is_lhs_nt, nt)) {
	/* ... ainsi que les predicats externes definis ds ce module */
	if (SXBA_bit_is_reset_set (nt_set, nt)) {
	  PUSH (nt_stack, nt);
	}
      }
    } while ((nt = sxba_scan (is_nt_external, nt)) > 0);
  }


  while (!IS_EMPTY (nt_stack)) {
    nt = POP (nt_stack);
    ntline = rhsnt2clause [nt];
    is_productive_clause = 0;

    while ((is_productive_clause = sxba_scan (ntline, is_productive_clause)) > 0) {
      rhs = XxY_Y (rcg_clauses, is_productive_clause);
	
      for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
	rhs_prdct = XH_list_elem (rcg_rhs, x);

	if (XH_list_elem (rcg_predicates, XH_X (rcg_predicates, rhs_prdct)) == 0 /* Appel positif */
	    && (nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, rhs_prdct)+1)) > 0 /* prdct utilisateur */) {
	  if (!SXBA_bit_is_set (nt_set, nt))
	    break;
	}
      }

      if (x == top) {
	if (SXBA_bit_is_reset_set (productive_clause_set, is_productive_clause)) {
	  /* is_productive_clause peut etre trouve plusieurs fois a cause des nt de nt_stack non encore examines */
	  is_productive_lhs_prdct = XxY_X (rcg_clauses, is_productive_clause) & lhs_prdct_filter;
	  nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, is_productive_lhs_prdct)+1);

	  if (SXBA_bit_is_reset_set (nt_set, nt)) {
	    PUSH (nt_stack, nt);
	  }
	}
      }
    }
  }

#if 0
  if ((nt = sxba_0_lrscan (nt_set, 0)) > 0) {
    SXINT nt_nb = 1;
    VARSTR vstr;

    vstr = varstr_alloc (128);

    do {
      nt_nb++;
      vstr = varstr_catenate (vstr, sxstrget (nt2ste [nt]));
      nt = sxba_0_lrscan (nt_set, nt); 

      if (nt > 0)
	varstr_catenate (vstr, ", ");
    }
    while (nt > 0);

    sxtmsg (sxsrcmngr.source_coord.file_name,
	    "%sThe following predicate%s non productive:\n{%s}\n",
	    sxtab_ptr->err_titles [2]+1,
	    nt_nb > 1 ? "s are" : " is",
	    varstr_tostr (vstr));

    varstr_free (vstr);
  }
#endif /* 0 */

  /* Le 27/09/04 remplace' par la sortie des predicats non productifs ... */
  is_productive_clause = 0;

  while ((is_productive_clause = sxba_0_lrscan (productive_clause_set, is_productive_clause)) > 0) {
    if (clause2identical [is_productive_clause] == is_productive_clause && SXBA_bit_is_reset_set (false_clause_set, is_productive_clause)) {
      PUSH (new_false_clause_stack, is_productive_clause);

      if (check_instantiable_clause)
	sxerror (clause2visited [is_productive_clause]->token.source_index,
		 sxtab_ptr->err_titles [2][0],
		 "%sNon productive clause.",
		 sxtab_ptr->err_titles [2]+1);
    }
  }

  sxba_empty (nt_set);
  nt_stack [0] = 0;
  sxba_empty (productive_clause_set);
}


static void
is_accessible (void)
{
    SXINT		nt, is_accessible_clause;
    SXBA	ntline, skeleton_line, accessible_clause_set;

    accessible_clause_set = sxba_calloc (last_clause+1);

    PUSH (nt_stack, 1); /* l'axiome est accessible ... */
    SXBA_1_bit (nt_set, 1);

    if ((nt = sxba_scan (is_nt_external, 1)) > 0 && sxba_scan (is_lhs_nt, 1)) {
      do {
	if (SXBA_bit_is_set (is_lhs_nt, nt)) {
	  /* ... ainsi que les predicats externes definis ds ce module */
	  PUSH (nt_stack, nt);
	  SXBA_1_bit (nt_set, nt);
	}
      } while ((nt = sxba_scan (is_nt_external, nt)) > 0);
    }

    while (!IS_EMPTY (nt_stack)) {
	nt = POP (nt_stack);
	ntline = lhsnt2clause [nt];
	is_accessible_clause = 0;

	while ((is_accessible_clause = sxba_scan (ntline, is_accessible_clause)) > 0) {
	    SXBA_1_bit (accessible_clause_set, is_accessible_clause);
	    skeleton_line = cf_skeleton [is_accessible_clause];
	    nt = 0;

	    while ((nt = sxba_scan (skeleton_line, nt)) > 0) {
		if (SXBA_bit_is_reset_set (nt_set, nt))
		    PUSH (nt_stack, nt);
	    }
	}
    }

    is_accessible_clause = 0;

    while ((is_accessible_clause = sxba_0_lrscan (accessible_clause_set, is_accessible_clause)) > 0) {
      if (SXBA_bit_is_reset_set (false_clause_set, is_accessible_clause)) {
	PUSH (new_false_clause_stack, is_accessible_clause);

	if (check_instantiable_clause)
	  sxerror (clause2visited [is_accessible_clause]->token.source_index,
		   sxtab_ptr->err_titles [2][0],
		   "%sNon accessible clause.",
		   sxtab_ptr->err_titles [2]+1);
      }
    }

    sxba_empty (nt_set);
    sxfree (accessible_clause_set);
}

static void
negative_language (void)
{
    /* On a des vrais predicats negatifs en RHS.  On verifie que le langage de A ne depend pas
       de !A */
    SXINT init_nt, nt, negative_language_clause, rhs_nt, rhs, pos, x, y, top, prdct;
    SXBA	line, skeleton_line;
    SXNODE	*node_ptr;

    sxinitialise(pos) /* Pour un compilateur croisé pour l'architecture itanium64 qui est gcc version 3.4.5
			 et qui dit "warning: 'pos' might be used uninitialized in this function" */;

    init_nt = 0;

    while ((init_nt = sxba_scan (negative_nt_set, init_nt)) > 0) {
	PUSH (nt_stack, init_nt);
	SXBA_1_bit (nt_set, init_nt);

	while (!IS_EMPTY (nt_stack)) {
	    nt = POP (nt_stack);
	    line = lhsnt2clause [nt];
	    negative_language_clause = 0;

	    while ((negative_language_clause = sxba_scan (line, negative_language_clause)) > 0) {
		skeleton_line = cf_skeleton [negative_language_clause];
		rhs_nt = 0;

		while ((rhs_nt = sxba_scan (skeleton_line, rhs_nt)) > 0) {
		    if (rhs_nt != init_nt) {
			if (SXBA_bit_is_reset_set (nt_set, rhs_nt))
			    PUSH (nt_stack, rhs_nt);
		    }
		    else {
			/* On regarde s'il existe une occurrence negative */
			rhs = XxY_Y (rcg_clauses, negative_language_clause);
			pos = 0;

			for (top = XH_X (rcg_rhs, rhs+1), x = XH_X (rcg_rhs, rhs); x < top; x++) {
			    pos++;
			    prdct = XH_list_elem (rcg_rhs, x);
			    y = XH_X (rcg_predicates, prdct);

			    if (XH_list_elem (rcg_predicates, y) == 1 &&
				XH_list_elem (rcg_predicates, y+1) == init_nt)
				break;
			}

			if (x < top)
			    break;
		    }
		}

		if (rhs_nt > 0)
		    break;
	    }

	    if (negative_language_clause > 0) {
		node_ptr = clause2visited [negative_language_clause]->son;

		while (--pos > 0)
		    node_ptr = node_ptr->brother;

		sxerror (node_ptr->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sSelf defined negative predicate.",
			 sxtab_ptr->err_titles [2]+1);

		nt_stack [0] = 0;
	    }
	}

	sxba_empty (nt_set);
    }
}


#if 0
/* Le 19/12/03 voir commentaire ci-dessous */
static SXINT
clause2rhsnt_nb (SXINT clause, SXINT nt)
{
  SXINT rhs, top, cur, nb = 0;

  if (rhs = XxY_Y (rcg_clauses, clause)) {
    top = XH_X (rcg_rhs, rhs+1);
    cur = XH_X (rcg_rhs, rhs);

    do {
      if (nt == XH_list_elem (rcg_predicates, XH_X (rcg_predicates, XH_list_elem (rcg_rhs, cur))+1))
	nb++;
    } while (++cur < top);
  }
    
  return nb;
}
#endif /* 0 */


static bool
OR (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXINT	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
    SXBA_ELT	val1, val2;
    bool		has_changed = false;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	val1 = *lhs_bits_ptr;
	val2 = val1 | *rhs_bits_ptr--;

	if (val1 != val2)
	{
	    has_changed = true;
	    *lhs_bits_ptr = val2;
	}

	lhs_bits_ptr--;
    }

    return has_changed;
}



#if 0
/* Ancienne version de empty_construction () */
static void
set_empty (SXINT nt, SXBA bvect)
{
  /* On associe le couple (nt, bvect) */
  SXINT		clause, rhsnb, pos;
  SXBA	        line;
  bool	is_first_occur, has_changed;
  SXBA          empty_vector;

  if (SXBA_bit_is_reset_set (nt2empty_vector_is_set, nt)) {
    has_changed = true;
    sxba_copy (nt2empty_vector [nt], bvect);
  }
  else {
    empty_vector = nt2empty_vector [nt];
    /* 0 => peut deriver ds le vide l'emporte */
    has_changed = false;
    pos = -1;

    while ((pos = sxba_scan (empty_vector, pos)) >= 0) {
      if (!SXBA_bit_is_set (bvect, pos)) {
	has_changed = true;
	SXBA_0_bit (empty_vector, pos);
      }
    }

#if 0
    /* Le 05/01/04 */
    sxba_and (empty_vector, bvect);
    has_changed = (sxba_first_difference (empty_vector, bvect) != -1);
#endif /* 0 */
  }

  if (has_changed) {
    line = rhsnt2clause [nt];
    is_first_occur = SXBA_bit_is_reset_set (nt_set, nt);
    clause = 0;

    while ((clause = sxba_scan (line, clause)) > 0) {
      if (clause2identical [clause] == clause && !SXBA_bit_is_set (false_clause_set, clause)) {
	/* On ne reexamine pas les clauses deja traitees dont le vecteur caracteristique
	   ne depend pas des valeurs des variables de la RHS */
	if (!SXBA_bit_is_set (completed_clause_set, clause)) {
	  if (is_first_occur) {
	    /* Il faut diminuer du nb d'occur de nt en rhs de clause */
	    rhsnb = --clause2rhsnb [clause];
#if 0
	    /* Le 19/12/03 plusieurs occur du meme predicat en RHS sont comptes pour 1 ds clause2rhsnb [clause]
	       Il semble donc que multiple_rhsnt2clause et clause2rhsnt_nb soient inutiles!! */
	    if (SXBA_bit_is_set (multiple_rhsnt2clause [nt], clause)) {
	      /* On calcule le nb d'occur de nt en rhs de clause */
	      rhsnb = clause2rhsnb [clause] -= clause2rhsnt_nb (clause, nt);
	    }
	    else
	      rhsnb = --clause2rhsnb [clause];
#endif /* 0 */
	  }
	  else
	    rhsnb = clause2rhsnb [clause];
		
	  if (rhsnb == 0 && SXBA_bit_is_reset_set (clause_set, clause)) {
	    /* Il faut (re)calculer une substitution des variables de clause */
	    PUSH (clause_stack, clause);
	  }
	}
      }
    }
  }
}

static void
substitute (SXINT clause)
{
  /* La substitution des variables de la partie droite est contenue dans V_set et NV_set.
     NV_set contient l'ensemble des variables de la RHS qui ne derivent pas dans epsilon.
     V_set contient l'ensemble des variables de la RHS qui derivent dans epsilon.
     Les variables qui ne sont ni dans V_set ni dans NV_set sont non contraintes.
  */
  SXINT		lhs_prdct, bot, top, nt, pos, param, bot2, top2, symb, uparnb;
  SXINT		x, y;
  bool	is_undef;

  lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
  bot = XH_X (rcg_predicates, lhs_prdct);
  nt = XH_list_elem (rcg_predicates, bot+1);

  /* bvect = 0; */
  sxba_empty (wbvect);
  uparnb = 0;

  for (pos = 0, top = XH_X (rcg_predicates, lhs_prdct+1), x = bot+2; x < top; pos++, x++) {
    param = XH_list_elem (rcg_predicates, x);

    if (param != 0) {
      bot2 = XH_X (rcg_parameters, param);
      top2 = XH_X (rcg_parameters, param+1);
      is_undef = false;

      for (y = bot2; y < top2; y++) {
	symb = XH_list_elem (rcg_parameters, y);

	if (symb < 0 || SXBA_bit_is_set (NV_set, symb)) {
	  /* terminal ou variable ne derivant pas dans le vide */
	  /* L'element caracteristique vaut 1 */
	  /* bvect += 1 << pos; */
	  SXBA_1_bit (wbvect, pos);
	  break;
	}
	else {
	  if (!SXBA_bit_is_set (V_set, symb))
	    is_undef = true;
	}
      }

      if (y >= top2 && is_undef) {
	/* param contient des variables non contraintes et eventuellement des variables vides */
	/* Elles sont non contraintes car
	   - Soit elles n'apparaissent pas en RHS,
	   - soit elles apparaissent en RHS et n'appartiennent ni a V_set ni a NV_set */
	/* ESSAI le 4/2/2002
	   On suppose que les variables non contraintes peuvent deriver dans le vide...
	   SXBA_1_bit (param_set, pos);
	   uparnb++;
	*/
      }
    }

    /* Si vide ou que des variables : bvect [pos] == 0 */
  }

  SXBA_1_bit (productive_eclause_set, clause);

  if (uparnb == 0) {
    set_empty (nt, wbvect);
    /* set_empty (nt, bvect); */
  }
#if 0
  else {
    /* Inutile le 4/2/2002!! */
    max = (1<<uparnb) - 1;
    bvect_init = bvect;

    for (v = max; v >= 0; v--) {
      bvect = bvect_init;
	  
      n = v;
      pos = -1;

      while (n > 0 && (pos = sxba_scan (param_set, pos)) >= 0) {
	if (n & 1) {
	  bvect |= 1 << pos;
	}

	n >>= 1;
      }

      set_empty (nt, bvect);
    }

    sxba_empty (param_set);
  }
#endif
}


static bool
is_lhs_complete (SXINT clause)
{
    /* On regarde si le vecteur caracteristique de clause peut se calculer sans regarder sa RHS */
    SXINT		lhs_prdct, bot, top, nt, pos, param, bot2, top2, symb;
    SXINT		x, y;
    bool	is_complete;

    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    bot = XH_X (rcg_predicates, lhs_prdct);
    nt = XH_list_elem (rcg_predicates, bot+1);

    /* bvect = 0; */
    sxba_empty (wbvect);
    is_complete = true;

    for (pos = 0, top = XH_X (rcg_predicates, lhs_prdct+1), x = bot+2; x < top; pos++, x++) {
	param = XH_list_elem (rcg_predicates, x);

	if (param != 0) {
	    bot2 = XH_X (rcg_parameters, param);
	    top2 = XH_X (rcg_parameters, param+1);

	    for (y = bot2; y < top2; y++) {
		symb = XH_list_elem (rcg_parameters, y);

		if (symb < 0) {
		    /* terminal L'element caracteristique vaut 1 */
		  /* bvect += 1 << (pos); */
		  SXBA_1_bit (wbvect, pos);
		    break;
		}
	    }

	    if (y >= top2) {
		/* param ne contient que des variables */
		/* On va examiner la RHS */
		is_complete = false;

		for (y = bot2; y < top2; y++) {
		    symb = XH_list_elem (rcg_parameters, y);
		    SXBA_1_bit (lhs_var_set, symb);
		}
	    }
	}

	/* Si vide  : bvect [pos] == 0 */
    }

    if (is_complete) {
      /* set_empty (nt, bvect); */
	set_empty (nt, wbvect);
	/* Le vecteur caracteristique de la LHS de cette clause sera toujours bvect, il est donc
	   inutile de la reexaminer + tard */
	SXBA_1_bit (completed_clause_set, clause);
	SXBA_1_bit (productive_eclause_set, clause);
    }

    return is_complete;
}

static void walk_rhs_prdct_bvect (SXINT clause, 
				  SXINT cur, 
				  SXINT top, 
				  SXINT bot2, 
				  SXINT top2, 
				  SXBA bvect, 
				  SXINT predef_bvect);

static void
walk_rhs_prdct (SXINT clause, SXINT cur, SXINT top)
{
    /* Parcourt de la rhs de clause.  Le predicat courant est en cur */
    SXINT		U_set_nb, Vtop, NVtop0, NVtop, xparam, param, bot3, cur3, top3, Vnb, NVnb, Unb, var;
    SXINT		prdct, bot2, top2, nt, arity;
    SXINT		n, max, v, i, l;
    bool	is_set;
    SXBA	U_line;

    if (cur == top) {
	/* La partie droite a ete parcourue en entier */
	if (!IS_EMPTY (param_stack)) {
	    /* Combinatorial clause */
	    /* Les parametres de param_stack ne contiennent que des variables
	       dont l'une au moins ne doit pas etre vide */
	    U_set_nb = 0;
	    NVtop0 = NV_stack [0];

	    for (xparam = 1; xparam <= param_stack [0]; xparam++) {
		param = param_stack [xparam];
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		Vnb = NVnb = Unb = 0;

		for (cur3 = bot3; cur3 < top3; cur3++) {
		    var = XH_list_elem (rcg_parameters, cur3);

		    if (SXBA_bit_is_set (V_set, var))
			Vnb++;
		    else {
			if (SXBA_bit_is_set (NV_set, var))
			    NVnb++;
			else {
			    PUSH (U_stack, var);
			    Unb++;
			}
		    }
		}

		if (NVnb + Unb == 0) {
		    /* Toutes les var sont vides => Echec */
		    return;
		}

		if (NVnb == 0) {
		    if (Unb == 1) {
			/* Elle passe de indefini a non vide */
			var = POP (U_stack);

			if (SXBA_bit_is_reset_set (NV_set, var))
			  PUSH (NV_stack, var);
		    }
		    else {
			/* Ici, plusieurs candidates : pb */
			/* Seules les variables sensibles de la partie gauche sont prises en compte */
			U_set_nb++;
			U_line = U_set [U_set_nb];
			is_set = false;
			
			while(!IS_EMPTY (U_stack)) {
			    var = POP (U_stack);

			    if (SXBA_bit_is_set (lhs_var_set, var)) {
				is_set = true;
				SXBA_1_bit (GU_set, var);
				SXBA_1_bit (U_line, var);
			    }
			}

			if (!is_set)
			    U_set_nb--;
		    }
		}
		else
		    /* parametre non vide */
		    U_stack [0] = 0;
	    }

	    if (U_set_nb > 0) {
		/* Ici, il faut trouver toutes les substitutions de variables qui assurent que
		   les ensembles de U_set sont non vides */
		n = sxba_cardinal (GU_set);
		/* Je suppose que le cas est rare et je vais etre bestial => 2^n essais */
		max = (1<<n) - 1;

		for (v = max; v >= 1; v--) {
		    n = v;
		    Vtop = V_stack [0];
		    NVtop = NV_stack [0];
		    var = 0;

		    while ((var = sxba_scan (GU_set, var)) > 0) {
			if (n & 1) {
			  if (SXBA_bit_is_reset_set (NV_set, var))
			    PUSH (NV_stack, var);
			}
			else {
			  if (SXBA_bit_is_reset_set (V_set, var))
			    PUSH (V_stack, var);
			}

			n >>= 1;
		    }

		    for (i = 1; i <= U_set_nb; i++) {
			if (!IS_AND (U_set [i], NV_set))
			    /* Echec */
			    break;
		    }

		    if (i > U_set_nb) {
			/* V_set, NV_set sont positionnes */
			/* Les variables de rhs_var_set qui ne sont ni dans V_set ni dans NV_set
			   sont non contraintes */
			substitute (clause);
		    }

		    while (NV_stack [0] > NVtop) {
			var = POP (NV_stack);
			SXBA_0_bit (NV_set, var);
		    }

		    while (V_stack [0] > Vtop) {
			var = POP (V_stack);
			SXBA_0_bit (V_set, var);
		    }
		}

		for (i = 1; i <= U_set_nb; i++)
		    sxba_empty (U_set [i]);

		sxba_empty (GU_set);
	    }
	    else
		substitute (clause);

	    while (NV_stack [0] > NVtop0) {
		var = POP (NV_stack);
		SXBA_0_bit (NV_set, var);
	    }
	}
	else
	    substitute (clause);

	return;
    }
    else {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	top2 = XH_X (rcg_predicates, prdct+1);
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt > 0) {
	    arity = nt2arity [nt];

	    if (arity == 0 ||
		XH_list_elem (rcg_predicates, bot2) == 1 ||
		(SXBA_bit_is_set (is_nt_external, nt) && !SXBA_bit_is_set (is_lhs_nt, nt)))
		/* arg_nb == 0 ou */
		/* Appel negatif */
		/* Appel d'un vrai predicat externe */
		/* Ses variables sont qcq */
		walk_rhs_prdct (clause, cur+1, top);
	    else {
#if 0
		XxY_Xforeach (empty, nt, elem) {
		    walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, XxY_Y (empty, elem));
		}
#endif
		walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, nt2empty_vector [nt], 0);
	    }
	}
	else {
	    /* Predicat predefini */
	    if (nt == FALSE_ic || nt == FIRST_ic || nt == LAST_ic || nt == CUT_ic || nt == TRUE_ic || nt == LEX_ic) {
		/* Dans tous les cas, on saute */
		walk_rhs_prdct (clause, cur+1, top);
	    }
	    else {
		if (XH_list_elem (rcg_predicates, bot2) == 0) {
		    /* Appel positif */
		    switch (nt) {
		    case STREQ_ic:
			cur_param = XH_list_elem (rcg_predicates, bot2+2);

			if (cur_param == 0) {
			    walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 0);
			    break;
			}

			if (is_in_Tstar (XH_X (rcg_parameters, cur_param), XH_X (rcg_parameters, cur_param+1)) == 1) {
			    walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 1);
			    break;
			}

		    case STREQLEN_ic:
			/* 2 param pouvant etre simultanement vides => bvect = 0 ou non vides => bvect = 3 */
			/* walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, NULL, 0);
			   walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, NULL, 3);
			   On ne sait rien */
		        walk_rhs_prdct (clause, cur+1, top);
			break;

		    case STRLEN_ic:
			l = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));

			if (l == 0)
			    /* &StrLen (0, ...) */
			    /* vide => bvect == 0 */
			    walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 0);
			else
			    /* &StrLen (n, ...) et n > 0 */
			    /* non vide => bvect == 1 */
			    walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 1);
		
			break;
		    }
		}
		else {
		    /* Appel negatif */
		    switch (nt) {
		    case STREQ_ic:
			cur_param = XH_list_elem (rcg_predicates, bot2+2);

			if (cur_param == 0) {
			    /* Le 2eme param doit etre non vide */
			    walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 1);
			    break;
			}

			if (is_in_Tstar (XH_X (rcg_parameters, cur_param), XH_X (rcg_parameters, cur_param+1)) == 1) {
			    /* On ne sait rien sur le 2eme param */
			    walk_rhs_prdct (clause, cur+1, top);
			    /* walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 0); */
			    /* walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 1); */
			    break;
			}

		    case STREQLEN_ic:
			/* Les 2 param ne peuvent pas etre simultanement vides bvect 1 ou 2 ou 3 */
		        /* walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, NULL, 1);
			   walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, NULL, 2);
			   walk_rhs_prdct_bvect (clause, cur, top, bot2+2, top2, NULL, 3); */
                        /* qcq, on saute */
		        walk_rhs_prdct (clause, cur+1, top);
			break;

		    case STRLEN_ic:
			l = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));

			if (l == 0)
			    /* !&StrLen (0, ...) */
			    /* !vide => bvect == 1 */
			    walk_rhs_prdct_bvect (clause, cur, top, bot2+3, top2, NULL, 1);
			else
			    /* !&StrLen (n, ...) et n > 0 */
			    /* qcq, on saute */
			    walk_rhs_prdct (clause, cur+1, top);
		
			break;
		    }
		}
	    }
	}
    }
}

static void
walk_rhs_prdct_bvect (SXINT clause, 
		      SXINT cur, 
		      SXINT top, 
		      SXINT bot2, 
		      SXINT top2, 
		      SXBA bvect, 
		      SXINT predef_bvect)
{
  /* On parcourt les arguments du predicat en position cur de clause dont le vecteur caracteristique
     est bvect. */
  /* Si bvect == NULL on a 0 <= predef_bvect <= 3 :
     0 => 2 arg vides
     1 => 1er arg non vide
     2 => 2eme arg non vide
     3 => 2 args non vides */
  /* Si bvect != NULL, c'est une chaine de bits  */

  SXINT		Vtop, NVtop, paramtop, pos, cur2, param, bot3, cur3, top3, symb, var; 
  bool	is_arg_empty, is_arg_non_empty, has_t;
  /* Le 19/12/03 */
  /* Si is_arg_empty ou is_arg_non_empty sont positionnes, on est su^r que c'est vide ou que c'est non vide
     sinon doute!! */

  Vtop = V_stack [0];
  NVtop = NV_stack [0];
  paramtop = param_stack [0];

  for (pos = 0, cur2 = bot2; cur2 < top2; pos++, cur2++) {
    /* is_arg_empty = (bvect & 1) == 0; */
    /* bvect >>= 1; */
    if (bvect) {
      is_arg_non_empty = SXBA_bit_is_set (bvect, pos);
      is_arg_empty = false; /* on sait pas */
    }
    else {
      /* La` on sait ... */
      is_arg_empty = (predef_bvect & (pos+1)) == 0;
      is_arg_non_empty = !is_arg_empty;
    }

    param = XH_list_elem (rcg_predicates, cur2);

    if (param != 0) {
      bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);
      has_t = false;

      for (cur3 = bot3; cur3 < top3; cur3++) {
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb < 0) {
	  /* terminal */
	  has_t = true;
	  break;
	}
      }

      if (has_t) {
	if (is_arg_empty)
	  /* Echec */
	  break;

	/* Les variables du param sont qcq */
      }
      else {
	if (is_arg_empty) {
	  /* Les "..." sont pris comme des variables, a voir! */
	  /* On met tout dans V si pas ds NV */
	  for (cur3 = bot3; cur3 < top3; cur3++) {
	    var = XH_list_elem (rcg_parameters, cur3); /* C'est une variable */

	    if (SXBA_bit_is_set (NV_set, var))
	      /* Echec */
	      break;

	    if (SXBA_bit_is_reset_set (V_set, var))
	      PUSH (V_stack, var);
	  }

	  if (cur3 < top3)
	    /* Echec */
	    break;
	}
	else {
	  if (is_arg_non_empty) {
	    /* Au moins une variable de ce param ne doit pas etre vide */
	    if (top3-bot3 == 1) {
	      /* C'est symb */
	      if (SXBA_bit_is_set (V_set, symb))
		/* Echec */
		break;

	      if (SXBA_bit_is_reset_set (NV_set, symb))
		PUSH (NV_stack, symb);
	    }
	    else {
	      /* La clause est "combinatorial" */
	      /* traitement differe' */
#if 0
	      /* POUR L'INSTANT ... */
	      PUSH (param_stack, param);
#endif /* 0 */
	    }
	  }
	}
      }
    }
    else {
      if (is_arg_non_empty)
	break;
    }
  }

  if (cur2 == top2) {
    walk_rhs_prdct (clause, cur+1, top);
  }

  /* On remet en etat */
  while (V_stack [0] > Vtop) {
    var = POP (V_stack);
    SXBA_0_bit (V_set, var);
  }

  while (NV_stack [0] > NVtop) {
    var = POP (NV_stack);
    SXBA_0_bit (NV_set, var);
  }

  param_stack [0] = paramtop;
}

static void
empty_construction (void)
{
  SXINT clause, rhs, top;

  clause = 0;

  while ((clause = sxba_scan (empty_clause_set, clause)) > 0) {
    if (clause2identical [clause] == clause && !SXBA_bit_is_set (false_clause_set, clause)) {
      /* RHS vide (ou uniquement avec des predef-prdct ou des predicats externes) */
      /* S'il y a des predef-prdct on peut quand meme glaner qq renseignements :
	 &StrLen(3, X) */
      rhs = XxY_Y (rcg_clauses, clause);

      if (rhs > 0) {
	walk_rhs_prdct (clause, XH_X (rcg_rhs, rhs), XH_X (rcg_rhs, rhs+1));
	sxba_empty (lhs_var_set);
      }
      else	/* NV_set = V_set = vide */
	substitute (clause);
    }
  }

  while (!IS_EMPTY (clause_stack)) {
    clause = POP (clause_stack);
    SXBA_0_bit (clause_set, clause);

    if (!is_lhs_complete (clause)) {
      /* On a besoin de la RHS */
      rhs = XxY_Y (rcg_clauses, clause);
      top = XH_X (rcg_rhs, rhs+1);
      walk_rhs_prdct (clause, XH_X (rcg_rhs, rhs), top);
      sxba_empty (lhs_var_set);
    }
  }

  /* Quand on tient compte des substitutions vides, le critere de productivite plus contreignant a
     pu detecter des clauses non productives qui avaient echappees a la procedure is_productive */
  clause = 0;

  while ((clause = sxba_0_lrscan (productive_eclause_set, clause)) > 0) {
    if (SXBA_bit_is_set (productive_clause_set, clause)) {
      if (SXBA_bit_is_reset_set (false_clause_set, clause)) {
	PUSH (new_false_clause_stack, clause);

	if (check_instantiable_clause)
	  sxerror (clause2visited [clause]->token.source_index,
		   sxtab_ptr->err_titles [2][0],
		   "%sNon productive clause.",
		   sxtab_ptr->err_titles [2]+1);
      }
    }
  }
}
#endif /* 0 */


#if 1
/* Nouvelle version qui utilise clause2attr_disp */
/* ... et qui ne decoupe pas par variable les arguments combinatoires */
static SXINT
walk_empty_memel (SXINT param, SXINT val)
{
  SXINT pair, local_pair, val2;

  if (XxY_set (&memel, param, param, &local_pair)) {
    /* deja visite */
    return val | memel2val [local_pair]; /* finesse */
  }

  memel2val [local_pair] = 0;

  XxY_Yforeach (memel, param, pair) {
    val2 = XxY_X (memel, pair);

    if (val2 != param) {
      if (val2 <= 0) {
	val |= (val2 == 0) ? 1 : 2;
      }
      else
	val = walk_empty_memel(val2, val);
    }
  }

  XxY_Xforeach (memel, param, pair) {
    val2 = XxY_Y (memel, pair);

    if (val2 != param)
      val = walk_empty_memel(val2, val);
  }

  return val;
}

/* Ici val est non nul, on l'affecte a tous les noeuds accessibles */
static void
propagate_empty_memel (SXINT param, SXINT val)
{
  SXINT pair, val2;

  pair = XxY_is_set (&memel, param, param);

  if (memel2val [pair] == val)
    return;

  memel2val [pair] = val;

  XxY_Yforeach (memel, param, pair) {
    val2 = XxY_X (memel, pair);

    if (val2 != param && val2 > 0) 
      propagate_empty_memel (val2, val);
  }

  XxY_Xforeach (memel, param, pair) {
    val2 = XxY_Y (memel, pair);

    if (val2 != param)
      propagate_empty_memel (val2, val);
  }
}

/* val > 1, on exploite */
static bool
process_memel_result (SXINT process_memel_result_clause, SXINT param, SXINT rhs_pos, SXINT val)
{
  SXINT     size;
  SXBA    process_memel_result_lhs_pos_set, empty_vector;
  bool has_changed;

  propagate_empty_memel (param, val);
    
  empty_vector = clause2empty_vector [process_memel_result_clause];

  if (val == 3) {
    /* incoherence ds la taille de param => la process_memel_result_clause est non instantiable */
    if (SXBA_bit_is_reset_set (false_clause_set, process_memel_result_clause)) {
      PUSH (new_false_clause_stack, process_memel_result_clause);

      if (check_instantiable_clause)
	sxerror (clause2visited [process_memel_result_clause]->token.source_index,
		 sxtab_ptr->err_titles [2][0],
		 "%sNon instantiable clause.",
		 sxtab_ptr->err_titles [2]+1);

      if (!sxba_is_empty (empty_vector)) {
	sxba_empty (empty_vector);
	return true; /* Ca a change */
      }
    }

    return false;
  }

  /* ... donc val == 2 => param ne derive pas ds le vide */
  process_memel_result_lhs_pos_set = clause2attr_disp [process_memel_result_clause] [rhs_pos];
  /* Attention process_memel_result_lhs_pos_set et empty_vector ne sont pas de la meme taille */
  /* truc */
  size = empty_vector [0];
  empty_vector [0] = process_memel_result_lhs_pos_set [0];

  if ((has_changed = !sxba_is_subset (process_memel_result_lhs_pos_set, empty_vector))) {
    sxba_or (empty_vector, process_memel_result_lhs_pos_set);
  }

  empty_vector [0] = size;

  return has_changed;
}

static void
empty_construction (void)
{
  SXINT     empty_construction_clause, bot, cur, top, empty_construction_lhs_prdct, lhs_pos, param, param1, param2, bot2, cur2, top2;
  SXINT     rhs, prdct, nt, pair, rhs_pos, cur_rhs_pos, pos, val; 
  SXBA    clause_empty_vector, nt_empty_vector, line;
  bool is_init_phase, has_changed, is_first;

  /* Initialement, on remplit nt2empty_vector et clause2empty_vector de facon telle que si nt2empty_vector [nt] [pos] ==
     1 => pos ne derive pas ds le vide
     0 => pos peut deriver ds le vide
  */

  /* Initialisation */
  sxinitialise(top); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(bot); /* pour faire taire gcc -Wuninitialized */
  for (empty_construction_clause = 1; empty_construction_clause <= last_clause; empty_construction_clause++) {
    if (clause2identical [empty_construction_clause] == empty_construction_clause && !SXBA_bit_is_set (false_clause_set, empty_construction_clause)) {
      PUSH (clause_stack, empty_construction_clause);
      SXBA_1_bit (clause_set, empty_construction_clause);
      sxba_empty (clause2empty_vector [empty_construction_clause]); /* Pour quand on n'est pas ds la 1ere passe */
      nt = clause2lhs_nt (empty_construction_clause);

      if (SXBA_bit_is_reset_set (nt_set, nt))
	sxba_empty (nt2empty_vector [nt]);
    }
  }

  sxba_empty (nt_set);
  is_init_phase = true;

  while (!IS_EMPTY (clause_stack)) {
    while (!IS_EMPTY (clause_stack)) {
      empty_construction_clause = POP (clause_stack);
      SXBA_0_bit (clause_set, empty_construction_clause);
      clause_empty_vector = clause2empty_vector [empty_construction_clause];
      has_changed = false;

      if (is_init_phase) {
	has_changed = true; /* Ds tous les cas */
	/* pos ne derive pas ds le vide si un arg contient un terminal */
	empty_construction_lhs_prdct = XxY_X (rcg_clauses, empty_construction_clause) & lhs_prdct_filter;
	bot = XH_X (rcg_predicates, empty_construction_lhs_prdct);

	for (lhs_pos = 0, top = XH_X (rcg_predicates, empty_construction_lhs_prdct+1), cur = bot+2; cur < top; lhs_pos++, cur++) {
	  param = XH_list_elem (rcg_predicates, cur);

	  if (param != 0) {
	    bot2 = XH_X (rcg_parameters, param);
	    top2 = XH_X (rcg_parameters, param+1);

	    for (cur2 = bot2; cur2 < top2; cur2++) {
	      /* On regarde si l'arg contient un terminal */
	      if (XH_list_elem (rcg_parameters, cur2) < 0) {
		SXBA_1_bit (clause_empty_vector, lhs_pos);
		break;
	      }
	    }
	  }
	}
      }

      rhs = XxY_Y (rcg_clauses, empty_construction_clause);

      if (rhs > 0) {
	/* On exploite les &Str eventuels */
	/* On met ds memel des certitudes :
	   0 => ne derive que ds le vide
	   -1 => ne derive jamais ds le vide */
	bot = XH_X (rcg_rhs, rhs);
	top = XH_X (rcg_rhs, rhs+1);
	
	for (cur = bot; cur < top; cur++) {
	  prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, prdct);
	  top2 = XH_X (rcg_predicates, prdct+1);
	  nt = XH_list_elem (rcg_predicates, bot2+1);

	  if (nt > 0) {
	    /* Predicat utilisateur */
	    if (!is_init_phase) {
	      /* Ds la phase initiale nt2empty_vector est entierement vide */
	      if (XH_list_elem (rcg_predicates, bot2) == 0 /* Appel positif */
		  && SXBA_bit_is_set (is_lhs_nt, nt) /* Definition locale */) {
		for (pos = 0, cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
		  if (SXBA_bit_is_set (nt2empty_vector [nt], pos)) {
		    /* L'arg courant ne derive jamais ds le vide */
		    param = XH_list_elem (rcg_predicates, cur2);
		    XxY_set (&memel, -1, param, &pair);
		  }
		}
	      }
	    }
	  }
	  else {
	    if (nt == STRLEN_ic || nt == STREQ_ic || nt == STREQLEN_ic) {
	      param1 = XH_list_elem (rcg_predicates, bot2+2);
	      param2 = XH_list_elem (rcg_predicates, bot2+3);

	      if (XH_list_elem (rcg_predicates, bot2) == 0) {
		/* Appel positif */
		if (nt == STRLEN_ic) {
		  param1 = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param1)); /* la longueur */
		  XxY_set (&memel, -param1, param2, &pair);
		}
		else {
		  if (param1 < param2)
		    XxY_set (&memel, param1, param2, &pair);
		  else {
		    if (param2 < param1)
		      XxY_set (&memel, param2, param1, &pair);
		  }
		}
	      }
	      else {
		/* Appel negatif */
		if (nt == STRLEN_ic) {
		  /* On peut faire qqchose si le 1er arg est 0 */
		  param1 = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param1)); /* la longueur */

		  if (param1 == 0) {
		    XxY_set (&memel, -1, param2, &pair);
		  }
		}
		else {
		  if (nt == STREQLEN_ic) {
		    /* On peut faire qqchose se l'un des 2 est de longueur nulle ! */
		    /* A FAIRE */
		  }
		}
	      }
	    }
	  }
	}
      }

      if (XxY_top (memel) > 0) {
	/* On exploite */
	rhs_pos = 0;

	for (cur = bot; cur < top; cur++) {
	  prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, prdct);
	  top2 = XH_X (rcg_predicates, prdct+1);
	  cur_rhs_pos = rhs_pos;
	  rhs_pos += top2-bot2-2;
	  nt = XH_list_elem (rcg_predicates, bot2+1);

	  if (nt > 0) {
	    /* Predicat utilisateur */
	    if (!is_init_phase) {
	      /* Ds la phase initiale nt2empty_vector est entierement vide */
	      if (XH_list_elem (rcg_predicates, bot2) == 0 /* Appel positif */
		  && SXBA_bit_is_set (is_lhs_nt, nt) /* Definition locale */) {
		for (pos = 0, cur2 = bot2+2; cur2 < top2; cur_rhs_pos++, pos++, cur2++) {
		  if (SXBA_bit_is_set (nt2empty_vector [nt], pos)) {
		    /* L'arg courant ne derive jamais ds le vide */
		    param = XH_list_elem (rcg_predicates, cur2);
		    val = walk_empty_memel (param, (SXINT)0);

		    /* val == 0 => peut peut_etre deriver ds le vide */
		    /* val == 1 => ne deriver que ds le vide */
		    /* val == 2 => ne derive jamais ds le vide */
		    /* val == 3 => contradiction, clause non instantiable */
		    if (val > 1 && process_memel_result (empty_construction_clause, param, cur_rhs_pos, val))
		      has_changed = true;
		  }
		}
	      }
	    }
	  }
	  else {
	    if (nt == STRLEN_ic || nt == STREQ_ic || nt == STREQLEN_ic) {
	      param1 = XH_list_elem (rcg_predicates, bot2+2);
	      param2 = XH_list_elem (rcg_predicates, bot2+3);

	      if (XH_list_elem (rcg_predicates, bot2) == 0) {
		/* Appel positif */
		if (nt == STRLEN_ic) {
		  val = walk_empty_memel (param2, (SXINT)0);
		    
		  if (val > 1 && process_memel_result (empty_construction_clause, param2, cur_rhs_pos+1, val))
		    has_changed = true;
		}
		else {
		  val = walk_empty_memel (param1, (SXINT)0);
		  /* val == walk_empty_memel (param2, 0) */
		    
		  if (val > 1 && (process_memel_result (empty_construction_clause, param1, cur_rhs_pos, val)
				  || process_memel_result (empty_construction_clause, param2, cur_rhs_pos+1, val)))
		    has_changed = true;
		}
	      }
	      else {
		/* Appel negatif */
		if (nt == STRLEN_ic) {
		  param1 = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param1)); /* la longueur */

		  /* On peut faire qqchose si le 1er arg est 0 */
		  if (param1 == 0) {
		    val = walk_empty_memel (param2, (SXINT)0);
		    
		    if (val > 1 && process_memel_result (empty_construction_clause, param2, cur_rhs_pos+1, val))
		      has_changed = true;
		  }
		}
		else {
		  if (nt == STREQLEN_ic) {
		    /* On peut faire qqchose se l'un des 2 est de longueur nulle ! */
		    /* A FAIRE */
		  }
		}
	      }
	    }
	  }
	}

	XxY_clear (&memel);
      }

      if (has_changed) {
	nt = clause2lhs_nt (empty_construction_clause);

	if (SXBA_bit_is_reset_set (nt_set, nt))
	  PUSH (nt_stack, nt);
      }
    }

    while (!IS_EMPTY (nt_stack)) {
      nt = POP (nt_stack);
      SXBA_0_bit (nt_set, nt);
      nt_empty_vector = nt2empty_vector [nt];
      line = lhsnt2clause [nt];

      /* Au moins une nt_clause a change' */
      empty_construction_clause = 0;
      is_first = true;

      while ((empty_construction_clause = sxba_scan (line, empty_construction_clause)) > 0) {
	if (clause2identical [empty_construction_clause] == empty_construction_clause && !SXBA_bit_is_set (false_clause_set, empty_construction_clause)) {
	  if (is_first) {
	    is_first = false;
	    sxba_copy (pos_set, clause2empty_vector [empty_construction_clause]);
	  }
	  else
	    sxba_and (pos_set, clause2empty_vector [empty_construction_clause]);
	}
      }

      if (sxba_first_difference (pos_set, nt_empty_vector) >= 0) {
	/* nt_empty_vector est strictement inclus ds pos_set */
	sxba_copy (nt_empty_vector, pos_set);
	
	/* On reevalue les clauses ou nt intervient en RHS */
	line = rhsnt2clause [nt];

	empty_construction_clause = 0;

	while ((empty_construction_clause = sxba_scan (line, empty_construction_clause)) > 0) {
	  if (clause2identical [empty_construction_clause] == empty_construction_clause && !SXBA_bit_is_set (false_clause_set, empty_construction_clause)) {
	    if (SXBA_bit_is_reset_set (clause_set, empty_construction_clause))
	      PUSH (clause_stack, empty_construction_clause);
	  }
	}
      }
    }

    is_init_phase = false;
  }
}
#endif /* 1 */



#if 1
/* ESSAI */
static void
first_last_complete (SXINT first_last_complete_clause)
{
  SXINT   first_last_complete_lhs_prdct, lhs_bot, lhs_nt, ntpos, lhs_pos, lhs_cur, lhs_top, lhs_param, lhs_bot2, symb, lhs_top2, lhs_param_size; 
  SXINT   rhs, rhs_top, rhs_bot, rhs_cur, rhs_prdct, rhs_bot2, rhs_top2, rhs_nt, Ak, pos, rhs_cur2, rhs_pos, rhs_param;
  SXINT   rhs_bot3, rhs_top3, rhs_cur3, elem, xx, Bh, elem2; 
  SXBA	cur_first, cur_last, empty_vector, *M_pos;

  sxinitialise(rhs_top); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(rhs_bot); /* pour faire taire gcc -Wuninitialized */
  sxinitialise(M_pos); /* pour faire taire gcc -Wuninitialized */
  first_last_complete_lhs_prdct = XxY_X (rcg_clauses, first_last_complete_clause) & lhs_prdct_filter;
  lhs_bot = XH_X (rcg_predicates, first_last_complete_lhs_prdct);
  lhs_nt = XH_list_elem (rcg_predicates, lhs_bot+1);

  if (nt2arity [lhs_nt] == 0) return;

  if (!SXBA_bit_is_set (empty_clause_set, first_last_complete_clause)) {
    /* Ce n'est pas une partie droite vide (generalisee) */
    rhs = XxY_Y (rcg_clauses, first_last_complete_clause);
    rhs_top = XH_X (rcg_rhs, rhs+1);
    rhs_bot = XH_X (rcg_rhs, rhs);
    M_pos = clause2attr_disp [first_last_complete_clause];
  }
  else
    rhs = 0;

  ntpos = A_k2Ak (lhs_nt, 0);

  for (lhs_pos = 0, lhs_top = XH_X (rcg_predicates, first_last_complete_lhs_prdct+1), lhs_cur = lhs_bot+2;
       lhs_cur < lhs_top;
       lhs_pos++, ntpos++, lhs_cur++) {
    lhs_param = XH_list_elem (rcg_predicates, lhs_cur);
    cur_first = first [ntpos];
    cur_last = last [ntpos];

    if (lhs_param != 0) {
      /* Traitement de first */
      lhs_bot2 = XH_X (rcg_parameters, lhs_param);

      if ((symb = -XH_list_elem (rcg_parameters, lhs_bot2)) > 0) {
	/* terminal ou entier */
	if (symb <= max_ste)
	  /* terminal */
	  SXBA_1_bit (cur_first, symb);
      }
      else {
	/* l'arg commence par une variable, on cherche les args de la rhs qui commencent param */
	if (rhs) {
	  rhs_pos = 0;

	  for (rhs_cur = rhs_bot; rhs_cur < rhs_top; rhs_cur++) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0
		&& XH_list_elem (rcg_predicates, rhs_bot2) == 0
		&& SXBA_bit_is_set (is_lhs_nt, rhs_nt)) {
	      /* Appel positif d'un predicat utilisateur defini localement */
	      empty_vector = nt2empty_vector [rhs_nt];
	      Ak = A_k2Ak (rhs_nt, 0);

	      for (pos = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; Ak++, pos++, rhs_pos++, rhs_cur2++) {
		if (SXBA_bit_is_set (M_pos [rhs_pos], lhs_pos)
		    && SXBA_bit_is_set (empty_vector, pos)) {
		  /* cet rhs arg est un sous arg de lhs_param ... */
		  /* ... de taille non vide */
		  rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

		  rhs_bot3 = XH_X (rcg_parameters, rhs_param);
		  rhs_top3 = XH_X (rcg_parameters, rhs_param+1);
		    
		  for (rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; rhs_cur3++) {
		    symb = XH_list_elem (rcg_parameters, rhs_cur3);

		    if (symb == 0) {
		      /* "..." est une limite ... */
		      /* ... on dit qu'on sait rien */
		      break;
		    }
		  }

		  if (rhs_cur3 == rhs_top3) {
		    if (is_a_sub_param (lhs_param, rhs_bot3, rhs_top3) == 0) {
		      /* C'est un prefixe */
		      /* On pourra recopier cur_first [Ak] dans cur_first [ntpos] */
		      PUSH (Ak_stack, Ak);
		    }
		  }
		}
	      }
	    }
	    else
	      rhs_pos += rhs_top2-rhs_bot2-2;
	  }

	  if (IS_EMPTY (Ak_stack)) {
	    sxba_fill (cur_first);

	    if (SXBA_bit_is_reset_set (first_Ak_set, ntpos))
	      PUSH (first_Ak_stack, ntpos);
	  }
	  else {
	    Ak = POP (Ak_stack);

	    if (IS_EMPTY (Ak_stack)) {
	      XxY_set (&first_rel, Ak, ntpos, &elem);
	    }
	    else {
	      /* La c'est l'intersection entre tous les Ak qui doit etre mise ds ntpos */
	      XH_push (first_Ak_list, Ak);

	      for (xx = Ak_stack [0]; xx > 0; xx--) {
		Bh = Ak_stack [xx];
		XH_push (first_Ak_list, Bh);
	      }

	      XH_push (first_Ak_list, ntpos);
	      XH_set (&first_Ak_list, &elem2);

	      XxY_set (&first_rel, Ak, -elem2, &elem);

	      while (!IS_EMPTY (Ak_stack)) {
		Bh = POP (Ak_stack);
		XxY_set (&first_rel, Bh, -elem2, &elem);
	      }
	    }
	  }
	}
	else
	  sxba_fill (cur_first);
      }

      /* On dit que first [ntpos] a ete modifie */
      if (SXBA_bit_is_reset_set (first_Ak_set, ntpos))
	PUSH (first_Ak_stack, ntpos);

      /* Traitement de last */
      lhs_top2 = XH_X (rcg_parameters, lhs_param+1);

      if ((symb = -XH_list_elem (rcg_parameters, lhs_top2-1)) > 0) {
	/* terminal ou entier */
	if (symb <= max_ste)
	  /* terminal */
	  SXBA_1_bit (cur_last, symb);
      }
      else {
	/* l'arg se termine par une variable, on cherche les args de la rhs qui terminent lhs_param */
	if (rhs) {
	  rhs_pos = 0;

	  for (rhs_cur = rhs_bot; rhs_cur < rhs_top; rhs_cur++) {
	    rhs_prdct = XH_list_elem (rcg_rhs, rhs_cur);
	    rhs_bot2 = XH_X (rcg_predicates, rhs_prdct);
	    rhs_top2 = XH_X (rcg_predicates, rhs_prdct+1);
	    rhs_nt = XH_list_elem (rcg_predicates, rhs_bot2+1);

	    if (rhs_nt > 0
		&& XH_list_elem (rcg_predicates, rhs_bot2) == 0
		&& SXBA_bit_is_set (is_lhs_nt, rhs_nt)) {
	      /* Appel positif d'un predicat utilisateur defini localement */
	      empty_vector = nt2empty_vector [rhs_nt];
	      Ak = A_k2Ak (rhs_nt, 0);

	      lhs_param_size = XH_X (rcg_parameters, lhs_param+1) - XH_X (rcg_parameters, lhs_param);

	      for (pos = 0, rhs_cur2 = rhs_bot2+2; rhs_cur2 < rhs_top2; Ak++, pos++, rhs_pos++, rhs_cur2++) {
		if (SXBA_bit_is_set (M_pos [rhs_pos], lhs_pos)
		    && SXBA_bit_is_set (empty_vector, pos)) {
		  /* cet rhs arg est un sous arg de lhs_param ... */
		  /* ... de taille non vide */
		  rhs_param = XH_list_elem (rcg_predicates, rhs_cur2);

		  rhs_bot3 = XH_X (rcg_parameters, rhs_param);
		  rhs_top3 = XH_X (rcg_parameters, rhs_param+1);
		    
		  for (rhs_cur3 = rhs_bot3; rhs_cur3 < rhs_top3; rhs_cur3++) {
		    symb = XH_list_elem (rcg_parameters, rhs_cur3);

		    if (symb == 0) {
		      /* "..." est une limite ... */
		      /* ... on dit qu'on sait rien */
		      break;
		    }
		  }

		  if (rhs_cur3 == rhs_top3) {
		    if (is_a_sub_param (lhs_param, rhs_bot3, rhs_top3) + (rhs_top3-rhs_bot3) == lhs_param_size) {
		      /* C'est un suffixe */
		      /* On pourra recopier cur_last [Ak] dans cur_last [ntpos] */
		      PUSH (Ak_stack, Ak);
		    }
		  }
		}
	      }
	    }
	    else
	      rhs_pos += rhs_top2-rhs_bot2-2;
	  }

	  if (IS_EMPTY (Ak_stack)) {
	    sxba_fill (cur_last);

	    if (SXBA_bit_is_reset_set (last_Ak_set, ntpos))
	      PUSH (last_Ak_stack, ntpos);
	  }
	  else {
	    Ak = POP (Ak_stack);

	    if (IS_EMPTY (Ak_stack)) {
	      XxY_set (&last_rel, Ak, ntpos, &elem);
	    }
	    else {
	      /* La c'est l'intersection entre tous les Ak qui doit etre mise ds ntpos */
	      XH_push (last_Ak_list, Ak);

	      for (xx = Ak_stack [0]; xx > 0; xx--) {
		Bh = Ak_stack [xx];
		XH_push (last_Ak_list, Bh);
	      }

	      XH_push (last_Ak_list, ntpos);
	      XH_set (&last_Ak_list, &elem2);

	      XxY_set (&last_rel, Ak, -elem2, &elem);

	      while (!IS_EMPTY (Ak_stack)) {
		Bh = POP (Ak_stack);
		XxY_set (&last_rel, Bh, -elem2, &elem);
	      }
	    }
	  }
	}
	else
	  sxba_fill (cur_last);
      }

      /* On dit que last [ntpos] a ete modifie */
      if (SXBA_bit_is_reset_set (last_Ak_set, ntpos))
	PUSH (last_Ak_stack, ntpos);
    }
  }
}


static void
first_last_construction (void)
{
  /* Il serait + fin (mais + cher) de correler a` la fois first et last et l'ensemble des parametres
     d'un nt. Ex si A(X,Y,Z) est un appel d'un predicat en RHS.  On a calcule' first-last[A] comme
     etant un ensemble de triplets de couples {(a1f,a1l),(a2f,a2l),(a3f,a3l) | \exists \rho, w^\rho =
     (a1f x a1l, a2f y a2l, a3f z a3l)} */
  SXINT 	first_last_construction_clause, A, k, Ak, Bh, Cl, elem, arity, bot, cur, top, pos;
  SXBA	cur_t_set, w_t_set, empty_vector, line;

  XxY_foreach (rcg_clauses, first_last_construction_clause) {
    if (clause2identical [first_last_construction_clause] == first_last_construction_clause && !SXBA_bit_is_set (false_clause_set, first_last_construction_clause)) {
      first_last_complete (first_last_construction_clause);
    }
  }

  w_t_set = sxba_calloc (max_t+1);

  while (!IS_EMPTY (first_Ak_stack)) {
    Ak = POP (first_Ak_stack);
    SXBA_0_bit (first_Ak_set, Ak);

    if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
      cur_t_set = first [Ak];

      if (!sxba_is_empty (cur_t_set)) {
	XxY_Xforeach (first_rel, Ak, elem) {
	  Bh = XxY_Y (first_rel, elem);

	  if (Bh > 0) {
	    if (OR (first [Bh], cur_t_set)) {
	      /* first [Bh] a ete modifie, on continue */
	      if (SXBA_bit_is_reset_set (first_Ak_set, Bh))
		PUSH (first_Ak_stack, Bh);
	    }
	  }
	  else {
	    /* Element de first_Ak_list */
	    bot = XH_X (first_Ak_list, -Bh);
	    top = XH_X (first_Ak_list, -Bh+1);

	    Bh = XH_list_elem (first_Ak_list, top-1);
	    sxba_copy (w_t_set, cur_t_set);

	    for (cur = top-2; bot <= cur; cur--) {
	      Cl = XH_list_elem (first_Ak_list, cur);

	      if (Cl != Ak)
		sxba_and (w_t_set, first [Cl]);
	    }

	    if (OR (first [Bh], w_t_set)) {
	      /* first [Bh] a ete modifie, on continue */
	      if (SXBA_bit_is_reset_set (first_Ak_set, Bh))
		PUSH (first_Ak_stack, Bh);
	    }
	  }
	}
      }
    }
  }

  while (!IS_EMPTY (last_Ak_stack)) {
    Ak = POP (last_Ak_stack);
    SXBA_0_bit (last_Ak_set, Ak);

    if (!SXBA_bit_is_set (counter_Ak_set, Ak)) {
      cur_t_set = last [Ak];

      if (!sxba_is_empty (cur_t_set)) {
	XxY_Xforeach (last_rel, Ak, elem) {
	  Bh = XxY_Y (last_rel, elem);

	  if (Bh > 0) {
	    if (OR (last [Bh], cur_t_set)) {
	      /* last [Bh] a ete modifie, on continue */
	      if (SXBA_bit_is_reset_set (last_Ak_set, Bh))
		PUSH (last_Ak_stack, Bh);
	    }
	  }
	  else {
	    /* Element de last_Ak_list */
	    bot = XH_X (last_Ak_list, -Bh);
	    top = XH_X (last_Ak_list, -Bh+1);

	    Bh = XH_list_elem (last_Ak_list, top-1);
	    sxba_copy (w_t_set, cur_t_set);

	    for (cur = top-2; bot <= cur; cur--) {
	      Cl = XH_list_elem (last_Ak_list, cur);

	      if (Cl != Ak)
		sxba_and (w_t_set, last [Cl]);
	    }

	    if (OR (last [Bh], w_t_set)) {
	      /* last [Bh] a ete modifie, on continue */
	      if (SXBA_bit_is_reset_set (last_Ak_set, Bh))
		PUSH (last_Ak_stack, Bh);
	    }
	  }
	}
      }
    }
  }
    
  for (A = 1; A <= max_nt; A++) {
    if ((arity = nt2arity [A])) {
      empty_vector = nt2empty_vector [A];

      for (Ak = A_k2Ak (A, 0), pos = 0; pos < arity; Ak++, pos++) {
	if (SXBA_bit_is_set (empty_vector, pos)) {
	  /* Ne derive pas ds le vide */
	  SXBA_0_bit (first [Ak], 0);
	  SXBA_0_bit (last [Ak], 0);
	}
	else {
	  SXBA_1_bit (first [Ak], 0); /* epsilon */
	  SXBA_1_bit (last [Ak], 0); /* epsilon */
	}
      }
    }
  }

  sxfree (w_t_set);

  /* Si une ligne de first ou last est vide => la grammaire n'est pas reduite Ex:
     S(X) --> A(X) B(X) .
     A(\a) --> .
     B(\b) --> .
  */
    
  for (A = 1; A <= max_nt; A++) {
    if ((arity = nt2arity [A]) && SXBA_bit_is_set (is_lhs_nt, A) && (nt2identical [A] == A)) {
      /* Seuls les predicats definis localement sont concernes */
      for (Ak = A_k2Ak (A, 0), k = 0; k < arity; Ak++, k++) {
	if (!SXBA_bit_is_set (counter_Ak_set, Ak) && (sxba_is_empty (first [Ak]) || sxba_is_empty (last [Ak]))) {
	  if (check_instantiable_clause)
	    sxtmsg (sxsrcmngr.source_coord.file_name,
		    "%sThe %ld%s parameter of predicate <%s> is non instantiable.",
		    sxtab_ptr->err_titles [2]+1,
		    k+1,
		    k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
		    sxstrget (nt2ste [A]));
	  else {
	    /* On enleve les A_clauses */ 
	    line = lhsnt2clause [A];
	    first_last_construction_clause = 0;
      
	    while ((first_last_construction_clause = sxba_scan (line, first_last_construction_clause)) > 0) {
	      if (clause2identical [first_last_construction_clause] == first_last_construction_clause && SXBA_bit_is_reset_set (false_clause_set, first_last_construction_clause)) {
		PUSH (new_false_clause_stack, first_last_construction_clause);
	      }
	    }

	    break;
	  }
	}
      }
    }
  }

}
#endif /* 1 */


#if 0
static void
first_last_complete (SXINT clause)
{
    /* La substitution des variables de la partie droite est contenue dans V_set et NV_set.
       NV_set contient l'ensemble des variables de la RHS qui ne derivent pas dans epsilon.
       V_set contient l'ensemble des variables de la RHS qui derivent dans epsilon.
       */
    SXINT		lhs_prdct, bot, top, nt, pos, param, bot2, top2, symb, x, y, z, t;
    SXINT		ntpos, *X_stack, Ak, param3, bot3, top3, x3, symb3;
    SXINT		xx, Bh, elem, elem2;
    SXBA	cur_first, cur_last;
    bool	is_t_arg;

    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    bot = XH_X (rcg_predicates, lhs_prdct);
    nt = XH_list_elem (rcg_predicates, bot+1);

    if (nt2arity [nt] == 0) return;

    pos = 0;
    ntpos = A_k2Ak (nt, 0);

    for (top = XH_X (rcg_predicates, lhs_prdct+1), x = bot+2; x < top; ntpos++, x++) {
	pos++;
	param = XH_list_elem (rcg_predicates, x);
	cur_first = first [ntpos];
	cur_last = last [ntpos];

	if (param != 0) {
	    bot2 = XH_X (rcg_parameters, param);
	    top2 = XH_X (rcg_parameters, param+1);

	    is_t_arg = false;

	    for (y = bot2; y < top2; y++) {
		/* On regarde si l'arg contient un terminal */
		symb = XH_list_elem (rcg_parameters, y);

		if (symb < 0) {
		    is_t_arg = true;
		    break;
		}
	    }

	    for (y = bot2; y < top2; y++) {
		/* Traitement de first de gauche a droite */
		symb = XH_list_elem (rcg_parameters, y);

		if (symb < 0) {
		    t = -symb;

		    SXBA_1_bit (cur_first, t);

		    if (SXBA_bit_is_reset_set (first_Ak_set, ntpos))
			PUSH (first_Ak_stack, ntpos);

		    break;
		}

		if (!SXBA_bit_is_set (rhs_var_set, symb)) {
		    /* variable non "utilisee" en rhs */
		    /* La valeur de symb est indefinie */
		    /* V_set ou NV_set peut quand meme etre positionne par des predicats predefinis! */
		    sxba_fill (cur_first);

		    if (is_t_arg)
			/* non vide */
			SXBA_0_bit (cur_first, 0);

		    if (SXBA_bit_is_reset_set (first_Ak_set, ntpos))
			PUSH (first_Ak_stack, ntpos);

		    /* On peut quitter */
		    break;
		}

		if (SXBA_bit_is_set (NV_set, symb)) {
		    /* variable de la partie droite qui :
		       - est utilisee ds un "vrai" appel de predicat
		       - ne derive pas dans le vide
		       - suit des variables derivant dans le vide
		    */
		    /* On cherche tous les arguments selectionnes (1 ds le vecteur caracteristique)
		       t.q. il y a une occurrence de cette var apres des var de V_set */
		    X_stack = var_stacks_disp [symb];

		    for (z = X_stack [0]; z > 0; z--) {
			param3 = X_stack [z];
			z--;
			Ak = X_stack [z];

			bot3 = XH_X (rcg_parameters, param3);
			top3 = XH_X (rcg_parameters, param3+1);

			for (x3 = bot3; x3 < top3; x3++) {
			    symb3 = XH_list_elem (rcg_parameters, x3);

			    if (symb3 == symb) {
				/* Ca marche */
				/* On pourra recopier cur_first [Ak] dans cur_first [ntpos] */
				PUSH (Ak_stack, Ak);
				break;
			    }

			    if (symb3 < 0 || !SXBA_bit_is_set (V_set, symb3))
				break;
			}
		    }

		    if (!IS_EMPTY (Ak_stack)) {
			Ak = POP (Ak_stack);

			if (IS_EMPTY (Ak_stack)) {
			    XxY_set (&first_rel, Ak, ntpos, &elem);
			}
			else {
			    /* La c'est l'intersection entre tous les Ak qui doit etre mise ds ntpos */
			    XH_push (first_Ak_list, Ak);

			    for (xx = Ak_stack [0]; xx > 0; xx--) {
				Bh = Ak_stack [xx];
				XH_push (first_Ak_list, Bh);
			    }

			    XH_push (first_Ak_list, ntpos);
			    XH_set (&first_Ak_list, &elem2);

			    XxY_set (&first_rel, Ak, -elem2, &elem);

			    while (!IS_EMPTY (Ak_stack)) {
				Bh = POP (Ak_stack);
				XxY_set (&first_rel, Bh, -elem2, &elem);
			    }
			}
		    }

		    break;
		}

		if (!SXBA_bit_is_set (V_set, symb)) {
		    /* La valeur de symb est indefinie */
#if 0
		    sxba_fill (cur_first), SXBA_0_bit (cur_first, 0);
#endif
		    sxba_fill (cur_first);

		    if (is_t_arg)
			/* non vide */
			SXBA_0_bit (cur_first, 0);

		    if (SXBA_bit_is_reset_set (first_Ak_set, ntpos))
			PUSH (first_Ak_stack, ntpos);

		    /* On peut quitter */
		    break;
		}

		/* ici symb =>+ \varepsilon */
	    }

	    for (y = top2-1; bot2 <= y; y--) {
		/* Traitement de last de droite a gauche */
		symb = XH_list_elem (rcg_parameters, y);

		if (symb < 0) {
		    t = -symb;

		    SXBA_1_bit (cur_last, t);

		    if (SXBA_bit_is_reset_set (last_Ak_set, ntpos))
			PUSH (last_Ak_stack, ntpos);

		    break;
		}

		if (!SXBA_bit_is_set (rhs_var_set, symb)) {
		    /* variable non "utilisee" en rhs */
		    /* La valeur de symb est indefinie */
		    /* V_set ou NV_set peut quand meme etre positionne par des predicats predefinis! */
		    sxba_fill (cur_last);

		    if (is_t_arg)
			/* non vide */
			SXBA_0_bit (cur_last, 0);

		    if (SXBA_bit_is_reset_set (last_Ak_set, ntpos))
			PUSH (last_Ak_stack, ntpos);

		    /* On peut quitter */
		    break;
		}

		if (SXBA_bit_is_set (NV_set, symb)) {
		    /* variable de la partie droite qui :
		          - ne derive pas dans le vide et
			  - est suivie de variables derivant dans le vide
		    */
		    /* On cherche tous les arguments selectionnes (1 ds le vecteur caracteristique)
		       t.q. il y a une occurrence de cette var avant des var de V_set */
		    X_stack = var_stacks_disp [symb];

		    for (z = X_stack [0]; z > 0; z--) {
			param3 = X_stack [z];
			z--;
			Ak = X_stack [z];

			bot3 = XH_X (rcg_parameters, param3);
			top3 = XH_X (rcg_parameters, param3+1);

			for (x3 = top3-1; bot3 <= x3; x3--) {
			    symb3 = XH_list_elem (rcg_parameters, x3);

			    if (symb3 == symb) {
				/* Ca marche */
				/* On pourra recopier cur_last [Ak] dans cur_last [ntpos] */
				PUSH (Ak_stack, Ak);
				break;
			    }

			    if (symb3 < 0 || !SXBA_bit_is_set (V_set, symb3))
				break;
			}
		    }

		    if (!IS_EMPTY (Ak_stack)) {
			Ak = POP (Ak_stack);

			if (IS_EMPTY (Ak_stack)) {
			    XxY_set (&last_rel, Ak, ntpos, &elem);
			}
			else {
			    /* La c'est l'intersection entre tous les Ak qui doit etre mise ds ntpos */
			    XH_push (last_Ak_list, Ak);

			    for (xx = Ak_stack [0]; xx > 0; xx--) {
				Bh = Ak_stack [xx];
				XH_push (last_Ak_list, Bh);
			    }

			    XH_push (last_Ak_list, ntpos);
			    XH_set (&last_Ak_list, &elem2);

			    XxY_set (&last_rel, Ak, -elem2, &elem);

			    while (!IS_EMPTY (Ak_stack)) {
				Bh = POP (Ak_stack);
				XxY_set (&last_rel, Bh, -elem2, &elem);
			    }
			}
		    }

		    break;
		}

		if (!SXBA_bit_is_set (V_set, symb)) {
		    /* La valeur de symb est indefinie */
		    sxba_fill (cur_last);

		    if (is_t_arg)
			/* non vide */
			SXBA_0_bit (cur_last, 0);

		    if (SXBA_bit_is_reset_set (last_Ak_set, ntpos))
			PUSH (last_Ak_stack, ntpos);

		    /* On peut quitter */
		    break;
		}

		/* ici symb =>+ \varepsilon */
	    }
	}
    }
}
#endif /* 0 */

#if 0
static void
first_last_complete (SXINT clause)
{
    /* Les variables communes a la lhs et rhs qui ne sont ni dans V_set ni ds NV_set sont
       quelconques */
    /* On les met ds V_set et NV_set de toutes les facons possibles */
    SXINT n, max, v, Vtop, NVtop, var;

    sxba_copy (GU_set, lhs_var_set);
    sxba_and (GU_set, rhs_var_set);
    sxba_minus (GU_set, V_set);
    sxba_minus (GU_set, NV_set);
    n = sxba_cardinal (GU_set);

    if (n == 0) {
	first_last_try (clause);
    }
    else {
	max = (1<<n) - 1;

	for (v = max; v >= 1; v--) {
	    n = v;
	    Vtop = V_stack [0];
	    NVtop = NV_stack [0];
	    var = 0;

	    while ((var = sxba_scan (GU_set, var)) > 0) {
		if (n & 1) {
		    SXBA_1_bit (NV_set, var);
		    PUSH (NV_stack, var);
		}
		else {
		    SXBA_1_bit (V_set, var);
		    PUSH (V_stack, var);
		}

		n >>= 1;
	    }

	    first_last_try (clause);

	    while (NV_stack [0] > NVtop) {
		var = POP (NV_stack);
		SXBA_0_bit (NV_set, var);
	    }

	    while (V_stack [0] > Vtop) {
		var = POP (V_stack);
		SXBA_0_bit (V_set, var);
	    }
	}

	sxba_empty (GU_set);
    }
}
#endif

#if 0
static bool first_last_arg_walk (SXINT clause, 
				    SXINT cur, 
				    SXINT top, 
				    SXINT bot2, 
				    SXINT top2, 
				    SXBA bvect, 
				    SXINT predef_bvect);

static bool
first_last_rhs_walk (SXINT clause, SXINT cur, SXINT top)
{
    /* Parcourt de la rhs de clause.  Le predicat courant est en cur */
    SXINT		U_set_nb, Vtop, NVtop0, NVtop, xparam, param, bot3, cur3, top3, Vnb, NVnb, Unb, var;
    SXINT		prdct, bot2, top2, nt, arity;
    SXINT		n, max, v, i, l;
    bool	is_set, ret_val;
    SXBA	U_line;

    if (cur == top) {
	/* La partie droite a ete parcourue en entier */
	if (!IS_EMPTY (param_stack)) {
	    /* Les parametres de param_stack ne contiennent que des variables
	       dont l'une au moins ne doit pas etre vide */
	    U_set_nb = 0;
	    NVtop0 = NV_stack [0];

	    for (xparam = 1; xparam <= param_stack [0]; xparam++) {
		param = param_stack [xparam];
		bot3 = XH_X (rcg_parameters, param);
		top3 = XH_X (rcg_parameters, param+1);

		Vnb = NVnb = Unb = 0;

		for (cur3 = bot3; cur3 < top3; cur3++) {
		    var = XH_list_elem (rcg_parameters, cur3);

		    if (SXBA_bit_is_set (V_set, var))
			Vnb++;
		    else {
			if (SXBA_bit_is_set (NV_set, var))
			    NVnb++;
			else {
			    /* Les "..." sont pris par ici */
			    PUSH (U_stack, var);
			    Unb++;
			}
		    }
		}

		if (NVnb + Unb == 0) {
		    /* Toutes les var sont vides => Echec */
		    return false;
		}

		if (NVnb == 0) {
		    if (Unb == 1) {
			/* Elle passe de indefini a non vide */
			var = POP (U_stack);

			if (var > 0) {
			  if (SXBA_bit_is_reset_set (NV_set, var))
			    PUSH (NV_stack, var);
			}
		    }
		    else {
			/* Ici, plusieurs candidates : pb */
			/* Seules les variables sensibles de la partie gauche sont prises en compte */
			U_set_nb++;
			U_line = U_set [U_set_nb];
			is_set = false;
			
			while(!IS_EMPTY (U_stack)) {
			    var = POP (U_stack);

			    if (var > 0 && SXBA_bit_is_set (lhs_var_set, var)) {
				is_set = true;
				SXBA_1_bit (GU_set, var);
				SXBA_1_bit (U_line, var);
			    }
			}

			if (!is_set)
			    U_set_nb--;
		    }
		}
		else
		    /* parametre non vide */
		    U_stack [0] = 0;
	    }

	    if (U_set_nb > 0) {
		/* Ici, il faut trouver toutes les substitutions de variable qui assurent que
		   les ensembles de U_set sont non vides */
		n = sxba_cardinal (GU_set);
		/* Je suppose que le cas est rare et je vais etre bestial => 2^n essais */
		max = (1<<n) - 1;

		for (v = max; v >= 1; v--) {
		    n = v;
		    Vtop = V_stack [0];
		    NVtop = NV_stack [0];
		    var = 0;

		    while ((var = sxba_scan (GU_set, var)) > 0) {
			if (n & 1) {
			  if (SXBA_bit_is_reset_set (NV_set, var))
			    PUSH (NV_stack, var);
			}
			else {
			  if (SXBA_bit_is_reset_set (V_set, var))
			    PUSH (V_stack, var);
			}

			n >>= 1;
		    }

		    for (i = 1; i <= U_set_nb; i++) {
			if (!IS_AND (U_set [i], NV_set))
			    break;
		    }

		    if (i > U_set_nb) {
			/* NV_set est un candidat */
			/* Les variables qui ne sont ni dans V_set ni dans NV_set sont non contraintes */
			first_last_complete (clause);
		    }

		    while (NV_stack [0] > NVtop) {
			var = POP (NV_stack);
			SXBA_0_bit (NV_set, var);
		    }

		    while (V_stack [0] > Vtop) {
			var = POP (V_stack);
			SXBA_0_bit (V_set, var);
		    }
		}

		for (i = 1; i <= U_set_nb; i++)
		    sxba_empty (U_set [i]);

		sxba_empty (GU_set);
	    }
	    else
		first_last_complete (clause);

	    while (NV_stack [0] > NVtop0) {
		var = POP (NV_stack);
		SXBA_0_bit (NV_set, var);
	    }
	}
	else {
	    /* rhs_var_set et V_set sont positionnes, U_var_set et [first|last]_NV_set sont vides */
	    first_last_complete (clause);
	}

	ret_val = true;
    }
    else {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);
      top2 = XH_X (rcg_predicates, prdct+1);
      nt = XH_list_elem (rcg_predicates, bot2+1);

      if (nt > 0) {
	arity = nt2arity [nt];

	if (arity == 0 ||
	    XH_list_elem (rcg_predicates, bot2) == 1 ||
	    (SXBA_bit_is_set (is_nt_external, nt) && !SXBA_bit_is_set (is_lhs_nt, nt)))
	  /* arg_nb == 0 ou */
	  /* Appel negatif */
	  /* Appel d'un vrai predicat externe */
	  /* Ses variables sont qcq */
	  ret_val = first_last_rhs_walk (clause, cur+1, top);
	else {
	  /* A FAIRE : manque le dernier arg !! */
	  ret_val = first_last_arg_walk (clause, cur, top, bot2+2, top2, nt2empty_vector [nt], 0);
#if 0
	  /* Le 19/12/03 */
	  if (is_empty_arg) {
	    /* Empty est alloue */
	    ret_val = first_last_arg_walk (clause, cur, top, bot2+2, top2, nt2empty_vector [nt], 0);
#if 0
	    ret_val = false;

	    XxY_Xforeach (empty, nt, elem) {
	      bvect = XxY_Y (empty, elem);

	      if (first_last_arg_walk (clause, cur, top, bot2+2, top2, bvect, 0))
		ret_val = true;
	    }
#endif
	  }
	  else {
	    ret_val = first_last_rhs_walk (clause, cur+1, top);
#if 0
	    bvect = (1 << arity) -1;
	    ret_val = first_last_arg_walk (clause, cur, top, bot2+2, top2, bvect);
#endif
	  }
#endif /* 0 */
	}
      }
      else {
	/* Predicat predefini */
	if (nt == FALSE_ic || nt == FIRST_ic || nt == LAST_ic || nt == CUT_ic || nt == TRUE_ic || nt == LEX_ic) {
	  /* Dans tous les cas, on saute */
	  ret_val = first_last_rhs_walk (clause, cur+1, top);
	}
	else {
	  if (XH_list_elem (rcg_predicates, bot2) == 0) {
	    /* Appel positif */
	    switch (nt) {
	    case STREQ_ic:
	      cur_param = XH_list_elem (rcg_predicates, bot2+2);

	      if (cur_param == 0) {
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, NULL, 0);
		break;
	      }

	      if (is_in_Tstar (XH_X (rcg_parameters, cur_param), XH_X (rcg_parameters, cur_param+1)) == 1) {
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, NULL, 1);
		break;
	      }

	    case STREQLEN_ic:
	      /* 2 param pouvant etre simultanement vides => bvect = 0 ou non vides => bvect = 3 */
	      ret_val = first_last_rhs_walk (clause, cur+1, top);
#if 0
	      ret_val = first_last_arg_walk (clause, cur, top, bot2+2, top2, 0);

	      if (first_last_arg_walk (clause, cur, top, bot2+2, top2, 3))
		ret_val = true;
#endif

	      break;

	    case STRLEN_ic:
	      l = XH_list_elem (rcg_parameters, XH_X (rcg_parameters, XH_list_elem (rcg_predicates, bot2+2)));

	      if (l == 0)
		/* &StrLen (0, ...) */
		/* vide => bvect == 0 */
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, NULL, 0);
	      else
		/* &StrLen (n, ...) et n > 0 */
		/* non vide => bvect == 1 */
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, NULL, 1);
		
	      break;
	    }
	  }
	  else {
	    /* Appel negatif */
	    switch (nt) {
	    case STREQ_ic:
	      cur_param = XH_list_elem (rcg_predicates, bot2+2);

	      if (cur_param == 0) {
		/* Le 2eme param doit etre non vide */
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, NULL, 1);
		break;
	      }

	      if (is_in_Tstar (XH_X (rcg_parameters, cur_param), XH_X (rcg_parameters, cur_param+1)) == 1) {
		/* On ne sait rien sur le 2eme param */
	        ret_val = first_last_rhs_walk (clause, cur+1, top);
#if 0
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, 0);

		if (first_last_arg_walk (clause, cur, top, bot2+3, top2, 1))
		  ret_val = true;
#endif
		break;
	      }

	    case STREQLEN_ic:
	      /* Les 2 param ne peuvent pas etre simultanement vides bvect 1 ou 2 ou 3 */
              ret_val = first_last_rhs_walk (clause, cur+1, top);
#if 0
	      ret_val = first_last_arg_walk (clause, cur, top, bot2+2, top2, 1);

	      if (first_last_arg_walk (clause, cur, top, bot2+2, top2, 2))
		ret_val = true;

	      if (first_last_arg_walk (clause, cur, top, bot2+2, top2, 3))
		ret_val = true;
#endif
	      break;

	    case STRLEN_ic:
	      if (XH_list_elem (rcg_parameters, XH_X (rcg_parameters,
						      XH_list_elem (rcg_predicates, bot2+2))) == 0)
		/* !&StrLen (0, ...) */
		/* non vide => bvect == 1 */
		ret_val = first_last_arg_walk (clause, cur, top, bot2+3, top2, NULL, 1);
	      else
		/* !&StrLen (n, ...) et n > 0 */
		/* qcq, on saute */
		ret_val = first_last_rhs_walk (clause, cur+1, top);
		
	      break;
	    }
	  }
	}
      }
    }

    return ret_val;
}

static bool
first_last_arg_walk (SXINT clause, 
		     SXINT cur, 
		     SXINT top, 
		     SXINT bot2, 
		     SXINT top2, 
		     SXBA bvect, 
		     SXINT predef_bvect)
{
  /* On parcourt les arguments du predicat en position cur de clause dont le vecteur caracteristique
     est bvect. */
  /* Si bvect == NULL on a 0 <= predef_bvect <= 3 :
     0 => 2 arg vides
     1 => 1er arg non vide
     2 => 2eme arg non vide
     3 => 2 args non vides */
  /* Si bvect != NULL, c'est une chaine de bits  */
  SXINT		Vtop, NVtop, paramtop, pos, cur2, param, bot3, cur3, top3, symb, var; 
  bool	is_arg_empty, is_arg_non_empty, has_t, ret_val;
  /* Le 19/12/03 */
  /* Si is_arg_empty ou is_arg_non_empty sont positionnes, on est su^r que c'est vide ou que c'est non vide
     sinon doute!! */

  Vtop = V_stack [0];
  NVtop = NV_stack [0];
  paramtop = param_stack [0];

  for (pos = 0, cur2 = bot2; cur2 < top2; pos++, cur2++) {
    if (bvect) {
      is_arg_non_empty = SXBA_bit_is_set (bvect, pos);
      is_arg_empty = false; /* on sait pas */
    }
    else {
      /* La` on sait ... */
      is_arg_empty = (predef_bvect & (pos+1)) == 0;
      is_arg_non_empty = !is_arg_empty;
    }
      
    param = XH_list_elem (rcg_predicates, cur2);

    if (param != 0) {
      bot3 = XH_X (rcg_parameters, param);
      top3 = XH_X (rcg_parameters, param+1);
      has_t = false;

      for (cur3 = bot3; cur3 < top3; cur3++) {
	symb = XH_list_elem (rcg_parameters, cur3);

	if (symb < 0) {
	  /* terminal */
	  has_t = true;
	  break;
	}
      }

      if (has_t) {
	if (is_arg_empty)
	  /* Echec */
	  break;

	/* Les variables du param sont qcq */
      }
      else {
	if (is_arg_empty) {
	  /* On met tout dans Vide */
	  for (cur3 = bot3; cur3 < top3; cur3++) {
	    var = XH_list_elem (rcg_parameters, cur3); /* C'est une variable */

	    if (SXBA_bit_is_set (NV_set, var))
	      /* Echec */
	      break;

	    if (SXBA_bit_is_reset_set (V_set, var))
	      PUSH (V_stack, var);
	  }

	  if (cur3 < top3)
	    /* Echec */
	    break;
	}
	else {
	  if (is_arg_non_empty) {
	    /* Au moins une variable de ce param ne doit pas etre vide */
	    if (top3-bot3 == 1) {
	      /* C'est symb */
	      if (SXBA_bit_is_set (V_set, symb))
		/* Echec */
		break;

	      if (SXBA_bit_is_reset_set (NV_set, symb))
		PUSH (NV_stack, symb);
	    }
	    else {
	      /* La clause est "combinatorial" */
	      /* traitement differe' */
#if 0
	      /* POUR L'INSTANT ... */
	      PUSH (param_stack, param);
#endif /* 0 */
	    }
	  }
	}
      }
    }
    else {
      if (is_arg_non_empty)
	break;
    }
  }

  if (cur2 == top2) {
    ret_val = first_last_rhs_walk (clause, cur+1, top);
  }
  else
    ret_val = false;

  /* On remet en etat */
  while (V_stack [0] > Vtop) {
    var = POP (V_stack);
    SXBA_0_bit (V_set, var);
  }

  while (NV_stack [0] > NVtop) {
    var = POP (NV_stack);
    SXBA_0_bit (NV_set, var);
  }

  param_stack [0] = paramtop;

  return ret_val;
}


static void
fill_lhs_var_set (SXINT clause)
{
    /* On note chaque variable X de la partie gauche impliquee ds un arg qui ne contient que des variables */
    SXINT lhs_prdct, bot, top, cur, param, bot2, cur2, top2, symb;

    lhs_prdct = XxY_X (rcg_clauses, clause) & lhs_prdct_filter;
    bot = XH_X (rcg_predicates, lhs_prdct);
    top = XH_X (rcg_predicates, lhs_prdct+1);

    for (cur = bot+2; cur < top; cur++) {
	param = XH_list_elem (rcg_predicates, cur);

	if (param != 0) {
	    bot2 = XH_X (rcg_parameters, param);
	    top2 = XH_X (rcg_parameters, param+1);

	    for (cur2 = bot2; cur2 < top2; cur2++) {
		symb = XH_list_elem (rcg_parameters, cur2);

		if (symb < 0) {
		    /* terminal */
		    break;
		}
	    }

	    if (cur2 >= top2) {
		/* param ne contient que des variables */
		/* On va examiner la RHS */
		for (cur2 = bot2; cur2 < top2; cur2++) {
		    symb = XH_list_elem (rcg_parameters, cur2);
		    SXBA_1_bit (lhs_var_set, symb);
		}
	    }
	}
    }
}


static void
fill_var_stacks (SXINT clause)
{
    /* Pour chaque variable X de la partie droite on empile dans var_stacks_disp [X] le couple
       (Ak, param) ou param est l'identifiant du kieme argument du predicat A, ssi X est une
       variable de cet argument */
    SXINT		rhs, pos, bot, cur, top, bot2, cur2, top2, bot3, cur3, top3, prdct, nt, Ak, param, symb;
    SXINT		*X_stack;

    rhs = XxY_Y (rcg_clauses, clause);
    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);

    for (cur = bot; cur < top; cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);
	top2 = XH_X (rcg_predicates, prdct+1);
	nt = XH_list_elem (rcg_predicates, bot2+1);

	for (pos = 0, cur2 = bot2+2; cur2 < top2; pos++, cur2++) {
	    if (nt > 0 &&
		XH_list_elem (rcg_predicates, bot2) == 0 &&
		SXBA_bit_is_set (is_lhs_nt, nt)) {
		Ak = A_k2Ak (nt, pos);

#if 0
		if (nt > 0)
		    Ak = A_k2Ak (nt, pos);
		else
		    Ak = -pos;
#endif
		param = XH_list_elem (rcg_predicates, cur2);

		if (param != 0 && (pos > 0 || nt != STRLEN_ic && nt != FIRST_ic && nt != LAST_ic && nt != LEX_ic)) {
		    bot3 = XH_X (rcg_parameters, param);
		    top3 = XH_X (rcg_parameters, param+1);

		    for (cur3 = bot3; cur3 < top3; cur3++) {
			symb = XH_list_elem (rcg_parameters, cur3);

			if (symb > 0) {
			    SXBA_1_bit (rhs_var_set, symb);
			    X_stack = var_stacks_disp [symb];
			    PUSH (X_stack, Ak);
			    PUSH (X_stack, param);
			}
		    }
		}
	    }
	}
    }
}

static void
clear_var_stacks (void)
{
    SXINT var = -1;

    while ((var = sxba_scan_reset (rhs_var_set, var)) >= 0)
	var_stacks_disp [var][0] = 0;
}
#endif /* 0 */


static void
print_nt (SXINT A)
{
    /* Il faudrait etre + fin */
    fprintf (listing, "%s", sxstrget (nt2ste [A]));
}

static void
print_t (SXINT t)
{
  /* Il faudrait etre + fin */
  char *str;

  if (t > max_ste)
    fprintf (listing, "%ld", t-max_ste-1);
  else {
    str = sxstrget (t2ste [t]);

    if (*str == '"')
      str++;

    fprintf (listing, "%s", str);
  }
}

static void
print_Ak (SXINT Ak, SXBA t_set)
{
    SXINT A, k, t;

    A = Ak2A (Ak);
    k = Ak_A2k (Ak, A);

    print_nt (A);
    fprintf (listing, "[%ld] = ", (SXINT) k+1);

    if (SXBA_bit_is_set (t_set, 0))
	putc ('*', listing);

    putc ('{', listing);

    t = sxba_scan (t_set, 0);

    while (t > 0) {
	print_t (t);

	t = sxba_scan (t_set, t);

	if (t > 0)
	    fputs (", ", listing);
    }

    fputs ("}\n", listing);
}

static void
print_first_last (void)
{
    SXINT Ak;

    fputs ("\n**** first ****\n", listing);

    for (Ak = 1; Ak <= max_Ak; Ak++) {
	print_Ak (Ak, first [Ak]);
    }

    fputs ("\n**** last ****\n", listing);

    for (Ak = 1; Ak <= max_Ak; Ak++) {
	print_Ak (Ak, last [Ak]);
    }
}

SXINT clause2lhs_nt (SXINT clause2lhs_nt_clause)
{
    SXINT clause2lhs_nt_lhs_prdct;

    clause2lhs_nt_lhs_prdct = XxY_X (rcg_clauses, clause2lhs_nt_clause) & lhs_prdct_filter;
    return XH_list_elem (rcg_predicates, XH_X (rcg_predicates, clause2lhs_nt_lhs_prdct)+1);
}
		
#if 0
static void
first_last_construction (void)
{
  /* Il serait + fin (mais + cher) de correler a` la fois first et last et l'ensemble des parametres
     d'un nt. Ex si A(X,Y,Z) est un appel d'un predicat en RHS.  On a calcule' first-last[A] comme
     etant un ensemble de triplets de couples {(a1f,a1l),(a2f,a2l),(a3f,a3l) | \exists \rho, w^\rho =
     (a1f x a1l, a2f y a2l, a3f z a3l)} */

  SXINT 	clause, rhs, A, k, Ak, Bh, Cl, elem, arity, bot, cur, top, pos;
  SXBA	cur_t_set, w_t_set, bvect;

  XxY_foreach (rcg_clauses, clause) {
    if (clause2identical [clause] == clause && !SXBA_bit_is_set (false_clause_set, clause)) {
      if (nt2arity [clause2lhs_nt (clause)]) {
	if (SXBA_bit_is_set (empty_clause_set, clause))
	  first_last_complete (clause);
	else {
	  fill_lhs_var_set (clause);
	  fill_var_stacks (clause);
	  rhs = XxY_Y (rcg_clauses, clause);

	  if (!first_last_rhs_walk (clause, XH_X (rcg_rhs, rhs), XH_X (rcg_rhs, rhs+1))) {
	    if (SXBA_bit_is_reset_set (false_clause_set, clause))
	      PUSH (new_false_clause_stack, clause);
      
	    if (check_instantiable_clause)
	      sxerror (clause2visited [clause]->token.source_index,
		       sxtab_ptr->err_titles [2][0],
		       "%sNon instantiable clause.",
		       sxtab_ptr->err_titles [2]+1);
	  }

	  sxba_empty (lhs_var_set);
	  clear_var_stacks ();
	}
      }
    }
  }

  w_t_set = sxba_calloc (max_t+1);

  while (!IS_EMPTY (first_Ak_stack)) {
    Ak = POP (first_Ak_stack);
    SXBA_0_bit (first_Ak_set, Ak);
    cur_t_set = first [Ak];

    XxY_Xforeach (first_rel, Ak, elem) {
      Bh = XxY_Y (first_rel, elem);

      if (Bh > 0) {
	if (OR (first [Bh], cur_t_set)) {
	  /* first [Bh] a ete modifie, on continue */
	  if (SXBA_bit_is_reset_set (first_Ak_set, Bh))
	    PUSH (first_Ak_stack, Bh);
	}
      }
      else {
	/* Element de first_Ak_list */
	bot = XH_X (first_Ak_list, -Bh);
	top = XH_X (first_Ak_list, -Bh+1);

	Bh = XH_list_elem (first_Ak_list, top-1);
	sxba_copy (w_t_set, cur_t_set);

	for (cur = top-2; bot <= cur; cur--) {
	  Cl = XH_list_elem (first_Ak_list, cur);

	  if (Cl != Ak)
	    sxba_and (w_t_set, first [Cl]);
	}

	if (OR (first [Bh], w_t_set)) {
	  /* first [Bh] a ete modifie, on continue */
	  if (SXBA_bit_is_reset_set (first_Ak_set, Bh))
	    PUSH (first_Ak_stack, Bh);
	}
      }
    }
  }

  while (!IS_EMPTY (last_Ak_stack)) {
    Ak = POP (last_Ak_stack);
    SXBA_0_bit (last_Ak_set, Ak);
    cur_t_set = last [Ak];

    XxY_Xforeach (last_rel, Ak, elem) {
      Bh = XxY_Y (last_rel, elem);

      if (Bh > 0) {
	if (OR (last [Bh], cur_t_set)) {
	  /* last [Bh] a ete modifie, on continue */
	  if (SXBA_bit_is_reset_set (last_Ak_set, Bh))
	    PUSH (last_Ak_stack, Bh);
	}
      }
      else {
	/* Element de last_Ak_list */
	bot = XH_X (last_Ak_list, -Bh);
	top = XH_X (last_Ak_list, -Bh+1);

	Bh = XH_list_elem (last_Ak_list, top-1);
	sxba_copy (w_t_set, cur_t_set);

	for (cur = top-2; bot <= cur; cur--) {
	  Cl = XH_list_elem (last_Ak_list, cur);

	  if (Cl != Ak)
	    sxba_and (w_t_set, last [Cl]);
	}

	if (OR (last [Bh], w_t_set)) {
	  /* last [Bh] a ete modifie, on continue */
	  if (SXBA_bit_is_reset_set (last_Ak_set, Bh))
	    PUSH (last_Ak_stack, Bh);
	}
      }
    }
  }
    
  for (A = 1; A <= max_nt; A++) {
    if (arity = nt2arity [A]) {
      bvect = nt2empty_vector [A];
      pos = -1;
      
      while ((pos = sxba_0_lrscan (bvect, pos)) >= 0 && pos < arity) {
	/* arg peut deriver ds le vide */
	Ak = A_k2Ak (A, pos);
	SXBA_1_bit (first [Ak], 0); /* epsilon */
	SXBA_1_bit (last [Ak], 0); /* epsilon */
      }
    }
  }

#if 0
  /* Le 19/12/03 */
  if (is_empty_arg) {
    for (A = 1; A <= max_nt; A++) {
      if (arity = nt2arity [A]) {
	bvect = nt2empty_vector [A];
	pos = -1;
      
	while ((pos = sxba_0_lrscan (bvect, pos)) >= 0 && pos < arity) {
	  /* arg peut deriver ds le vide */
	  Ak = A_k2Ak (A, pos);
	  SXBA_1_bit (first [Ak], 0); /* epsilon */
	  SXBA_1_bit (last [Ak], 0); /* epsilon */
	}
      }
    }
  }
#endif /* 0 */

  sxfree (w_t_set);

  /* Si une ligne de first ou last est vide => la grammaire n'est pas reduite Ex:
     S(X) --> A(X) B(X) .
     A(\a) --> .
     B(\b) --> .
  */

  for (Ak = 1; Ak <= max_Ak; Ak++) {
    if (sxba_is_empty (first [Ak]) || sxba_is_empty (last [Ak])) {
      A = Ak2A (Ak);

      if (SXBA_bit_is_set (is_lhs_nt, A) && (nt2identical [A] == A)) {
	/* Seuls les predicats definis localement sont concernes */
	k = Ak_A2k (Ak, A);

	sxtmsg (sxsrcmngr.source_coord.file_name,
		"%sThe %ld%s parameter of predicate <%s> is non instantiable.",
		sxtab_ptr->err_titles [2]+1,
		k+1,
		k==0?"st":(k==1?"nd":(k==2?"rd":"th")),
		sxstrget (nt2ste [A]));
      }
    }
  }
}
#endif /* 0 */


#include "SS.h"

static struct Ak_memo {
    SXINT	min, max;
} memo_val, *clausek2memo, *local_memo;
static XxY_header	AknoteqAk;
static SXINT		*clause2iargs, *infix_args;
static struct iargs2arg_disp {
    SXINT arg, disp;
}			*iargs2arg_disp;
static struct arg_memo {
    SXINT min, max, disp;
}			*arg_memo;
static struct iarg_struct {
    SXINT		lgth;
    SXBA	set;
} iarg1_struct, iarg2_struct;
static struct can_struct {
    SXINT		lgth;
    SXBA	set;
    SXINT         *nb;
} can1_struct, can2_struct;
static SXBA		A_set, iarg_rel_set, Ak_set, noteq_nt_set, noteq_clause_set;
static SXINT		*A_stack, *param_stack, *memel_SS_stack, *clause2clausek;
static SXINT		infix_args_top, infix_args_size, iargs2arg_disp_top, iargs2arg_disp_size, arg_memo_top, arg_memo_size,
                        max_iarg_rel;
#define TOP_DOWN_ERASED	(1U<<31)



static bool
set_iargs2arg_disp (SXINT set_iargs2arg_disp_clause, SXINT param, SXINT *iarg)
{
    /* Retourne le iarg ou est range param */
    SXINT x;

    for (x = clause2iargs [set_iargs2arg_disp_clause]; x < iargs2arg_disp_top; x++) {
	if (iargs2arg_disp [x].arg == param) {
	    *iarg = x;
	    return true;
	}
    }

    iargs2arg_disp [*iarg = x].arg = param;
    iargs2arg_disp_top++;

    return false;
}


static void
fill_infix_args (SXINT fill_infix_args_clause, SXINT lhs_cur3, SXINT lhs_top3, SXINT rhs)
{
    /* On met ds infix_args les iargs qui correspondend a la reconnaissance de la position lhs_cur3 */
    SXINT		symb, cur, top, bot, prdct, bot2, top2, nt, cur2, param, bot3, top3, lcur3, cur3, x;
    bool	is_lgth_1 = false;

    symb = XH_list_elem (rcg_parameters, lhs_cur3);

    /* On doit pouvoir traiter des trucs comme
       A(XYZ) --> B(XY) C(YZ). et
       ...--> ... &A(XaY) ... . (min(|XaY|) = 1 bien que A soit externe */
    /* Ce qui suit permet de sauter (de facon non-deterministe) au symbole suivant ds tous les cas
       avec un decompte minimal */
    if (symb < 0) {
	is_lgth_1 = true;
	infix_args [infix_args_top] = -1;
	infix_args_top++;

	if (infix_args_top >= infix_args_size)
	    infix_args = (SXINT*) sxrealloc (infix_args, (infix_args_size *= 2)+1, sizeof (SXINT));
    }

    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);

    param_stack [0] = 0;
		    
    for (cur = bot; cur < top; cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);

	if (XH_list_elem (rcg_predicates, bot2) == 0) {
	    /* appel positif */
	    nt = XH_list_elem (rcg_predicates, bot2+1);

	    if ((nt > 0  && SXBA_bit_is_set (is_lhs_nt, nt)) || nt == STRLEN_ic || nt == STREQ_ic || nt == STREQLEN_ic) {
		top2 = XH_X (rcg_predicates, prdct+1);

		for (cur2 = bot2+2; cur2 < top2; cur2++) {
		    if (cur2 > bot2+2 || nt != STRLEN_ic) {
			param = XH_list_elem (rcg_predicates, cur2);
			/* lhs_cur3, lhs_top3 sont ds arg
			   On ne cherche pas a l'interieur d'un m arg */
			for (x = param_stack [0]; x > 0; x--) {
			    if (param_stack [x] == param)
				break;
			}

			if (x == 0) {
			    bot3 = XH_X (rcg_parameters, param);
			    top3 = XH_X (rcg_parameters, param+1);

			    for (lcur3 = lhs_cur3, cur3 = bot3; cur3 < top3 && lcur3 < lhs_top3; lcur3++, cur3++) {
				if (XH_list_elem (rcg_parameters, lcur3) != XH_list_elem (rcg_parameters, cur3))
				    break;
			    }

			    if (cur3 == top3) {
				/* trouve' */
#if 0
			      /* Supprime' le 03/02/2004 car a priori c,a ne correspond pas a du TOP_DOWN_ERASED!! */
				if (top3-bot3 == 1)
				    is_lgth_1 = true;
#endif /* 0 */
				is_lgth_1 = true;

				set_iargs2arg_disp (fill_infix_args_clause, param, &(infix_args [infix_args_top]));
				infix_args_top++;

				if (infix_args_top >= infix_args_size)
				    infix_args = (SXINT*) sxrealloc (infix_args, (infix_args_size *= 2)+1, sizeof (SXINT));

				/* Si on retombe sur "param", ne pas le remettre */
				PUSH (param_stack, param);
			    }
			}
		    }
		}
	    }
	}
    }

    if (!is_lgth_1) {
	infix_args [infix_args_top] = TOP_DOWN_ERASED;
	infix_args_top++;

	if (infix_args_top >= infix_args_size)
	    infix_args = (SXINT*) sxrealloc (infix_args, (infix_args_size *= 2)+1, sizeof (SXINT));
    }
}



static SXINT
walk_memel (SXINT iarg_bot, SXINT iarg, bool with_Ak)
{
    SXINT pair, iarg1, iarg2, iarg_rel;

    iarg_rel = iarg-iarg_bot;

    if (SXBA_bit_is_reset_set (iarg_rel_set, iarg_rel)) {
	XxY_Yforeach (memel, iarg, pair) {
	    iarg1 = XxY_X (memel, pair);

	    if (iarg1 > 0)
		iarg1 = walk_memel (iarg_bot, iarg1, with_Ak);

	    if (iarg1 <= 0)
		/* C'est une longueur */
		/* Elle a priorite */
		return iarg1;
	}

	XxY_Xforeach (memel, iarg, pair) {
	    iarg2 = XxY_Y (memel, pair);

	    if (iarg2 < 0 && with_Ak) {
		/* C'est un Ak */
		iarg2 = -iarg2;

		if (SXBA_bit_is_reset_set (Ak_set, iarg2))
		    PUSH (Ak_stack, iarg2);
	    }
	    else {
		if (iarg2 > 0) {
		    iarg2 = walk_memel (iarg_bot, iarg2, with_Ak);

		    if (iarg2 <= 0)
			/* C'est une longueur */
			return iarg2;
		}
	    }
	}
    }

    return 1;
}

/* retourne l'oppose de la longueur (unique) de la classe d'equivalence iarg si elle existe sinon -1 */
static SXINT
get_memel_lgth (SXINT iarg_bot, SXINT iarg)
{
  SXINT pair, iarg1, iarg2, iarg_rel;

  iarg_rel = iarg-iarg_bot;

  if (SXBA_bit_is_reset_set (iarg_rel_set, iarg_rel)) {
    XxY_Yforeach (memel, iarg, pair) {
      iarg1 = XxY_X (memel, pair);

      if (iarg1 > 0)
	iarg1 = get_memel_lgth (iarg_bot, iarg1);

      if (iarg1 <= 0)
	/* C'est une longueur */
	/* Elle a priorite */
	return iarg1;
    }

    XxY_Xforeach (memel, iarg, pair) {
      iarg2 = XxY_Y (memel, pair);

      if (iarg2 > 0) {
	/* Ce n'est pas un Ak */
	iarg2 = get_memel_lgth (iarg_bot, iarg2);

	if (iarg2 <= 0)
	  /* C'est une longueur */
	  return iarg2;
      }
    }
  }

  return 1;
}

static void
update_memel (SXINT update_memel_clause, SXINT rhs)
{
  /* On complete memel avec tous les couples NOUVEAUX (iargh, iargk) tq
     XxY_is_set (&AknoteqAk, Bh, Bk) == 0 si B(..., h, ..., k, ...)
     B est un appel positif et B>0 */
  /* Le 09/04/44 (et oui) il ne faut pas dire que iarg1 et iarg2 ont meme longueur si les classes
     iarg1 et iarg2 ont des longueurs calculees differentes */
  SXINT		top, cur, bot, prdct, bot2, nt, arity, cur2, cur3, top2, arg1, A1, A2, iarg1, arg2, iarg2, pair, base;
  SXINT         iarg1_lgth, iarg2_lgth;
  bool	is_old;

  if (rhs == 0) return;

  base = clause2iargs [update_memel_clause];

  top = XH_X (rcg_rhs, rhs+1);
  bot = XH_X (rcg_rhs, rhs);

  for (cur = bot; cur < top; cur++) {
    prdct = XH_list_elem (rcg_rhs, cur);
    bot2 = XH_X (rcg_predicates, prdct);

    if (XH_list_elem (rcg_predicates, bot2) == 0) {
      /* appel positif */
      nt = XH_list_elem (rcg_predicates, bot2+1);

      if (nt > 0 && SXBA_bit_is_set (is_lhs_nt, nt)) {
	arity = nt2arity [nt];

	if (arity >= 2) {
	  top2 = XH_X (rcg_predicates, prdct+1)-1;

	  for (A1 = A_k2Ak (nt, 0), cur2 = bot2+2; cur2 < top2; A1++, cur2++) {
	    arg1 = 0;

	    for (A2 = A1+1, cur3 = cur2+1; cur3 <= top2; A2++, cur3++) {
	      if (XxY_is_set (&AknoteqAk, A1, A2) == 0) {
		if (arg1 == 0) {
		  arg1 = XH_list_elem (rcg_predicates, cur2);
		  set_iargs2arg_disp (update_memel_clause, arg1, &iarg1);
		}

		arg2 = XH_list_elem (rcg_predicates, cur3);
		set_iargs2arg_disp (update_memel_clause, arg2, &iarg2);

		if (iarg1 != iarg2) {
		  iarg1_lgth = -get_memel_lgth (base, iarg1);
		  sxba_empty (iarg_rel_set);

		  iarg2_lgth = -get_memel_lgth (base, iarg2);
		  sxba_empty (iarg_rel_set);
				    
		  if (iarg1_lgth == -1 || iarg2_lgth == -1 || iarg1_lgth == iarg2_lgth) {
		    if (iarg1 < iarg2)
		      is_old = XxY_set (&memel, iarg1, iarg2, &pair);
		    else
		      is_old = XxY_set (&memel, iarg2, iarg1, &pair);

		    if (!is_old)
		      SS_push (memel_SS_stack, pair);

		    /* petit debug */
#if EBUG_PRDCT
		    if (nt == EBUG_PRDCT) {
		      fprintf (sxtty, "%s-clause(%ld) [%ld == %ld]***%s\n",
			       sxstrget (nt2ste [nt]),
			       update_memel_clause,
			       Ak_A2k (A1,nt),
			       Ak_A2k (A2,nt),
			       is_old ? "old" : "new");
		    }
#endif /* EBUG_PRDCT */
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}




static void
canonical_form (SXINT canonical_form_clause, struct iarg_struct *iarg_struct, struct can_struct *can_struct)
{
    SXINT base, loc_iarg, iarg;

    can_struct->lgth = iarg_struct->lgth;

    sxba_empty (can_struct->set);

    base = clause2iargs [canonical_form_clause];
    loc_iarg = -1;

    while ((loc_iarg = sxba_scan (iarg_struct->set, loc_iarg)) >= 0) {
	iarg = loc_iarg+base;

	if ((iarg = walk_memel (base, iarg, false)) <= 0)
	    can_struct->lgth -= iarg;
	else {
	    /* On choisit comme representant de la classe d'equivalence, le + petit iarg */
	    iarg = sxba_scan (iarg_rel_set, -1);

	    /* Le 03/02/2004 la structure can_struct devient une iarg_struct + un tableau d'entiers nb
	       qui compte pour chaque classe d'equivalence son nombre d'occurrences ds l'argument */
#if 0
	    SXBA_1_bit (can_struct->set, iarg);
#endif /* 0 */

	    if (SXBA_bit_is_reset_set (can_struct->set, iarg))
	      can_struct->nb [iarg] = 1;
	    else
	      can_struct->nb [iarg]++;
	}

	sxba_empty (iarg_rel_set);
	sxba_empty (Ak_set);
	Ak_stack [0] = 0;
    }
}

static bool
fill_iarg2_struct (SXINT fill_iarg2struct_clause, struct arg_memo *bot, struct arg_memo *top)
{
    SXINT			iarg, x, arg, next;
    SXINT			*bot2, *top2;
    bool             ret_val;

    if (bot == top) {
	canonical_form (fill_iarg2struct_clause, &iarg2_struct, &can2_struct);

	/* On compare can1_struct et can2_struct */
	if (can2_struct.lgth != can1_struct.lgth)
	    return false;

#if 0
	/* Le 03/02/2004 */
	return sxba_first_difference (can1_struct.set, can2_struct.set) == -1;
#endif /* 0 */

	if (sxba_first_difference (can1_struct.set, can2_struct.set) >= 0)
	  return false;

	iarg = -1;

	while ((iarg = sxba_scan (can1_struct.set, iarg)) >= 0) {
	  if (can1_struct.nb [iarg] != can2_struct.nb [iarg])
	    return false;
	}

	return true;
    }

    /* Traitement du symbole courant */
    top2 = infix_args + bot [1].disp;
    bot2 = infix_args + bot->disp;

    while (bot2 < top2) {
	iarg = *bot2;

	if (iarg <= 0) {
	    if ((SXUINT)iarg != TOP_DOWN_ERASED) {
		iarg2_struct.lgth -= iarg;

		ret_val = fill_iarg2_struct (fill_iarg2struct_clause, bot+1, top);

		iarg2_struct.lgth += iarg;

		if (ret_val) return true;
	    }
	}
	else {
	    x = iarg - clause2iargs [fill_iarg2struct_clause];
	    SXBA_1_bit (iarg2_struct.set, x);
	    arg = iargs2arg_disp [iarg].arg;
	    next = XH_X (rcg_parameters, arg+1)-XH_X (rcg_parameters, arg);

	    ret_val = fill_iarg2_struct (fill_iarg2struct_clause, bot+next, top);

	    SXBA_0_bit (iarg2_struct.set, x);

	    if (ret_val) return true;
	}
	
	bot2++;
    }

    return false;
}


static bool
fill_iarg1_struct (SXINT fill_iarg1struct_clause, struct arg_memo *bot, struct arg_memo *top, SXINT iarg2)
{
    SXINT arg2, iarg, x, arg, next;
    SXINT	*bot2, *top2;
    struct arg_memo	*pbot, *ptop;
    bool             ret_val;

    if (bot == top) {
	canonical_form (fill_iarg1struct_clause, &iarg1_struct, &can1_struct);

	arg2 = iargs2arg_disp [iarg2].arg;
	pbot = arg_memo + iargs2arg_disp [iarg2].disp;
	ptop = pbot+((arg2 == 0) ? 0 : XH_X (rcg_parameters, arg2+1)-XH_X (rcg_parameters, arg2));

	return fill_iarg2_struct (fill_iarg1struct_clause, pbot, ptop);
    }

    /* Traitement du symbole courant */
    top2 = infix_args + bot [1].disp;
    bot2 = infix_args + bot->disp;

    while (bot2 < top2) {
	iarg = *bot2;

	if (iarg <= 0) {
	    if ((SXUINT)iarg != TOP_DOWN_ERASED) {
		iarg1_struct.lgth -= iarg;

		ret_val = fill_iarg1_struct (fill_iarg1struct_clause, bot+1, top, iarg2);

		iarg1_struct.lgth += iarg;

		if (ret_val) return true;
	    }
	}
	else {
	    x = iarg - clause2iargs [fill_iarg1struct_clause];
	    SXBA_1_bit (iarg1_struct.set, x);
	    arg = iargs2arg_disp [iarg].arg;
	    next = XH_X (rcg_parameters, arg+1)-XH_X (rcg_parameters, arg);

	    ret_val = fill_iarg1_struct (fill_iarg1struct_clause, bot+next, top, iarg2);

	    SXBA_0_bit (iarg1_struct.set, x);

	    if (ret_val) return true;
	}
	
	bot2++;
    }

    return false;
}

static bool
is_iarg_equality (SXINT is_iarg_equality_clause, SXINT iarg1, SXINT iarg2)
{
    SXINT arg1;
    struct arg_memo	*pbot, *ptop;

    arg1 = iargs2arg_disp [iarg1].arg;
    pbot = arg_memo + iargs2arg_disp [iarg1].disp;
    ptop = pbot+((arg1 == 0) ? 0 : XH_X (rcg_parameters, arg1+1)-XH_X (rcg_parameters, arg1));

    return fill_iarg1_struct (is_iarg_equality_clause, pbot, ptop, iarg2);
}



/* Nelle version le 04/02/2004 */
static struct Ak_memo
eval_arg (SXINT eval_arg_clause, struct arg_memo *bot, struct arg_memo *top)
{
  /* La valeur retournee est le max des min!! */
  SXINT			Ak, next, iarg, min, lgth, x; 
  SXINT			*top2, *bot2;
  struct Ak_memo        return_val, suffix_val, local_val;	

  if (bot == top) {
    /* Tous les symboles ont ete examines */
    /* On initialise le "compte a rebours" */
    return_val.max = return_val.min = -2;

    return return_val;
  }

  min = bot->min;

  if (min == -3) {
    /* boucle => calcule' et inconnu */
    return_val.min = bot->min = 0;
    return_val.max = bot->max = -1; /* unknown! */

    return return_val;
  }

  if (min != -2) {
    /* Deja calcule */
    return_val.min = min;
    return_val.max = bot->max;
	
    return return_val;
  }

  bot->min = -3; /* pour les boucles */

  /* Traitement du symbole courant */
  top2 = infix_args + bot [1].disp;
  bot2 = infix_args + bot->disp;

  return_val.min = return_val.max = -2;

  while (bot2 < top2) {
    iarg = *bot2;

    /* On calcule local_val ... */
    if (iarg <= 0) {
      if ((SXUINT)iarg == TOP_DOWN_ERASED)
	/* top-down erasing */
	local_val.min = local_val.max = -1;
      else
	local_val.min = local_val.max = -iarg;
    }
    else {
      /* A FAIRE : On pourrait detecter des clauses inutiles :
	 ... -> len(1,X) len (2,Y) A(X,Y) .
	 Si de plus on a A0==A1 */
      if ((lgth = walk_memel (clause2iargs [eval_arg_clause], iarg, true)) <= 0) {
	local_val.min = local_val.max = -lgth;
      }
      else {
	local_val.max = -1;
	local_val.min = -1;
		
	for (x = Ak_stack [0]; x > 0; x--) {
	  Ak = Ak_stack [x];

	  if (Ak2min [Ak] >= 0) {
	    if (local_val.min == -1 || Ak2min [Ak] > local_val.min)
	      /* max des min */
	      local_val.min = Ak2min [Ak];
	  }

	  if (Ak2max [Ak] >= 0) {
	    if (local_val.max == -1 || Ak2max [Ak] < local_val.max)
	      /* min des max */
	      local_val.max = Ak2max [Ak];
	  }
	}
      }

      sxba_empty (iarg_rel_set);
      sxba_empty (Ak_set);
      Ak_stack [0] = 0;
    }

    /* On a calcule' local ... */
    /* ... on calcule le suffixe */
    if (iarg <= 0)
      next = 1;
    else {
      SXINT arg = iargs2arg_disp [iarg].arg;
      next = XH_X (rcg_parameters, arg+1)-XH_X (rcg_parameters, arg);
    }

    suffix_val = eval_arg (eval_arg_clause, bot+next, top);

    /* ... et on merge suffix_val ds local_val ... */

    if (suffix_val.min != -2) {
      /* Y'a du suffixe */
      if (local_val.min == -1) {
	/* taille min locale inconnue, la taille min totale est donc au moins celle du suffixe */
	local_val.min = suffix_val.min;
      }
      else {
	/* taille min locale connue, la taille min totale est donc au moins celle locale */
	if (suffix_val.min != -1)
	  local_val.min += suffix_val.min;
      }
    }
    /* else initialisation => aucune composition avec le suffixe */

    if (suffix_val.max != -2) {
      /* Y'a du suffixe */
      if (local_val.max != -1) {
	/* taille max locale connue */
	if (suffix_val.max == -1)
	  /* taille max du suffixe inconnue => taille totale max inconnue */
	  local_val.max = -1;
	else
	  /* taille max du suffixe connue => taille totale max est la somme des 2 */
	  local_val.max += suffix_val.max;
      }
      /* else taille max locale inconnue, la taille max totale reste inconnue */
    }
    /* else initialisation => aucune composition avec le suffixe */

    /* ... et local_val dans return_val */
    if (return_val.min == -2)
      return_val = local_val;
    else {
      /* vraie composition ... */
      /* ... on calcule le max des min ... */
      if (return_val.min == -1)
	return_val.min = local_val.min;
      else {
	if (local_val.min != -1) {
	  if (local_val.min > return_val.min)
	    return_val.min = local_val.min;
	}
      }
    }

    if (return_val.max == -2)
      return_val = local_val;
    else {
      /* vraie composition ... */
      /* ... et le min des max */
      if (return_val.max == -1)
	return_val.max = local_val.max;
      else {
	if (local_val.max != -1) {
	  if (local_val.max < return_val.max)
	    return_val.max = local_val.max;
	}
      }
    }

    bot2++;
  }

  bot->min = return_val.min;
  bot->max = return_val.max;

  return return_val;
}


#if 0
static void
eval_arg (clause, bot, top)
    SXINT			clause;
    struct arg_memo	*bot, *top;
{
  /* La valeur retournee est le max des min!! */
  SXINT			max, Ak, next, iarg, min, local_min, local_max;
  SXINT			x /*, len, local_len */; 
  SXINT			*top2, *bot2;

  if (bot == top) {
    /* Tous les symboles ont ete examines */
    /* On initialise le "compte a rebours" */
    memo_val.max = memo_val.min = 0;

    return;
  }

  if ((min = bot->min) >= 0 && bot->max >= 0) {
    /* Deja calcule */
    memo_val.min = min;
    memo_val.max = bot->max;
	
    return;
  }

  if (min == -2) {
    /* boucle */
    memo_val.min = bot->min = 0;
    memo_val.max = bot->max = -1; /* unknown! */

    return;
  }

  bot->min = -2; /* pour les boucles */

  /* Traitement du symbole courant */
  top2 = infix_args + bot [1].disp;
  bot2 = infix_args + bot->disp;
  max = -1;
  /* len = -1; */
  min = -1;

  while (bot2 < top2) {
    iarg = *bot2;

    if (iarg <= 0)
      next = 1;
    else {
      SXINT arg = iargs2arg_disp [iarg].arg;
      next = XH_X (rcg_parameters, arg+1)-XH_X (rcg_parameters, arg);
    }

    eval_arg (clause, bot+next, top);

    if (iarg <= 0) {
      if (iarg == TOP_DOWN_ERASED)
	/* top-down erasing */
	local_min = local_max /* = local_len  */= -1;
      else
	local_min = local_max /* = local_len */= -iarg;
    }
    else {
      /* A FAIRE : On pourrait detecter des clauses inutiles :
	 ... -> len(1,X) len (2,Y) A(X,Y) .
	 Si de plus on a A0==A1 */
      if ((iarg = walk_memel (clause2iargs [clause], iarg, true)) <= 0) {
	local_min = /* local_len = */local_max = -iarg;
      }
      else {
	local_max = -1;
	/* local_len = -1; */
	local_min = -1;
		
	for (x = Ak_stack [0]; x > 0; x--) {
	  Ak = Ak_stack [x];

	  if (Ak2min [Ak] >= 0) {
	    if (local_min == -1 || Ak2min [Ak] > local_min)
	      /* max des min */
	      local_min = Ak2min [Ak];
	  }

	  if (Ak2max [Ak] >= 0) {
	    if (local_max == -1 || Ak2max [Ak] < local_max)
	      /* min des max */
	      local_max = Ak2max [Ak];
	  }
	}
      }

      sxba_empty (iarg_rel_set);
      sxba_empty (Ak_set);
      Ak_stack [0] = 0;
    }

    if (local_min >= 0 && (min == -1 || local_min > min))
      /* max des min */
      min = local_min;

    if (local_max >= 0 && (max == -1 || local_max < max))
      /* min des max */
      max = local_max;

    bot2++;
  }

  if (memo_val.min >= 0) {
    if (min >= 0)
      bot->min = memo_val.min += min;
    else
      bot->min = memo_val.min = -1;
  }

  if (memo_val.max >= 0) {
    if (max >= 0)
      bot->max = memo_val.max += max;
    else
      bot->max = memo_val.max = -1;
  }

  return;
}
#endif /* 0 */



static void
clear_arg_memo (SXINT clear_arg_memo_clause)
{
    struct arg_memo	*memo_bot, *memo_top;

    memo_bot = arg_memo+iargs2arg_disp [clause2iargs [clear_arg_memo_clause]].disp;
    memo_top = arg_memo+iargs2arg_disp [clause2iargs [clear_arg_memo_clause+1]].disp;

    while (memo_bot <= --memo_top) {
	memo_top->min = memo_top->max = -2;
    }
}


/* indique pour chaque nt la liste des clauses ds lesquelles intervient nt en PARTIE DROITE */
/* On en profite pour calculer lhsnt2clause */
/* On ne recense que les appels positifs, prudence */
static void
nt2clause_construction (void)
{
  SXINT max_nt2clause, nt2clause_construction_clause, rhs, bot, top, cur, prdct, bot2, nt, nt2clause_construction_lhs_prdct;

  lhsnt2clause = sxbm_calloc (max_nt+1, last_clause+1);
  nt2clause_disp = (SXINT*) sxcalloc (max_nt+2, sizeof (SXINT));

  for (nt2clause_construction_clause = 1; nt2clause_construction_clause <= last_clause; nt2clause_construction_clause++) {
    nt2clause_construction_lhs_prdct = XxY_X (rcg_clauses, nt2clause_construction_clause) & lhs_prdct_filter;
    nt = XH_list_elem (rcg_predicates, XH_X (rcg_predicates, nt2clause_construction_lhs_prdct)+1);
    SXBA_1_bit (lhsnt2clause [nt], nt2clause_construction_clause);	    

    rhs = XxY_Y (rcg_clauses, nt2clause_construction_clause);

    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
		    
    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);

      if (XH_list_elem (rcg_predicates, bot2) == 0) {
	/* appel positif !! */
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt > 0) {
	  nt2clause_disp [nt]++;
	}
      }
    }
  }

  max_nt2clause = 1;

  for (nt = 1; nt <= max_nt; nt++) {
    max_nt2clause = nt2clause_disp [nt] += max_nt2clause;
  }

  nt2clause_disp [nt] = max_nt2clause;
  nt2clause = (SXINT*) sxalloc (max_nt2clause+1, sizeof (SXINT));
  
  for (nt2clause_construction_clause = 1; nt2clause_construction_clause <= last_clause; nt2clause_construction_clause++) {	    
    rhs = XxY_Y (rcg_clauses, nt2clause_construction_clause);

    top = XH_X (rcg_rhs, rhs+1);
    bot = XH_X (rcg_rhs, rhs);
		    
    for (cur = bot; cur < top; cur++) {
      prdct = XH_list_elem (rcg_rhs, cur);
      bot2 = XH_X (rcg_predicates, prdct);

      if (XH_list_elem (rcg_predicates, bot2) == 0) {
	/* appel positif */
	nt = XH_list_elem (rcg_predicates, bot2+1);

	if (nt > 0) {
	  nt2clause [--nt2clause_disp [nt]] = nt2clause_construction_clause;
	}
      }
    }
  }
}

/* petit debug */
#if EBUG_PRDCT
static void 
print_debug_prdct (SXINT Ai, SXINT Aj)
{
  SXINT A, i, j;

  A = Ak2A (Ai);

  if (A == EBUG_PRDCT) {
    i = Ak_A2k (Ai,A);
    j = Ak_A2k (Aj,A);

    fprintf (sxtty, "%s(%ld) [%ld != %ld]\n", sxstrget (nt2ste [A]), A, i, j);
  }
}
#endif /* EBUG_PRDCT */


/* On ne recense que les appels positifs, prudence */
static void
nt2clause_free (void)
{
  if (nt2clause_disp) sxfree (nt2clause_disp), nt2clause_disp = NULL;
  if (nt2clause) sxfree (nt2clause), nt2clause = NULL;
  if (lhsnt2clause) sxbm_free (lhsnt2clause), lhsnt2clause = NULL;
}


static void
Ak2min_construction (void)
{
  SXINT			x, y, max_pos, Ak2min_construction_clause, lhs_nt, Ak2min_construction_lhs_prdct, lhs_bot2, lhs_cur2, lhs_top2, lhs_arity, param, rhs, top, bot, cur;
  SXINT			prdct, bot2, cur2, top2, nt, iarg1, iarg2, pair, k, Ak, bot3, cur3, top3, iarg;
  SXINT			min, max, lhs_pos, A1, A2, son1, son2, arg1, arg2, A1_min, arity, Ak2min_construction_max_clausek, clausek;
  SXINT			local_min, local_max, xbot, nt2IeqJ_top, nt2IeqJ_size;
  SXINT			*ip;
  bool		is_initial, noteq_has_changed = false, has_min_changed;
  struct arg_memo	*pbot, *ptop;
  SXBA		        ntline, is_initial_set, prev_Ak_max_set, Ak_max_set;
  SXINT                   StrLen1;

  if (sxverbosep) {
    fputs ("\tMin/Max Computing .... ", sxtty);
    sxttycol1p = false;
  }

#if 0
  Ak2min = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT));
  Ak2max = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT));
  Ak2len = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT));
#else
  /* ESSAI le 05/02/04 j'essaie d'utiliser max_Ak+1 pour representer le 2eme arg de !&Strlen(0,...) */
  /* l'idee est de pouvoir utiliser le fait que si l'on a !&Strlen(0, X), |X| est au moins de longueur 1 */
  StrLen1 = max_Ak+1;
  Ak2min = (SXINT*) sxalloc (StrLen1+1, sizeof (SXINT));
  Ak2max = (SXINT*) sxalloc (StrLen1+1, sizeof (SXINT));
  Ak2len = (SXINT*) sxalloc (StrLen1+1, sizeof (SXINT));
  Ak2min [StrLen1] = 1; /* !&StrLen(0, ...) */
  Ak2max [StrLen1] = -1;
  Ak2len [StrLen1] = -1;
#endif /* 0 */

  for (x = max_Ak; x >= 0; x--) {
    Ak2min [x] = -1;
    Ak2max [x] = -1;
    Ak2len [x] = -1;
  }
    
  XxY_alloc (&AknoteqAk, "AknoteqAk", max_Ak+1, 1, 0, 0, NULL, NULL);

  clause2clausek = (SXINT*) sxalloc (last_clause+2, sizeof (SXINT));

  Ak2min_construction_max_clausek = 1;

  arg_memo_size = 1;
  max_iarg_rel = 0;
  iargs2arg_disp_size = 1;

  for (Ak2min_construction_clause = 1; Ak2min_construction_clause <= last_clause; Ak2min_construction_clause++) {
    if ((x = clause2identical [Ak2min_construction_clause]) == Ak2min_construction_clause && !SXBA_bit_is_set (false_clause_set, Ak2min_construction_clause)) {
      Ak2min_construction_lhs_prdct = XxY_X (rcg_clauses, Ak2min_construction_clause) & lhs_prdct_filter;
      lhs_bot2 = XH_X (rcg_predicates, Ak2min_construction_lhs_prdct);
      lhs_top2 = XH_X (rcg_predicates, Ak2min_construction_lhs_prdct+1);
      lhs_arity = nt2arity [XH_list_elem (rcg_predicates, lhs_bot2+1)];
      max_pos = lhs_arity;
      clause2clausek [Ak2min_construction_clause] =  Ak2min_construction_max_clausek;

      if (lhs_arity) {
	Ak2min_construction_max_clausek += lhs_arity;

	/* En lhs si un predicat est d'arite 0, il a un param qui vaut 0 */
	for (lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_cur2++) {
	  param = XH_list_elem (rcg_predicates, lhs_cur2);

	  if (param > 0) {
	    arg_memo_size += XH_X (rcg_parameters, param+1)-XH_X (rcg_parameters, param);
	  }

	  arg_memo_size++; /* prudence */
	}
      }
	    
      rhs = XxY_Y (rcg_clauses, Ak2min_construction_clause);

      top = XH_X (rcg_rhs, rhs+1);
      bot = XH_X (rcg_rhs, rhs);
		    
      for (cur = bot; cur < top; cur++) {
	prdct = XH_list_elem (rcg_rhs, cur);
	bot2 = XH_X (rcg_predicates, prdct);

	if (XH_list_elem (rcg_predicates, bot2) == 0) {
	  /* appel positif */
	  nt = XH_list_elem (rcg_predicates, bot2+1);

	  if ((nt > 0 && SXBA_bit_is_set (is_lhs_nt, nt)) || nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic) {
	    top2 = XH_X (rcg_predicates, prdct+1);

	    for (k = 0, cur2 = bot2+2; cur2 < top2; k++, cur2++) {
	      if (nt != STRLEN_ic || cur2 > bot2+2) {
		max_pos++;
	      }
	    }
	  }
	}
      }

      if (max_pos > max_iarg_rel)
	max_iarg_rel = max_pos;

      iargs2arg_disp_size += max_pos;
    }
    else {
      /* Le 16/07/2003 Prudence */
      if (x != Ak2min_construction_clause)
	clause2clausek [Ak2min_construction_clause] = clause2clausek [x];
      else
	clause2clausek [Ak2min_construction_clause] = 0; /* sxalloc */
    }
  }
    
  clause2clausek [Ak2min_construction_clause] =  Ak2min_construction_max_clausek;

  clause2iargs = (SXINT*) sxalloc (last_clause+2, sizeof (SXINT));
  iargs2arg_disp = (struct iargs2arg_disp*) sxalloc (iargs2arg_disp_size+2, sizeof (struct iargs2arg_disp));

  arg_memo = (struct arg_memo*) sxalloc (arg_memo_size+1, sizeof (struct arg_memo));

  infix_args = (SXINT*) sxalloc ((infix_args_size = arg_memo_size)+1, sizeof (SXINT));

  A_stack = (SXINT*) sxalloc (max_nt+1, sizeof (SXINT));
  A_stack [0] = 0;
  A_set = sxba_calloc (max_nt+1);

  iarg_rel_set = sxba_calloc (max_iarg_rel+1);
  Ak_stack = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT));
  Ak_stack [0] = 0;
  Ak_set = sxba_calloc (max_Ak+2);

  memel_SS_stack = SS_alloc (max_clause_arg_nb+1);
  iarg1_struct.set = sxba_calloc (max_iarg_rel+1);
  iarg1_struct.lgth = 0;
  iarg2_struct.set = sxba_calloc (max_iarg_rel+1);
  iarg2_struct.lgth = 0;
  can1_struct.set = sxba_calloc (max_iarg_rel+1);
  can1_struct.nb = (SXINT*) sxalloc (max_iarg_rel+1, sizeof (SXINT));
  can2_struct.set = sxba_calloc (max_iarg_rel+1);
  can2_struct.nb = (SXINT*) sxalloc (max_iarg_rel+1, sizeof (SXINT));
  noteq_nt_set = sxba_calloc (max_nt+1);
  noteq_clause_set = sxba_calloc (last_clause+1);

  is_initial_set = sxba_calloc (last_clause+1);

  clausek2memo = (struct Ak_memo*) sxalloc (Ak2min_construction_max_clausek+1, sizeof (struct Ak_memo));
  local_memo = (struct Ak_memo*) sxalloc (max_garity+1, sizeof (struct Ak_memo));

  iargs2arg_disp_top = 1;
  arg_memo_top = 1;
  infix_args_top = 1;

  clause2iargs [1] = iargs2arg_disp_top;
  arg_memo [arg_memo_top].disp = infix_args_top;

  for (Ak2min_construction_clause = 1; Ak2min_construction_clause <= last_clause; Ak2min_construction_clause++) {
    if (clause2identical [Ak2min_construction_clause] == Ak2min_construction_clause && !SXBA_bit_is_set (false_clause_set, Ak2min_construction_clause)) {
      is_initial = true;
      Ak2min_construction_lhs_prdct = XxY_X (rcg_clauses, Ak2min_construction_clause) & lhs_prdct_filter;
      lhs_bot2 = XH_X (rcg_predicates, Ak2min_construction_lhs_prdct);
      lhs_nt = XH_list_elem (rcg_predicates, lhs_bot2+1);
      lhs_arity = nt2arity [lhs_nt];

      rhs = XxY_Y (rcg_clauses, Ak2min_construction_clause);
      top = XH_X (rcg_rhs, rhs+1);
      bot = XH_X (rcg_rhs, rhs);

      if (lhs_arity) {
	/* En lhs si un predicat est d'arite 0, il a un param qui vaut 0 */
	clausek = clause2clausek [Ak2min_construction_clause];
	lhs_top2 = XH_X (rcg_predicates, Ak2min_construction_lhs_prdct+1);

	for (lhs_cur2 = lhs_bot2+2; lhs_cur2 < lhs_top2; lhs_cur2++) {
	  clausek2memo [clausek].min = -1;
	  clausek2memo [clausek].max = -1;
	  clausek++;
		
	  param = XH_list_elem (rcg_predicates, lhs_cur2);

	  if (param > 0 && !set_iargs2arg_disp (Ak2min_construction_clause, param, &iarg)) { /* nouvel arg */
	    iargs2arg_disp [iarg].disp = arg_memo_top;
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);
	    arg_memo_top += top3-bot3;
	  }
	  else { /* identique a iarg */
	    /* En LHS, meme si 2 args sont identiques, ils sont mis ds iargs2arg_disp */
	    iargs2arg_disp [iargs2arg_disp_top].arg = param;
	    iargs2arg_disp [iargs2arg_disp_top].disp = (param == 0) ? arg_memo_top : iargs2arg_disp [iarg].disp;
	    iargs2arg_disp_top++;
	  }
	}
		    
	for (cur = bot; cur < top; cur++) {
	  prdct = XH_list_elem (rcg_rhs, cur);
	  bot2 = XH_X (rcg_predicates, prdct);
	  nt = XH_list_elem (rcg_predicates, bot2+1);

	  if (XH_list_elem (rcg_predicates, bot2) == 0) {
	    /* appel positif */
	    if ((nt > 0 && SXBA_bit_is_set (is_lhs_nt, nt)) || nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic) {
	      if (nt > 0) {
		is_initial = false;
	      }

	      sxinitialise (iarg1); /* pour faire taire gcc -Wuninitialized */
	      sxinitialise (iarg2); /* pour faire taire gcc -Wuninitialized */
	      top2 = XH_X (rcg_predicates, prdct+1);

	      for (k = 0, cur2 = bot2+2; cur2 < top2; k++, cur2++) {
		param = XH_list_elem (rcg_predicates, cur2);

		/* Attention le 1er arg de &StrEq ou &StrEqLen peut etre une chaine terminale, on prend la longueur */
		if (k == 0) {
		  if (nt < 0) {
		    if ((nt == STREQ_ic || nt == STREQLEN_ic)) {
		      bot3 = XH_X (rcg_parameters, param);
		      top3 = XH_X (rcg_parameters, param+1);

		      for (cur3 = bot3; cur3 < top3; cur3++) {
			if (XH_list_elem (rcg_parameters, cur3) >= 0)
			  break;
		      }

		      if (cur3 == top3) {
			/* Le 1er arg ne contient que des terminaux */
			iarg1 = -(top3-bot3);
		      }
		      else {
			iarg1 = 1;
		      }
		    }
		    else {
		      /* nt == STRLEN_ic */
		      iarg1 = -XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param));
		    }
		  }
		  else
		    iarg1 = 1;
		}

		if (k > 0 || iarg1 == 1) {
		  /* L'arg designe un range */
		  if (!set_iargs2arg_disp (Ak2min_construction_clause, param, &iarg)) { /* nouvel arg en RHS */
		    iargs2arg_disp [iarg].disp = 0; /* rien en RHS */
		  }

		  if (nt > 0) {
		    Ak = A_k2Ak (nt, k);
		    XxY_set (&memel, iarg, -Ak, &pair);
		  }
		  else {
		    if (k == 0)
		      iarg1 = iarg;
		    else
		      iarg2 = iarg;
		  }
		}
	      }

	      if (nt < 0) {
		/* 2 args */
		/* nt == STREQ_ic || nt == STREQLEN_ic || nt == STRLEN_ic */
		/* iarg1 <= 0 => nt == STRLEN_ic */
		if (iarg1 < iarg2)
		  XxY_set (&memel, iarg1, iarg2, &pair);
		else
		  XxY_set (&memel, iarg2, iarg1, &pair);
	      }
	    }
	  }
	  else {
	    /* Appel negatif */
	    /* On ne traite que !&StrLen(0,X) */
	    if (nt == STRLEN_ic) {
	      param = XH_list_elem (rcg_predicates, bot2+2);

	      if (XH_list_elem (rcg_parameters, XH_X (rcg_parameters, param)) == 0) {
		/* taille nulle */
		param = XH_list_elem (rcg_predicates, bot2+3);

		if (!set_iargs2arg_disp (Ak2min_construction_clause, param, &iarg)) { /* nouvel arg en RHS */
		  iargs2arg_disp [iarg].disp = 0; /* rien en RHS */
		}

		XxY_set (&memel, iarg, -StrLen1 /* C'est un Ak particulier */, &pair);
	      }
	    }
	  }
	}
      }
	    
      clause2iargs [Ak2min_construction_clause+1] = iargs2arg_disp_top;
      iargs2arg_disp [iargs2arg_disp_top].disp = arg_memo_top; /* pour clear_arg_memo */

      y = 0;

      for (iarg = clause2iargs [Ak2min_construction_clause], lhs_pos = 0; lhs_pos < lhs_arity; lhs_pos++, iarg++) {
	param = iargs2arg_disp [iarg].arg;

	if (param > 0) {
	  if ((x = iargs2arg_disp [iarg].disp) > y) {
	    /* not yet processed */
	    y = x;
	    bot3 = XH_X (rcg_parameters, param);
	    top3 = XH_X (rcg_parameters, param+1);

	    for (cur3 = bot3; cur3 < top3; cur3++) {
	      fill_infix_args (Ak2min_construction_clause, cur3, top3, rhs);
	      x++;
	      arg_memo [x].disp = infix_args_top;
	    }
	  }
	}
      }
	
	

#if 0
      if (is_initial)
#endif /* 0 */ 
	{
	  /* independant de la partie droite, on peut le calculer */
	  /* Le 04/06/04 on execute c,a ds tous les cas car si la rhs n'est jamais modifiee (exemple appels de predicats
	     externes direct/indirect) cette clause ne sera jamais evaluee */
	  clear_arg_memo (Ak2min_construction_clause);

	  for (clausek = clause2clausek [Ak2min_construction_clause], iarg = clause2iargs [Ak2min_construction_clause], x = 0;
	       x < lhs_arity;
	       clausek++, iarg++, x++) {
	    param = (iarg == 0) ? 0 : iargs2arg_disp [iarg].arg;

	    if (param == 0) {
	      memo_val.min = memo_val.max = 0;
	    }
	    else {
	      pbot = arg_memo + iargs2arg_disp [iarg].disp;
	      ptop = pbot+((param == 0) ? 0 : XH_X (rcg_parameters, param+1)-XH_X (rcg_parameters, param));

	      memo_val = eval_arg (Ak2min_construction_clause, pbot, ptop);
	    }

	    clausek2memo [clausek] = memo_val;
	  }

	  if (SXBA_bit_is_reset_set (A_set, lhs_nt))
	    PUSH (A_stack, lhs_nt);
	}

      if (lhs_arity >= 2) {
	/* On complete memel avec tous les couples NOUVEAUX (iargh, iargk) tq
	   XxY_is_set (&AknoteqAk, Bh, Bk) == 0 si B(..., h, ..., k, ...)
	   B est un appel positif et B>0 */
	if (!is_initial) {
	  update_memel (Ak2min_construction_clause, rhs);
	}

	iarg = clause2iargs [Ak2min_construction_clause];

	for (iarg1 = iarg, A1 = A_k2Ak (lhs_nt, 0), son1 = 1; son1 < lhs_arity; iarg1++, A1++, son1++) {
	  arg1 = iargs2arg_disp [iarg1].arg;
	  A1_min = Ak2min [A1];

	  for (iarg2 = iarg1+1, A2 = A1+1, son2 = son1+1; son2 <= lhs_arity; iarg2++, A2++, son2++) {
	    if ((pair = XxY_is_set (&AknoteqAk, A1, A2)) == 0 && A1_min == Ak2min [A2]) {
	      arg2 = iargs2arg_disp [iarg2].arg;

	      if (arg1 != arg2) {
		if (!is_iarg_equality (Ak2min_construction_clause, iarg1, iarg2)) {
		  XxY_set (&AknoteqAk, A1, A2, &pair);
#if EBUG_PRDCT
		  print_debug_prdct (A1,A2);
#endif /* EBUG_PRDCT */
		  noteq_has_changed = true;
		  SXBA_1_bit (noteq_nt_set, lhs_nt);
		}
		else {
		  /* Retester Ak2min_construction_clause si sa rhs  a change */
		  if (!is_initial)
		    SXBA_1_bit (noteq_clause_set, Ak2min_construction_clause);
		}
	      }
	    }
	    else {
	      if (pair == 0) {
		XxY_set (&AknoteqAk, A1, A2, &pair);
#if EBUG_PRDCT
		print_debug_prdct (A1,A2);
#endif /* EBUG_PRDCT */
		noteq_has_changed = true;
		SXBA_1_bit (noteq_nt_set, lhs_nt);
	      }
	    }
	  }
	}

	/* enlever de memel ce qu'on y a mis */
	if (!is_initial) {
	  while (!SS_is_empty (memel_SS_stack)) {
	    pair = SS_pop (memel_SS_stack);
	    XxY_erase (memel, pair);
	  }
	}
      }

      if (is_initial)
	SXBA_1_bit (is_initial_set, Ak2min_construction_clause);
    }
    else
      clause2iargs [Ak2min_construction_clause+1] = iargs2arg_disp_top;
  }

  while (noteq_has_changed) {
    noteq_has_changed = false;

    nt = 0;

    while ((nt = sxba_scan_reset (noteq_nt_set, nt)) > 0) {
      xbot = nt2clause_disp [nt];

      for (x = nt2clause_disp [nt+1]-1; x >= xbot; x--) {
	Ak2min_construction_clause = nt2clause [x];

	if (SXBA_bit_is_set_reset (noteq_clause_set, Ak2min_construction_clause)) {
	  lhs_nt = clause2lhs_nt (Ak2min_construction_clause);
	  lhs_arity = nt2arity [lhs_nt];
	  rhs = XxY_Y (rcg_clauses, Ak2min_construction_clause);

	  update_memel (Ak2min_construction_clause, rhs);

	  iarg = clause2iargs [Ak2min_construction_clause];

	  for (iarg1 = iarg, A1 = A_k2Ak (lhs_nt, 0), son1 = 1; son1 < lhs_arity; iarg1++, A1++, son1++) {
	    arg1 = iargs2arg_disp [iarg1].arg;
	    A1_min = Ak2min [A1];

	    for (iarg2 = iarg1+1, A2 = A1+1, son2 = son1+1; son2 <= lhs_arity; iarg2++, A2++, son2++) {
	      if ((pair = XxY_is_set (&AknoteqAk, A1, A2)) == 0 && A1_min == Ak2min [A2]) {
		arg2 = iargs2arg_disp [iarg2].arg;

		if (arg1 != arg2) {
		  if (!is_iarg_equality (Ak2min_construction_clause, iarg1, iarg2)) {
		    XxY_set (&AknoteqAk, A1, A2, &pair);
#if EBUG_PRDCT
		    print_debug_prdct (A1,A2);
#endif /* EBUG_PRDCT */
		    noteq_has_changed = true;
		    SXBA_1_bit (noteq_nt_set, lhs_nt);
		  }
		  else {
		    /* Retester Ak2min_construction_clause si sa rhs  a change */
		    if (!SXBA_bit_is_set (is_initial_set, Ak2min_construction_clause))
		      SXBA_1_bit (noteq_clause_set, Ak2min_construction_clause);
		  }
		}
	      }
	      else {
		if (pair == 0) {
		  XxY_set (&AknoteqAk, A1, A2, &pair);
#if EBUG_PRDCT
		  print_debug_prdct (A1,A2);
#endif /* EBUG_PRDCT */
		  noteq_has_changed = true;
		  SXBA_1_bit (noteq_nt_set, lhs_nt);
		}
	      }
	    }
	  }

	  while (!SS_is_empty (memel_SS_stack)) {
	    pair = SS_pop (memel_SS_stack);
	    XxY_erase (memel, pair);
	  }
	}
      }
    }
  }

  nt2IeqJ_disp = (SXINT*) sxalloc (max_nt+2, sizeof (SXINT));
  nt2IeqJ = (SXINT*) sxalloc ((nt2IeqJ_size = max_nt+1)+1, sizeof (SXINT));
  nt2IeqJ_disp [1] = nt2IeqJ_top = 1;

  for (nt = 1; nt <= max_nt; nt++) {
    if (SXBA_bit_is_set (is_lhs_nt, nt) && nt2identical [nt] == nt) {
      arity = nt2arity [nt];

      if (arity >= 2) {
	for (A1 = A_k2Ak (nt, 0), son1 = 1; son1 < arity; A1++, son1++) {
	  if (!SXBA_bit_is_set (counter_Ak_set, A1) && !SXBA_bit_is_set (Ak_set, A1)) {
	    for (A2 = A1+1, son2 = son1+1; son2 <= arity; A2++, son2++) {
	      if (!SXBA_bit_is_set (counter_Ak_set, A2) && XxY_is_set (&AknoteqAk, A1, A2) == 0) {
		nt2IeqJ [nt2IeqJ_top] = (son1-1)*max_garity + (son2-1);

		if (++nt2IeqJ_top >= nt2IeqJ_size) {
		  nt2IeqJ = (SXINT*) sxrealloc (nt2IeqJ, (nt2IeqJ_size *= 2) + 1, sizeof (SXINT));
		}

		SXBA_1_bit (Ak_set, A2);
#if 0
		/* Debug bien utile ... */
		/* ... deplace' ds rcg_lo () le 25/05/04 */
		fprintf (sxtty, "%s(%ld) [%ld == %ld]\n", sxstrget (nt2ste [nt]), nt, son1, son2);
#endif
	      }
	    }
	  }
	}
      }
    }

    nt2IeqJ_disp [nt+1] = nt2IeqJ_top;
  }

  /* Le 03/02/2004 */
  sxba_empty (Ak_set);

  for (Ak2min_construction_clause = 1; Ak2min_construction_clause <= last_clause; Ak2min_construction_clause++) {
    if (clause2identical [Ak2min_construction_clause] == Ak2min_construction_clause && !SXBA_bit_is_set (false_clause_set, Ak2min_construction_clause)) {
      rhs = XxY_Y (rcg_clauses, Ak2min_construction_clause);
      update_memel (Ak2min_construction_clause, rhs);
    }
  }

  prev_Ak_max_set = sxba_calloc (max_Ak+1);
  Ak_max_set = sxba_calloc (max_Ak+1);

  while (!IS_EMPTY (A_stack)) {
    while (!IS_EMPTY (A_stack)) {
      nt = POP (A_stack);
      SXBA_0_bit (A_set, nt);
      arity = nt2arity [nt];

      for (x = 0; x < arity; x++) {
	/* On recommence tout... */
	local_memo [x].min = local_memo [x].max = -2;
#if 0
	/* Le 06/01/04 */
	local_memo [x].min = -1;
	local_memo [x].max = -1;
#endif /* 0 */
      }

      /* On met a jour les (min, len) des ntk a partir des (min, len) des nt_clauses */
      ntline = lhsnt2clause [nt];
      Ak2min_construction_clause = 0;

      while ((Ak2min_construction_clause = sxba_scan (ntline, Ak2min_construction_clause)) > 0) {
	if (clause2identical [Ak2min_construction_clause] == Ak2min_construction_clause && !SXBA_bit_is_set (false_clause_set, Ak2min_construction_clause)) {
	  clausek = clause2clausek [Ak2min_construction_clause];

	  for (x = 0; x < arity; clausek++, x++) {
	    min = clausek2memo [clausek].min;
	    max = clausek2memo [clausek].max;

	    if (local_memo [x].min == -2 && local_memo [x].max == -2) {
	      /* 1ere A_clause : Initialisation */
	      local_memo [x].min = (min < 0) ? -1 : min;
	      local_memo [x].max = (max < 0) ? -1 : max;
	    }
	    else {
	      /* Sur des A_clauses multiples, on prend le min des min... */
	      if (min < 0)
		/* Pas calcule' => min Pas calcule' */
		local_memo [x].min = -1;
	      else {
		if (min < local_memo [x].min)
		  local_memo [x].min = min;
	      }
	    
	      /* ... et le max des max */
	      if (max < 0 || local_memo [x].max == -1)
		/* Pas calcule' => max inde'termine' */
		local_memo [x].max = -1;
	      else {
		if (max > local_memo [x].max)
		  local_memo [x].max = max;
	      }
	    }
	  }
	}
      }

      /* On repercute le resultat vers les Ak */
      Ak = A_k2Ak (nt, 0);
      has_min_changed = false;

      for (x = 0; x < arity; Ak++, x++) {
	/* Le 03/02/04 on reaffecte completemet les nouvelles valeurs des Ak2m[in|ax], idependamment des anciennes */
	/* Donc un truc calcule' peut ne plus l'e^tre !! */
	ip = &(Ak2min [Ak]);
	local_min = local_memo [x].min;

	if (local_min >= 0 && (*ip == -1 || *ip > local_min)) {
	  /* On prend le min des min */
	  *ip = local_min;
	  has_min_changed = true;
	}

	ip = &(Ak2max [Ak]);
	local_max = local_memo [x].max;

	if (local_max >= 0 && (*ip == -1 || *ip < local_max)) {
	  /* On prend le max des max */
	  *ip = local_max;
	  SXBA_1_bit (Ak_max_set, Ak);
	  /* has_max_changed = true; MODIF du 18/6/2002 */
	}
      }

      if (has_min_changed) {
	/* MODIF du 18/6/2002 */
	/* On calcule les min en premier.  Quand c'est stabilise', on s'occupe des max */
	/* sxba_empty (Ak_max_set); */
	xbot = nt2clause_disp [nt];

	for (x = nt2clause_disp [nt+1]-1; x >= xbot; x--) {
	  Ak2min_construction_clause = nt2clause [x];

	  if (clause2identical [Ak2min_construction_clause] == Ak2min_construction_clause && !SXBA_bit_is_set (false_clause_set, Ak2min_construction_clause)) /* Le 16/07/2003 */
	    if (SXBA_bit_is_reset_set (clause_set, Ak2min_construction_clause))
	      PUSH (clause_stack, Ak2min_construction_clause);
	}
      }
    }

    if (IS_EMPTY (clause_stack)) {
      /* Ici les min sont stabilises, on s'occupe des max ... */
      /* Comme les max peuvent augmenter regulierement sur les clauses recursives, il faut
	 le detecter et dire que l'on ne sait pas calculer le max */
      if (sxba_scan (Ak_max_set, 0) > 0) {
	if (sxba_first_difference (prev_Ak_max_set, Ak_max_set) == -1) {
	  /* Ici, sur 2 tours successifs complets, ce sont les memes Ak qui ont augmentes =>
	     on suppose donc qu'ils sont recursifs et que leur max ne peut etre calcule'... */
	  /*  => on passe le max a -1 */
	  Ak = 0;
	    
	  while ((Ak = sxba_scan_reset (Ak_max_set, Ak)) > 0) {
	    Ak2max [Ak] = -1;
	  }
	}
	else {
	  sxba_copy (prev_Ak_max_set, Ak_max_set);

	  Ak = 0;
	    
	  while ((Ak = sxba_scan_reset (Ak_max_set, Ak)) > 0) {
	    nt = Ak2A (Ak);
	    xbot = nt2clause_disp [nt];

	    for (x = nt2clause_disp [nt+1]-1; x >= xbot; x--) {
	      Ak2min_construction_clause = nt2clause [x];
	      
	      if (clause2identical [Ak2min_construction_clause] == Ak2min_construction_clause && !SXBA_bit_is_set (false_clause_set, Ak2min_construction_clause)) /* Le 16/07/2003 */
		if (SXBA_bit_is_reset_set (clause_set, Ak2min_construction_clause))
		  PUSH (clause_stack, Ak2min_construction_clause);
	    }
	  }
	}
      }
    }

    while (!IS_EMPTY (clause_stack)) {
      Ak2min_construction_clause = POP (clause_stack);
      SXBA_0_bit (clause_set, Ak2min_construction_clause);
      lhs_nt = clause2lhs_nt (Ak2min_construction_clause);
      lhs_arity = nt2arity [lhs_nt];

      clear_arg_memo (Ak2min_construction_clause);
      iarg = clause2iargs [Ak2min_construction_clause];
      clausek = clause2clausek [Ak2min_construction_clause];

      for (x = 0; x < lhs_arity; clausek++, iarg++, x++) {
	param = iargs2arg_disp [iarg].arg;

	if (param == 0) {
	  /* LHS argument vide */
	  memo_val.min = memo_val.max = 0;
	}
	else {
	  pbot = arg_memo + iargs2arg_disp [iarg].disp;
	  ptop = pbot+XH_X (rcg_parameters, param+1)-XH_X (rcg_parameters, param);

	  memo_val = eval_arg (Ak2min_construction_clause, pbot, ptop);
	}

	ip = &(clausek2memo [clausek].min);

	if (memo_val.min >= 0 /* On vient de calculer une valeur minimale ... */
	    && (*ip == -1 || *ip < memo_val.min) /* ...et c'est la 1ere fois ou la valeur deja calculee est + petite */) {
	  /* Le 03/02/2004 On note le max des min */
	  *ip = memo_val.min;

	  if (SXBA_bit_is_reset_set (A_set, lhs_nt))
	    PUSH (A_stack, lhs_nt);
	}

	ip = &(clausek2memo [clausek].max);

	if (memo_val.max >= 0 /* On vient de calculer une valeur maximale ... */
	    && (*ip == -1 || memo_val.max > *ip) /* ...et c'est la 1ere fois ou la valeur deja calculee est + petite */) {
	  /* Le 04/02/2004 On note le max des max... */
	  /* ... la croissance non bornee sera bloquee par la gestion des Ak_max_set !! */
	  *ip = memo_val.max;

	  if (SXBA_bit_is_reset_set (A_set, lhs_nt))
	    PUSH (A_stack, lhs_nt);
	}
      }
    }
  }

  /* s'il reste des -1, ont les passe a 0 */
  /* On calcule len a partir de min et max */
  for (x = max_Ak; x > 0; x--) {
    if (SXBA_bit_is_set (counter_Ak_set, x)) {
      min = -1;
      max = -2;
    }
    else {
      min = Ak2min [x];
      max = Ak2max [x];
    }

    Ak2len [x] = (min == max && min >= 0) ? min : -1;
      
    if (min == -1) {
#if 0
      /* Il peut rester des -1 */
      nt = Ak2A (x);

      if (SXBA_bit_is_set (is_lhs_nt, nt) && (nt2identical [nt] == nt)) {
	/* Les predicats externes ne sont pas positionnes */
	k = Ak_A2k (x, nt);
	fprintf (sxtty, "%s [%ld] is not set\n", sxstrget (nt2ste [nt]), k+1);
      }
#endif

      Ak2min [x] = 0;
    }

    if (max == -2) {
      Ak2max [x] = -1; /* Pas de max */
    }
  }

  sxfree (clause2iargs);
  sxfree (arg_memo);
  sxfree (infix_args);
  sxfree (A_stack);
  sxfree (A_set);
  sxfree (iarg_rel_set);
  sxfree (Ak_stack);
  sxfree (Ak_set);
  SS_free (memel_SS_stack);
  XxY_free (&AknoteqAk);
  sxfree (iarg1_struct.set);
  sxfree (iarg2_struct.set);
  sxfree (can1_struct.set);
  sxfree (can1_struct.nb);
  sxfree (can2_struct.set);
  sxfree (can2_struct.nb);
  sxfree (noteq_nt_set);
  sxfree (noteq_clause_set);
  sxfree (clausek2memo);
  sxfree (clause2clausek);
  sxfree (local_memo);
  sxfree (is_initial_set);
  sxfree (prev_Ak_max_set);
  sxfree (Ak_max_set);

#if 0
  /* Debug bien utile */
  /* ... deplace' ds rcg_lo () le 25/05/04*/
  for (nt = 1; nt <= max_nt; nt++) {
    if (SXBA_bit_is_set (is_lhs_nt, nt) && nt2identical [nt] == nt) {
      fprintf (sxtty, "%s(%ld) [", sxstrget (nt2ste [nt]), nt);
      lhs_arity = nt2arity [nt];
      Ak = A_k2Ak (nt, 0);

      for (x = 1; x <= lhs_arity; x++) {
	fprintf (sxtty, "(<%ld..%ld>, %ld)", Ak2min [Ak], Ak2max [Ak], Ak2len [Ak]);
	Ak++;

	if (x < lhs_arity)
	  fputs (", ", sxtty);
		     
      }

      fputs ("]\n", sxtty);
    }
  }
#endif

  if (sxverbosep) {
    fputs ("done\n", sxtty);
    sxttycol1p = true;
  }
}


static void
rcg_lo (void)
{
  SXINT    rcg_lo_clause;
  VARSTR vstr;
  char	 *lst_name;

  if (sxverbosep) {
    fputs ("\tListing Output .... ", sxtty);
    sxttycol1p = false;
  }

  vstr = varstr_alloc (32);

  vstr = varstr_catenate (vstr, prgentname);
  vstr = varstr_catenate (vstr, ".rcg.l");
  lst_name = varstr_tostr (vstr);

  if ((listing = sxfopen (lst_name, "w")) == NULL) {
    fprintf (sxstderr, "rcg_lo: cannot open (create) ");
    sxperror (lst_name);
    return;
  }

  varstr_free (vstr), vstr = NULL;

  put_edit_initialize (listing);

  put_edit_nnl (9, "Listing of:");
  put_edit_nnl (25, sxsrcmngr.source_coord.file_name);
  put_edit_nnl (9, "By:");
  put_edit (25, by_mess);
  put_edit (25, release);
  put_edit_nnl (9, "Generated on:");

  {
    time_t date_time;

    date_time = time (0);
    put_edit (25, ctime (&date_time));
  }

  put_edit_nl (2);

  sxlisting_output (listing);

  if (first) {
    put_edit_nl (2);
    print_first_last ();
  }

  if (!check_instantiable_clause && (rcg_lo_clause = sxba_scan (false_clause_set, 0)) > 0) {
    fputs ("\n\nThe following clauses are not instantiable:\n", listing);
	
    do {
      gen_clause_comment (listing, rcg_lo_clause);
    } while ((rcg_lo_clause = sxba_scan (false_clause_set, rcg_lo_clause)) > 0);
  }

  if (!IS_ERROR) {
    SXINT nt, arity, bot, top, IJ, I, J, x, Ak;

    fputs ("\n\nArgs of same size\n", listing);

    for (nt = 1; nt <= max_nt; nt++) {
      if (SXBA_bit_is_set (is_lhs_nt, nt) && nt2identical [nt] == nt) {
	arity = nt2arity [nt];

	if (arity >= 2) {
	  bot = nt2IeqJ_disp [nt];
	  top = nt2IeqJ_disp [nt+1];

	  while (bot < top) {
	    IJ = nt2IeqJ [bot];
	    J = IJ % max_garity;
	    I = IJ / max_garity;
		
	    fprintf (listing, "%s(%ld) [%ld == %ld]\n", sxstrget (nt2ste [nt]), (SXINT) nt, (SXINT) I+1, (SXINT) J+1);

	    bot++;
	  }
	}
      }
    }

    fputs ("\n\nMin, Max & Size\n", listing);

    for (nt = 1; nt <= max_nt; nt++) {
      if (SXBA_bit_is_set (is_lhs_nt, nt) && nt2identical [nt] == nt) {
	fprintf (listing, "%s(%ld) [", sxstrget (nt2ste [nt]), (SXINT) nt);
	arity = nt2arity [nt];
	Ak = A_k2Ak (nt, 0);

	for (x = 1; x <= arity; x++) {
	  fprintf (listing, "(<%ld..%ld>, %ld)", (SXINT) Ak2min [Ak], (SXINT) Ak2max [Ak], (SXINT) Ak2len [Ak]);
	  Ak++;

	  if (x < arity)
	    fputs (", ", listing);
		     
	}

	fputs ("]\n", listing);
      }
    }
  }

  put_edit_finalize ();

  if (sxverbosep) {
    fputs ("done\n", sxtty);
    sxttycol1p = true;
  }
}



static void
smpopen (void)
{
    sxatcvar.atc_lv.node_size = sizeof (struct rcg_node);
}

static void
smppass (void)
{
    struct rcg_node	*np, *lhs_p, *son;
    SXINT nb; 

    if (sxatcvar.atc_lv.abstract_tree_is_error_node) return;

    if (sxverbosep) {
	fputs ("\tSemantic Pass .... ", sxtty);
	sxttycol1p = false;
    }

    /*   I N I T I A L I S A T I O N S   */

    rule_nb = sxatcvar.atc_lv.abstract_tree_root->degree;

    max_ste = SXSTRtop ();

    max_prdct_nb = 0;
    max_lhs_pos_nb = 0;

    for (np = sxatcvar.atc_lv.abstract_tree_root->son; np != NULL; np = np->brother) {
      lhs_p = np->son;
	max_prdct_nb += 1 /* lhs */ + lhs_p->brother->degree /* rhs */;

	lhs_p = lhs_p->son->brother;
	/* lhs_p -> name == ARGUMENT_S_n */
	nb = 0;

	for (son = lhs_p->son; son != NULL; son = son->brother)
	  nb += son->degree;

	if (nb > max_lhs_pos_nb)
	  max_lhs_pos_nb = nb;
    }

    /* ds ce cas on s'arrange pour que le XxY_header rcg_clauses ne detecte pas d'egalite
       sur les clauses (lhs,rhs) en fabriquant des lhs differentes i<<log_max_prdct_nb+lhs
       pour la 1ere valeur de i qui marche.  La bonne valeur de lhs est extraite par filtrage */
    log_max_prdct_nb = sxlast_bit (max_prdct_nb);
    lhs_prdct_filter = (1<<log_max_prdct_nb)-1;

    XxY_alloc (&rcg_clauses, "rcg_clauses", rule_nb + 1, 1, 1, 0, NULL, NULL);
    XH_alloc (&rcg_rhs, "rcg_rhs", rule_nb + 1, 1, ml, NULL, NULL);
    clause2visited = (struct rcg_node **) sxalloc (rule_nb + 1, sizeof (struct rcg_node *));
    
    /* rcg_predicates : oper nt param_11 ... param_p.
       oper : 0 => +, 1 => not */
    XH_alloc (&rcg_predicates, "rcg_predicates", max_prdct_nb+1, 1, mp+2, NULL, NULL);

    XH_alloc (&rcg_parameters, "rcg_parameters", mp * max_prdct_nb+1, 1, ms, NULL, NULL);

    lhs_pos_hd_set = sxba_calloc (max_lhs_pos_nb + 1);
    lhs_pos_set = sxba_calloc (max_lhs_pos_nb + 1);
    lhs_pos2ste = (SXINT*) sxalloc (max_lhs_pos_nb + 1, sizeof (SXINT));
    lhs_var_ste_set = sxba_calloc (max_ste + 1);
    rhs_var_ste_set = sxba_calloc (max_ste + 1);
    ste_set = sxba_calloc (max_ste + 1);
    ste_stack = (SXINT*) sxalloc (max_ste + 1, sizeof (SXINT)), ste_stack [0] = 0;
    XxY_alloc (&var_couples, "var_couples", mp*ms + 1, 1, 1, 1, NULL, NULL);

    ste2nt = (SXINT*) sxcalloc (max_ste + 1, sizeof (SXINT));
    nt2ste = (SXINT*) sxalloc (max_ste + 1, sizeof (SXINT));
    nt2arity = (SXINT*) sxalloc (max_ste + 1, sizeof (SXINT));
    /* On permet les predicats d'arite nulle */
    /* En particulier pour la semantique qui peut les utiliser comme point d'accrochage
       des traitements */
    /* Exemple (extrait de meteo.drcg) :
       bearing()(D,1)			--> {338=<D}.
       bearing()(D,1)			--> {D=<22}.
       ...
       bearing()(D,7)			--> {248=<D, D=<292}.
       bearing()(D,8)			--> {293=<D, D=<337}.
       */
    /* Pb distinction entre arity == 0 et arity == 1 et argument == epsilon */
    /* En rhs, si A() => arite == 0 */
    /* En lhs, si A() => arite == 0  ou arite == 1 et arg vide */
    /* Si une occurrence est non vide => arite au moins 1, si toutes vides, arite 0 */

    is_lhs_nt = sxba_calloc (max_ste + 1);
    is_rhs_nt = sxba_calloc (max_ste + 1);
    is_nt_external = sxba_calloc (max_ste + 1);
    is_nt_internal = sxba_calloc (max_ste + 1);
    ste2t = (SXINT*) sxcalloc (max_ste + 1, sizeof (SXINT));
    t2ste = (SXINT*) sxalloc (max_ste + 1, sizeof (SXINT));
    ste2var = (SXINT*) sxcalloc (max_ste + 1, sizeof (SXINT));
    var2ste = (SXINT*) sxalloc (max_ste + 1, sizeof (SXINT));
    var2param = (SXINT*) sxcalloc (max_ste + 1, sizeof (SXINT));

    clause2lhs_var = (SXINT *) sxalloc (rule_nb + 1, sizeof (SXINT));
    clause2max_var = (SXINT *) sxalloc (rule_nb + 1, sizeof (SXINT));
    clause2action = (SXINT *) sxalloc (rule_nb + 1, sizeof (SXINT));
    clause2bottom_up_erasing = sxba_calloc (rule_nb + 1);
    clause2non_range_arg = sxba_calloc (rule_nb + 1);
    false_clause_set = sxba_calloc (rule_nb + 1);
    new_false_clause_stack = (SXINT*) sxalloc (rule_nb + 1, sizeof (SXINT));

    has_an_identical_clause_set = sxba_calloc (rule_nb+1);
    clause2identical = (SXINT *) sxalloc (rule_nb+1, sizeof (SXINT));

    clause2attr_disp = (SXBA**) sxcalloc (rule_nb+1, sizeof (SXBA*));

    

    if (is_drcg) {
	/* Au cas ou pour les DRCGs */
	clause2da_disp = (SXINT*) sxalloc (rule_nb + 2, sizeof (SXINT));
	clause2da_disp [1] = da_top = 0;
	clause2da = (SXINT*) sxalloc ((da_size = rule_nb + 2) + 1, sizeof (SXINT));
    }
	
    /*   A T T R I B U T E S    E V A L U A T I O N   */
    /* is_a_complete_terminal_grammar ssi toutes les clauses sont de la forme A (t) --> . */
    is_proper_grammar = is_simple_grammar = is_left_linear_grammar = is_right_linear_grammar = is_a_complete_terminal_grammar = true;
    is_bottom_up_erasing_grammar = is_top_down_erasing_grammar = is_loop_grammar = is_combinatorial_grammar = is_overlapping_grammar = false;

    sxsmp (sxatcvar.atc_lv.abstract_tree_root, rcg_pi, rcg_pd);

    sxfree (lhs_pos_hd_set), lhs_pos_hd_set = NULL;
    sxfree (lhs_pos_set), lhs_pos_set = NULL;
    sxfree (lhs_pos2ste), lhs_pos2ste = NULL;

    sxfree (lhs_var_ste_set), lhs_var_ste_set = NULL;
    sxfree (rhs_var_ste_set), rhs_var_ste_set = NULL;
    sxfree (ste_set), ste_set = NULL;
    sxfree (ste_stack), ste_stack = NULL;
    XxY_free (&var_couples);

    if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = true;
    }
    
    false_A_set = sxba_calloc (max_nt + 1);

    if (!IS_ERROR) {
      SXINT size, nt, smppass_clause;

      nt2clause_construction ();

      next_rhs_set = sxba_calloc (max_rhs_prdct+1);
      next_var2cur = (SXINT *) sxcalloc (max_gvar+1, sizeof (SXINT));

      is_a_set ();

      sxfree (next_rhs_set), next_rhs_set = NULL;
      sxfree (next_var2cur), next_var2cur = NULL;

      XxY_alloc (&memel, "memel", last_clause+1, 1, 1, 1, memel_oflw, NULL);
      memel2val = (SXINT*) sxalloc (XxY_size (memel)+1, sizeof (SXINT));
      nt2empty_vector = sxbm_calloc (max_nt+1, max_garity+1);
      clause2empty_vector = sxbm_calloc (last_clause+1, max_garity+1);
      clause_stack = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT)), clause_stack [0] = 0;
      clause_set = sxba_calloc (last_clause+1);
      nt_stack =  (SXINT*) sxalloc (max_nt+1, sizeof (SXINT)), nt_stack [0] = 0;
      nt_set = sxba_calloc (max_nt+1);
      pos_set = sxba_calloc (max_garity+1);

      {
	SXINT smppass_nt, Ak, arity, k;
	max_Ak = 0; /* pour des passages multiples eventuels !! */

	for (smppass_nt = 1; smppass_nt <= max_nt; smppass_nt++) {
	  max_Ak += nt2arity [smppass_nt];
	}

	A2A0 = (SXINT*) sxalloc (max_nt+1, sizeof (SXINT));
	A02A = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT));

	A2A0 [0] = A02A [0] = 0;

	Ak = 1;

	for (smppass_nt = 1; smppass_nt <= max_nt; smppass_nt++) {
	  A2A0 [smppass_nt] = Ak;
	  arity = nt2arity [smppass_nt];
	    
	  for (k = 0; k < arity; k++)
	    A02A [Ak] = smppass_nt, Ak++;
	}
      }

      counter_Ak_set = sxba_calloc (max_Ak+1);

      check_counter_args (); /* Ajoute' le 04/04/08 */

      check_linearity ();  /* Ajoute' le 25/04/08 (ca tient compte des compteurs) */

#if 0
      check_rhs_args_as_substrings (); /* Ajoute' le 04/04/08 */
#endif /* 0 */

      if (!IS_ERROR) {
	do {
	  if (sxverbosep) {
	    fputs ("\tChecking .... ", sxtty);
	    sxttycol1p = false;
	  }

	  false_clause_propagate ();

	  /* Le Lun 13 Nov 2000 11:20:32, on supprime le detection de predicats equivalents (et la
	     reduction correspondante de la grammaire) car on a seulement l'equivalence faible entre
	     les deux grammaires et pas l'equivalence forte d'ou un probleme avec la semantique
	     associee ... Ex la grammaire ambigue
	     S(X) --> S1(X) .
	     S(X) --> S2(X) .
	     S1() --> .
	     S2() --> .
	     se reduit en la grammaire non ambigue (S1 = S2 et clause1 = clause2)
	     S(X) --> S1(X) .
	     S1() --> .
	     Et la "reconstruction" de l'equivalence forte ne semble pas immediate */
	  /* On laisse cependant en place les structures has_an_identical_nt_set et nt2identical */
	   
	   
	  if (has_an_identical_nt_set) {
	    sxba_empty (has_an_identical_nt_set);
	  }
	  else {
	    has_an_identical_nt_set = sxba_calloc (rule_nb+1);
	    nt2identical = (SXINT *) sxalloc (rule_nb+1, sizeof (SXINT));
	  }

	  for (nt = 1; nt <= max_nt; nt++)
	    nt2identical [nt] = nt;

	  param_set = sxba_calloc (XH_top (rcg_parameters)+1);
	  multiple_var_set = sxba_calloc (max_gvar+1);
	  var_set = sxba_calloc (max_gvar+1);
	  multiple_var2eq = (SXINT*) sxcalloc (max_gvar+1, sizeof (SXINT));

	  size = max_clause_arg_nb*(max_arg_size+1);
	  state_stack = (SXINT*) sxalloc (size+1, sizeof (SXINT)), state_stack [0] = 0;
	  equal = sxbm_calloc (size+1, size+1);
	  plus1 = sxbm_calloc (size+1, size+1);
	  moins1 = sxbm_calloc (size+1, size+1);
	  S1_alloc (equal_stack, size+1);
	  less = sxbm_calloc (size+1, size+1);
	  less_eq = sxbm_calloc (size+1, size+1);

	  is_instantiable ();

	  sxfree (param_set), param_set = NULL;
	  sxfree (multiple_var_set), multiple_var_set = NULL;
	  sxfree (var_set), var_set = NULL;
	  sxfree (multiple_var2eq), multiple_var2eq = NULL;
	  sxfree (state_stack), state_stack = NULL;
	  S1_free (equal_stack), equal_stack = NULL;
	  sxbm_free (equal), equal = NULL;
	  sxbm_free (plus1), plus1 = NULL;
	  sxbm_free (moins1), moins1 = NULL;
	  sxbm_free (less), less = NULL;
	  sxbm_free (less_eq), less_eq = NULL;

	  productive_clause_set = sxba_calloc (last_clause+1);
	  cf_skeleton = sxbm_calloc (last_clause+1, max_nt+1);
	  rhsnt2clause = sxbm_calloc (max_nt+1, last_clause+1);
	  /* multiple_rhsnt2clause = sxbm_calloc (max_nt+1, last_clause+1); */
	  clause2rhsnb = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));

	  if (empty_clause_set) {
	    sxba_empty (empty_clause_set);
	    sxba_empty (negative_nt_set);
	  }
	  else {
	    empty_clause_set = sxba_calloc (last_clause+1);
	    negative_nt_set = sxba_calloc (max_nt+1);
	  }

	  Xref ();

	  /* Si l'arite d'un nt est -1, il est defini ds ce module ( nt() --> ...
	     et non utilise ds ce module.  Il est donc obligatoirement externe.
	     On lui donne l'arite 1 */
	  for (nt = 1; nt <= max_nt; nt++) {
	    if (nt2arity [nt] == -1) {
#if EBUG
	      if (!(SXBA_bit_is_set (is_lhs_nt, nt) &&
		    !SXBA_bit_is_set (is_rhs_nt, nt)))
		sxtrap (ME, "smppass");
#endif

	      if (!SXBA_bit_is_set (is_nt_external, nt)) {
		smppass_clause = sxba_scan (lhsnt2clause [nt], 0);
		sxerror (clause2visited [smppass_clause]->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sThe LHS predicate is unused in this module, it must be external.",
			 sxtab_ptr->err_titles [2]+1);
	      }

	      nt2arity [nt] = 1;
	    }
	  }

	  is_productive ();

	  is_accessible ();

	  if (is_negative && check_self_definition)
	    negative_language ();

	  sxbm_free (cf_skeleton), cf_skeleton = NULL;

	  if (!IS_ERROR && is_first_last) {
#if 0
	    if (lhs_var_set) {
	      SXINT i;

	      sxba_empty (lhs_var_set);
	      sxba_empty (GU_set);
	      sxba_empty (NV_set);
	      sxba_empty (V_set);

	      for (i = 0; i <=max_rhs_arity; i++)
		sxba_empty (U_set [i]);
	    }
	    else {
	      lhs_var_set = sxba_calloc (max_gvar+1);
	      U_set = sxbm_calloc (max_rhs_arity+1, max_gvar+1);
	      GU_set = sxba_calloc (max_gvar+1);
	      NV_set = sxba_calloc (max_gvar+1);
	      V_set = sxba_calloc (max_gvar+1);
	      U_stack = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
	      NV_stack = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
	      V_stack = (SXINT*) sxalloc (max_gvar+1, sizeof (SXINT));
	      param_stack = (SXINT*) sxalloc (max_rhs_arity+1, sizeof (SXINT));
	    }

	    U_stack [0] = 0;
	    NV_stack [0] = 0;
	    V_stack [0] = 0;
	    param_stack [0] = 0;
#endif /* 0 */

	    /* les "arguments vides" peuvent s'obtenir par d'autres moyens que
	       l'absence d'arg.
	       - Appel d'un vrai predicat externe (on ne sait rien sur lui)
	       - A(X,Y,Z) --> &StrLen(0, X Y) B(X,Y,Z) . On reutilise X Y ou meme
	       - A(X,Y,Z) --> &StrLen(4, X Y) . X ou Y peuvent etre vides! */
#if 0
	    /* Qu'est-ce ? */
	    nt = 0;

	    while ((nt = sxba_scan (is_nt_external, nt)) > 0) {
	      if (!SXBA_bit_is_set (is_lhs_nt, nt))
		/* nt est un "vrai" externe */
		break;
	    }
#endif /* 0 */

#if 0
	    /* Le 19/12/03 on execute c,a tout le temps, ca permet de prendre des trucs comme
	       A(X) --> &StrLen (0, X) .
	    */
	    if (is_empty_arg || nt > 0 || sxba_scan (clause2non_range_arg, 0) > 0)
	      ;
#endif /* 0 */
	    {
	      /* La grammaire n'est pas epsilon-free, on construit la structure nt2empty_vector
		 qui a chaque nt associe l'ensemble de ses arg (0 <= arg < arity (nt))
		 les 0 reperent les arguments pouvant etre vides
		 les 1 ceux qui ne peuvent pas deriver ds le vide
	      */
	      /* is_empty_arg = true; "empty" est alloue' */
#if 0
	      param_set = sxba_calloc (max_garity+1);
	      productive_eclause_set = sxba_calloc (last_clause+1);
	      completed_clause_set = sxba_calloc (last_clause+1);

	      nt2empty_vector_is_set = sxba_calloc (max_nt+1);
	      wbvect = sxba_calloc (max_garity+1);
#endif /* 0 */

	      /* XxY_alloc (&empty, "empty", 2*max_nt + 1, 1, 1, 0, NULL, NULL); */

	      empty_construction ();

#if 0
	      sxfree (nt2empty_vector_is_set), nt2empty_vector_is_set = NULL;
	      sxfree (wbvect), wbvect = NULL;

	      sxfree (param_set), param_set = NULL;
	      sxfree (productive_eclause_set), productive_eclause_set = NULL;
	      sxfree (completed_clause_set), completed_clause_set = NULL;
#endif /* 0 */
	    }
	  }

	  sxfree (productive_clause_set), productive_clause_set = NULL;

	  is_start_symbol_in_rhs = !sxba_is_empty (rhsnt2clause [1]);

	  sxbm_free (rhsnt2clause), rhsnt2clause = NULL;
	  /* sxbm_free (multiple_rhsnt2clause), multiple_rhsnt2clause = NULL; */
	  sxfree (clause2rhsnb), clause2rhsnb = NULL;

	  if (sxverbosep) {
	    fputs ("done\n", sxtty);
	    sxttycol1p = true;
	  }

	  if (!IS_ERROR) {
	    SXINT smppass_clause_2, main_clause;

	    if (loop_clause_set) {
	      sxba_empty (loop_clause_set);
	      sxba_empty (loop_nt_set);
	    }
	    else {
	      loop_clause_set = sxba_calloc (last_clause+1);
	      loop_nt_set = sxba_calloc (max_nt+1);
	    }

	    has_loops ();

	    if (clause2clause0 == NULL) {
	      /* max_clausek et clause2clause0 sont utilises ds 1rcg_2var */
	      clause2clause0 = (SXINT*) sxalloc (last_clause+1, sizeof (SXINT));
	    }

	    clause2clause0 [0] = 0;

	    max_clausek = 1;

	    for (smppass_clause_2 = 1; smppass_clause_2 <= last_clause; smppass_clause_2++) {
	      if ((main_clause = clause2identical [smppass_clause_2]) == smppass_clause_2) {
		clause2clause0 [smppass_clause_2] = max_clausek;
		max_clausek += nt2arity [clause2lhs_nt (smppass_clause_2)];
	      }
	      else {
		clause2clause0 [smppass_clause_2] = clause2clause0 [main_clause];
	      }
	    }

	    if (is_first_last && max_t) {
	      /* Si |T| == 0, inutile */
	      if (sxverbosep) {
		fputs ("\tFirst & last .... ", sxtty);
		sxttycol1p = false;
	      }
    
	      if (first) {
		SXINT i;

		for (i = 0; i <= max_Ak; i++) {
		  sxba_empty (first [i]);
		  sxba_empty (last [i]);
		}
	      }
	      else {
		first = sxbm_calloc (max_Ak+1, max_t+1);
		last = sxbm_calloc (max_Ak+1, max_t+1);
	      }

	      XxY_alloc (&first_rel, "first_rel", 2*max_Ak+1, 1, 1, 0, NULL, NULL);
	      XxY_alloc (&last_rel, "last_rel", 2*max_Ak+1, 1, 1, 0, NULL, NULL);
	      XH_alloc (&first_Ak_list, "first_Ak_list", rule_nb + 1, 1, 3, NULL, NULL);
	      XH_alloc (&last_Ak_list, "last_Ak_list", rule_nb + 1, 1, 3, NULL, NULL);
	      Ak_stack = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT)), Ak_stack [0] = 0;

	      first_Ak_set = sxba_calloc (max_Ak+1);
	      first_Ak_stack = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT)), first_Ak_stack [0] = 0;
	      last_Ak_set = sxba_calloc (max_Ak+1);
	      last_Ak_stack = (SXINT*) sxalloc (max_Ak+1, sizeof (SXINT)), last_Ak_stack [0] = 0;

#if 0
	      rhs_var_set = sxba_calloc (max_gvar+1);
	      var_stacks_disp = (SXINT**) sxalloc (max_gvar+1, sizeof (SXINT*));
	      p = var_stacks = (SXINT*) sxalloc (max_gvar*(2*max_rhs_arity+1)+1, sizeof (SXINT));

	      var_stacks_disp [0] = NULL;
	
	      for (var = 1; var <= max_gvar; var++) {
		var_stacks_disp [var] = p;
		*p = 0;
		p += 2*max_rhs_arity+1;
	      }
#endif /* 0 */

	      first_last_construction ();

	      XxY_free (&first_rel);
	      XxY_free (&last_rel);
	      XH_free (&first_Ak_list);
	      XH_free (&last_Ak_list);
	      sxfree (Ak_stack), Ak_stack = NULL;
	      sxfree (first_Ak_set), first_Ak_set = NULL;
	      sxfree (first_Ak_stack), first_Ak_stack = NULL;
	      sxfree (last_Ak_set), last_Ak_set = NULL;
	      sxfree (last_Ak_stack), last_Ak_stack = NULL;

#if 0
	      sxfree (rhs_var_set), rhs_var_set = NULL;
	      sxfree (var_stacks_disp), var_stacks_disp = NULL;
	      sxfree (var_stacks), var_stacks = NULL;
#endif /* 0 */

	      if (sxverbosep) {
		fputs ("done\n", sxtty);
		sxttycol1p = true;
	      }
	    }
	  }
	  /* Cette boucle a ete ajoutee le 13/04/04 */
	  /* Si l'option check_instantiable_clause n'est pas positionnee, on genere le parseur meme s'il y a des
	     clauses non instantiables. Pour ca ces clauses sont mises ds false_clause_set et l'on boucle pour etre su^r
	     de ne pas en manquer */
	  /* A FAIRE : mettre fill_param_pos () ds la boucle */
	} while (!IS_ERROR && !IS_EMPTY (new_false_clause_stack));
      }
  
      sxbm_free (nt2empty_vector), nt2empty_vector = NULL;
      sxbm_free (clause2empty_vector), clause2empty_vector = NULL;
      sxfree (nt_stack), nt_stack = NULL;
      sxfree (nt_set), nt_set = NULL;
      sxfree (pos_set), pos_set = NULL;

#if 0
      /* De'place' ds rcg_lo () le 25/05/04 */
      if (!check_instantiable_clause && (smppass_clause = sxba_scan (false_clause_set, 0)) > 0) {
	fputs ("\nThe following clauses are not instantiable:\n", sxtty);
	
	do {
	  gen_clause_comment (sxtty, smppass_clause);
	} while ((smppass_clause = sxba_scan (false_clause_set, smppass_clause)) > 0);
      }
#endif
    }


    /*   F I N A L I S A T I O N S   */
    if (!IS_ERROR) {
      param_stack = (SXINT*) sxalloc (max_rhs_arity+1, sizeof (SXINT)), param_stack [0] = 0;

      Ak2min_construction (); /* et Ak2len */
    
      sxfree (param_stack), param_stack = NULL;
    }

    if (is_listing_output)
	rcg_lo ();

#if 0
    if (U_set) sxbm_free (U_set), U_set = NULL;
    if (GU_set) sxfree (GU_set), GU_set = NULL;
    if (NV_set) sxfree (NV_set), NV_set = NULL;
    if (V_set) sxfree (V_set), V_set = NULL;
    if (U_stack) sxfree (U_stack), U_stack = NULL;
    if (NV_stack) sxfree (NV_stack), NV_stack = NULL;
    if (V_stack) sxfree (V_stack), V_stack = NULL;
    if (param_stack) sxfree (param_stack), param_stack = NULL;
#endif /* 0 */

    if (!IS_ERROR) {
	/* if (!check_instantiable_clause)
	  les erreurs de clauses non instantiables n'ont pas ete sorties mais uniquement notees ds false_clause_set
	  on va quand meme faire la generation. */
	if (is_1_RCG) {
	    /* On construit une 1-RCG qui definit un sur-langage */
	    one_rcg ();
	}
	else {
	    if (is_2var_form)
	        /* On construit une RCG en "2var-form" */
	        two_var_form ();
	    else {
	        if (lfsa_file || rfsa_file)
		  lrfsa ();

	        if (lbnf_file || rbnf_file)
		  lrbnf ();
	    }
	}

	if (tig_file)
	  tig_form ();
	else {
	  rcg_gen_parser ();

	  if (is_gen_bnf)
	    gen_bnf ();
	}

	if (Ak2min) sxfree (Ak2min), Ak2min = NULL;
	if (Ak2max) sxfree (Ak2max), Ak2max = NULL;
	if (Ak2len) sxfree (Ak2len), Ak2len = NULL;
	if (nt2IeqJ_disp) sxfree (nt2IeqJ_disp), nt2IeqJ_disp = NULL;
	if (nt2IeqJ) sxfree (nt2IeqJ), nt2IeqJ = NULL;

	if (is_1_RCG) {
	    sxfree (guide_clause2clause), guide_clause2clause = NULL;
	    sxfree (guide_clause2clause_id), guide_clause2clause_id = NULL;
	    sxfree (guide_clause2arg_pos), guide_clause2arg_pos = NULL;
	    sxfree (clause2rhs_pos), clause2rhs_pos = NULL;
	    sxfree (clausek2guide_Aclause), clausek2guide_Aclause = NULL;
	    sxfree (clausek2guide_clause), clausek2guide_clause = NULL;
	    sxfree (rhs_pos2disp), rhs_pos2disp = NULL;
	    sxfree (rhs_pos2lhs_pos), rhs_pos2lhs_pos = NULL;
	    sxfree (rhs_pos2lhs_arg_no), rhs_pos2lhs_arg_no = NULL;
	    sxfree (rhs_pos2rhs_nt), rhs_pos2rhs_nt = NULL;
	    sxfree (rhs_pos2rhs_pos_1rcg), rhs_pos2rhs_pos_1rcg = NULL;
	    sxfree (Ak2guide_nt), Ak2guide_nt = NULL;
	}
	else {
	    if (is_2var_form) {
	        sxfree (from2var2clause), from2var2clause = NULL;
	        sxfree (from2var2clause_disp), from2var2clause_disp = NULL;
	        sxfree (from2var2clause_list), from2var2clause_list = NULL;
	        sxfree (clause2rhs_stack_map), clause2rhs_stack_map = NULL;
	        sxfree (rhs_stack_map), rhs_stack_map = NULL;
	    }
	}
    }
      
    if (clause_set) {
      XxY_free (&memel);
      sxfree (memel2val), memel2val = NULL;
      sxfree (clause_stack), clause_stack = NULL;
      sxfree (clause_set), clause_set = NULL;
    }

    if (negative_nt_set) sxfree (negative_nt_set), negative_nt_set = NULL;

    nt2clause_free ();

    if (loop_clause_set) sxfree (loop_clause_set), loop_clause_set = NULL;
    if (loop_nt_set) sxfree (loop_nt_set), loop_nt_set = NULL;

    if (empty_clause_set) sxfree (empty_clause_set), empty_clause_set = NULL;
#if 0
    if (lhs_var_set) sxfree (lhs_var_set), lhs_var_set = NULL;
#endif /* 0 */
    if (first) sxbm_free (first), first = NULL;
    if (last) sxbm_free (last), last = NULL;

    if (A2A0) sxfree (A2A0), A2A0 = NULL;
    if (A02A) sxfree (A02A), A02A = NULL;

    if (counter_Ak_set) sxfree (counter_Ak_set), counter_Ak_set = NULL;
    if (non_linear_Ak_set) sxfree (non_linear_Ak_set), non_linear_Ak_set = NULL;
    if (clause2non_right_linear_Ak_set) {
      sxbm_free (clause2non_right_linear_Ak_set), clause2non_right_linear_Ak_set = NULL;
      sxfree (non_right_linear_clause_set), non_right_linear_clause_set = NULL;
    }

    if (clause2Aclause) sxfree (clause2Aclause), clause2Aclause = NULL;
    if (clause2clause0) sxfree (clause2clause0), clause2clause0 = NULL;

    sxfree (ste2nt), ste2nt = NULL;
    sxfree (nt2ste), nt2ste = NULL;

    if (nt2arity)
	/* Fait ds rcg_gen_parser */
	sxfree (nt2arity), nt2arity = NULL; 

    sxfree (is_lhs_nt), is_lhs_nt = NULL;
    sxfree (is_rhs_nt), is_rhs_nt = NULL;
    sxfree (is_nt_external), is_nt_external = NULL;
    sxfree (is_nt_internal), is_nt_internal = NULL;
    sxfree (ste2t), ste2t = NULL;
    sxfree (ste2var), ste2var = NULL;
    sxfree (var2ste), var2ste = NULL;
    sxfree (var2param), var2param = NULL;
    XxY_free (&rcg_clauses);
    XH_free (&rcg_rhs);
    sxfree (clause2visited), clause2visited = NULL;
    sxfree (clause2lhs_var), clause2lhs_var = NULL;
    sxfree (clause2max_var), clause2max_var = NULL;
    sxfree (clause2bottom_up_erasing), clause2bottom_up_erasing = NULL;
    sxfree (clause2non_range_arg), clause2non_range_arg = NULL;
    sxfree (clause2action), clause2action = NULL;
    sxfree (false_clause_set), false_clause_set = NULL;
    sxfree (new_false_clause_stack), new_false_clause_stack = NULL;
    sxfree (false_A_set), false_A_set = NULL;
    XH_free (&rcg_predicates);
    XH_free (&rcg_parameters);

    if (has_an_identical_clause_set) {
	sxfree (has_an_identical_clause_set), has_an_identical_clause_set = NULL;
	sxfree (clause2identical), clause2identical = NULL;
    }

    if (has_an_identical_nt_set) {
	sxfree (has_an_identical_nt_set), has_an_identical_nt_set = NULL;
	sxfree (nt2identical), nt2identical = NULL;
    }

    {
      SXINT smppass_clause_3;
      SXBA *M;

      for (smppass_clause_3 = 1; smppass_clause_3 <= rule_nb; smppass_clause_3++) {
	if ((M = clause2attr_disp [smppass_clause_3]))
	  sxbm_free (M);
      }

      sxfree (clause2attr_disp), clause2attr_disp = NULL;
    }
}

void
rcg_smp (SXINT what, SXTABLES *sxtables_ptr)
{
    switch (what) {
    case SXOPEN:
	sxtab_ptr = sxtables_ptr;
	smpopen ();
	break;
    case SXSEMPASS:		/* For [sub-]tree evaluation during parsing */
	break;
    case SXACTION:
	smppass ();
	break;
    case SXCLOSE:
	if (clause2da_disp)
	    sxfree (clause2da_disp), clause2da_disp = NULL;

	if (clause2da)
	    sxfree (clause2da), clause2da = NULL;
	
	if (t2ste)
	    sxfree (t2ste), t2ste = NULL;

	break;
    default:
	break;
    }
}
