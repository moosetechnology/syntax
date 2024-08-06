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

static char	ME [] = "semat";

#include "sxversion.h"
#include "sxunix.h"
#include <ctype.h>
#include "sem_by.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "put_edit.h"
#include "T_tables.h"
#include "varstr.h"

char WHAT_SEMATSACT[] = "@(#)SYNTAX - $Id: semat_sact.c 4143 2024-08-02 08:50:12Z garavel $" WHAT_DEBUG;

#define NO_ACT 			0
#define CREATE_FAMILY 		1
#define ADD_LEFT_RECUR_SON 	2
#define ADD_RIGHT_RECUR_SON 	3
#define ERROR_ACT 		4
#define CREATE_LEAF 		5
#define CREATE_LIST 		6

/*   E X T E R N A L    E N T R I E S   */

extern SXINT	genat_c (struct T_ag_item *T_ag);
extern bool  semat_write (struct T_ag_item *T_ag, char *langname);
extern void     semat_free (struct T_ag_item *T_ag);

/*   E X T E R N   */

extern struct sxtables	semat_tables;
extern bool		is_check, is_c;

/*   S T A T I C   */

struct T_ag_item	T_ag;
static bool	SUCCESS;
static SXINT	tok_lgth, rule_no;
static struct sxtoken	null_token = {0, 0, {NULL,0,0}, NULL};
static struct sxtoken	*tok /* extensible */ ;
static SXBA	b_used /* nbpro */ ;
static struct st {
	   SXINT	alpha_to_node_name;
	   SXINT	arity;
	   SXINT	node_name_to_alpha;
	   SXINT	old_to_new;
       }	*st /* 1:st_top */ ;
static SXINT	*sorted /* 1:st_top */ ;
static SXINT	*t_to_node_name /* 1:-tmax */ ;
static SXINT	st_top, internal_node_name, max_ate;
static char	VIDE [] = "VOID";
static bool	b_list, b_nt, b_recur, b_right_list;
static char	*node_name;
static struct sxsource_coord	current_rule_semi_colon_source_index;

static struct SXT_node_info empty_SXT_node_info = {0, 0};


/*   P R O C E D U R E   */

SXINT	delta_to_lispro (SXINT x_prod, SXINT delta)
{
    return bnf_ag.WS_NBPRO [x_prod + 1].prolon - 2 - delta;
}



static VARSTR varstr_strip (VARSTR vstr)
{
    *(--vstr->current) = SXNUL;
    return vstr;
}




static SXINT	get_ate (char *get_ate_node_name, SXINT x_prod)
{
    SXINT	ate;
    char	*s, c;
    bool	is_id;


/* We first check that the get_ate_node_name is a C identifier : */
/* letter {letter|digit|"_"} */

    is_id = true;

    if (isalpha (*get_ate_node_name)) {
	for (c = *(s = get_ate_node_name + 1); (c != SXNUL) && is_id; c = *++s)
	    if (!(isalpha (c) || isdigit (c) || c == '_'))
		is_id = false;
    }
    else
	is_id = false;

    if (!is_id)
	sxerror (tok [x_prod].source_index,
		 semat_tables.err_titles [2][0],
		 "%sAt rule %ld: the node %s must be a C identifier.",
		 semat_tables.err_titles [2]+1,
		 x_prod,
		 get_ate_node_name);

    ate = sxstrsave (get_ate_node_name);

    if (max_ate < ate)
	max_ate = ate;

    return ate;
}



static void	symbol_table (char *symbol_table_node_name, 
			      SXINT x_prod, 
			      SXINT degree, 
			      SXINT ate, 
			      SXINT *node_name_code)
{
    SXINT	i;

    *node_name_code = st [ate].alpha_to_node_name;

    if (*node_name_code == 0) {
	/* A new node name */
	*node_name_code = st [ate].alpha_to_node_name = ++(T_ag.T_constants.T_nbnod);
	st [ate].arity = degree;
	T_ag.T_constants.T_node_name_string_lgth += strlen (symbol_table_node_name) + 1;
    }
    else if (st [ate].arity != degree) {
	if (degree == -1) {
	    /* other actions are forced to create_list */
	    for (i = 1; i <= x_prod - 1; i++) {
		if (T_ag.SXT_node_info [i].T_node_name == *node_name_code)
		    bnf_ag.WS_NBPRO [i].action = CREATE_LIST;
	    }

	    st [ate].arity = -1;
	}
	else if (st [ate].arity == -1)
	    bnf_ag.WS_NBPRO [x_prod].action = CREATE_LIST;
	    /* current action is forced to create_list */
	else
	    sxerror (tok [x_prod].source_index,
		     semat_tables.err_titles [2][0],
		     "%sThe node \"%s\" defined rule %ld has not the same arity as previously defined.",
		     semat_tables.err_titles [2]+1,
		     symbol_table_node_name,
		     x_prod);
    }
}



