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

/*------------------------------*/
/*          rcg.h          	*/
/*------------------------------*/
/*				*/
/*  Global VARIABLES for rcg	*/
/*				*/
/*------------------------------*/

/* Chaque déclaration de variable doit aussi être une définition de cette
   variable, selon la valeur donnée à SX_GLOBAL_VAR_RCG : "extern" ou rien.

   Typiquement, cela donne, pour la variable "variable" de type "type"
   et de valeur initiale "valeur" :

   SX_GLOBAL_VAR_RCG  type  variable  SX_INIT_VAL(valeur);
*/

#ifndef SX_GLOBAL_VAR_RCG /* Inutile de le faire plusieurs fois */
#  ifdef SX_DFN_EXT_VAR_RCG
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_RCG	/*rien*/
#    define SX_INIT_VAL_RCG(v)	= v
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_RCG	extern
#    define SX_INIT_VAL_RCG(v)	/*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_RCG */
#endif /* #ifndef SX_GLOBAL_VAR_RCG */


#include "rcg_td.h"
#include "XH.h"
#include "XxY.h"

#if defined(SXVERSION) && STATS
#define statistics	stdout
#else
#define statistics	NULL
#endif


extern bool	IS_AND (SXBA lhs_bits_array, SXBA rhs_bits_array);

SX_GLOBAL_VAR_RCG SXINT	options_set;
SX_GLOBAL_VAR_RCG bool		is_listing_output, check_self_definition, is_CF_parser, check_set, is_OLTAG, is_1_RCG, is_gen_bnf;
SX_GLOBAL_VAR_RCG bool		decrease_order, is_simple, is_proper, is_2var_form, is_factorize, is_combinatorial, is_full_lex;
SX_GLOBAL_VAR_RCG bool		is_left_corner, used_nfndfsa, is_first_last, is_look_ahead, is_tagger, is_shift_reduce, is_shallow;
SX_GLOBAL_VAR_RCG bool             is_keep_lex, is_keep_epsilon_rules, check_instantiable_clause, is_col1;
SX_GLOBAL_VAR_RCG SXINT                 h_value;
SX_GLOBAL_VAR_RCG char		        *prgentname, *suffixname, *lfsa_file_name, *rfsa_file_name, *shallow_file_name, *lbnf_file_name, *rbnf_file_name, *Lex_file_name, *lrprod_file_name, *tig_file_name, *terminal_file_name;
SX_GLOBAL_VAR_RCG char	                *by_mess;
SX_GLOBAL_VAR_RCG FILE		        *listing, *lfsa_file, *rfsa_file, *useless_clause_file, *shallow_file, *lbnf_file, *rbnf_file, *Lex_file, *lrprod_file, *tig_file, *terminal_file;
SX_GLOBAL_VAR_RCG struct sxtables	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

#define release "release 0.1"

#define SXTOP	((((SXUINT)(~0))>>1))	/* + grand entier positif */

struct rcg_node {
    SXNODE_HEADER_S SXVOID_NAME;

/*
your attribute declarations...
*/
};


#define S1_size(t)	(t)[-1]
#define S1_top(t)	(t)[0]
#define S1_clear(t)	S1_top(t)=0
#define S1_alloc(t,l)	(t=(SXINT*) sxalloc(l+2,sizeof(SXINT))+1),S1_size(t)=(l),S1_clear(t)
#define S1_free(t)	sxfree((t)-1)
#define S1_is_empty(t)	(S1_top(t)==0)
#define S1_sature(t)	S1_size(t)*=2,t=(SXINT*)sxrealloc(t-1,S1_size(t)+2,sizeof(SXINT))+1
#define S1_push(t,x)	(t[++(S1_top(t))]=(x)),((S1_top(t)>=S1_size(t))?(S1_sature(t)):NULL)
#define S1_pop(t)	t[S1_top(t)--]
#define S1_get(t,i)	t[i]

#include "sxstack.h"

/* ATTENTION, voir ds rcg_gen_parser la generation de "static SXINT nt2arity [] = {..." */
#define	STREQ_ic	((SXINT)-1)
#define	STREQLEN_ic	((SXINT)-2)
#define	STRLEN_ic	((SXINT)-3)
#define	FALSE_ic	((SXINT)-4)
#define	FIRST_ic	((SXINT)-5)
#define	LAST_ic		((SXINT)-6)
#define	TRUE_ic		((SXINT)-7)
#define	LEX_ic		((SXINT)-8)
/* ATTENTION, CUT_ic doit avoir le +petit code des predefinis !! */
#define	CUT_ic		((SXINT)-9)


#define A_k2Ak(A,k)	(A2A0[A]+k)
#define Ak2A(Ak)	A02A[Ak]
#define Ak_A2k(Ak,A)	(Ak-A2A0[A])

#define clause_k2clausek(clause,k)	(clause2clause0[clause]+k)

SX_GLOBAL_VAR_RCG bool		is_drcg;

