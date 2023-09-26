/*------------------------------*/
/*          rcg.h          	*/
/*------------------------------*/
/*				*/
/*  Global VARIABLES for rcg	*/
/*				*/
/*------------------------------*/



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 19991209 18:47 (phd):	"extern" pour "by_mess"			*/
/************************************************************************/
/* 19991209 18:46 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "rcg_td.h"
#include "XH.h"
#include "XxY.h"

#if STATS
#define statistics	stdout
#else
#define statistics	NULL
#endif


extern BOOLEAN	IS_AND ();

long		options_set;
BOOLEAN		sxverbosep, is_listing_output, check_self_definition, is_CF_parser, check_set, is_OLTAG, is_1_RCG, is_gen_bnf;
BOOLEAN		decrease_order, is_simple, is_proper, is_2var_form, is_factorize, is_combinatorial, is_full_lex;
BOOLEAN		is_left_corner, used_nfndfsa, is_first_last, is_look_ahead, is_tagger, is_shift_reduce, is_shallow;
BOOLEAN         is_keep_lex, is_keep_epsilon_rules, check_instantiable_clause;
int             h_value;
char		*prgentname, *suffixname, *lfsa_file_name, *rfsa_file_name, *shallow_file_name, *lbnf_file_name, *rbnf_file_name, *Lex_file_name, *lrprod_file_name, *tig_file_name;
extern char	by_mess [];
FILE		*listing, *lfsa_file, *rfsa_file, *useless_clause_file, *shallow_file, *lbnf_file, *rbnf_file, *Lex_file, *lrprod_file, *tig_file;
struct sxtables	*sxtab_ptr;

#define IS_ERROR (sxerrmngr.nbmess[2]>0) /* nombre de messages d'erreurs */

#define release "release 0.1"

#define SXTOP	((int)(((unsigned int)(~0))>>1))	/* + grand entier positif */

struct rcg_node {
    SXNODE_HEADER_S VOID_NAME;

/*
your attribute declarations...
*/
};


#define S1_size(t)	(t)[-1]
#define S1_top(t)	(t)[0]
#define S1_clear(t)	S1_top(t)=0
#define S1_alloc(t,l)	(t=(int*) sxalloc(l+2,sizeof(int))+1),S1_size(t)=(l),S1_clear(t)
#define S1_free(t)	sxfree((t)-1)
#define S1_is_empty(t)	S1_top(t)==0
#define S1_sature(t)	S1_size(t)*=2,t=(int*)sxrealloc(t-1,S1_size(t)+2,sizeof(int))+1
#define S1_push(t,x)	(t[++(S1_top(t))]=(x)),((S1_top(t)>=S1_size(t))?(S1_sature(t)):NULL)
#define S1_pop(t)	t[S1_top(t)--]
#define S1_get(t,i)	t[i]

#include "sxstack.h"

/* ATTENTION, voir ds rcg_gen_parser la generation de "static int nt2arity [] = {..." */
#define	STREQ_ic	-1
#define	STREQLEN_ic	-2
#define	STRLEN_ic	-3
#define	FALSE_ic	-4
#define	FIRST_ic	-5
#define	LAST_ic		-6
#define	TRUE_ic		-7
#define	LEX_ic		-8
/* ATTENTION, CUT_ic doit avoir le +petit code des predefinis !! */
#define	CUT_ic		-9


#define A_k2Ak(A,k)	(A2A0[A]+k)
#define Ak2A(Ak)	A02A[Ak]
#define Ak_A2k(Ak,A)	(Ak-A2A0[A])

#define clause_k2clausek(clause,k)	(clause2clause0[clause]+k)

BOOLEAN		is_drcg;

int		lhs_prdct_filter; /* si check_set filtre la valeur du lhs_prdct des rcg_clauses */
int		max_gvar, max_var, component_nb, max_lhs_adr, max_Ak, max_garity, max_nt, max_t, last_clause; 
int		max_clause_arg_nb, max_rhs_arg_nb, max_arg_size, max_rhs_prdct, max_gcomponent_nb, max_clausek, gsize;
int		max_prdct_nb, rule_nb;
BOOLEAN		sxttycol1p, is_start_symbol_in_rhs, has_first_last_pp, has_sem_act;
BOOLEAN		is_bottom_up_erasing, is_top_down_erasing, is_left_linear, is_right_linear, is_loop_grammar, is_combinatorial_grammar;
BOOLEAN         is_proper_grammar, is_simple_grammar;


int		*A2A0, *A02A, *clause2clause0, *clause2Aclause, *clausek2guide_Aclause, *clausek2guide_clause;
int		*ste2nt, *nt2ste, *nt2arity, *ste2t, *t2ste, *ste2var, *var2ste;
int		*clause2lhs_var, *clause2max_var, *clause2son_nb, *clause2action;
SXBA		empty_clause_set, clause2bottom_up_erasing, clause2non_range_arg, false_clause_set, false_A_set;
int             *new_false_clause_stack;
SXBA		loop_clause_set, loop_nt_set;
SXBA		is_lhs_nt, is_rhs_nt, is_nt_external, is_nt_internal;
SXBA		*first, *last, *lhsnt2clause;
XxY_header	rcg_clauses;
XH_header	rcg_rhs, rcg_predicates, rcg_parameters;
struct rcg_node	**clause2visited;
int		*Ak2min, *Ak2max, *Ak2len, *nt2IeqJ_disp, *nt2IeqJ;
int		guide_clause, *guide_clause2clause, *guide_clause2arg_pos, *guide_clause2clause_id;
int		guide_lognt, guide_max_clause_id;
SXBA		has_an_identical_clause_set, has_an_identical_nt_set;
int		*clause2identical, *nt2identical;
BOOLEAN		has_identical_clauses, has_identical_nts;
int		*clause2rhs_pos, *rhs_pos2disp, *rhs_pos2lhs_pos, *rhs_pos2lhs_arg_no, *rhs_pos2rhs_nt, *rhs_pos2rhs_pos_1rcg;
int		*Ak2guide_nt;
int		*from2var2clause, *from2var2clause_disp, *from2var2clause_list, from2var2clause_list_top;
int		new_clause_nb, new_max_nt;
int		*clause2rhs_stack_map, *rhs_stack_map, rhs_stack_map_top;

BOOLEAN         is_negative;
SXBA            negative_nt_set;
  
SXBA            **clause2attr_disp;

/* Pour Definite RCG */
int 		*clause2da_disp, *clause2da, da_top, da_size;


/* Pour acceder au numero de release courant */
#define rcg_sglbl_h
#include "rcg_sglbl.h"
/*
#define CURRENT_RCG_PARSER_RELEASE p
*/
