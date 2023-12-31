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
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040212 11:15 (phd):	Modif pour calmer le compilo HPUX	*/
/************************************************************************/
/* 20030520 10:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 25-10-93 11:30 (pb):	Oubli du "&" ds sxsyntax (ACTION, &semat_tables)*/
/*			Merci IBM...					*/
/************************************************************************/
/* 22-10-93 10:00 (pb):		Adaptation a OSF			*/
/************************************************************************/
/* 12-01-93 14:15 (pb):	Si <A_LIST> = <B_LIST> ; et aucun nom de noeud	*/
/*			n'est specifie, alors la B_LIST sera completee	*/
/*			avec les elements de la A_LIST.			*/
/************************************************************************/
/* 11-01-93 15:17 (pb):	Le "source_index" d'un noeud vide devient celui	*/
/*			du ";" de sa regle et non pas le "source_index"	*/
/*			du lhsnt de la regle suivante...		*/
/************************************************************************/
/* 23-12-91 15:45 (pb):	Ajout du sxsrcpush malencontreusement oublie	*/
/*			le 19-12					*/
/************************************************************************/
/* 19-12-91 11:00 (pb):	Suite a la creation du point d'entree INIT du	*/
/*			parser (version 3.7a), l'appel d'un autre parser*/
/*			doit se faire par sxsyntax (ACTION, ...)	*/
/************************************************************************/
/* 11-04-90 11:55 (pb):		Mise explicite a 0 du nom de noeud bidon*/
/*				associe a la production nbprod+1	*/
/************************************************************************/
/* 11-10-88 12:20 (pb):		CREATE_FAMILY avec schema de pile vide	*/
/*				est engendre' si PD est ds {[X]Tng|@}*	*/
/************************************************************************/
/* 08-04-88 10:47 (pb):		Adaptation aux XNT			*/
/************************************************************************/
/* 30-03-88 11:01 (pb):		Actions prises comme nt dans le listing	*/
/************************************************************************/
/* 23-06-87 14:08 (pb):		Traitement des options "c" et "pascal" 	*/
/************************************************************************/
/* 15-06-87 10:50 (pb):		Adaptation aux actions et predicats   	*/
/************************************************************************/
/* 25-05-87 18:02 (phd):	Ajout de l'entree SEMPASS	   	*/
/************************************************************************/
/* 25-05-87 16:18 (phd):	Point d'entree ERROR			*/
/************************************************************************/
/* 18-05-87 15:15 (pb&phd):	Adaptation au bnf nouveau		*/
/************************************************************************/
/* 31-03-87 15:22 (pb):		Sortie du nom des fils dans la trame   	*/
/************************************************************************/
/* 16-03-87 08:57 (pb):		Ajout de @2		          	*/
/************************************************************************/
/* 10-12-86 18:38 (phd):	Amelioration d'un message d'erreur   	*/
/************************************************************************/
/* 20-11-86 10:35 (pb):	Suppression de l'arret sur "$" en colonne 1   	*/
/************************************************************************/
/* 30-10-86 14:25 (pb):	Ajout de cette rubrique "modifications"	   	*/
/************************************************************************/

static char	ME [] = "semat";

/********************************************************************/
/*                                                                  */
/*                                                                  */
/*  This program has been translated from sem_by_at.pl1             */
/*  on Wednesday the twenty-fourth of September, 1986, at 12:14:56, */
/*  on the Multics system at INRIA,                                 */
/*  by the release 3.3k PL1_C translator of INRIA,                  */
/*         developped by the "Langages et Traducteurs" team,        */
/*         using the SYNTAX (*), FNC and Paradis systems.           */
/*                                                                  */
/*                                                                  */
/********************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                              */
/********************************************************************/


#include "sxunix.h"
#include <ctype.h>
#include "sem_by.h"
#include "B_tables.h"
#include "bnf_vars.h"
#include "put_edit.h"
#include "T_tables.h"
#include "varstr.h"
char WHAT_SEMATSACT[] = "@(#)SYNTAX - $Id: semat_sact.c 625 2007-06-12 08:35:56Z syntax $" WHAT_DEBUG;