static void	put_in_stack_schema (SXINT delta)
{
    T_ag.T_stack_schema [T_ag.T_constants.T_stack_schema_size++] = delta;
}



static bool	compare_node_name (SXINT i, SXINT j)
{
    return (strcmp (sxstrget (st [i].node_name_to_alpha), sxstrget (st [j].node_name_to_alpha)) < 0);
}



static bool	is_post (SXINT nt, char *post)
{
    SXINT	l, lpost;
    char	*s;

    if ((l = strlen (s = bnf_ag.NT_STRING + bnf_ag.ADR [nt]) - (lpost = strlen (post)) - 1) <= 0)
	return false;

    return strncmp (s + l, post, lpost) == 0;
}



SXINT	get_node_name_ref (SXINT i)
{
    SXINT	j, k, lim;

    if ((j = bnf_ag.WS_NBPRO [i].action) == ADD_LEFT_RECUR_SON || j == ADD_RIGHT_RECUR_SON) {
	/* list */
	j = bnf_ag.WS_NBPRO [i].reduc;
	lim = bnf_ag.WS_NTMAX [j + 1].npg;

	for (k = bnf_ag.WS_NTMAX [j].npg; k < lim; k++) {
	    if ((j = T_ag.SXT_node_info [bnf_ag.WS_NBPRO [k].numpg].T_node_name) != 0)
		return j;
	}
    }

    return T_ag.SXT_node_info [i].T_node_name;
}



static char	*get_node_name (SXINT i)
{
    return T_ag.T_node_name_string + T_ag.T_nns_indx [get_node_name_ref (i)];
}



static void	print_node_name (SXINT nt)
{
    SXINT	j, k, m, n, lim1, lim2;

    lim1 = bnf_ag.WS_NTMAX [nt + 1].npg;

    for (m = bnf_ag.WS_NTMAX [nt].npg; m < lim1; m++) {
	n = bnf_ag.WS_NBPRO [m].numpg;

	if (!sxba_bit_is_set (b_used, n)) {
	    sxba_1_bit (b_used, n);

	    if (bnf_ag.WS_NBPRO [n].action != 0) {
		node_name = get_node_name (n);
		put_edit_fnb (21, 3, n);
		put_edit_nnl (31, "\"");
		put_edit_apnnl (node_name);
		put_edit_ap ("\"");
	    }
	    else {
		lim2 = bnf_ag.WS_NBPRO [n + 1].prolon;

		for (j = bnf_ag.WS_NBPRO [n].prolon; j < lim2; j++) {
		    if ((k = bnf_ag.WS_INDPRO [j].lispro) > 0 /* [x]nt ou action*/ && !IS_A_PARSACT (k)) {
			print_node_name (XNT_TO_NT (k));
		    }
		}
	    }
	}
    }
}


