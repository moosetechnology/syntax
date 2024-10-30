/* ********************************************************************
   *  This program has been generated from dcg.at                     *
   *  on Thu Feb 27 10:12:17 1997                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */


/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1996 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/* Genere a partir d'une DCG
   - la [x]BNF correspondante
   - les tables des cdg sont produites en  C ds L_dcgt.c */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Mar 25 Fev 1997 14:45 (pb):	Expressions arithmetiques, rel et is	*/
/************************************************************************/
/* Mer 19 Fev 1997 13:25 (pb):	Prise en compte des variables comme 	*/
/*				token					*/
/************************************************************************/
/* Ven 27 Dec 1996 10:27 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT_DCG_SMP[] = "@(#)dcg_smp.c	- SYNTAX [unix] - Mar 25 Fev 1997 14:45:21";

static char	ME [] = "dcg_smp";


/* Si une variable est un element d'une liste de terminaux :
   [a, X, b], on genere "a" <Atom>(X) "b" et la production
   <Atom> = %dcg_atom ;
   L'utilisateur doit decrire au niveau lexical %dcg_atom qui
   doit couvrir l'ensemble des terminaux que l'on s'apprete a reconnaitre dynamiquement.
   Dynamiquement, a la reconnaissance de la regle <Atom> = %dcg_atom ; on met dans la variable
   associee a <Atom> l'atome dynamique (tok_no, DYNAM_ATOM) ou tok_no est le numero du token
   Ca permet de retrouver son source, etc...
   */


/*   I N C L U D E S   */
#include "sxversion.h"
#include "varstr.h"
#define SXNODE struct dcg_node
#include "sxunix.h"
#include "dcg.h"
#include "XH.h"
#include "XxY.h"
#include "sxba.h"
#include <ctype.h>

struct dcg_node {
    SXNODE_HEADER_S SXVOID_NAME;

/*
your attribute declarations...
*/
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ARGUMENT_S_n 2
#define BODY_S_n 3
#define CATENATE_n 4
#define DCG_n 5
#define DIV_n 6
#define EMPTY_LIST_n 7
#define IS_n 8
#define LESS_n 9
#define LESS_EQUAL_n 10
#define LHS_n 11
#define LIST_BODY_n 12
#define MAIN_FUNCTOR_n 13
#define MINUS_n 14
#define MOD_n 15
#define MULT_n 16
#define PLUS_n 17
#define PROLOG_S_n 18
#define RHS_ITEM_S_n 19
#define RULE_n 20
#define RULE_S_n 21
#define SUP_n 22
#define SUP_EQUAL_n 23
#define TERM_n 24
#define TERMINAL_S_n 25
#define UNARY_MINUS_n 26
#define VOID_n 27
#define atom_n 28
#define floating_point_number_n 29
#define integer_number_n 30
#define string_n 31
#define variable_n 32
/*
E N D   N O D E   N A M E S
*/

#include "dcg_cst.h"

static bool		sxttycol1p;
static int		max_ste;
static int		lhs_functor, ste, lhs_or_rhs, bnf_dum_post_act;
static SXINT		dcg_lhs_part, dcg_rhs_part;
static char		*str, *lhs_functor_string;

static VARSTR		vstr;
static int		xprod, rhs_symb_nb;
static XH_header	dcg_part [2];
static XxY_header	dcg_rules;
static int		*dcg_rule_to_line_no;

static XH_header	bnf_rules;
static int		*ste2nt, *ste2t, *nt2ste, *t2ste;
static SXBA		dum_bnf_rule;
static int		BNFmaxt, BNFmaxnt, BNFmaxrhsnt, BNFprod, BNFmaxitem, rhsnt;
static int		*BNFlhs, *BNFprolon, *BNFlispro, *BNFprod2nbnt;
static char		**BNFntstring;

static int		lhs_param_nb, max_param_nb;
static int		*DCGlhs2param_nb, *DCGrhs2param_nb;
static int		*DCGlhs_functor2param, *DCGrhs_functor2param, *DCGmain_terms;
static int		DCGmain_terms_top, DCGmain_terms_size, lhs_main_terms_top;

static int		SXDCGmax_atom, SXDCGterm_size;
static int		*ste2atom, *atom2ste;

static int		SXDCGmax_var;
static int		*ste2var, *var_stack, var_stack_top, *prod2var_nb;
static int		lhs_var_nb, _ste;

static XH_header	term_hd;
static XxY_header	list_hd;
static int		*term_stack, term_stack_size, term_stack_top, tss;
static int		max_arg_per_prod, lhs_arg_nb, rhs_arg_nb;

static int		TERM_lgth, TERM_nb, LIST_nb;


/* Gestion des chaines formant les noms des variables */
static int		SXDCGmax_gvar, SXDCGmax_gvarppp, gvar_nb, *ste2gvar, *gvar2ste;
static SXINT		*SXDCG2var_disp;
static XH_header	SXDCGvar_names;

/* On a des expression arithmetiques, elles sont stockees comme des termes */
static bool		has_integer_op;

/* Le prolog accepte */
static bool		is_prolog, has_prolog, is_dynam_constant, is_prolog_tree;
static int		Dcg_constant_ste;
static int		*t2kind;
static int		*DCGprod2dynam_constant;
static int		*PPPprolon, *PPPlispro, PPPmaxitem, ppp_symb_nb, maxpppprod;
static int		*PPPmain_terms, PPPmain_terms_top, PPPmain_terms_size;

#define prolog_name_nb	12
#define prolog_code_nb	11
#define prolog_param_nb	3

static int prolog_name2prolog_code [prolog_name_nb+1] = {
    0,
/* 1 */    MEMBER		/* member */,
/* 2 */    MEMBER		/* in */,
/* 3 */    CONCAT		/* concat */,
/* 4 */    SIZE			/* size */,
/* 5 */    NTH			/* nth */,
/* 6 */    REVERSE		/* reverse */,
/* 7 */    LESS			/* < */,
/* 8 */    SUP			/* > */,
/* 9 */    LESS_EQUAL		/* =< */,
/* 10 */   SUP_EQUAL		/* >= */,
/* 11 */   RANDOM		/* random */,
/* 12 */   IS			/* is */,
};

#if 0
static int prolog_code2prolog_name [prolog_code_nb+1] = {
    0,
/* 1 */    /* concat */ 3,
/* 2 */    /* member, in */ 1, /* member est la classe d'equivalence */
/* 3 */    /* size */ 4,
/* 4 */    /* nth */ 5,
/* 5 */    /* reverse */ 6,
/* 6 */    /* < */7,
/* 7 */    /* > */8,
/* 8 */    /* =< */9,
/* 9 */    /* >= */10,
/* 10 */    /* random */11,
/* 11 */    /* is */12,
};
#endif

static char *prolog_name2str [prolog_name_nb+1] = {
    "",
    "member",
    "in",
    "concat",
    "size",
    "nth",
    "reverse",
    "<",
    ">",
    "=<",
    ">=",
    "random",
    "is",
};

#if 0
static char *prolog_code2nt_str [prolog_code_nb+1] = {
    "",
    "Concat",
    "Member",
    "Size",
    "Nth",
    "Reverse",
    "Less",
    "Sup",
    "Less_Equal",
    "Sup_Equal",
    "Random",
    "Is",
};
#endif

static char *ppp2str [] = {
    "",
    "concat",
    "member",
    "size",
    "nth",
    "reverse",
    "<",
    ">",
    "=<",
    ">=",
    "random",
    "is",
    "/",
    "-" /* binary minus */,
    "mod",
    "*",
    "+",
    "-" /* unary minus */,
};

static int prolog_code2param_nb [prolog_code_nb+1] = {
    0,
    3				/* CONCAT */,
    2				/* MEMBER */,
    2				/* SIZE */,
    3				/* NTH */,
    2				/* REVERSE */,
    2				/* LESS */,
    2				/* SUP */,
    2				/* LESS_EQUAL */,
    2				/* SUP_EQUAL */,
    2				/* RANDOM */,
    2				/* IS */,
};

static int prolog_code2param_type [prolog_code_nb+1] [prolog_param_nb] = {
{0},
/* CONCAT(List,List,List) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<STATIC_LIST) , (1<<VARIABLE) + (1<<STATIC_LIST)},
/* MEMBER(Elem,List) */ {(1<<ATOM) + (1<<VARIABLE) + (1<<TERM) + (1<<STATIC_LIST) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<STATIC_LIST)},
/* SIZE(List,Int) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* NTH(List,Int,Elem) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<ATOM) + (1<<VARIABLE) + (1<<TERM) + (1<<STATIC_LIST) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* REVERSE(List,List) */ {(1<<VARIABLE) + (1<<STATIC_LIST), (1<<VARIABLE) + (1<<STATIC_LIST)},
/* LESS(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* SUP(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* LESS_EQUAL(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* SUP_EQUAL(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* RANDOM(Int,Int) */  {(1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP), (1<<VARIABLE) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
/* IS(Var,Elem) */  {(1<<VARIABLE), (1<<ATOM) + (1<<VARIABLE) + (1<<TERM) + (1<<STATIC_LIST) + (1<<INTEGER_CONSTANT) + (1<<INTEGER_OP)},
};

static int	*ste2prolog_code;
#if 0
static int	prolog_code2nt [prolog_code_nb+1];
static int	*nt2prolog_code;
#endif
static SXBA	*gvar2pppitem_set;

static int	plus_ste, minus_ste, uminus_ste, mult_ste, div_ste, mod_ste,
                is_ste, less_ste, less_equal_ste, sup_ste, sup_equal_ste;

static void process_nt (int ste0, bool is_nt)
{
    if (is_nt) {
	if (ste2nt [ste0] == 0) {
	    ste2nt [ste0] = ++BNFmaxnt;
	    nt2ste [BNFmaxnt] = ste0;
	    BNFntstring [BNFmaxnt] = sxstrget (ste0);
	}

	XH_push (bnf_rules, ste0);

	if (lhs_or_rhs == 1) 
	    rhs_symb_nb++;
    }

    XH_push (dcg_part [lhs_or_rhs], ste0);

    if (tss > term_stack_size)
	term_stack_size = tss;
}


static void dcg_pi ()
{

    /*
       I N H E R I T E D
       */

    switch (SXVISITED->father->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	break;

    case BODY_S_n :		/* SXVISITED->name = RHS_ITEM_S_n */
	rhs_arg_nb = 0;

	if (SXVISITED->position > 1) {
	    /* On visite les alternatives d'une clause */
	    XH_push (bnf_rules, lhs_functor);
	}
	break;

    case CATENATE_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {CATENATE_n, EMPTY_LIST_n, LIST_BODY_n, variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #1");
	}

	break;

    case DCG_n :		/* SXVISITED->name = RULE_S_n */
	break;

    case DIV_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #2");
	}

	break;

    case IS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = variable_n */
	    break;

	case 2 :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #3");
	}