SX_GLOBAL_VAR_RCG SXINT		lhs_prdct_filter; /* si check_set filtre la valeur du lhs_prdct des rcg_clauses */
SX_GLOBAL_VAR_RCG SXINT		max_gvar, max_var, component_nb, max_lhs_adr, max_Ak, max_garity, max_nt, max_t, last_clause; 
SX_GLOBAL_VAR_RCG SXINT		max_clause_arg_nb, max_rhs_arg_nb, max_arg_size, max_rhs_prdct, max_gcomponent_nb, max_clausek, gsize;
SX_GLOBAL_VAR_RCG SXINT		max_prdct_nb, rule_nb;
SX_GLOBAL_VAR_RCG bool	sxttycol1p, is_start_symbol_in_rhs, has_first_last_pp, has_sem_act;
SX_GLOBAL_VAR_RCG bool	is_bottom_up_erasing_grammar, is_top_down_erasing_grammar, is_left_linear_grammar, is_right_linear_grammar, is_loop_grammar, is_combinatorial_grammar;
SX_GLOBAL_VAR_RCG bool     is_proper_grammar, is_simple_grammar, is_overlapping_grammar, is_a_complete_terminal_grammar;

SX_GLOBAL_VAR_RCG SXINT		*A2A0, *A02A, *clause2clause0, *clause2Aclause, *clausek2guide_Aclause, *clausek2guide_clause;
SX_GLOBAL_VAR_RCG SXINT		*ste2nt, *nt2ste, *nt2arity, *ste2t, *t2ste, *ste2var, *var2ste;
SX_GLOBAL_VAR_RCG SXINT		*clause2lhs_var, *clause2max_var, *clause2son_nb, *clause2action;
SX_GLOBAL_VAR_RCG SXBA		empty_clause_set, clause2bottom_up_erasing, clause2non_range_arg, false_clause_set, false_A_set;
SX_GLOBAL_VAR_RCG SXINT         *new_false_clause_stack;
SX_GLOBAL_VAR_RCG SXBA		loop_clause_set, loop_nt_set;
SX_GLOBAL_VAR_RCG SXBA		is_lhs_nt, is_rhs_nt, is_nt_external, is_nt_internal;
SX_GLOBAL_VAR_RCG SXBA		*first, *last, *lhsnt2clause;
SX_GLOBAL_VAR_RCG XxY_header	rcg_clauses;
SX_GLOBAL_VAR_RCG XH_header	rcg_rhs, rcg_predicates, rcg_parameters;
SX_GLOBAL_VAR_RCG struct rcg_node	**clause2visited;
SX_GLOBAL_VAR_RCG SXINT		*Ak2min, *Ak2max, *Ak2len, *nt2IeqJ_disp, *nt2IeqJ;
SX_GLOBAL_VAR_RCG SXINT		guide_clause, *guide_clause2clause, *guide_clause2arg_pos, *guide_clause2clause_id;
SX_GLOBAL_VAR_RCG SXINT		guide_lognt, guide_max_clause_id;
SX_GLOBAL_VAR_RCG SXBA		has_an_identical_clause_set, has_an_identical_nt_set;
SX_GLOBAL_VAR_RCG SXINT		*clause2identical, *nt2identical;
SX_GLOBAL_VAR_RCG bool	has_identical_clauses, has_identical_nts;
SX_GLOBAL_VAR_RCG SXINT		*clause2rhs_pos, *rhs_pos2disp, *rhs_pos2lhs_pos, *rhs_pos2lhs_arg_no, *rhs_pos2rhs_nt, *rhs_pos2rhs_pos_1rcg;
SX_GLOBAL_VAR_RCG SXINT		*Ak2guide_nt;
SX_GLOBAL_VAR_RCG SXINT		*from2var2clause, *from2var2clause_disp, *from2var2clause_list, from2var2clause_list_top;
SX_GLOBAL_VAR_RCG SXINT		new_clause_nb, new_max_nt;
SX_GLOBAL_VAR_RCG SXINT		*clause2rhs_stack_map, *rhs_stack_map, rhs_stack_map_top;

SX_GLOBAL_VAR_RCG bool     is_negative;
SX_GLOBAL_VAR_RCG SXBA          negative_nt_set;
  
SX_GLOBAL_VAR_RCG SXBA          **clause2attr_disp;

SX_GLOBAL_VAR_RCG SXINT         max_radius /* max val des l ds &StrLen(l, X) */, max_ste;
SX_GLOBAL_VAR_RCG SXBA          counter_Ak_set /* Contient les Ak des predicats qui sont des valeurs entieres */;
SX_GLOBAL_VAR_RCG SXBA          non_linear_Ak_set /* Contient les Ak des predicats impliques dans de la non linearite' et sur lesquels on doit calculer des fsa */;

SX_GLOBAL_VAR_RCG SXBA          *clause2non_right_linear_Ak_set, non_right_linear_clause_set;  

/* Pour Definite RCG */
SX_GLOBAL_VAR_RCG SXINT 	*clause2da_disp, *clause2da, da_top, da_size;


/* Pour acceder au numero de release courant et aux operations NL_* */
#include "rcg_sglbl.h"
/*
#define CURRENT_RCG_PARSER_RELEASE p
*/