#define NO_ACT 			0
#define CREATE_FAMILY 		1
#define ADD_LEFT_RECUR_SON 	2
#define ADD_RIGHT_RECUR_SON 	3
#define ERROR_ACT 		4
#define CREATE_LEAF 		5
#define CREATE_LIST 		6

#define FF "\f"

/*   E X T E R N A L    E N T R I E S   */

extern int	genat_c ();
extern int	genat_pascal ();
extern BOOLEAN  semat_write (struct T_ag_item *T_ag, char *langname);
extern VOID     semat_free (register struct T_ag_item *T_ag);

/*   E X T E R N   */

extern struct sxtables	semat_tables;
BOOLEAN		is_check, is_c, is_pascal, is_list, sxverbosep;

/*   S T A T I C   */

struct T_ag_item	T_ag;
static BOOLEAN	SUCCESS;
static int	tok_lgth, rule_no;
static struct sxtoken	null_token = {0, 0, {NULL,0,0}, NULL};
static struct sxtoken	*tok /* extensible */ ;
static SXBA	b_used /* nbpro */ ;
static struct st {
	   int	alpha_to_node_name;
	   int	arity;
	   int	node_name_to_alpha;
	   int	old_to_new;
       }	*st /* 1:st_top */ ;
static int	*sorted /* 1:st_top */ ;
static int	*t_to_node_name /* 1:-tmax */ ;
static int	st_top, internal_node_name, max_ate;
static char	VIDE [] = "VOID";
static BOOLEAN	b_list, b_nt, b_recur, b_right_list;
static char	*node_name;
static struct sxsource_coord	current_rule_semi_colon_source_index;




/*   P R O C E D U R E   */

int	delta_to_lispro (x_prod, delta)
    int		x_prod, delta;
{
    return bnf_ag.WS_NBPRO [x_prod + 1].prolon - 2 - delta;
}



static VARSTR varstr_strip (vstr)
VARSTR vstr;
{
    *(--vstr->current) = NUL;
    return vstr;
}




static int	get_ate (get_ate_node_name, x_prod)
    register char	*get_ate_node_name;
    int		x_prod;
{
    register int	ate;
    register char	*s, c;
    register BOOLEAN	is_id;


/* We first check that the get_ate_node_name is a C identifier : */
/* letter {letter|digit|"_"} */

    is_id = TRUE;

    if (isalpha (*get_ate_node_name)) {
	for (c = *(s = get_ate_node_name + 1); (c != NUL) && is_id; c = *++s)
	    if (!(isalpha (c) || isdigit (c) || c == '_'))
		is_id = FALSE;
    }
    else
	is_id = FALSE;

    if (!is_id)
	sxput_error (tok [x_prod].source_index, "%sAt rule %d: the node %s must be a C identifier.", semat_tables.
	     err_titles [2], x_prod, get_ate_node_name);

    ate = sxstrsave (get_ate_node_name);

    if (max_ate < ate)
	max_ate = ate;

    return ate;
}



static SXVOID	symbol_table (symbol_table_node_name, x_prod, degree, ate, node_name_code)
    char	*symbol_table_node_name;
    int		x_prod, degree, ate, *node_name_code;
{
    register int	i;

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
	    sxput_error (tok [x_prod].source_index,
		 "%sThe node \"%s\" defined rule %d has not the same arity as previously defined.", semat_tables.
		 err_titles [2], symbol_table_node_name, x_prod);
    }
}



static SXVOID	put_in_stack_schema (delta)
    int		delta;
{
    T_ag.T_stack_schema [T_ag.T_constants.T_stack_schema_size++] = delta;
}



static BOOLEAN	compare_node_name (i, j)
    int		i, j;
{
    return (strcmp (sxstrget (st [i].node_name_to_alpha), sxstrget (st [j].node_name_to_alpha)) < 0);
}



static BOOLEAN	is_post (nt, post)
    int		nt;
    register char	*post;
{
    register int	l, lpost;
    register char	*s;

    if ((l = strlen (s = bnf_ag.NT_STRING + bnf_ag.ADR [nt]) - (lpost = strlen (post)) - 1) <= 0)
	return FALSE;

    return strncmp (s + l, post, lpost) == 0;
}