	break;

    case LESS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #4");
	}

	break;

    case LESS_EQUAL_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #5");
	}

	break;

    case LHS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = MAIN_FUNCTOR_n */
	    lhs_functor = SXVISITED->son->token.string_table_entry;
	    break;

	case 2 :		/* SXVISITED->name = {TERMINAL_S_n, VOID_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #6");
	}

	break;

    case LIST_BODY_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {CATENATE_n, LIST_BODY_n, VOID_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #7");
	}

	break;

    case MAIN_FUNCTOR_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = atom_n */
	    break;

	case 2 :		/* SXVISITED->name = {ARGUMENT_S_n, VOID_n} */
	    tss = 0;
	    break;

	default :
	    sxtrap (ME, "unknown switch case #8");
	}

	break;

    case MINUS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #9");
	}

	break;

    case MOD_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #10");
	}

	break;

    case MULT_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #11");
	}

	break;

    case PLUS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #12");
	}

	break;

    case PROLOG_S_n :		/* SXVISITED->name = {IS_n, LESS_n, LESS_EQUAL_n, SUP_n, SUP_EQUAL_n, TERM_n} */
	tss = 0;
	is_prolog_tree = true;
	break;

    case RHS_ITEM_S_n :		/* SXVISITED->name = {MAIN_FUNCTOR_n, PROLOG_S_n, TERMINAL_S_n, VOID_n} */
	break;

    case RULE_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = LHS_n */
	    lhs_or_rhs = 0;
	    lhs_arg_nb = 0;
	    break;

	case 2 :		/* SXVISITED->name = BODY_S_n */
	    XH_set (&dcg_part [lhs_or_rhs], &dcg_lhs_part);
	    lhs_or_rhs = 1;
	    break;

	default :
	    sxtrap (ME, "unknown switch case #13");
	}

	break;

    case RULE_S_n :		/* SXVISITED->name = RULE_n */
	break;

    case SUP_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #14");
	}

	break;

    case SUP_EQUAL_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #15");
	break;
	}
	break;

    case TERM_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = atom_n */
	    break;

	case 2 :		/* SXVISITED->name = ARGUMENT_S_n */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #16");
	}

	break;

    case TERMINAL_S_n :		/* SXVISITED->name = {atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	break;

    case UNARY_MINUS_n :/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
			   integer_number_n, variable_n} */
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end dcg_pi */
}