#ifdef __GNUC__
__attribute__ ((noreturn))
#endif
static void	gripe (void)
{
    fputs ("\nA function of \"semat\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}


void semat_semact (SXINT code, SXINT numact, struct sxtables *arg)
{
    (void) arg;
    switch (code) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
    case SXSEMPASS:
    case SXERROR:
	/* Work is done either in bnf's scanner actions or in semat's main */
	break;

    case SXACTION:
	switch (numact) {
	case 0:
	    tok [rule_no] = null_token;
	    tok [rule_no].source_index = current_rule_semi_colon_source_index;
	    return;

	case 1:
	    tok [rule_no] = SXSTACKtoken (SXSTACKtop ());
	    return;
	default:
#if EBUG
	  sxtrap("semat_sact","unknown switch case #1");
#endif
	  gripe ();
	}

    default:
	gripe ();
    }
}



bool		semat_sem (void)
{
    if (bnf_ag.WS_TBL_SIZE.nbpro != rule_no) {
	fprintf (sxstderr, "%s: internal inconsistency %ld-%ld;\n\ttables will not be produced.\n", ME, bnf_ag.WS_TBL_SIZE.
	     nbpro, rule_no);
	return SUCCESS = false;
    }

    SUCCESS = true;

/* semantic processing failed (a priori) */

    bnf_ag.WS_TBL_SIZE.sem_kind = -1;

    if (sxverbosep) {
	fputs ("   Abstract Tree Processing\n", sxtty);
    }


/* Allocations */

    T_ag.SXT_node_info = (struct SXT_node_info*) sxalloc (bnf_ag.WS_TBL_SIZE.nbpro + 2, sizeof (struct SXT_node_info));
    T_ag.SXT_node_info [0] = T_ag.SXT_node_info [bnf_ag.WS_TBL_SIZE.nbpro+1] = empty_SXT_node_info;
    T_ag.T_constants.T_nbpro = bnf_ag.WS_TBL_SIZE.nbpro;
    T_ag.T_stack_schema = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.indpro + 1, sizeof (SXINT));
    T_ag.T_stack_schema [0] = 0;
    T_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
    st_top = sxstrmngr.top - bnf_ag.WS_TBL_SIZE.tmax;
    st = (struct st*) sxcalloc (st_top + 1, sizeof (struct st));
    sorted = (SXINT*) sxalloc (st_top + 1, sizeof (SXINT));
    t_to_node_name = (SXINT*) sxcalloc (-bnf_ag.WS_TBL_SIZE.tmax + 1, sizeof (SXINT));
    T_ag.T_constants.T_ter_to_node_name_size = 0;
    T_ag.T_constants.T_nbnod = T_ag.T_constants.T_stack_schema_size
			       /* The internal node name 1 is reserved for SXERROR */
								    = 1;
    T_ag.T_constants.T_node_name_string_lgth = 0;
    max_ate = sxstrmngr.top;

    {
	SXINT	x_prod;
	VARSTR vstr;

	vstr = varstr_alloc (32);

	for (x_prod = 1; x_prod <= bnf_ag.WS_TBL_SIZE.nbpro; x_prod++) {
	    SXINT	i, nt;
	    SXINT	lim;
	    SXINT	nb_rhs, degree, t_or_nt_code, ate;
	    bool		rhs_list = false;

	    T_ag.SXT_node_info [x_prod].T_ss_indx = T_ag.T_constants.T_stack_schema_size;
	    nt = bnf_ag.WS_NBPRO [x_prod].reduc;
	    b_nt = b_recur = false;

	    if ((b_list = is_post (nt, "_LIST")))
		b_right_list = is_post (nt, "_RIGHT_LIST");
	    else
		b_right_list = false;

	    {
		SXINT	x;

		x = nb_rhs = 0;
		lim = bnf_ag.WS_NBPRO [x_prod].prolon;

		for (i = bnf_ag.WS_NBPRO [x_prod + 1].prolon - 2; i >= lim; i--) {
		    t_or_nt_code = bnf_ag.WS_INDPRO [i].lispro;

		    if (t_or_nt_code > 0) {
			/* [x]nt ou action*/
			if (!IS_A_PARSACT (t_or_nt_code)) {
			    /* [x]nt */
			    nb_rhs++;
			    b_nt = true;

			    if ((t_or_nt_code = XNT_TO_NT (t_or_nt_code)) == nt)
				b_recur = true;

			    if (nb_rhs == 1)
				rhs_list = is_post (t_or_nt_code, "_LIST");

			    put_in_stack_schema (x);
			}
		    }
		    else {
			/* [extended] terminal */
			t_or_nt_code = XT_TO_T (t_or_nt_code);

			if (sxba_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, -t_or_nt_code)) {
			    /* Generic terminal */
			    nb_rhs++;
			    put_in_stack_schema (x);
			}
		    }

		x++;
		}
	    }

	    sxinitialise (degree);

	    {
		SXINT action_no = NO_ACT;

		if (b_list && b_recur && nb_rhs != 2) {
		    sxerror (tok [x_prod].source_index,
			     semat_tables.err_titles [2][0],
			     "%sAt rule %ld: only a single node can be added to a list at a time.",
			     semat_tables.err_titles [2]+1,
			     x_prod);
		}

		ate = tok [x_prod].string_table_entry;

		if (ate == 0) {
		    /* no node name */
		    node_name = NULL;

		    if (nb_rhs > 2 || (nb_rhs == 2 && (!b_list || !b_recur))) {
			sxerror (tok [x_prod].source_index,
				 semat_tables.err_titles [2][0],
				 "%sAt rule %ld: a node name must be specified.",
				 semat_tables.err_titles [2]+1,
				 x_prod);
		    }
		    else if (nb_rhs == 2)
			if (b_right_list)
			    action_no = ADD_RIGHT_RECUR_SON;
			else
			    action_no = ADD_LEFT_RECUR_SON;
		    else if (nb_rhs == 1) {
			if (b_list) {
			    /* Initialization of a list without node name. */
			    if (b_nt && rhs_list) {
				/* Un seul non-terminal qui est une liste,
				   initialisation correcte... */
				node_name = NULL;
			    }
			    else {
				/* the name of the LHS non terminal is given */
				node_name = varstr_tostr (varstr_strip (varstr_catenate (varstr_raz (vstr), bnf_ag.NT_STRING + bnf_ag.ADR [nt] + 1)));
				degree = -1;
				action_no = CREATE_LIST; /* create list */
			    }
			}
			else if (!b_nt) {
			    /* A [extended] generic terminal alone in the RHS */
			    action_no = CREATE_LEAF;
			    degree = 0;
			    lim = bnf_ag.WS_NBPRO [x_prod + 1].prolon - 2;

			    for (i = bnf_ag.WS_NBPRO [x_prod].prolon; i <= lim; i++) {
				if ((t_or_nt_code = bnf_ag.WS_INDPRO [i].lispro) < 0) {
				    t_or_nt_code = XT_TO_T (t_or_nt_code);
				    /* t_or_nt_code < 0 */

				    if (sxba_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, -t_or_nt_code)) {
					node_name = bnf_ag.T_STRING + bnf_ag.ADR [t_or_nt_code] + 1;
				    }
				}
			    }
			}
			else
			    /* single non terminal */
			    node_name = NULL;
		    }
		    else if (nb_rhs == 0) {
			if (b_list) {
			    node_name = varstr_tostr (varstr_strip (varstr_catenate (varstr_raz (vstr), bnf_ag.NT_STRING + bnf_ag.ADR [nt] + 1)));
			    action_no = CREATE_LIST;
			    /* create an empty list */
			    degree = -1;
			}
			else {
			    /* Empty RHS, the name void is given */
			    node_name = VIDE;
			    /* create family with an empty stack_schema */
			    action_no = CREATE_FAMILY;
			    degree = 0;
			}
		    }
		}
		else {
		    /* A SXNODE NAME IS SPECIFIED   */
		    node_name = sxstrget (ate);

		    if (nb_rhs == 0 && !b_list) {
			/* create family with an empty stack_schema */
			action_no = CREATE_FAMILY;
			degree = 0;
		    }
		    else if (nb_rhs == 1 && !b_nt && !b_list) {
			/* A generic terminal alone in the RHS action : create leaf with the given name */
			action_no = CREATE_LEAF;
			degree = 0;
		    }
		    else {
			if (b_list)
			    if (b_recur)
				sxerror (tok [x_prod].source_index,
					 semat_tables.err_titles [2][0],
					 "%sAt rule %ld: a node name can only be associated with the list initialization.",
					 semat_tables.err_titles [2]+1,
					 x_prod);
			    else {
				action_no = CREATE_LIST;
				degree = -1;
			    }
			else {
			    action_no = CREATE_FAMILY;
			    degree = T_ag.T_constants.T_stack_schema_size - T_ag.SXT_node_info [x_prod].T_ss_indx;
			}

			lim = bnf_ag.WS_NBPRO [x_prod + 1].prolon - 2;

			for (i = bnf_ag.WS_NBPRO [x_prod].prolon; i <= lim; i++) {
			    char	*nn;

			    if ((t_or_nt_code = bnf_ag.WS_INDPRO [i].lispro) < 0) {
				t_or_nt_code = XT_TO_T (t_or_nt_code);

				if (sxba_bit_is_set (bnf_ag.IS_A_GENERIC_TERMINAL, -t_or_nt_code)) {
				    nn = bnf_ag.T_STRING + bnf_ag.ADR [t_or_nt_code] + 1;
				    symbol_table (nn, x_prod, (SXINT)0 /* degree */ , get_ate (nn, x_prod), &internal_node_name)
					 ;

				    if (T_ag.T_constants.T_ter_to_node_name_size < -t_or_nt_code)
					T_ag.T_constants.T_ter_to_node_name_size = -t_or_nt_code;

				    t_to_node_name [-t_or_nt_code] = internal_node_name;
				}
			    }
			}
		    }
		}

		if (action_no != NO_ACT)
		    bnf_ag.WS_NBPRO [x_prod].bprosimpl = false;

		bnf_ag.WS_NBPRO [x_prod].action = action_no;
	    }

	    if (node_name != NULL) {
		if (ate == 0)
		    ate = get_ate (node_name, x_prod);

		symbol_table (node_name, x_prod, degree, ate, &internal_node_name);
	    }
	    else
		internal_node_name = 0;

	    T_ag.SXT_node_info [x_prod].T_node_name = internal_node_name;
	}

    varstr_free (vstr);
    }

    T_ag.SXT_node_info [bnf_ag.WS_TBL_SIZE.nbpro + 1].T_ss_indx = T_ag.T_constants.T_stack_schema_size;
    T_ag.SXT_node_info [bnf_ag.WS_TBL_SIZE.nbpro + 1].T_node_name = 0;
    T_ag.T_stack_schema [T_ag.T_constants.T_stack_schema_size] = 0;