int	get_node_name_ref (i)
    register int	i;
{
    register int	j, k, lim;

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



static char	*get_node_name (i)
    int		i;
{
    return T_ag.T_node_name_string + T_ag.T_nns_indx [get_node_name_ref (i)];
}



static SXVOID	print_node_name (nt)
    int		nt;
{
    register int	j, k, m, n, lim1, lim2;

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



static VOID	gripe ()
{
    fputs ("\nA function of \"semat\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
}


int
semat_semact (code, numact)
    int		code;
    int		numact;
{
    switch (code) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
    case SEMPASS:
    case ERROR:
	/* Work is done either in bnf's scanner actions or in semat's main */
	break;

    case ACTION:
	switch (numact) {
	case 0:
	    tok [rule_no] = null_token;
	    tok [rule_no].source_index = current_rule_semi_colon_source_index;
	    return 0;

	case 1:
	    tok [rule_no] = STACKtoken (STACKtop ());
	    return 0;
	default:
#if EBUG
	  sxtrap("semat_sact","unknown switch case #1");
#endif
	  break;
	}

    default:
	gripe ();
    }

    return 0;
}



BOOLEAN		semat_sem ()
{
    if (bnf_ag.WS_TBL_SIZE.nbpro != rule_no) {
	fprintf (sxstderr, "%s: internal inconsistency %d-%d;\n\ttables will not be produced.\n", ME, bnf_ag.WS_TBL_SIZE.
	     nbpro, rule_no);
	return SUCCESS = FALSE;
    }

    SUCCESS = TRUE;

/* semantic processing failed (a priori) */

    bnf_ag.WS_TBL_SIZE.sem_kind = -1;

    if (sxverbosep) {
	fputs ("   Abstract Tree Processing\n", sxtty);
    }


/* Allocations */

    T_ag.SXT_node_info = (struct SXT_node_info*) sxalloc (bnf_ag.WS_TBL_SIZE.nbpro + 2, sizeof (struct SXT_node_info));
    T_ag.T_constants.T_nbpro = bnf_ag.WS_TBL_SIZE.nbpro;
    T_ag.T_stack_schema = (int*) sxalloc (bnf_ag.WS_TBL_SIZE.indpro + 1, sizeof (int));
    T_ag.bnf_modif_time = bnf_ag.bnf_modif_time;
    st_top = sxstrmngr.top - bnf_ag.WS_TBL_SIZE.tmax;
    st = (struct st*) sxcalloc (st_top + 1, sizeof (struct st));
    sorted = (int*) sxalloc (st_top + 1, sizeof (int));
    t_to_node_name = (int*) sxcalloc (-bnf_ag.WS_TBL_SIZE.tmax + 1, sizeof (int));
    T_ag.T_constants.T_ter_to_node_name_size = 0;
    T_ag.T_constants.T_nbnod = T_ag.T_constants.T_stack_schema_size
			       /* The internal node name 1 is reserved for ERROR */
								    = 1;
    T_ag.T_constants.T_node_name_string_lgth = 0;
    max_ate = sxstrmngr.top;

    {
	register int	x_prod;
	VARSTR vstr;

	vstr = varstr_alloc (32);

	for (x_prod = 1; x_prod <= bnf_ag.WS_TBL_SIZE.nbpro; x_prod++) {
	    register int	i, nt;
	    register int	lim;
	    register int	nb_rhs, degree, t_or_nt_code, ate;
	    BOOLEAN		rhs_list = FALSE;

	    T_ag.SXT_node_info [x_prod].T_ss_indx = T_ag.T_constants.T_stack_schema_size;
	    nt = bnf_ag.WS_NBPRO [x_prod].reduc;
	    b_nt = b_recur = FALSE;

	    if ((b_list = is_post (nt, "_LIST")))
		b_right_list = is_post (nt, "_RIGHT_LIST");
	    else
		b_right_list = FALSE;

	    {
		register int	x;

		x = nb_rhs = 0;
		lim = bnf_ag.WS_NBPRO [x_prod].prolon;

		for (i = bnf_ag.WS_NBPRO [x_prod + 1].prolon - 2; i >= lim; i--) {
		    t_or_nt_code = bnf_ag.WS_INDPRO [i].lispro;

		    if (t_or_nt_code > 0) {
			/* [x]nt ou action*/
			if (!IS_A_PARSACT (t_or_nt_code)) {
			    /* [x]nt */
			    nb_rhs++;
			    b_nt = TRUE;

			    if ((t_or_nt_code = XNT_TO_NT (t_or_nt_code)) == nt)
				b_recur = TRUE;

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
		register	int action_no = NO_ACT;

		if (b_list && b_recur && nb_rhs != 2) {
		    sxput_error (tok [x_prod].source_index,
			 "%sAt rule %d: only a single node can be added to a list at a time.", semat_tables.err_titles [2
			 ], x_prod);
		}

		ate = tok [x_prod].string_table_entry;

		if (ate == 0) {
		    /* no node name */
		    node_name = NULL;

		    if (nb_rhs > 2 || (nb_rhs == 2 && (!b_list || !b_recur))) {
			sxput_error (tok [x_prod].source_index, "%sAt rule %d: a node name must be specified.",
			     semat_tables.err_titles [2], x_prod);
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
			    /* Empty RHS, the name VOID is given */
			    node_name = VIDE;
			    /* create family with an empty stack_schema */
			    action_no = CREATE_FAMILY;
			    degree = 0;
			}
		    }
		}
		else {
		    /* A NODE NAME IS SPECIFIED   */
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
				sxput_error (tok [x_prod].source_index,
				     "%sAt rule %d: a node name can only be associated with the list initialization.",
				     semat_tables.err_titles [2], x_prod);
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
				    symbol_table (nn, x_prod, 0 /* degree */ , get_ate (nn, x_prod), &internal_node_name)
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
		    bnf_ag.WS_NBPRO [x_prod].bprosimpl = FALSE;

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
	register int	ate;

	for (ate = 1; ate <= max_ate; ate++) {
	    internal_node_name = st [ate].alpha_to_node_name;

	    if (internal_node_name != 0)
		st [internal_node_name].node_name_to_alpha = ate;
	}
    }

    {
	register int	i;

	for (i = 1; i <= T_ag.T_constants.T_nbnod; i++) {
	    sorted [i] = i;
	}
    }

    sort_by_tree (sorted, 2, T_ag.T_constants.T_nbnod, compare_node_name);
    /* sorted(1) = 1  ERROR */
    T_ag.T_constants.T_node_name_string_lgth += 6;
    /* ERROR */
    T_ag.T_nns_indx = (int*) sxalloc (T_ag.T_constants.T_nbnod + 2, sizeof (int));
    T_ag.T_ter_to_node_name = (int*) sxcalloc (T_ag.T_constants.T_ter_to_node_name_size + 1, sizeof (int));
    T_ag.T_node_name_string = (char*) sxalloc (T_ag.T_constants.T_node_name_string_lgth, sizeof (char));
    strcpy (T_ag.T_node_name_string, "ERROR");
    T_ag.T_nns_indx [0] = T_ag.T_nns_indx [1] = 0;
    T_ag.T_nns_indx [2] = 6;
    st [1].old_to_new = 1;

    {
	register int	new_name;

	for (new_name = 2; new_name <= T_ag.T_constants.T_nbnod; new_name++) {
	    register	int i = T_ag.T_nns_indx [new_name], old_name = sorted [new_name], ate = st [old_name].
		 node_name_to_alpha;

	    st [old_name].old_to_new = new_name;
	    T_ag.T_nns_indx [new_name + 1] = i + sxstrlen (ate) + 1;
	    strcpy (T_ag.T_node_name_string + i, sxstrget (ate));
	}
    }

    {
	register int	x_prod;

	for (x_prod = 1; x_prod <= bnf_ag.WS_TBL_SIZE.nbpro; x_prod++) {
	    register	int old_name = T_ag.SXT_node_info [x_prod].T_node_name;

	    if (old_name != 0)
		T_ag.SXT_node_info [x_prod].T_node_name = st [old_name].old_to_new;
	}
    }

    {
	register int	t;

	for (t = 1; t <= T_ag.T_constants.T_ter_to_node_name_size; t++) {
	    register	int old_name = t_to_node_name [t];

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
		SUCCESS = FALSE;
	    else {
		/* Generation, si necessaire des trames des passes
		       heritees et synthetisees */
		if (sxverbosep) {
		    fputs ("   Frames Output\n", sxtty);
		}

		if (is_c)
		    genat_c (&T_ag);

		if (is_pascal)
		    genat_pascal (&T_ag);

	    }

	    bnf_ag.WS_TBL_SIZE.sem_kind = sem_by_abstract_tree;
	}
    }

    sxfree (t_to_node_name);
    sxfree (sorted);
    sxfree (st);
    return SUCCESS;
}



VOID	semat_lo ()
{
    /*     L I S T I N G _ O U T P U T     */
    if (is_list && SUCCESS) {
	register int	i, j, k, m;

	if (sxverbosep) {
	    fputs ("   Abstract Tree Listing Output\n", sxtty);
	}

	b_used = sxba_calloc (bnf_ag.WS_TBL_SIZE.nbpro + 1);
	put_edit_ap (FF);
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
		    register int	lim;

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



static int	semat_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    static struct {
	       struct sxsvar	sxsvar;
	       struct sxplocals		sxplocals;
	   }	bnf, semat;

    switch (code) {
    case OPEN:
	tok = (struct sxtoken*) sxalloc (tok_lgth = 128, sizeof (struct sxtoken));
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	syntax (OPEN, &semat_tables);
	semat.sxsvar = sxsvar;
	semat.sxplocals = sxplocals;
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	break;

    case CLOSE:
	bnf.sxsvar = sxsvar;
	bnf.sxplocals = sxplocals;
	sxsvar = semat.sxsvar;
	sxplocals = semat.sxplocals;
	syntax (CLOSE, &semat_tables);
	sxsvar = bnf.sxsvar;
	sxplocals = bnf.sxplocals;
	sxfree (tok);
	break;

    case INIT:
	rule_no = 0;

    case FINAL:
	break;

    case ACTION:
	switch (act_no) {
	case 1:
	    if (tok_lgth == ++rule_no) {
		tok = (struct sxtoken*) sxrealloc (tok, tok_lgth *= 2, sizeof (struct sxtoken));
	    }

	    current_rule_semi_colon_source_index = sxsvar.lv.terminal_token.source_index;
	    bnf.sxsvar = sxsvar;
	    bnf.sxplocals = sxplocals;
	    sxsvar = semat.sxsvar;
	    sxplocals = semat.sxplocals;
	    sxsrcpush (EOF, ";", sxsrcmngr.source_coord);
/* Ca permet de commencer l'analyse des actions (apres le get_next_char() de la partie
   INIT du scanner) avec ";" comme previous char et le caractere suivant le ';' comme
   caractere courant. */
	    sxsyntax (ACTION, &semat_tables);
/*
	    (*(semat_tables.analyzers.scanner)) (INIT, &semat_tables);

	    if (semat_tables.SXS_tables.S_is_user_action_or_prdct)
		(*(semat_tables.SXS_tables.scanact)) (INIT, &semat_tables);

	    (*(semat_tables.analyzers.parser)) (ACTION, &semat_tables);

	    if (semat_tables.SXS_tables.S_is_user_action_or_prdct)
		(*(semat_tables.SXS_tables.scanact)) (FINAL, &semat_tables);

	    (*(semat_tables.analyzers.scanner)) (FINAL, &semat_tables);
*/
	    semat.sxsvar = sxsvar;
	    semat.sxplocals = sxplocals;
	    sxsvar = bnf.sxsvar;
	    sxplocals = bnf.sxplocals;
	    return 0;
	default:
#if EBUG
	  sxtrap("semat_sact","unknown switch case #2");
#endif
	  break;
	}

    default:
	gripe ();
    }

    return 0;
}



int	(*more_scan_act) () = {semat_scan_act};