static void dcg_pd ()
{

    /*
       D E R I V E D
       */

    switch (SXVISITED->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :
	if (lhs_or_rhs == 0)
	    lhs_arg_nb += SXVISITED->degree;
	else
	    rhs_arg_nb += SXVISITED->degree;

	tss += SXVISITED->degree;
	break;

    case BODY_S_n :
	break;

    case CATENATE_n :
	LIST_nb++;
	break;

    case DCG_n :
	break;

    case DIV_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (dcg_part [lhs_or_rhs], div_ste);
	break;

    case EMPTY_LIST_n :
	tss++;

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DCGmain_terms_size++;

	break;

    case IS_n :
	process_nt (is_ste, false);
	break;

    case LESS_n :
	process_nt (less_ste, false);
	break;

    case LESS_EQUAL_n :
	process_nt (less_equal_ste, false);
	break;

    case LHS_n :
	break;

    case LIST_BODY_n :
	tss++;
	LIST_nb++;

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DCGmain_terms_size++;

	break;

    case MAIN_FUNCTOR_n :
	process_nt (SXVISITED->son->token.string_table_entry, true);
	break;

    case MINUS_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (dcg_part [lhs_or_rhs], minus_ste);
	break;

    case MOD_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (dcg_part [lhs_or_rhs], mod_ste);
	break;

    case MULT_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (dcg_part [lhs_or_rhs], mult_ste);
	break;

    case PLUS_n :
	TERM_nb++;
	TERM_lgth += 3;
	tss++;
	XH_push (dcg_part [lhs_or_rhs], plus_ste);
	break;

    case PROLOG_S_n :
	is_prolog = true;
	is_prolog_tree = false;
	ppp_symb_nb += SXVISITED->degree;
	break;

    case RHS_ITEM_S_n :
	{	
	SXINT rule_no;
	int nb;

	XH_set (&dcg_part [lhs_or_rhs], &dcg_rhs_part);

	if (XxY_set (&dcg_rules, dcg_lhs_part, dcg_rhs_part, &rule_no)) {
	    /* La regle existe deja en dcg */
	    /* La specig DCG n'est pas un ensemble */
	    sxerror (SXVISITED->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sIdentical with rule at line #%i.",
			 sxtab_ptr->err_titles [2]+1,
			 dcg_rule_to_line_no [rule_no]);
	}
	else {
	    SXINT bnf_rule_no;

	    dcg_rule_to_line_no [rule_no] = SXVISITED->token.source_index.line;

	    if (XH_set (&bnf_rules, &bnf_rule_no)) {
		/* La regle existe deja en bnf */
		SXBA_1_bit (dum_bnf_rule, rule_no);
	    }
	}

	if ((nb = lhs_arg_nb + rhs_arg_nb) > max_arg_per_prod)
	    max_arg_per_prod = nb;
    }
	break;

    case RULE_n :
	break;

    case RULE_S_n :
	break;

    case SUP_n :
	process_nt (sup_ste, false);
	break;

    case SUP_EQUAL_n :
	process_nt (sup_equal_ste, false);
	break;

    case TERM_n :
	if (SXVISITED->father->name == PROLOG_S_n) {
	    ste = SXVISITED->son->token.string_table_entry;

	    if (ste2prolog_code [ste] == 0)
		/* ppp fonctionnel inconnu */
		sxerror (SXVISITED->son->token.source_index,
			     sxtab_ptr->err_titles [2][0],
			     "%sThis prolog predicate is not (yet?) implemented.",
			     sxtab_ptr->err_titles [2]+1);
	    else {
		process_nt (ste, false);
	    }
	}
	else {
	    TERM_nb++;
	    TERM_lgth += SXVISITED->son->brother->degree + 1;
	    tss++;
	}
	break;

    case TERMINAL_S_n :
	break;

    case UNARY_MINUS_n :
	TERM_nb++;
	TERM_lgth += 2;
	tss++;
	XH_push (dcg_part [lhs_or_rhs], uminus_ste);
	break;

    case VOID_n :
	break;

    case atom_n :
	ste = SXVISITED->token.string_table_entry;
	XH_push (dcg_part [lhs_or_rhs], ste);

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DCGmain_terms_size++;

	if (SXVISITED->father->name == TERMINAL_S_n && SXVISITED->father->father->name == RHS_ITEM_S_n) {
	    /* Pour l'instant, on ne traite pas les "terminaux" en LHS. */
	    if (ste2t [ste] == 0) {
		ste2t [ste] = -(++BNFmaxt);
		t2ste [BNFmaxt] = ste;
	    }

	    XH_push (bnf_rules, ste);
	    rhs_symb_nb++;
	}

	break;

    case floating_point_number_n :
	break;

    case integer_number_n :
	ste = SXVISITED->token.string_table_entry;
	XH_push (dcg_part [lhs_or_rhs], ste);

	if (is_prolog_tree)
	    PPPmain_terms_size++;
	else
	    DCGmain_terms_size++;

	if (SXVISITED->father->name == TERMINAL_S_n && SXVISITED->father->father->name == RHS_ITEM_S_n) {
	    /* Pour l'instant, on ne traite pas les "terminaux" en LHS. */
	    if (ste2t [ste] == 0) {
		ste2t [ste] = -(++BNFmaxt);
		t2ste [BNFmaxt] = ste;
	    }

	    XH_push (bnf_rules, ste);
	    rhs_symb_nb++;
	}

	break;

    case string_n :
	break;

    case variable_n :
	ste = SXVISITED->token.string_table_entry;
	XH_push (dcg_part [lhs_or_rhs], ste);
	
	if (is_prolog_tree) {
	    /* On assigne les noms des variables globales en commencant par celles impliquees dans des ppp */
	    PPPmain_terms_size++;

	    if (ste == _ste) {
		gvar2ste [0] = _ste;
		/* ste2gvar [_ste] == 0 */
		/* ste2var [_ste] == 0 */
	    }
	    else {
		if (ste2gvar [ste] == 0) {
		    ste2gvar [ste] = ++SXDCGmax_gvarppp;
		    gvar2ste [SXDCGmax_gvarppp] = ste;
		}
	    }
	}
	else {
	    DCGmain_terms_size++;
	    gvar_nb++;
	}

	if (SXVISITED->father->name == TERMINAL_S_n && SXVISITED->father->father->name == RHS_ITEM_S_n) {
	    /* Pour l'instant, on ne traite pas les "terminaux" en LHS. */
	    /* Pour la variable "X", tout se passe comme si on avait trouve le main functor "dcg_constant(X)" */
	    is_dynam_constant = true;

	    if (ste2nt [Dcg_constant_ste] == 0) {
		ste2nt [Dcg_constant_ste] = ++BNFmaxnt;
		nt2ste [BNFmaxnt] = Dcg_constant_ste;
		BNFntstring [BNFmaxnt] = sxstrget (Dcg_constant_ste);
	    }

	    XH_push (bnf_rules, Dcg_constant_ste);

	    if (term_stack_size == 0)
		term_stack_size = 1;

	    rhs_symb_nb++;
	}
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end dcg_pd */
}



static void gen_ppp (int ste0, int param_nb, SXNODE *node_ptr)
{
    int		code, bot, i, type, param, kind, x;
    /* On verifie le "type" des parametres */
    code = ste2prolog_code [ste0];
    bot = term_stack_top - param_nb;

    for (i = 0; bot++, i < param_nb;i++) {
	type = prolog_code2param_type [code] [i];
	param = term_stack [bot];
	kind = REF2KIND (param);

	if ((type & (1<<kind)) == 0) {
	    sxerror (node_ptr->token.source_index,
			 sxtab_ptr->err_titles [2][0],
			 "%sArgument type mismatch.",
			 sxtab_ptr->err_titles [2]+1);
	    break;
	}

	node_ptr = node_ptr->brother;
    }

    if (i == param_nb) {
	/* C'est un bon predicat, il faut generer le code... */
	/* On est ds la regle BNFprod, derriere l'item BNFmaxitem */
#if 0
	prolog_code2nt [code] = nt = ste2nt [ste0];
	nt2prolog_code [nt] = code;

	DCGrhs_functor2param [BNFmaxitem] = DCGmain_terms_top;
#endif
#if 0
	/* on connait le nb de param de chaque ppp */
	PPP2param [PPPmaxitem] = PPPmain_terms_top;
#endif

	for (x = 1; x <= term_stack_top; x++) {
	    PPPmain_terms [PPPmain_terms_top++] = term_stack [x];
	}
    }

    term_stack_top = 0; /* On est au top level */
}

static void integer_op (int op, int param_nb)
{
    int 	bot, x, ref;
    SXINT 	term;

    has_integer_op = true;

    ref = KV2REF (INTEGER_OP, op);
    XH_push (term_hd, ref);

    bot = term_stack_top - param_nb + 1;

    for (x = bot; x <= term_stack_top; x++) {
	ref = term_stack [x];
	XH_push (term_hd, ref);
    }

    XH_set (&term_hd, &(term));
    /* Les expressions arithmetiques sont stockees avec les termes */
    term_stack [bot] = KV2REF (INTEGER_OP, term);
    term_stack_top = bot;
}

static void dcg_pi2 ()
{

    /*
       I N H E R I T E D
       */

    switch (SXVISITED->father->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	break;

    case BODY_S_n :		/* SXVISITED->name = RHS_ITEM_S_n */
	if (SXVISITED->position > 1) {
	    /* On visite les alternatives d'une clause */
	    xprod++;
	    printf ("<%s>", lhs_functor_string);
	    BNFlhs [BNFprod] = ste2nt [lhs_functor];
	    BNFprod2nbnt [BNFprod] = 0;

	    if (SXBA_bit_is_set (dum_bnf_rule, xprod))
		printf (" @%i", ++bnf_dum_post_act);

	    DCGlhs_functor2param [BNFprod] = lhs_main_terms_top;
	}

	fputs (" =", stdout);
	DCGlhs2param_nb [BNFprod] = lhs_param_nb;
	rhsnt = 0;
	break;

    case CATENATE_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {CATENATE_n, EMPTY_LIST_n, LIST_BODY_n, variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #17");
	break;
	}

	break;

    case DCG_n :		/* SXVISITED->name = RULE_S_n */
	break;

    case DIV_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #18");
	break;
	}

	break;

    case IS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = variable_n */
	    break;

	case 2 :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #19");
	break;
	}

	break;

    case LESS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n,
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #20");
	break;
	}

	break;

    case LESS_EQUAL_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #21");
	break;
	}

	break;

    case LHS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = MAIN_FUNCTOR_n */
	    lhs_functor = SXVISITED->son->token.string_table_entry;
	    xprod++;
	    lhs_functor_string = sxstrget (lhs_functor);
	    printf ("<%s>", lhs_functor_string);
	    BNFlhs [BNFprod] = ste2nt [lhs_functor];
	    BNFprod2nbnt [BNFprod] = 0;

	    if (SXBA_bit_is_set (dum_bnf_rule, xprod))
		printf (" @%i", ++bnf_dum_post_act);

	    break;

	case 2 :		/* SXVISITED->name = {TERMINAL_S_n, VOID_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #22");
	break;
	}

	break;

    case LIST_BODY_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {CATENATE_n, DIV_n, EMPTY_LIST_n, LIST_BODY_n, MINUS_n, MOD_n, MULT_n, PLUS_n, TERM_n, 
				   UNARY_MINUS_n, atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {CATENATE_n, LIST_BODY_n, VOID_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #23");
	break;
	}

	break;

    case MAIN_FUNCTOR_n :
	switch (SXVISITED->position) {
	    int 	param_nb;
	    char	*str0;

	case 1 :		/* SXVISITED->name = atom_n */
	    param_nb = SXVISITED->brother->degree;

	    if (SXVISITED->father->father->name == RHS_ITEM_S_n) {
		ste = SXVISITED->token.string_table_entry;
		str0 = sxstrget (ste);
		printf (" <%s>", str0);
		BNFlispro [++BNFmaxitem] = ste2nt [ste];
		BNFprod2nbnt [BNFprod]++;
		rhsnt++;
		DCGrhs2param_nb [BNFmaxitem] = param_nb;
	    }
	    else
		lhs_param_nb = param_nb;

	    if (param_nb > max_param_nb)
		max_param_nb = param_nb;

	    break;

	case 2 :		/* SXVISITED->name = {ARGUMENT_S_n, VOID_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #24");
	break;
	}

	break;

    case MINUS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #25");
	break;
	}

	break;

    case MOD_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #26");
	break;
	}

	break;

    case MULT_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #27");
	break;
	}

	break;

    case PLUS_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #28");
	break;
	}

	break;

    case PROLOG_S_n :		/* SXVISITED->name = {IS_n, LESS_n, LESS_EQUAL_n, SUP_n, SUP_EQUAL_n, TERM_n} */
    {
	is_prolog_tree = true;

	if (SXVISITED->name == IS_n)
	    ste = is_ste;
	else if (SXVISITED->name == LESS_n)
	    ste = less_ste;
	else if (SXVISITED->name == LESS_EQUAL_n)
	    ste = less_equal_ste;
	else if (SXVISITED->name == SUP_n)
	    ste = sup_ste;
	else if (SXVISITED->name == SUP_EQUAL_n)
	    ste = sup_equal_ste;
	else if (SXVISITED->name == TERM_n)
	    ste = SXVISITED->son->token.string_table_entry;

	PPPlispro [++PPPmaxitem] = ste2prolog_code [ste];
	maxpppprod = BNFprod;
#if 0
	str = prolog_code2nt_str [ste2prolog_code [ste]];
	printf (" <%s>", str);
	BNFlispro [++BNFmaxitem] = ste2nt [ste];
	BNFprod2nbnt [BNFprod]++;
	rhsnt++;
	DCGrhs2param_nb [BNFmaxitem] = param_nb;
	
	if (param_nb > max_param_nb)
	    max_param_nb = param_nb;
#endif
    }

	break;

    case RHS_ITEM_S_n :		/* SXVISITED->name = {MAIN_FUNCTOR_n, PROLOG_S_n, TERMINAL_S_n, VOID_n} */
	break;

    case RULE_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = LHS_n */
	    break;

	case 2 :		/* SXVISITED->name = BODY_S_n */
	    lhs_var_nb = var_stack_top;
	    break;

	default :
	    sxtrap (ME, "unknown switch case #29");
	break;
	}

	break;

    case RULE_S_n :		/* SXVISITED->name = RULE_n */
	break;

    case SUP_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #30");
	break;
	}

	break;

    case SUP_EQUAL_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	case 2 :		/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, integer_number_n, 
				   variable_n} */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #31");
	break;
	}

	break;

    case TERM_n :
	switch (SXVISITED->position) {
	case 1 :		/* SXVISITED->name = atom_n */
	    break;

	case 2 :		/* SXVISITED->name = ARGUMENT_S_n */
	    break;

	default :
	    sxtrap (ME, "unknown switch case #32");
	break;
	}

	break;

    case TERMINAL_S_n :		/* SXVISITED->name = {atom_n, floating_point_number_n, integer_number_n, string_n, variable_n} */
	if (SXVISITED->father->father->name == RHS_ITEM_S_n) {
	    /* Pour l'instant, on ne traite pas les "terminaux" en LHS. */
	    if (SXVISITED->name == variable_n) {
		printf (" <%s>", "Dcg_constant");
		BNFlispro [++BNFmaxitem] = ste2nt [Dcg_constant_ste];
		BNFprod2nbnt [BNFprod]++;
		rhsnt++;
		DCGrhs2param_nb [BNFmaxitem] = 1;

		if (1 > max_param_nb)
		    max_param_nb = 1;
	    }
	    else {
		int t;

		ste = SXVISITED->token.string_table_entry;
		str = sxstrget (ste);

		if (SXVISITED->name != atom_n || isalpha (*str))
		    printf (" %s", str);
		else
		    printf (" #%s", str);

		BNFlispro [++BNFmaxitem] = t = ste2t [ste];

		/* Utilise si is_dynam_constant */
		if (SXVISITED->name == integer_number_n)
		    t2kind [-t] = INTEGER_CONSTANT;
		else
		    t2kind [-t] = ATOM; /* Pour l'instant */
	    }
	}
	break;

    case UNARY_MINUS_n :/* SXVISITED->name = {DIV_n, MINUS_n, MOD_n, MULT_n, PLUS_n, UNARY_MINUS_n, floating_point_number_n, 
integer_number_n, variable_n} */
	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end dcg_pi */
}