/*  N E W   N A M E S    C O M P U T I N G    */

    {
	SXINT	ate;

	for (ate = 1; ate <= max_ate; ate++) {
	    internal_node_name = st [ate].alpha_to_node_name;

	    if (internal_node_name != 0)
		st [internal_node_name].node_name_to_alpha = ate;
	}
    }

    {
	SXINT	i;

	for (i = 1; i <= T_ag.T_constants.T_nbnod; i++) {
	    sorted [i] = i;
	}
    }

    sort_by_tree (sorted, 2, T_ag.T_constants.T_nbnod, compare_node_name);
    /* sorted(1) = 1  SXERROR */
    T_ag.T_constants.T_node_name_string_lgth += 6;
    /* SXERROR */
    T_ag.T_nns_indx = (SXINT*) sxalloc (T_ag.T_constants.T_nbnod + 2, sizeof (SXINT));
    T_ag.T_ter_to_node_name = (SXINT*) sxcalloc (T_ag.T_constants.T_ter_to_node_name_size + 1, sizeof (SXINT));
    T_ag.T_node_name_string = (char*) sxalloc (T_ag.T_constants.T_node_name_string_lgth, sizeof (char));
    strcpy (T_ag.T_node_name_string, "ERROR");
    T_ag.T_nns_indx [0] = T_ag.T_nns_indx [1] = 0;
    T_ag.T_nns_indx [2] = 6;
    st [1].old_to_new = 1;

    {
	SXINT	new_name;

	for (new_name = 2; new_name <= T_ag.T_constants.T_nbnod; new_name++) {
	    SXINT i = T_ag.T_nns_indx [new_name], old_name = sorted [new_name], ate = st [old_name].
		 node_name_to_alpha;

	    st [old_name].old_to_new = new_name;
	    T_ag.T_nns_indx [new_name + 1] = i + sxstrlen (ate) + 1;
	    strcpy (T_ag.T_node_name_string + i, sxstrget (ate));
	}
    }

    {
	SXINT	x_prod;

	for (x_prod = 1; x_prod <= bnf_ag.WS_TBL_SIZE.nbpro; x_prod++) {
	    SXINT old_name = T_ag.SXT_node_info [x_prod].T_node_name;

	    if (old_name != 0)
		T_ag.SXT_node_info [x_prod].T_node_name = st [old_name].old_to_new;
	}
    }

    {
	SXINT	t;

	for (t = 1; t <= T_ag.T_constants.T_ter_to_node_name_size; t++) {
	    SXINT old_name = t_to_node_name [t];

	    if (old_name != 0)
		T_ag.T_ter_to_node_name [t] = st [old_name].old_to_new;
	}
    }

    if (sxerrmngr.nbmess [2] == 0) {
	/* On cree les tables permanentes */
	    /* on ecrase les anciennes si elles se trouvent dans le
	       working_dir */
	if (!is_check) {
	    if (!semat_write (&T_ag, prgentname))
		SUCCESS = false;
	    else {
		/* Generation, si necessaire des trames des passes
		       heritees et synthetisees */
		if (sxverbosep) {
		    fputs ("   Frames Output\n", sxtty);
		}

		if (is_c)
		    genat_c (&T_ag);
	    }

	    bnf_ag.WS_TBL_SIZE.sem_kind = sem_by_abstract_tree;
	}
    }

    sxfree (t_to_node_name);
    sxfree (sorted);
    sxfree (st);
    return SUCCESS;
}



void	semat_lo (void)
{
    /*     L I S T I N G _ O U T P U T     */
    if (is_list && SUCCESS) {
	SXINT	i, j, k, m;

	if (sxverbosep) {
	    fputs ("   Abstract Tree Listing Output\n", sxtty);
	}

	b_used = sxba_calloc (bnf_ag.WS_TBL_SIZE.nbpro + 1);
	put_edit_ap ("\n");
	put_edit (31, "ABSTRACT    TREE");

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.nbpro; i++) {
	    if (bnf_ag.WS_NBPRO [i].action != 0) {
		node_name = get_node_name (i);
		put_edit_nl (2);
		put_edit_fnb (5, 3, i);
		put_edit_nnl (15, "\"");
		put_edit_apnnl (node_name);
		put_edit_apnnl ("\"");
		put_edit_nnl (41, "name=");
		put_edit_fnb (46, 3, T_ag.SXT_node_info [i].T_node_name);
		put_edit_nnl (51, "action=");
		put_edit_fnb (59, 2, bnf_ag.WS_NBPRO [i].action);
		put_edit_nl (1);

		if (bnf_ag.WS_NBPRO [i].action != 5) {
		    SXINT	lim;

		    m = 0;
		    lim = T_ag.SXT_node_info [i].T_ss_indx;

		    for (j = T_ag.SXT_node_info [i + 1].T_ss_indx - 1; j >= lim; j--) {
			m++;
			k = bnf_ag.WS_INDPRO [delta_to_lispro (i, T_ag.T_stack_schema [j])].lispro;
			put_edit_fnb (15, 2, m);
			put_edit_nl (1);

			if (k < 0) {
			    node_name = bnf_ag.T_STRING + bnf_ag.ADR [XT_TO_T (k)];
			    put_edit_nnl (31, "\"");
			    put_edit_apnnl (node_name + 1);
			    put_edit_ap ("\"");
			}
			else /* [x]nt ou action*/
			    if (!IS_A_PARSACT (k)) {
				sxba_empty (b_used);
				print_node_name (XNT_TO_NT (k));
			    }
		    }
		}
	    }
	}

	sxfree (b_used);
    }

    semat_free (&T_ag);
}