static void dcg_pd2 ()
{
    int var_nb;
    /*
       D E R I V E D
       */

    switch (SXVISITED->name) {

    case ERROR_n :
	break;

    case ARGUMENT_S_n :
	break;

    case BODY_S_n :
	break;

    case CATENATE_n :
    {
	int op1, op2;
	SXINT op;

	op1 = term_stack [term_stack_top-1];
	op2 = term_stack [term_stack_top];

#if EBUG
	if (REF2KIND (op1) != STATIC_LIST && REF2KIND (op2) != VARIABLE)
	    sxtrap (ME, "dcg_pd2");
#endif

	XxY_set (&list_hd, op1, op2, &op);

	term_stack [--term_stack_top] = KV2REF (STATIC_LIST, 2*op-1);
    }
	break;

    case DCG_n :
	break;

    case DIV_n :
	integer_op (DIVIDE, 2);
	break;

    case EMPTY_LIST_n :
	term_stack [++term_stack_top] = EMPTY_LIST;
	break;

    case IS_n :
	gen_ppp (is_ste, 2, SXVISITED->son);
	break;

    case LESS_n :
	gen_ppp (less_ste, 2, SXVISITED->son);
	break;

    case LESS_EQUAL_n :
	gen_ppp (less_equal_ste, 2, SXVISITED->son);
	break;

    case LHS_n :
	break;

    case LIST_BODY_n :
    {
	int op1, op2;
	SXINT op;

	if (SXVISITED->son->brother->name == VOID_n) {
	    term_stack [++term_stack_top] = EMPTY_LIST;
	}

	op1 = term_stack [term_stack_top-1];
	op2 = term_stack [term_stack_top];

#if EBUG
	if (REF2KIND (op1) != STATIC_LIST && REF2KIND (op2) != STATIC_LIST)
	    sxtrap (ME, "dcg_pd2");
#endif

	XxY_set (&list_hd, op1, op2, &op);

	term_stack [--term_stack_top] = KV2REF (STATIC_LIST, 2*op-1);
    }
	break;

    case MAIN_FUNCTOR_n :
    {
	int x;

	if (SXVISITED->father->name == LHS_n)
	    DCGlhs_functor2param [BNFprod] = lhs_main_terms_top = DCGmain_terms_top;
	else
	    DCGrhs_functor2param [BNFmaxitem] = DCGmain_terms_top;

	for (x = 1; x <= term_stack_top; x++) {
	    DCGmain_terms [DCGmain_terms_top++] = term_stack [x];
	}

	term_stack_top = 0;
    }
	break;

    case MINUS_n :
	integer_op (MINUS, 2);
	break;

    case MOD_n :
	integer_op (MODULO, 2);
	break;

    case MULT_n :
	integer_op (MULTIPLY, 2);
	break;

    case PLUS_n :
	integer_op (PLUS, 2);
	break;

    case PROLOG_S_n :
	is_prolog_tree = false;
	break;

    case RHS_ITEM_S_n :
    {
	int x, gvar;

	prod2var_nb [BNFprod] = var_nb = var_stack_top;

	if (var_nb > 0) {
	    for (x = 1; x <= var_nb;x++) {
		gvar = ste2gvar [var_stack [x]];
		XH_push (SXDCGvar_names, gvar);
	    }

	    XH_set (&SXDCGvar_names, &(SXDCG2var_disp [BNFprod]));
	}
	else
	    SXDCG2var_disp [BNFprod] = 0;


	if (var_nb > SXDCGmax_var)
	    SXDCGmax_var = var_nb;

	while (var_nb > lhs_var_nb) {
	    ste = var_stack [var_nb];
	    ste2var [ste] = 0;
	    var_nb--;
	}

	var_stack_top = lhs_var_nb;

	puts (" ;");
	BNFlispro [++BNFmaxitem] = 0;
	BNFprolon [++BNFprod] = BNFmaxitem+1;

	if (rhsnt > BNFmaxrhsnt)
	    BNFmaxrhsnt = rhsnt;

	if (is_prolog) {
	    PPPprolon [BNFprod] = PPPmaxitem+1;
	}
    }
	break;

    case RULE_n :
	while (lhs_var_nb > 0) {
	    ste = var_stack [lhs_var_nb];
	    ste2var [ste] = 0;
	    lhs_var_nb--;
	}

	var_stack_top = 0;
	break;

    case RULE_S_n :
	break;

    case SUP_n :
	gen_ppp (sup_ste, 2, SXVISITED->son);
	break;

    case SUP_EQUAL_n :
	gen_ppp (sup_equal_ste, 2, SXVISITED->son);
	break;

    case TERM_n :
	if (SXVISITED->father->name == PROLOG_S_n) {
	    /* On verifie que l'on sait traiter (au moins grossierement) ce predicat prologue */
	    int		code, param_nb;
	    SXNODE	*son1;

	    son1 =  SXVISITED->son;
	    ste = son1->token.string_table_entry;

	    if ((code = ste2prolog_code [ste]) != 0) {
		/* Il est connu */
		param_nb = prolog_code2param_nb [code];

		if (param_nb == son1->brother->degree) {
		    gen_ppp (ste, param_nb, son1->brother->son);
		}
		else {
		    sxerror (son1->token.source_index,
				 sxtab_ptr->err_titles [2][0],
				 "%sWrong number of arguments for this prolog predicate.",
				 sxtab_ptr->err_titles [2]+1);
		}
	    }
	}
	else {
	    int 	bot, x, ref;
	    SXINT term;

	    bot = term_stack_top - SXVISITED->son->brother->degree;

	    for (x = bot; x <= term_stack_top; x++) {
		ref = term_stack [x];
		XH_push (term_hd, ref);
	    }

	    XH_set (&term_hd, &(term));
	    term_stack [bot] = KV2REF (TERM, term);
	    term_stack_top = bot;
	}
	break;

    case TERMINAL_S_n :
	break;

    case UNARY_MINUS_n :
	integer_op (UMINUS, 1);
	break;

    case VOID_n :
	break;

    case atom_n :
	if (SXVISITED->father->name != TERMINAL_S_n &&
	    SXVISITED->father->father->name != PROLOG_S_n &&
	    SXVISITED->father->name != MAIN_FUNCTOR_n) {
	    int	atom;

	    ste = SXVISITED->token.string_table_entry;

	    if ((atom = ste2atom [ste]) == 0) {
		atom = ste2atom [ste] = ++SXDCGmax_atom;
		atom2ste [SXDCGmax_atom] = ste;
	    }

	    term_stack [++term_stack_top] = KV2REF (ATOM, atom);
	}
	break;

    case floating_point_number_n :
	break;

    case integer_number_n :
	if (SXVISITED->father->name != TERMINAL_S_n) {
	    int	val;

	    val = atoi (sxstrget (SXVISITED->token.string_table_entry));
	    term_stack [++term_stack_top] = KV2REF (INTEGER_CONSTANT, val);
	}
	break;

    case string_n :
	break;

    case variable_n :
    {
	int var, gvar;

	ste = SXVISITED->token.string_table_entry;
	gvar = ste2gvar [ste];

	if (is_prolog_tree) {
	    /* On est dans un sous arbre PROLOG_S_n */
	    /* On note le fait que la variable gvar intervient dans le ppp qui se trouve
	       a l'item BNFmaxitem. */
	    SXBA_1_bit (gvar2pppitem_set [gvar], PPPmaxitem);
	}

	if (ste == _ste) {
	    gvar2ste [0] = _ste;
	    /* ste2gvar [_ste] == 0 */
	    /* ste2var [_ste] == 0 */
	    var_stack [++var_stack_top] = _ste;
	    var = 0;
	}
	else {
	    if (gvar == 0) {
		ste2gvar [ste] = ++SXDCGmax_gvar;
		gvar2ste [SXDCGmax_gvar] = ste;
	    }

	    if ((var = ste2var [ste]) == 0) {
		ste2var [ste] = var = ++var_stack_top;
		var_stack [var] = ste;
	    }
	}

	term_stack [++term_stack_top] = KV2REF (VARIABLE, var);
    }

	if (SXVISITED->father->name == TERMINAL_S_n) {
	    DCGrhs_functor2param [BNFmaxitem] = DCGmain_terms_top;
	    DCGmain_terms [DCGmain_terms_top++] = term_stack [term_stack_top];
	    term_stack_top = 0;
	}

	break;



	/*
	   Z Z Z Z
	   */

    default:
	break;
    }
    /* end dcg_pd */
}




#define out_define(str,val)	fprintf(F_dcg, "#define %s\t%i\n",str,val)


static bool dcg_write ()
{
    long		lt;

    FILE		*F_dcg;
    char		file_name [64];
    VARSTR		vstr0;
    int			prod, X, item;
    int			param_nb, x, *term_ptr, term, list;
    int			bot, top, atom, var;
    int			SXDCGmax_term, SXDCGmax_list;
    int			gvar;
    int			term_top, list_top, val, ref, kind, ppp, ppp_term_top, param;

static char *kind2str [] = {
    "",
/* ATOM		1 */ "A",
/* VARIABLE	2 */ "V",
/* TERM		3 */ "T",
/* STATIC_LIST	4 */ "SL",
/* DYNAM_LIST	5 */ "DL",
/* INTEGER_CONSTANT	6 */ "IC",
/* DYNAM_ATOM	7 */ "DA",
/* INTEGER_OP	8 */ "IO",
};
  

    if ((F_dcg = fopen (strcat (strcpy (file_name, prgentname), "_dcgt.c"), "w")) == NULL) {
	if (sxverbosep && !sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = true;
	}

	fprintf (sxstderr, "%s: cannot open (create) ", "dcg_write");
	sxperror (file_name);
	return false;
    }

    vstr0 = varstr_alloc (64);

    lt = time (NULL);
    /* *ctime() = "Wed Aug 21 11:02:26 1996\n\0" */
    /* the 25th character is a "\n" */
    fprintf (F_dcg, "static char	what [] = \"@(#)Definite Clause Grammar Parser for \\\"%s\\\" - SYNTAX [unix] - %.24s\";\n",
	     prgentname, ctime (&lt));
    
    fprintf (F_dcg, "static char	ME [] = \"%s_dcgt.c\";\n", prgentname);

    fputs ("\n#include \"sxunix.h\"", F_dcg);
    fputs ("\n#include \"dcg_cst.h\"\n\n", F_dcg);

    out_define ("SXDCGmax_var", SXDCGmax_var);
    out_define ("SXDCGmax_gvar", SXDCGmax_gvar);
    out_define ("SXDCGrhs_maxnt", BNFmaxrhsnt);
    out_define ("SXDCGmax_atom", SXDCGmax_atom);
    SXDCGmax_term = term_top = XH_top (term_hd)-1;
    out_define ("SXDCGmax_term", SXDCGmax_term);
    SXDCGmax_list = list_top = XxY_top (list_hd);
    out_define ("SXDCGmax_list", SXDCGmax_list);
    out_define ("SXDCGhas_integer_op", has_integer_op ? 1 : 0);
    out_define ("SXDCGmaxpppprod", maxpppprod);

    fprintf (F_dcg, "\n\nstatic int SXDCG2param_nb [%i][%i] = {\n\
/* 0 */ {0, 0,},\n", BNFprod, BNFmaxrhsnt+1);

    for (prod = 1; prod < BNFprod; prod++) {
	fprintf (F_dcg, "/* %i */\t{%i, ", prod, DCGlhs2param_nb [prod]);

	item = BNFprolon [prod];

	while ((X = BNFlispro [item]) != 0) {
	    if (X > 0) {
		fprintf (F_dcg, "%i, ", DCGrhs2param_nb [item]);
	    }

	    item++;
	}

	fputs ("},\n", F_dcg);
    }

    fputs ("};", F_dcg);

    fprintf (F_dcg, "\n\nstatic int SXDCG2var_nb [%i] = {\n\
/* 0 */ 0,\n", BNFprod);

    for (prod = 1; prod < BNFprod; prod++) {
	fprintf (F_dcg, "/* %i */\t%i,\n", prod, prod2var_nb [prod]);
    }

    fputs ("};", F_dcg);

    fprintf (F_dcg, "\n\nstatic int SXDCG2gvar_disp [%i] = {\n\
/* 0 */ 0,\n", BNFprod);

    for (prod = 1; prod < BNFprod; prod++) {
	x = SXDCG2var_disp [prod];

	if (x != 0)
	    x = XH_X (SXDCGvar_names, x);

	fprintf (F_dcg, "/* %i */\t%i,\n", prod, x);
    }

    fputs ("};", F_dcg);

    top = 0; /* dummy assignment to silence a Gcc warning */

    fprintf (F_dcg, "\n\nstatic int SXDCGgvar_name [%ld] = {\n\
/* 0 */ 0,\n", XH_list_top (SXDCGvar_names));

    for (x = 1; x < XH_top (SXDCGvar_names); x++) {
	 fprintf (F_dcg, "/* %i */\t", x);

	 bot = XH_X (SXDCGvar_names, x);
	 top = XH_X (SXDCGvar_names, x+1);

	 do {
	     fprintf (F_dcg, "%ld, ", XH_list_elem (SXDCGvar_names, bot));
	 } while (++bot < top);

	 fputs ("\n", F_dcg);
    }

    fputs ("};", F_dcg);

    if (term_top > 0) {
	fprintf (F_dcg, "\n\nstatic int SXDCGterm_disp [%i] = {\n\
/* 0 */\t0,\n\
/* 1 */\t1,\n", term_top+2);

	for (term = 2; term <= term_top+1; term++) {
	    top = XH_X (term_hd, term);
	    fprintf (F_dcg, "/* %i */\t%i,\n", term, top);
	}
    
	fputs ("};", F_dcg);

	fprintf (F_dcg, "\n\nstatic int SXDCGterm_list [%i] = {\n\
/* 0 */\t0,\n", top);
	top = 1;

	for (term = 1; term <= term_top; term++) {
	    fprintf (F_dcg, "/* %i */\t", term);
	    top = XH_X (term_hd, term+1);

	    for (bot = XH_X (term_hd, term); bot < top; bot++) {
		ref = XH_list_elem (term_hd, bot);
		kind = REF2KIND (ref);
		val = REF2VAL (ref);
		fprintf (F_dcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
	    }

	    fputs ("\n", F_dcg);
	}
    
	fputs ("};", F_dcg);
    }

    if (SXDCGmax_list > 0) {
	fprintf (F_dcg, "\n\nstatic int SXDCGlist_list [%i] = {\n\
/* 0 */\t0,\n", 2*SXDCGmax_list+1);
	for (list = 1; list <= SXDCGmax_list; list++) {
	    ref = XxY_X (list_hd, list);
	    kind = REF2KIND (ref);
	    val = REF2VAL (ref);
	    fprintf (F_dcg, "/* %i */\t%i /* (%s, %i) */, ", list, ref, kind2str [kind], val);

	    ref = XxY_Y (list_hd, list);
	    kind = REF2KIND (ref);
	    val = REF2VAL (ref);
	    fprintf (F_dcg, "%i /* (%s, %i) */,\n", ref, kind2str [kind], val);
	}
    
	fputs ("};", F_dcg);
    }

    fputs ("\n\n/* SXDCG2term [prod][nt][param] = var|atom|term|list */\n", F_dcg);
    fprintf (F_dcg, "static int SXDCG2term [%i][%i][%i] = {\n\
/* 0 */ {{0}, },\n", BNFprod, BNFmaxrhsnt+1, max_param_nb);

    for (prod = 1; prod < BNFprod; prod++) {
	fprintf (F_dcg, "/* %i */\t{", prod);

	fputs ("{", F_dcg);
	param_nb = DCGlhs2param_nb [prod];

	if (param_nb > 0) {
	    x = DCGlhs_functor2param [prod];
	    term_ptr = &(DCGmain_terms [x]);

	    for (x = 0; x < param_nb; x++) {
		ref = *term_ptr++;
		kind = REF2KIND (ref);
		val = REF2VAL (ref);
		fprintf (F_dcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
	    }
	}
	else
	    fputs ("0", F_dcg);

	fputs ("}, ", F_dcg);

	item = BNFprolon [prod];

	while ((X = BNFlispro [item]) != 0) {
	    if (X > 0) {
		fputs ("{", F_dcg);
		param_nb = DCGrhs2param_nb [item];

		if (param_nb > 0) {
		    x = DCGrhs_functor2param [item];
		    term_ptr = &(DCGmain_terms [x]);

		    for (x = 0; x < param_nb; x++) {
			ref = *term_ptr++;
			kind = REF2KIND (ref);
			val = REF2VAL (ref);
			fprintf (F_dcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
		    }
		}
		else
		    fputs ("0", F_dcg);

		fputs ("}, ", F_dcg);
	    }

	    item++;
	}

	fputs ("},\n", F_dcg);
    }

    fputs ("};\n\n", F_dcg);

    fputs ("\nstatic char *SXDCGatom_str [] = {\n\
/* 0 */ \"\",\n", F_dcg);

    for (atom = 1; atom <= SXDCGmax_atom; atom++) {
	fprintf (F_dcg, "/* %i */\t\"%s\",\n", atom, sxstrget (atom2ste [atom]));
    }

    fputs ("};\n", F_dcg);

    fputs ("\nstatic char *SXDCGvar_str [] = {\n", F_dcg);

    for (gvar = 0; gvar <= SXDCGmax_gvar; gvar++) {
	fprintf (F_dcg, "/* %i */\t\"%s\",\n", gvar, sxstrget (gvar2ste [gvar]));
    }

    fputs ("};\n", F_dcg);

    if (SXDCGmax_var <= 1)
	fprintf (F_dcg, "\n#define SXDCGclear_vars(var_nb)\t%s;\n", SXDCGmax_var==0? "0" : "*var_ptr = 0");
    else {
	fputs ("\n#define SXDCGclear_vars(var_nb)\\\n\
switch (var_nb) {\\\n", F_dcg);

	for (var = SXDCGmax_var; var > 0 ; var--) {
	    if (var > 1)
		fprintf (F_dcg, "\
case %i:\\\n\
   *var_ptr-- = 0;\\\n", var);
	    else
		fprintf (F_dcg, "\
case %i:\\\n\
   *var_ptr = 0;\\\n", var);
	}

	fputs ("}\n", F_dcg);
    }

    out_define ("SXDCGis_dynam_constant", is_dynam_constant ? 1 : 0);

    if (is_dynam_constant) {
	fputs ("\nstatic int SXDCGprod2dc [] = {\n\
/* 0 */ 0,\n", F_dcg);

	for (prod = 1; prod < BNFprod; prod++) {
	    fprintf (F_dcg, "/* %i */\t%i,\n", prod, DCGprod2dynam_constant [prod]);
	}

	fputs ("};\n", F_dcg);
    }

    out_define ("SXDCGhas_prolog", has_prolog ? 1 : 0);
    out_define ("SXDCGmax_gvarppp", SXDCGmax_gvarppp);

    if (has_prolog) {
	fprintf (F_dcg, "\nstatic int SXDCGppp_prolon [%i] = {\n", maxpppprod+2);

	for (prod = 0; prod <= maxpppprod+1; prod++) {
	    fprintf (F_dcg, "/* %i */\t%i,\n", prod, PPPprolon [prod]);
	}

	fputs ("};\n", F_dcg);

	fprintf (F_dcg, "\nstatic int SXDCGppp_lispro [%i] = {0,\n", PPPmaxitem+1);

	for (prod = 1; prod <= maxpppprod; prod++) {
	    top = PPPprolon [prod+1];
	    bot = PPPprolon [prod];

	    if (bot < top) {
		fprintf (F_dcg, "/* %i */\t", prod);

		do {
		    ppp = PPPlispro [bot];
		    fprintf (F_dcg, "%i /* %s */, ", ppp, ppp2str [ppp]);
		} while (++bot < top);

		fputs ("\n", F_dcg);
	    }
	}

	fputs ("};\n", F_dcg);

	fprintf (F_dcg, "\nstatic int SXDCGppp_term_hd [%i] = {0,\n", PPPmaxitem+2);

	ppp_term_top = 0;

	for (prod = 1; prod <= maxpppprod; prod++) {
	    top = PPPprolon [prod+1];
	    bot = PPPprolon [prod];

	    if (bot < top) {
		fprintf (F_dcg, "/* %i */\t", prod);

		do {
		    fprintf (F_dcg, "%i, ", ppp_term_top);
		    ppp_term_top += prolog_code2param_nb [PPPlispro [bot]];
		} while (++bot < top);

		fputs ("\n", F_dcg);
	    }
	}

	fprintf (F_dcg, "/* %i */\t%i\n};\n", prod, ppp_term_top);

	fprintf (F_dcg, "\nstatic int SXDCGppp_term [%i] = {\n", ppp_term_top);

	ppp_term_top = 0;

	for (prod = 0; prod <= maxpppprod; prod++) {
	    top = PPPprolon [prod+1];
	    bot = PPPprolon [prod];

	    if (bot < top) {
		fprintf (F_dcg, "/* %i */\t", prod);

		do {
		    param_nb = prolog_code2param_nb [PPPlispro [bot]];

		    for (param = 1; param <= param_nb;param++) {
			ref = PPPmain_terms [ppp_term_top++];
			kind = REF2KIND (ref);
			val = REF2VAL (ref);
			fprintf (F_dcg, "%i /* (%s, %i) */, ", ref, kind2str [kind], val);
		    }
		} while (++bot < top);

		fputs ("\n", F_dcg);
	    }
	}

	fputs ("};\n", F_dcg);

#if 0
	fputs ("\nstatic int SXDCGppp [] = {\n\
/* 0 */ 0,\n", F_dcg);

	for (nt = 1; nt <= BNFmaxnt; nt++) {
	    fprintf (F_dcg, "/* %i %s */\t%i,\n", nt, sxstrget (nt2ste [nt]), nt2prolog_code [nt]);
	}

	fputs ("};\n", F_dcg);
#endif

	sxbm2c (F_dcg, gvar2pppitem_set, SXDCGmax_gvarppp+1, "SXDCGgvar2ppp_item_set", "",
		true /* is_static */, vstr0);
    }

    if (has_prolog || has_integer_op) {
	fputs ("\nstatic char *SXDCGppp2str [] = {\n\
/* 0 */ \"\",\n", F_dcg);

	for (ppp = 1; ppp <= LAST_PPP; ppp++) {
	    fprintf (F_dcg, "/* %i */\t\"%s\",\n", ppp, ppp2str [ppp]);
	}

	fputs ("};\n", F_dcg);
    }

    fputs ("\n\n#include \"herbrand.h\"\n", F_dcg);

    varstr_free (vstr0);

    fclose (F_dcg);

    return true;
}


static void prolog_init ()
{
    /* Il y a des tokens dont le nom est celui de predicats prolog predefinis, il s'agit
       peut etre de d'appels prolog  i.e. { ... } */
    int	name, ste0, code;

    for (name = 1; name <= prolog_name_nb; name++) {
	ste0 = sxstrretrieve (prolog_name2str [name]);

	if (ste != SXERROR_STE) {
	    code = prolog_name2prolog_code [name];
	    ste2prolog_code [ste0] = code;
	}
    }
}


static void process_dynam_constant ()
{
    /* On genere:
       pour chaque terminal ti
       <Dcg_constant> = ti ;
       puis
       <Dcg_constant> = %Dcg_dynam_atom ;
       <Dcg_constant> = %Dcg_dynam_int ;
       */
    int 	t, lhs;
    SXINT 	var_list;
    char	*str0;

    DCGmain_terms [DCGmain_terms_top] = KV2REF (VARIABLE, 1); /* variable 1 */
    XH_push (SXDCGvar_names, 1); /* On prend la variable dont le nom est 1 */
    XH_set (&SXDCGvar_names, &var_list);

    lhs = ste2nt [Dcg_constant_ste];

    for (t = 1; t <= BNFmaxt; t++) {
	DCGlhs2param_nb [BNFprod] = 1;
	DCGlhs_functor2param [BNFprod] = DCGmain_terms_top;
	prod2var_nb [BNFprod] = 1;
	SXDCG2var_disp [BNFprod] = var_list;
	DCGprod2dynam_constant [BNFprod] = t2kind [t];

	str0 = sxstrget (t2ste [t]);

	if (isalpha (*str0))
	    printf ("<Dcg_constant> = %s ;\n",  str0);
	else
	    printf ("<Dcg_constant> = #%s ;\n",  str0);

	BNFlhs [BNFprod] = lhs;
	BNFprod2nbnt [BNFprod] = 0;
	BNFlispro [++BNFmaxitem] = -t;
	BNFlispro [++BNFmaxitem] = 0;
	BNFprolon [++BNFprod] = BNFmaxitem+1;

	if (is_prolog) {
	    PPPprolon [BNFprod] = PPPmaxitem+1;
	}
    }

    DCGlhs2param_nb [BNFprod] = 1;
    DCGlhs_functor2param [BNFprod] = DCGmain_terms_top;
    prod2var_nb [BNFprod] = 1;
    SXDCG2var_disp [BNFprod] = var_list;
    DCGprod2dynam_constant [BNFprod] = ATOM;

    puts ("<Dcg_constant> = %Dcg_dynam_atom ;");

    BNFlhs [BNFprod] = lhs;
    BNFprod2nbnt [BNFprod] = 0;
    BNFlispro [++BNFmaxitem] = -(t++);
    BNFlispro [++BNFmaxitem] = 0;
    BNFprolon [++BNFprod] = BNFmaxitem+1;

    if (is_prolog) {
	PPPprolon [BNFprod] = PPPmaxitem+1;
    }

    DCGlhs2param_nb [BNFprod] = 1;
    DCGlhs_functor2param [BNFprod] = DCGmain_terms_top;
    prod2var_nb [BNFprod] = 1;
    SXDCG2var_disp [BNFprod] = var_list;
    DCGprod2dynam_constant [BNFprod] = INTEGER_CONSTANT;

    puts ("<Dcg_constant> = %Dcg_dynam_int ;");

    BNFlhs [BNFprod] = lhs;
    BNFprod2nbnt [BNFprod] = 0;
    BNFlispro [++BNFmaxitem] = -t;
    BNFlispro [++BNFmaxitem] = 0;
    BNFprolon [++BNFprod] = BNFmaxitem+1;

    if (is_prolog) {
	PPPprolon [BNFprod] = PPPmaxitem+1;
    }
}


#if 0
static void process_ppp ()
{
    /* Traitement des predicats prolog predefinis */
    int 	prolog_code, nt;
    char	*str;

    for (prolog_code = 1; prolog_code <= prolog_code_nb; prolog_code++) {
	if ((nt = prolog_code2nt [prolog_code]) != 0) {
	    /* On fait comme si on avait eu le source :
	       ppp --> .
	       */
 	    has_prolog = true;

	    DCGlhs2param_nb [BNFprod] = 0;
	    DCGlhs_functor2param [BNFprod] = 0;
	    prod2var_nb [BNFprod] = 0;
	    SXDCG2var_disp [BNFprod] = 0;

	    str = prolog_code2nt_str [prolog_code];
	    printf ("<%s> = ;\n", str);
	    BNFlhs [BNFprod] = nt;
	    BNFprod2nbnt [BNFprod] = 0;
	    BNFlispro [++BNFmaxitem] = 0;
	    BNFprolon [++BNFprod] = BNFmaxitem+1;

	    if (is_prolog) {
		PPPprolon [BNFprod] = PPPmaxitem+1;
	    }
	}
    }
}
#endif



static void smpopen (SXTABLES *sxtables_ptr)
{
    (void) sxtables_ptr;
    sxatcvar.atc_lv.node_size = sizeof (struct dcg_node);
}


static void smppass ()
{
    int			rule_nb, ml;
    struct dcg_node	*np;

    if (sxatcvar.atc_lv.abstract_tree_is_error_node) return;

    if (sxverbosep) {
	fputs ("\tSemantic Pass 1, ", sxtty);
	sxttycol1p = false;
    }

    /*   I N I T I A L I S A T I O N S   */
    /* ........... */

    /* Traitement des variables comme terminaux */
    Dcg_constant_ste = sxstrsave ("Dcg_constant");

    /* On met aussi en string_table les operateurs binaires infixes de prolog,
       ils peuvent etre des foncteurs principayx (non-terminaux), mais pas de simples atomes */
    
    plus_ste = sxstrsave (ppp2str [PLUS]);
    uminus_ste = minus_ste = sxstrsave (ppp2str [MINUS]);
    mult_ste = sxstrsave (ppp2str [MULTIPLY]);
    div_ste = sxstrsave (ppp2str [DIVIDE]);
    mod_ste = sxstrsave (ppp2str [MODULO]);
    less_ste = sxstrsave (ppp2str [LESS]);
    less_equal_ste = sxstrsave (ppp2str [LESS_EQUAL]);
    sup_ste = sxstrsave (ppp2str [SUP]);
    sup_equal_ste = sxstrsave (ppp2str [SUP_EQUAL]);
    is_ste = sxstrsave (ppp2str [IS]);

    max_ste = SXSTRtop ();
    ste2nt = (int*) sxcalloc (max_ste + 1, sizeof (int));
    ste2t = (int*) sxcalloc (max_ste + 1, sizeof (int));
    nt2ste = (int*) sxalloc (max_ste + 1, sizeof (int));
    t2ste = (int*) sxalloc (max_ste + 1, sizeof (int));
    BNFntstring = (char**) sxcalloc (max_ste + 1, sizeof (char*)); 

    /* On suppose l'utilisation de tous les ppp! */
    rule_nb = 0;

    for (np = sxatcvar.atc_lv.abstract_tree_root->son->son;
	 np != NULL;
	 np = np->brother) {
	/* np->son->brother->name == BODY_S_n */
	rule_nb += np->son->brother->degree;
    }

    vstr = varstr_alloc (64);

    ml = 4;

    /* Ne doit pas deborder! */
    dcg_rule_to_line_no = (int*) sxalloc (rule_nb + 1, sizeof (int));
    XH_alloc (&dcg_part[0], "dcg_rules[0]", rule_nb + 1, 1, 2*ml, NULL, NULL);
    XH_alloc (&dcg_part[1], "dcg_rules[1]", rule_nb + 1, 1, 2*ml, NULL, NULL);

    XxY_alloc (&dcg_rules, "dcg_rules", rule_nb + 1, 1, 0, 0, NULL, NULL);

    dum_bnf_rule = sxba_calloc (rule_nb + 1);
    XH_alloc (&bnf_rules, "bnf_rules", rule_nb + 1, 1, ml, NULL, NULL);

    ste2prolog_code = (int*) sxcalloc (max_ste + 1, sizeof (int));
    prolog_init ();

    ste2gvar = (int*) sxcalloc (max_ste + 1, sizeof (int));
    gvar2ste = (int*) sxalloc (max_ste + 1, sizeof (int));
    /* Peut etre modifie par le ste de la variable bidon "_" */
    gvar2ste [0] = SXEMPTY_STE;

    sxsmp (sxatcvar.atc_lv.abstract_tree_root, dcg_pi, dcg_pd);

    XH_free (&dcg_part[0]);
    XH_free (&dcg_part[1]);
    XxY_free (&dcg_rules);
    XH_free (&bnf_rules);

    if (sxverbosep) {
	fputs ("done\n", sxtty);
	sxttycol1p = true;
    }

    if (!IS_ERROR) {
	/* La DCG est un ensemble */
	/* On recopie le display des actions et predicats de la DCG. */
	if (sxverbosep) {
	    fprintf (sxtty, "\t%s.bnf on stdout\n", prgentname);
	}

	if (is_prolog)
	     /* On suppose l'utilisation de tous les ppp! */
	    rule_nb += prolog_code_nb;

	if (is_dynam_constant) {
	    rule_nb += BNFmaxt + 2;
	    rhs_symb_nb += BNFmaxt + 2;
	    /* Pour le param des productions traitant les affectations dynamiques des terminaux
	       aux variables. */
	    DCGmain_terms_size++;
	}

	/* on genere la bnf */
	BNFlhs = (int*) sxalloc (rule_nb+1, sizeof (int));
	BNFprolon  = (int*) sxalloc (rule_nb+2, sizeof (int));

	BNFprod2nbnt = (int*) sxalloc (rule_nb+1, sizeof (int));
	BNFlispro = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
	BNFprolon [BNFprod = 1] = (BNFmaxitem = 3) + 1;

	BNFmaxrhsnt = 1; /* 0: S' -> $ S $ */

	DCGlhs2param_nb = (int*) sxalloc (rule_nb+1, sizeof (int));
	DCGlhs2param_nb [0] = 0;
	DCGrhs2param_nb = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
	DCGrhs2param_nb [0] = DCGrhs2param_nb [1] = DCGrhs2param_nb [2] = DCGrhs2param_nb [3] = 0;

	DCGlhs_functor2param = (int*) sxalloc (rule_nb+1, sizeof (int));
	DCGrhs_functor2param = (int*) sxalloc (rule_nb+rhs_symb_nb+1+4, sizeof (int));
	DCGmain_terms = (int*) sxalloc (DCGmain_terms_size+1, sizeof (int));

	if (is_prolog) {
	    PPPprolon  = (int*) sxalloc (rule_nb+2, sizeof (int));
	    PPPlispro = (int*) sxalloc (ppp_symb_nb+1, sizeof (int));
	    PPPmaxitem = 0;
	    PPPprolon [0] = PPPprolon [BNFprod] = PPPmaxitem+1;
	    PPPmain_terms = (int*) sxalloc (PPPmain_terms_size+1, sizeof (int));
	}

	ste2atom = (int*) sxcalloc (max_ste + 1, sizeof (int));
	atom2ste = (int*) sxalloc (max_ste + 1, sizeof (int));

	ste2var = (int*) sxcalloc (max_ste + 1, sizeof (int));
	/* Il peut y avoir plusieurs "_" ds var_stack */
	var_stack = (int*) sxalloc (max_arg_per_prod + 1, sizeof (int));
	prod2var_nb = (int*) sxalloc (rule_nb+1, sizeof (int));

	/* La variable "_" joue un role particulier */
	_ste = sxstrretrieve ("_");

	SXDCGterm_size = TERM_nb == 0 ? 0 : (TERM_lgth/TERM_nb)+1;

	XH_alloc (&term_hd, "term_hd", TERM_nb+1, 1, SXDCGterm_size+1, NULL, NULL);
	term_stack = (int*) sxalloc (term_stack_size + 1 + 1 /* EMPTY_LIST */, sizeof (int));
	
	XxY_alloc (&list_hd, "list_hd", LIST_nb+1, 1, 0, 0, NULL, NULL);

	SXDCG2var_disp = (SXINT *) sxalloc (rule_nb+1, sizeof (SXINT));

	gvar_nb += SXDCGmax_gvarppp;
	SXDCGmax_gvar = SXDCGmax_gvarppp;

	XH_alloc (&SXDCGvar_names, "SXDCGvar_names", gvar_nb+1, 1, (SXDCGmax_gvar/(rule_nb+1))+1, NULL, NULL);

	if (is_prolog) {
#if 0
	    nt2prolog_code = (int*) sxcalloc (BNFmaxnt + 1, sizeof (int));
#endif
	    gvar2pppitem_set = sxbm_calloc (SXDCGmax_gvarppp+1, ppp_symb_nb+1);
	}

	t2kind = (int*) sxalloc (BNFmaxt+1, sizeof (int));

	sxsmp (sxatcvar.atc_lv.abstract_tree_root, dcg_pi2, dcg_pd2);

	if (is_dynam_constant) {
	    DCGprod2dynam_constant = (int*) sxcalloc (rule_nb+1, sizeof (int));
	    process_dynam_constant ();
	}

	has_prolog = is_prolog;

#if 0
	if (is_prolog)
	    process_ppp ();
#endif

	if (sxverbosep) {
	    fputs ("done\n", sxtty);
	    sxttycol1p = true;
	}

	if (sxverbosep) {
	    fprintf (sxtty, "\t%s_dcgt.c file output ... ", prgentname);
	    sxttycol1p = false;
	}

	dcg_write ();

	if (sxverbosep) {
	    fputs ("done\n", sxtty);
	    sxttycol1p = true;
	}

	sxfree (BNFlhs);
	sxfree (BNFprolon);

	if (is_prolog) {
	    sxfree (PPPprolon);
	    sxfree (PPPlispro);
	    sxfree (PPPmain_terms);
	}

	sxfree (BNFprod2nbnt);
	sxfree (BNFlispro);

	sxfree (DCGlhs2param_nb);
	sxfree (DCGrhs2param_nb);

	sxfree (DCGlhs_functor2param);
	sxfree (DCGrhs_functor2param);
	sxfree (DCGmain_terms);

	sxfree (ste2atom);
	sxfree (atom2ste);

	sxfree (ste2var);
	sxfree (var_stack);
	sxfree (prod2var_nb);
	sxfree (SXDCG2var_disp);

	XH_free (&term_hd);
	sxfree (term_stack);
	XxY_free (&list_hd);

	XH_free (&SXDCGvar_names);

	if (is_dynam_constant) {
	    sxfree (DCGprod2dynam_constant);
	}

	if (is_prolog) {
#if 0
	    sxfree (nt2prolog_code);
#endif
	    sxbm_free (gvar2pppitem_set);
	}

	sxfree (t2kind);
    }

    sxfree (dum_bnf_rule);
    sxfree (dcg_rule_to_line_no);
    sxfree (ste2t);
    sxfree (ste2nt);
    sxfree (t2ste);
    sxfree (nt2ste);
    sxfree (ste2prolog_code);
    sxfree (ste2gvar);
    sxfree (gvar2ste);

    /*   F I N A L I S A T I O N S   */
    /* ........... */

}

void dcg_smp (int what, SXTABLES *sxtables_ptr)
{
    switch (what) {
    case SXOPEN:
	sxtab_ptr = sxtables_ptr;
	smpopen (sxtables_ptr);
	break;
    case SXSEMPASS:		/* For [sub-]tree evaluation during parsing */
	break;
    case SXACTION:
	smppass ();
	break;
    case SXCLOSE:
	break;
    default:
	break;
    }
}