bool semat_scan_act (SXINT code, SXINT act_no)
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals		sxplocals;
	   }	bnf, semat;

    switch (code) {
    case SXOPEN:
	tok = (struct sxtoken*) sxalloc (tok_lgth = 128, sizeof (struct sxtoken));
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	syntax (SXOPEN, &semat_tables);
	semat.sxsvar = sxsvar;
	semat.sxplocals = sxplocals;
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	break;

    case SXCLOSE:
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	sxsvar = semat.sxsvar;
	sxplocals = semat.sxplocals;
	syntax (SXCLOSE, &semat_tables);
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	sxfree (tok);
	break;

    case SXINIT:
	rule_no = 0;

    case SXFINAL:
	break;

    case SXACTION:
	switch (act_no) {
	case 1:
	    if (tok_lgth == ++rule_no) {
		tok = (struct sxtoken*) sxrealloc (tok, tok_lgth *= 2, sizeof (struct sxtoken));
	    }

	    current_rule_semi_colon_source_index = sxsvar.sxlv.terminal_token.source_index;
	    bnf.sxsvar = sxsvar;
	    bnf.sxplocals = sxplocals;
	    sxsvar = semat.sxsvar;
	    sxplocals = semat.sxplocals;
	    sxsrcpush (EOF, ";", sxsrcmngr.source_coord);
/* Ca permet de commencer l'analyse des actions (apres le get_next_char() de la partie
   SXINIT du scanner) avec ";" comme previous char et le caractere suivant le ';' comme
   caractere courant. */
	    sxsyntax (SXACTION, &semat_tables);
	    semat.sxsvar = sxsvar;
	    semat.sxplocals = sxplocals;
	    sxsvar = bnf.sxsvar;
	    sxplocals = bnf.sxplocals;
	    return SXANY_BOOL;

	default:
#if EBUG
	  sxtrap("semat_sact","unknown switch case #2");
#endif
	  gripe ();
	}

    default:
	gripe ();
    }

    return SXANY_BOOL;
}


