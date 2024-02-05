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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/*	TRAITEMENT DES DOMAINES DE HERBRAND ASSOCIES AUX RCG		*/

static char	ME [] = "DRCG_sem_fun.c";

/* POUR L'INSTANT */
#define DRCG_is_cyclic	0

#include "sxunix.h";
#include "dcg_cst.h";
#include "drcg_glbl.h";

#include <stdlib.h>  /* pour random() */

char WHAT_RCGTESTDRCG[] = "@(#)SYNTAX - $Id: DRCG_sem_fun.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

typedef struct {
    struct bag_disp_hd {
	SXINT		size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    SXINT		hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    SXINT		used_size, prev_used_size, total_size; /* #ifdef statistics */
} bag_header;


static void
OR (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXINT	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));

#if EBUG
    if (*lhs_bits_array < *rhs_bits_array)
	sxtrap (ME, "OR");
#endif
    
    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	*lhs_bits_ptr-- |= *rhs_bits_ptr--;
    }
}

static bool
IS_AND (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXINT	slices_number = SXNBLONGS (SXBASIZE (rhs_bits_array));
    SXINT	lhs_slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    if (lhs_slices_number < slices_number)
	slices_number = lhs_slices_number;

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0)
    {
	if (*lhs_bits_ptr-- & *rhs_bits_ptr--)
	    return true;
    }

    return false;
}



static void
bag_alloc (pbag, name, size)
    bag_header	*pbag;
    char	*name;
    SXINT 	size;
{
    SXBA_ELT	*ptr;

    pbag->name = name;
    pbag->hd_top = 0;
    pbag->hd_high = 0;
    pbag->hd_size = 1;
    pbag->pool_size = size;
    pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
    pbag->pool_top = pbag->hd [0].set = (SXBA_ELT*) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
    pbag->hd [0].size = pbag->pool_size;
    pbag->room = pbag->pool_size;

#ifdef statistics 
    pbag->total_size = pbag->pool_size + 2;
    pbag->used_size = pbag->prev_used_size = 0;
#endif
}



static SXBA
bag_get (pbag, size)
    bag_header	*pbag;
    SXINT		size;
{
    SXINT 	slice_nb = SXNBLONGS (size) + 1;
    SXBA	set;

    if (slice_nb > pbag->room)
    {
	if (++pbag->hd_top >= pbag->hd_size)
	    pbag->hd = (struct bag_disp_hd*) sxrealloc (pbag->hd,
							pbag->hd_size *= 2,
							sizeof (struct bag_disp_hd));

	if (pbag->hd_top <= pbag->hd_high) {
	    pbag->pool_size = pbag->hd [pbag->hd_top].size;

	    while (slice_nb > (pbag->room = pbag->pool_size)) {
		if (++pbag->hd_top >  pbag->hd_high)
		    break;

		pbag->pool_size = pbag->hd [pbag->hd_top].size;
	    }
	}

	if (pbag->hd_top > pbag->hd_high) {	    
	    while (slice_nb > (pbag->room = (pbag->pool_size *= 2)));

#ifdef statistics
	    fprintf (sxtty, "Bag %s: New bag of size %i is created.\n", pbag->name, pbag->pool_size);
	    pbag->total_size += pbag->pool_size + 2;
#endif

	    pbag->hd_high = pbag->hd_top;
	    pbag->hd [pbag->hd_top].set = (SXBA) sxcalloc (pbag->pool_size + 2, sizeof (SXBA_ELT));
	    pbag->hd [pbag->hd_top].size = pbag->pool_size;
	}

	pbag->pool_top = pbag->hd [pbag->hd_top].set;
    }

    *(set = pbag->pool_top) = size;
    pbag->pool_top += slice_nb;
    pbag->room -= slice_nb;

#ifdef statistics
    pbag->used_size += slice_nb;
#endif

    return set;
}


static void
bag_reuse (pbag, set)
    bag_header	*pbag;
    SXBA	set;
{
    /* On recupere la place allouee depuis set. */
    /* Dangereux, c'est la responsabilite de l'utilisateur */
    SXINT	slice_nb;

    if (set < pbag->hd [pbag->hd_top].set || set > pbag->pool_top) {
	/* set pointe ne pointe plus ds le courant, on recupere tout le courant */
	set = pbag->hd [pbag->hd_top].set;
    }

    if ((slice_nb = pbag->pool_top - set) > 0) {
	pbag->room += slice_nb;

#ifdef statistics
	pbag->used_size -= slice_nb;
#endif

	/* On nettoie */
	do {
	    *--pbag->pool_top = 0;
	} while (pbag->pool_top > set);
    }
}
    

static void
bag_free (pbag)
    bag_header	*pbag;
{
#ifdef statistics
    fprintf (sxtty, "Bag %s: used_size = %i bytes, total_size = %i bytes\n",
	    pbag->name,
	    (pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) *
	    sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd),
    	    pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd));
#endif

    do {
	sxfree (pbag->hd [pbag->hd_high].set);
    } while (--pbag->hd_high >= 0);

    sxfree (pbag->hd);
}
    

static void
bag_clear (pbag)
    bag_header	*pbag;
{
    /* On suppose que les SXBA sont empty. */
    pbag->hd_top = 0;
    pbag->pool_top = pbag->hd [0].set;
    pbag->pool_size = pbag->hd [0].size;
    pbag->room = pbag->pool_size;

#ifdef statistics
    if (pbag->prev_used_size < pbag->used_size)
	pbag->prev_used_size = pbag->used_size;

    pbag->used_size = 0;
#endif
}
    
/* ************************************************************************************ */

/* Ce programme doit etre compile avec l'option
       -DPID=p S'il y a p modules */
struct drcg_struct	drcg[PID];

static SXINT	max_var, has_prolog, is_dynam_atom, max_atom, rhs_maxnt, max_term, max_list, has_integer_op;

static SXINT	glob_tree_top, tree_size, Aij2tree_size;

#if DRCG_is_cyclic==1
static bool		is_cyclic_prod;	
static struct cyclic_tree {
    SXINT		loop_nb;
} *cyclic_tree;
static SXINT		cyclic_glob_tree_top, cyclic_tree_size;

static void
cyclic_tree_oflw ()
{
    cyclic_tree_size *= 2;
    cyclic_tree = (struct cyclic_tree*) sxrealloc (cyclic_tree, cyclic_tree_size+1, sizeof (struct cyclic_tree));
}

#endif


static bag_header	tree_bag;

static SXINT		*node_list, node_list_top, node_list_size;
static struct var_list {
    SXINT		next, root, sub_tree, val;
} *var_list;
static SXINT		var_list_top, var_list_size;
static SXINT		*var_hd, var_hd_top, var_hd_size;

static SXINT		*sons_stack;
static SXINT		*Frhs_stack, *Fsons;

static SXINT		*undo_stack, undo_stack_top, undo_stack_size;

static bool		is_unified;

#include "XxY.h"

static SXINT		ppp; /* valeur du predicat */

struct list_struct {
    SXINT tree_id, list_ref, elem_ref;
};

static struct list_struct	*list_stack, *list_stack_ptr, *list_stack1, *list_stack1_ptr,
                        *list_stack2, *list_stack2_ptr, *list_stack3, *list_stack3_ptr;
static SXINT		list_stack_top, list_stack_size, list_stack1_size, list_stack2_size, list_stack3_size;

static XxY_header	pppredo;
static SXINT		glob_ppp_item;

static SXINT	*atom2dynam_atom [PID];
static SXINT	*dynam_atom_list;



static void
var_list_oflw ()
{
    var_list_size *= 2;
    var_list = (struct var_list*) sxrealloc (var_list, var_list_size+1, sizeof (struct var_list));
}

put_in_pppredo (sub_tree, item)
    SXINT sub_tree, item;
{
    /* On met (sub_tree, item) ds pppredo s'il ne s'y trouve pas deja */
    SXINT x;

    XxY_set (&pppredo, sub_tree, item, &x);
}

static void
create_substitution (sub_tree_var, x, sub_tree_term, t)
    SXINT sub_tree_var, x, sub_tree_term, t;
{
    /* On ajoute la substitution (x/t) a la base */
    /* x est une variable et t est quelconque */
    SXINT			*var_hd_ptr, indx, var_val;
    struct var_list	*var_list_ptr;
    struct drcg_tree2attr	*sub_tree_ptr;

#if EBUG
    if (sub_tree_var == 0)
	sxtrap (ME, "create_substitution");
#endif

    if (x == DUM_VAR || t == DUM_VAR || x == t && sub_tree_var == sub_tree_term) return;

    if (++var_list_top > var_list_size)
	var_list_oflw ();

    sub_tree_ptr = &(drcg_tree2attr [sub_tree_var]);
    var_val = REF2VAL (x);
    indx = sub_tree_ptr->var_hd + var_val;

    if (++undo_stack_top >= undo_stack_size)
	undo_stack = (SXINT*) sxrealloc (undo_stack, (undo_stack_size *= 2) + 1, sizeof (SXINT));
    
    undo_stack [undo_stack_top] = indx;

    var_hd_ptr = &(var_hd [indx]);
    var_list_ptr = &(var_list [var_list_top]);
    var_list_ptr->next = *var_hd_ptr;
    *var_hd_ptr = var_list_top;
    var_list_ptr->root = glob_tree_top;
    var_list_ptr->sub_tree = sub_tree_term;
    var_list_ptr->val = t;

    if (has_prolog==1) {
	/* On vient de substituer une variable libre.
	   On regarde si cette variable n'est pas utilisee dans un ppp
	   qu'il faudra donc [re]executer, car l'info sur ses params a augmente. */
	SXINT			prod, pid, ppp_item, init_ppp_item, next_ppp_item;
	SXBA			ppp_item_set;
	SXINT			*ppp_prolon;
	struct drcg_struct	*pdrcg;

	prod = sub_tree_ptr->prod;
	pid = sub_tree_ptr->pid;
	pdrcg = drcg+pid;

	if (prod <= pdrcg->cst.maxpppprod &&
	    var_val <= pdrcg->var_nb [prod] /* pas une var dynam */ &&
	    var_val <= pdrcg->cst.max_varppp /* var_val intervient dans des ppp */) {
	    ppp_prolon = pdrcg->ppp_prolon + prod;

	    if ((next_ppp_item = ppp_prolon [1]) - (init_ppp_item = *ppp_prolon) > 0) {
		/* prod a des ppp */
		ppp_item_set = pdrcg->var2ppp_item_set [var_val];
		ppp_item = --init_ppp_item;

		while ((ppp_item = sxba_scan (ppp_item_set, ppp_item)) > 0 && ppp_item < next_ppp_item) {
		    /* La variable gvar_val intervient dans le ppp en position ppp_item */
		    if (sub_tree_var != glob_tree_top || ppp_item < glob_ppp_item)
			put_in_pppredo (sub_tree_var, ppp_item);
		}
	    }
	}
    }

}



void
DRCGsubstitute (sub_tree, var_ref)
    SXINT *sub_tree, *var_ref;
{
    /* var_ref est une variable de sub_tree, on regarde vers quoi elle pointe */
    SXINT			x, y, prev_sub_tree, var, n, root, next, last_next;
    struct var_list	*var_list_ptr;

    x = *var_ref;
    y = *sub_tree;

    do {
	if (x == DUM_VAR)
	    break;

	var = REF2VAL (x);
	prev_sub_tree = y;
	/* On reference la liste des variables var du sous-arbre y */
#if EBUG
	n = 0;
	last_next = 0;

	if (prev_sub_tree == 0)
	    sxtrap (ME, "DRCGsubstitute");
#endif

	for (next = var_hd [drcg_tree2attr [prev_sub_tree].var_hd + var]; next > 0; next = var_list_ptr->next) {
	    var_list_ptr = var_list + next;
	    root = var_list_ptr->root;

#if EBUG
	    if (root <= 0)
		sxtrap (ME, "DRCGsubstitute");
#endif

	    if (root < root_set [0] /* est aussi utilise quant tous les arbres sont construits */
		&& SXBA_bit_is_set (root_set, root)) {
		/* C'est un bon chemin */
		/* Ce doit etre le seul! */
#if EBUG
		n++;
		last_next = next;
#else
		break;
#endif
	    }
	}

#if EBUG
	if (n > 1)
	    sxtrap (ME, "DRCGsubstitute");

	next = last_next;
	var_list_ptr = var_list + next;
#endif

	if (next > 0) {
	    /* on a trouve */
	    x = var_list_ptr->val;
	    y = var_list_ptr->sub_tree;
#if EBUG
	    if (x == 0 || y == 0)
		sxtrap (ME, "DRCGsubstitute");
#endif
	}
	else {
	    /* variable libre */
	    /* x est OK */
	    y = prev_sub_tree;
	    break;
	}
    } while (REF2KIND (x) == VARIABLE);

    *var_ref = x;
    *sub_tree = y;
}

static bool
integer_val (tree_id, ref, val)
    SXINT *tree_id, *ref, *val;
{
    /* On calcule la valeur de l'expression dont la racine est (tree_id, exp_ref) */
    /* Si true, tree_id et ref sont non significatifs */
    /* Si false, tree_id et ref designe la racine de l'echec */
    SXINT kind, val1, val2, bot, int_op, op_ref, op1_tree, op1_ref, op2_tree, op2_ref;
    struct drcg_struct	*pdrcg;

    kind = REF2KIND (*ref);

    if (kind == VARIABLE) {
	/* Variable */
	/* Est-elle liee ? */
	DRCGsubstitute (tree_id, ref);
	kind = REF2KIND (*ref);
    }

    if (kind == VARIABLE) return false;

    val1 = REF2VAL (*ref);

    if (kind == INTEGER_CONSTANT) {
	*val = val1;
	return true;
    }

    if (kind == DYNAM_ATOM) {
	/* C'est un terminal de la grammaire, on regarde s'il peut etre interprete comme un entier */
	/* val1 est une ste */
	char *str, *ptr;

	ptr = str = sxstrget (val1);
	*val = (SXINT) strtol (str, &ptr, 10);
	return ptr != str; /* Si #, c'est un entier */
    }

    if (kind != INTEGER_OP) return false;

    if (max_term==0)
	return false;

    pdrcg = drcg+drcg_tree2attr[*tree_id].pid;
    bot = pdrcg->term_disp [val1];
    op_ref = pdrcg->term_list [bot];

    /* On prend le 1er operande */
    op1_tree = *tree_id;
    op1_ref = pdrcg->term_list [bot+1];

    if (!integer_val (&op1_tree, &op1_ref, &val1)) return false;

    int_op = REF2VAL (op_ref);

    if (int_op == UMINUS) {
	*val = -val1;
	return true;
    }

    /* On prend le 2eme operande */
    op2_tree = *tree_id;
    op2_ref = pdrcg->term_list [bot+2];

    if (!integer_val (&op2_tree, &op2_ref, &val2)) return false;

    switch (int_op) {
    case DIVIDE:
	*val = val1 / val2;
	break;

    case MINUS:
	*val = val1 - val2;
	break;

    case MODULO:
	*val = val1 % val2;
	break;

    case MULTIPLY:
	*val = val1 * val2;
	break;

    case PLUS:	
	*val = val1 + val2;
	break;

#if EBUG
    default:
	sxtrap (ME, "integer_val");
#endif
    }

    return true;
}


#if occur_check==1
static bool occur ();

static bool
occur_body (tree_var, X, tree_t1, t1)
    SXINT tree_var, X, tree_t1, t1;
{
    SXINT	kind;

    kind = REF2KIND (t1);

    if (kind == VARIABLE) {
	/* Variable */
	/* Est-elle liee ? */
	DRCGsubstitute (&tree_t1, &t1);

	kind = REF2KIND (t1);

	if (kind == VARIABLE) {
	    /* variable libre */
	    if (tree_var == tree_t1 && X == t1)
		/* C'est X */
		return true;	/* Le test d'occurrence a echoue */
	}
    }

    if (kind != VARIABLE) {
	if (occur (tree_var, X, tree_t1, t1))
	    return true;
    }

    return false;
}




static bool
occur (tree_var, X, tree_term, t)
    SXINT tree_var, X, tree_term, t;
{
    /* Test d'occurrence de la variable (tree_var, X) ds le (vrai) terme (tree_term, t) */
    /* Resultat :
       1 => X \in t (et t est non clos)
       0 => t est clos
       -1 => t est non clos
    */
    SXINT	kind, bot, top, t1, val, tree_t1;
    struct drcg_struct	*pdrcg;

    kind = REF2KIND (t);
    
    if (kind == ATOM || kind == INTEGER_CONSTANT || kind == DYNAM_ATOM)
	/* clos */
	return false;

    t = REF2VAL (t);
    pdrcg = drcg+drcg_tree2attr [tree_term].pid;

    if (max_term!=0) {
	if (kind == TERM || kind == INTEGER_OP) {
	    /* t est un terme ou une operation arithmetique */
	    bot = pdrcg->term_disp [t];
	    top = pdrcg->term_disp [t+1];

	    /* On saute le foncteur */
	    while (++bot < top) {
		if (occur_body (tree_var, X, tree_term, pdrcg->term_list [bot]))
		    return true;
	    }

	}
    }
    else {
	/* t est une liste non close */
	if (occur_body (tree_var, X, tree_term, pdrcg->list_list [t]))
	    return true;

	if (occur_body (tree_var, X, tree_term, pdrcg->list_list [t+1]))
	    return true;
    }

    return false;
}
#endif


static bool
unify_int_expr (val, tree, expr)
    SXINT val, tree, expr;
{
    SXINT kind, bot, op1_tree, op1_ref, op1_kind, op2_tree, op2_ref, op2_kind, int_op, op_ref, val1, val2;
    struct drcg_struct	*pdrcg;

    kind = REF2KIND (expr);

    if (kind == VARIABLE) {
	create_substitution (tree, expr, tree, KV2REF (INTEGER_CONSTANT, val));
	return true;
    }

#if EBUG
    if (kind == INTEGER_CONSTANT)
	sxtrap (ME, "unify_int_expr");
#endif

    if (kind != INTEGER_OP) return false;

    pdrcg = drcg+drcg_tree2attr [tree].pid;
    bot = pdrcg->term_disp [REF2VAL (expr)];

    op1_tree = tree;
    op1_ref = pdrcg->term_list [bot+1];
    op1_kind = integer_val (&op1_tree, &op1_ref, &val1);

    op_ref = pdrcg->term_list [bot];
    int_op = REF2VAL (op_ref);

    if (int_op == UMINUS) {
#if EBUG
	if (op1_kind)
	    sxtrap (ME, "unify_int_expr");
#endif

	return unify_int_expr (-val, op1_tree, op1_ref);
    }

    op2_tree = tree;
    op2_ref = pdrcg->term_list [bot+2];
    op2_kind = integer_val (&op2_tree, &op2_ref, &val2);

    if (!op1_kind && !op2_kind) return false;

    if (op2_kind) {
	val1 = val2;
	op2_tree = op1_tree;
	op2_ref = op1_ref;
    }
    else {
    }

    switch (int_op) {
    case DIVIDE:
	if (op1_kind)
	    return unify_int_expr (val1/val, op2_tree, op2_ref);
	else
	    return unify_int_expr (val*val1, op2_tree, op2_ref);

    case MINUS:
	if (op1_kind)
	    return unify_int_expr (val1-val, op2_tree, op2_ref);
	else
	    return unify_int_expr (val+val1, op2_tree, op2_ref);

    case MODULO:
	return false; /* pas unique */

    case MULTIPLY:
	return unify_int_expr (val/val1, op2_tree, op2_ref);

    case PLUS:	
	return unify_int_expr (val-val1, op2_tree, op2_ref);

#if EBUG
    default:
	sxtrap (ME, "unify_int_expr");
#endif
    }
}



static bool
unify (sub_tree1, t1, sub_tree2, t2)
    SXINT sub_tree1, t1, sub_tree2, t2;
{
    SXINT		x, t, bot1, bot2, top1, sub_tree_var, sub_tree_term, what;
    SXINT 	t1_kind, t2_kind, t1_val, t2_val, val, t1_ref, t2_ref;
    char 	*str, *ptr;
    bool	is_t1_var, is_t2_var, is_t_var;
    struct drcg_struct	*pdrcg1, *pdrcg2;
    SXINT			pid1, pid2;


    if (REF2KIND (t1) == VARIABLE) {
	/* variable */
	DRCGsubstitute (&sub_tree1, &t1);
	/* Ici, terme, liste, ... ou une variable libre */
	is_t1_var = (REF2KIND (t1) == VARIABLE);
    }
    else
	is_t1_var = false;

    if (REF2KIND (t2) == VARIABLE) {
	/* variable */
	DRCGsubstitute (&sub_tree2, &t2);
	/* Ici, terme, liste, ... ou une variable libre */
	is_t2_var = (REF2KIND (t2) == VARIABLE);
    }
    else
	is_t2_var = false;

    if (sub_tree1 == sub_tree2 && t1 == t2)
	/* Variables ou termes identiques => substitution vide, unification reussie */
	return true;
	
    if (is_t1_var || is_t2_var) {
	/* L'un des 2 termes (au moins) est une variable libre */
	if (is_t1_var) {
	    x = t1, sub_tree_var = sub_tree1, t = t2, sub_tree_term = sub_tree2;

#if occur_check==1
	    is_t_var = is_t2_var;
#endif
	}
	else {
	    x = t2, sub_tree_var = sub_tree2, t = t1, sub_tree_term = sub_tree1;

#if occur_check==1
	    is_t_var = is_t1_var;
#endif
	}

#if occur_check==1
	if (!is_t_var && occur (sub_tree_var, x, sub_tree_term, t)) {
	    /* La variable (sub_tree_var,x) occure ds t qui n'est pas une variable */
	    return false;
	}
#endif

	create_substitution (sub_tree_var, x, sub_tree_term, t);
	return true;
    }

    /* t1 et t2 ne sont pas des variables */
    t1_kind = REF2KIND (t1);
    t2_kind = REF2KIND (t2);

    what = what2do [t1_kind] [t2_kind];

    if (what >= 0)
	return what == 0 ? false : t1 == t2 /* ATOM/ATOM ou INTEGER_CONSTANT/INTEGER_CONSTANT */;

    t1_val = REF2VAL (t1);
    t2_val = REF2VAL (t2);

    pid1 = drcg_tree2attr [sub_tree1].pid;
    pdrcg1 = drcg+pid1;
    pid2 = drcg_tree2attr [sub_tree2].pid;
    pdrcg2 = drcg+pid2;

    switch (what) {
    case ADA: /* L'un au moins de t1 ou t2 est un atome dynamique */
	if (max_atom!=0) {
	    if (t1_kind == ATOM)
		t1_val = atom2dynam_atom [pid1] [t1_val];

	    if (t2_kind == ATOM)
		t2_val = atom2dynam_atom [pid2] [t2_val];
	}

	return t1_val == t2_val;


    case LL: /* 2 listes, statiques ou dynamiques */
	if (has_prolog==1) {
	    if (t1_val == 0 || t2_val == 0)
		/* unifiable ssi les 2 listes sont vides */
		return t1_val == 0 && t2_val == 0;

	    t1_ref = (t1_kind == DYNAM_LIST) ? XxY_X (dynam_list, t1_val) : pdrcg1->list_list [t1_val];
	    t2_ref = (t2_kind == DYNAM_LIST) ? XxY_X (dynam_list, t2_val) : pdrcg2->list_list [t2_val];
	
	    if (!unify (sub_tree1, t1_ref, sub_tree2, t2_ref))
		return false;

	    t1_ref = (t1_kind == DYNAM_LIST) ? XxY_Y (dynam_list, t1_val) : pdrcg1->list_list [t1_val+1];
	    t2_ref = (t2_kind == DYNAM_LIST) ? XxY_Y (dynam_list, t2_val) : pdrcg2->list_list [t2_val+1];
	
	    return unify (sub_tree1, t1_ref, sub_tree2, t2_ref);
	}

	/* Les 2 listes sont obligatoirement statiques */
	if (t1_val == 0 || t2_val == 0)
	    /* l'un est la liste vide et pas l'autre */
	    return false;

	t1_ref = pdrcg1->list_list [t1_val];
	t2_ref = pdrcg2->list_list [t2_val];
	
	if (!unify (sub_tree1, t1_ref, sub_tree2, t2_ref))
	    return false;

	t1_ref = pdrcg1->list_list [t1_val+1];
	t2_ref = pdrcg2->list_list [t2_val+1];
	
	return unify (sub_tree1, t1_ref, sub_tree2, t2_ref);

    case TT: /* On a deux termes */
	bot1 = pdrcg1->term_disp [t1_val];
	bot2 = pdrcg2->term_disp [t2_val];

	if (t1_val == t2_val) {
	    /* meme terme => meme foncteur et meme arite */
	    top1 = pdrcg1->term_disp [t1_val+1];
	}
	else {
	    if (pdrcg1->term_list [bot1] != pdrcg2->term_list [bot2])
		/* foncteurs differents */
		return false;

	    top1 = pdrcg1->term_disp [t1_val+1];

	    if (top1-bot1 != pdrcg2->term_disp [t2_val+1]-bot2)
		/* arites differentes */
		return false;
	}

	while (++bot1 < top1) {
	    bot2++;

	    if (!unify (sub_tree1, pdrcg1->term_list [bot1], sub_tree2, pdrcg2->term_list [bot2]))
		return false;
	}

	return true;

    case II: /* INT/INT (au moins un INTEGER_OP) */
	if (t1_kind == INTEGER_OP) {
	    if (integer_val (&sub_tree1, &t1, &t1_val))
		t1_kind = INTEGER_CONSTANT;
	}
	
	if (t2_kind == INTEGER_OP) {
	    if (integer_val (&sub_tree2, &t2, &t2_val))
		t2_kind = INTEGER_CONSTANT;
	}

	if (t1_kind == INTEGER_CONSTANT && t2_kind == INTEGER_CONSTANT)
	    return t1_val == t2_val;

	if (t1_kind == INTEGER_OP && t2_kind == INTEGER_OP)
	    /* On pourrait repondre vrai si les expr entieres etaient traitees comme
	       des contraintes! */
	    return false;

	/* Si l'echec du calcul de la valeur est du a la presence d'une seule variable
	   libre, on peut (peut etre) l'instancier :
	   15 et +(*(2,X),3) => X=6
	   16 et +(*(2,X),3) => echec */
	if (t2_kind == INTEGER_CONSTANT) {
	    t1_val = t2_val;
	    sub_tree2 = sub_tree1;
	    t2 = t1;
	}

	return unify_int_expr (t1_val, sub_tree2, t2);

    case DAI: /* DYNAM_ATOM/INT (INT_CST ou INTEGER_OP) */
	if (t2_kind == DYNAM_ATOM) {
	    t2_kind = t1_kind;
	    val = t2_val, t2_val = t1_val, t1_val = val;
	    sub_tree2 = sub_tree1;
	    t2 = t1;
	}

	ptr = str = sxstrget (t1_val);
	t1_val = (SXINT) strtol (str, &ptr, 10);

	if (ptr == str)
	    return false; /* L'atome dynamique n'est pas un entier */
	
	if (has_integer_op==1) {
	    if (t2_kind == INTEGER_OP) {
		if (integer_val (&sub_tree2, &t2, &t2_val))
		    t2_kind = INTEGER_CONSTANT;
	    }

	    if (t2_kind == INTEGER_CONSTANT)
		return t1_val == t2_val;

	    return unify_int_expr (t1_val, sub_tree2, t2);
	}

	return t1_val == t2_val;

#if EBUG
    default:
	sxtrap (ME, "unify");
#endif

    }

    return true;
}


static void
var_hd_oflw ()
{
    var_hd_size *= 2;
    var_hd = (SXINT*) sxrealloc (var_hd, var_hd_size+1, sizeof (SXINT));
}

static void
node_list_oflw ()
{
    node_list_size *= 2;
    node_list = (SXINT*) sxrealloc (node_list, node_list_size+1, sizeof (SXINT));
}

static void
tree_oflw ()
{
    tree_size *= 2;
    drcg_tree2attr = (struct drcg_tree2attr*) sxrealloc (drcg_tree2attr, tree_size+1, sizeof (struct drcg_tree2attr));
}



/* ********************************** PREDEFINED PROLOG PREDICATES ********************************** */

static bool
list2stack (tree_id, list_ref)
    SXINT tree_id, list_ref;
{
    SXINT			kind, list_val, elem_ref;
    bool		end_reached = false;

    do {
	if ((kind = REF2KIND (list_ref)) == VARIABLE) {
	    /* Variable */
	    DRCGsubstitute (&tree_id, &list_ref);
	    kind = REF2KIND (list_ref);
	}

	if (kind == ATOM || kind == INTEGER_CONSTANT
	    || kind == DYNAM_ATOM
	    || kind == TERM
	    || kind == INTEGER_OP
	    )
	    return false;

	if (kind != VARIABLE && list_ref != EMPTY_LIST) {
	    list_val = REF2VAL (list_ref);

	    if (kind == DYNAM_LIST)
		elem_ref = XxY_X (dynam_list, list_val);
	    else
		/* kind == STATIC_LIST */
		elem_ref = drcg [drcg_tree2attr [tree_id].pid].list_list [list_val];
	}
	else {
	    elem_ref = 0;
	    end_reached = true;
	}

	if (++list_stack_top >= list_stack_size) {
	    list_stack = (struct list_struct*) sxrealloc (list_stack,
							  (list_stack_size *= 2) + 1,
							  sizeof (struct list_struct));
	    list_stack_ptr = list_stack + list_stack_top;
	}
	else
	    list_stack_ptr++;

	list_stack_ptr->tree_id = tree_id;
	list_stack_ptr->list_ref = list_ref;
	list_stack_ptr->elem_ref = elem_ref;

	if (!end_reached) {
	    if (kind == DYNAM_LIST)
		list_ref = XxY_Y (dynam_list, list_val);
	    else
		/* kind == STATIC_LIST */
		list_ref = drcg [drcg_tree2attr [tree_id].pid].list_list [list_val+1];
	}
    } while (!end_reached);

    return true;
}

static bool
unify_list (list_ptr1, list_ptr2, lgth)
    struct list_struct *list_ptr1, *list_ptr2;
    SXINT 		lgth;
{
    while (lgth-- > 0) {
	if (!unify (list_ptr1->tree_id, list_ptr1->elem_ref,
		    list_ptr2->tree_id, list_ptr2->elem_ref))
	    return false;

	list_ptr1++, list_ptr2++;
    }

    return true;
}


static bool
unify_reverse_list (list_ptr1, list_ptr2, lgth)
    struct list_struct *list_ptr1, *list_ptr2;
    SXINT 		lgth;
{
    while (lgth-- > 0) {
	if (!unify (list_ptr1->tree_id, list_ptr1->elem_ref,
		    list_ptr2->tree_id, list_ptr2->elem_ref))
	    return false;

	list_ptr1++, list_ptr2--;
    }

    return true;
}


static SXINT
set_dynam_var ()
{
    /* Cree ds la zone des variables du pere (glob_tree_top) une nouvelle variable. */
    if (++var_hd_top >= var_hd_size)
	var_hd_oflw ();

    var_hd [var_hd_top] = 0;	/* clear */
    return KV2REF (VARIABLE, var_hd_top - drcg_tree2attr [glob_tree_top].var_hd);
}

static SXINT
set_dynam_list (list_ptr, lgth, tail)
    struct list_struct *list_ptr;
    SXINT 		lgth, tail;
{
    SXINT DL, DV;
    struct list_struct *ls_ptr;

    DL = tail;

    if (lgth > 0) {
	ls_ptr = list_ptr+lgth-1;

	do {
	    DV = set_dynam_var ();
	    create_substitution (glob_tree_top, DV, ls_ptr->tree_id, ls_ptr->elem_ref);
	    XxY_set (&dynam_list, DV, DL, &DL);
	    DL = KV2REF(DYNAM_LIST, DL);
	} while (--ls_ptr >= list_ptr);
    }

    return DL;
}

static SXINT
set_dynam_reverse_list (list_ptr, lgth, tail)
    struct list_struct *list_ptr;
    SXINT 		lgth, tail;
{
    SXINT DL, DV;

    DL = tail;

    while (lgth-- > 0) {
	list_ptr++;
	DV = set_dynam_var ();
	create_substitution (glob_tree_top, DV, list_ptr->tree_id, list_ptr->elem_ref);
	XxY_set (&dynam_list, DV, DL, &DL);
	DL = KV2REF(DYNAM_LIST, DL);
    }

    return DL;
}

static SXINT
dum_dynam_list (lgth)
    SXINT 	lgth;
{
    SXINT DL;

    DL = EMPTY_LIST;

     while (lgth-- > 0) {
	XxY_set (&dynam_list, DUM_VAR, DL, &DL);
	DL = KV2REF(DYNAM_LIST, DL);
    }

    return DL;
}

static bool
is (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* X is term */
    /* unifie X et term */
    return unify (tree_id, param_ptr [0], tree_id, param_ptr [1]);
}


static bool
random_ppp (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* random (N, Max) */
    /* (subtree_id, sub_tree_ptr) est une reserve de 0 variables libres */
    /* Genere aleatoirement un nombre entier entre N entre 1 et max */

    SXINT		ref1, ref2, tree1, tree2, N, Max;

    ref2 = param_ptr [1];
    tree2 = tree_id;
    /* Max non connu, on differe la decision (traitement des contraintes) */
    if (!integer_val (&tree2, &ref2, &Max)) return true;

    ref1 = param_ptr [0];
    tree1 = tree_id;

    if (integer_val (&tree1, &ref1, &N)) {
	/* N et Max sont connus, on verifie les bornes */
	return 1<=N && N<=Max;
    }

    /* Max est connu, N est inconnu */
    if (REF2KIND (ref1) == VARIABLE) {
	N = (random ()%Max) + 1;
	create_substitution (tree1, ref1, tree1, KV2REF(INTEGER_CONSTANT,N));
	return true;
    }

    /* N est inconnu et ce n'est pas une variable libre */
    /* random n'est pas un test de bornes, je rends faux! */
    return false;
}

static bool
compare (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* compare (X1, X2) */
    /* (subtree_id, sub_tree_ptr) est une reserve de 0 variables libres */
    SXINT		ref1, ref2, tree1, tree2, val1, val2;

    ref1 = param_ptr [0];
    tree1 = tree_id;

    /* 1er operande non connu, on differe la decision (traitement des contraintes) */
    if (!integer_val (&tree1, &ref1, &val1)) return true;

    ref2 = param_ptr [1];
    tree2 = tree_id;

    /* 2eme operande non connu, on differe la decision (traitement des contraintes) */
    if (!integer_val (&tree2, &ref2, &val2)) return true;

    switch(ppp) {
    case LESS:
	return val1 < val2;
    case SUP:
	return val1 > val2;
    case LESS_EQUAL:
	return val1 <= val2;
    case SUP_EQUAL:
	return val1 >= val2;

#if EBUG
    default:
	sxtrap (ME, "prolog_compare");
#endif
    }
    
}


static bool
is_closed (tree_id, ref)
    SXINT tree_id, ref;
{
    SXINT kind, val, bot, top, op;
    struct drcg_struct	*pdrcg;

    kind = REF2KIND (ref);

    if (kind == VARIABLE) {
	/* C'est une variable */
	DRCGsubstitute (&tree_id, &ref);
	kind = REF2KIND (ref);
    }

    val = REF2VAL (ref);
    pdrcg = drcg+drcg_tree2attr [tree_id].pid;

    switch (kind) {
    case VARIABLE:
	return false;
    case ATOM:
    case DYNAM_ATOM:
    case INTEGER_CONSTANT:
	return true;

        case TERM:
	bot = pdrcg->term_disp [val];
	top = pdrcg->term_disp [val+1];

	while (++bot < top) {
	    if (!is_closed (tree_id, pdrcg->term_list [bot]))
		return false;
	}

	return true;

        case INTEGER_OP:
	bot = pdrcg->term_disp [val];
	op = REF2VAL (pdrcg->term_list [bot]);

	if (op != UMINUS) {
	    if (!is_closed (tree_id, pdrcg->term_list [++bot]))
		return false;
	}

	return is_closed (tree_id, pdrcg->term_list [++bot]);

    case STATIC_LIST:
	if (!is_closed (tree_id, pdrcg->list_list [val]))
	    return false;

	return is_closed (tree_id, pdrcg->list_list [val+1]);

    case DYNAM_LIST:
	if (!is_closed (tree_id, XxY_X (dynam_list, val)))
	    return false;

	return is_closed (tree_id, XxY_Y (dynam_list, val));

#if EBUG
        default:
	sxtrap (ME, "is_closed");
#endif
    }
}


static bool
reverse (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* reverse (L1, L2) */
    /* (subtree_id, sub_tree_ptr) est une reserve de 0 variables libres */
    SXINT		kind, l1, l2, l3, DL;

    list_stack_ptr = list_stack = list_stack1, list_stack_top = 0, list_stack_size = list_stack1_size;
    if (!list2stack (tree_id, param_ptr [0])) return false;
    list_stack1 = list_stack, list_stack1_ptr = list_stack_ptr, l1 = list_stack_top-1,
    list_stack1_size = list_stack_size;

    list_stack_ptr = list_stack = list_stack2, list_stack_top = 0, list_stack_size = list_stack2_size;
    if (!list2stack (tree_id, param_ptr [1])) return false;
    list_stack2 = list_stack, list_stack2_ptr = list_stack_ptr, l2 = list_stack_top-1,
    list_stack2_size = list_stack_size;

    kind = (list_stack1_ptr->list_ref == EMPTY_LIST) << 1;
    kind += (list_stack2_ptr->list_ref == EMPTY_LIST);

    switch (kind) {
    case 3:
	/* close/close */
	if (l1 != l2) return false;

	return unify_reverse_list (list_stack1+1, list_stack2_ptr-1, l1);

    case 2:
	/* close/open */
	if (l2 > l1) return false;

	if (!unify_reverse_list (list_stack1+l1-l2+1, list_stack2_ptr-1, l2))
	    return false;

	/* On cree la liste de longueur l1-l2, inverse du prefixe de L1 */
	DL = set_dynam_reverse_list (list_stack1, l1-l2, EMPTY_LIST);
	/* L2_tail = dynam_list */
	create_substitution (list_stack2_ptr->tree_id, list_stack2_ptr->list_ref,
			     glob_tree_top, DL);
	return true;

    case 1:
	/* close/open */
	if (l1 > l2) return false;

	if (!unify_reverse_list (list_stack1+1, list_stack2_ptr-1, l1))
	    return false;

	/* On cree la liste de longueur l2-l1, inverse du prefixe de L2 */
	DL = set_dynam_reverse_list (list_stack2, l2-l1, EMPTY_LIST);
	/* L1_tail = dynam_list */
	create_substitution (list_stack1_ptr->tree_id, list_stack1_ptr->list_ref,
			     glob_tree_top, DL);
	return true;

    case 0:
	/* open/open */
	return true; /* Contrainte verifiee + tard */
    }
}

static bool
nth (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* nth (L1, N, Elem) */
    /* (subtree_id, sub_tree_ptr) est une reserve de 0 variables libres */
    SXINT		l1, Lgth_ref, Lgth_tree, Lgth_kind, val, kind, la1;
    bool	done;

    list_stack_ptr = list_stack = list_stack1, list_stack_top = 0, list_stack_size = list_stack1_size;
    if (!list2stack (tree_id, param_ptr [0])) return false;
    list_stack1 = list_stack, list_stack1_ptr = list_stack_ptr, l1 = list_stack_top-1,
    list_stack1_size = list_stack_size;

    kind = (list_stack1_ptr->list_ref == EMPTY_LIST) << 1;

    Lgth_ref = param_ptr [1];
    Lgth_tree = tree_id;

    if (integer_val (&Lgth_tree, &Lgth_ref, &val))
	kind += 1;
    else
	Lgth_kind = REF2KIND (Lgth_ref);

    switch (kind) {
    case 3:
	/* close/val/? */
	if (val > l1) return false;

	list_stack1_ptr = list_stack1 + val;
	return unify (tree_id, param_ptr [2], list_stack1_ptr->tree_id, list_stack1_ptr->elem_ref);

    case 1:
	/* open/val/? */
	if (val > l1) return true; /* contrainte verifiee = tard */

	list_stack1_ptr = list_stack1 + val;
	return unify (tree_id, param_ptr [2], list_stack1_ptr->tree_id, list_stack1_ptr->elem_ref);

    case 2:
	/* close/?/? */
	if (l1 == 0) return false;

	/* On ne pourrait repondre de facon sure que si au plus un couple (N, Elem) marchait
	   Echec => return false
	   1 couple marche => return true
	   plus d'un couple marche => Quelle unif choisir (il en faut une et une seule) ? */

	return true;

    case 0:
	/* open/?/? */
	return true; /* contrainte verifiee + tard */
    }
}

static bool
size (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* size (L1, Lgth) */
    /* (subtree_id, sub_tree_ptr) est une reserve de 0 variables libres */
    SXINT		l1, Lgth_ref, Lgth_tree, Lgth_kind, val, DL;

    list_stack_ptr = list_stack = list_stack1, list_stack_top = 0, list_stack_size = list_stack1_size;
    if (!list2stack (tree_id, param_ptr [0])) return false;
    list_stack1 = list_stack, list_stack1_ptr = list_stack_ptr, l1 = list_stack_top-1,
    list_stack1_size = list_stack_size;

    Lgth_ref = param_ptr [1];
    Lgth_tree = tree_id;

    if (integer_val (&Lgth_tree, &Lgth_ref, &val))
	Lgth_kind = 0;
    else
	Lgth_kind = REF2KIND (Lgth_ref);

    if (list_stack1_ptr->list_ref == EMPTY_LIST) {
	/* L1 est ferme */
	if (Lgth_kind == 0) {
	    /* C'est une expression dont on a calcule la valeur */
	    return val == l1;
	}

	if (has_integer_op==1)
	    return unify_int_expr (l1, Lgth_tree, Lgth_ref);

	if (Lgth_kind != VARIABLE) return false;

	create_substitution (Lgth_tree, Lgth_ref, Lgth_tree, KV2REF (INTEGER_CONSTANT, l1));
	return true;
    }

    /* L1 est ouvert */
    if (Lgth_kind != 0)
	/* et Lgth n'est pas calcule */
	return true; /* Contrainte verifiee + tard */

    /* Lgth == val */
    if (val < l1) return false;

    /* On cree une liste bidon de longueur val-l1 */
    DL = dum_dynam_list (val-l1);
    create_substitution (list_stack1_ptr->tree_id, list_stack1_ptr->list_ref,
			 list_stack1_ptr->tree_id, DL);
    return true;
}


static bool
member (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* member (E, L1) */
    /* (subtree_id, sub_tree_ptr) est une reserve de 0 variables libres */
    SXINT		l1, la1;
    bool	done;

    list_stack_ptr = list_stack = list_stack1, list_stack_top = 0, list_stack_size = list_stack1_size;
    if (!list2stack (tree_id, param_ptr [1])) return false;
    list_stack1 = list_stack, l1 = list_stack_top-1, list_stack1_size = list_stack_size;

    /* Si open on retourne vrai (la contrainte sera verifiee + tard) ! */
    if (list_stack_ptr->list_ref != EMPTY_LIST) return true;

    if (l1 == 0) return false;

    if (!is_closed (tree_id, param_ptr [0]))
	return true; /* on manque d'infos */

    la1 = 0;
    list_stack1_ptr = list_stack1;
    done = false;

    do {
	list_stack1_ptr++;
	done = unify (tree_id, param_ptr [0], list_stack1_ptr->tree_id, list_stack1_ptr->elem_ref);
    } while ((++la1 < l1) && !done);

    return done;
}


static bool
concat (tree_id, /* sub_tree_id, */ param_ptr)
    SXINT tree_id, /* sub_tree_id, */ *param_ptr;
{
    /* concat (L1, L2, L3)
       (tree_id, param_ptr) = L1/L2/L3 */
    /* subtree_id est l'arbre concat   */
    SXINT		kind, l1, l2, l3, DL, DV, la3, lb3;
    bool	done;

    list_stack_ptr = list_stack = list_stack1, list_stack_top = 0, list_stack_size = list_stack1_size;
    if (!list2stack (tree_id, param_ptr [0])) return false;
    list_stack1 = list_stack, list_stack1_ptr = list_stack_ptr, l1 = list_stack_top-1,
    list_stack1_size = list_stack_size;

    list_stack_ptr = list_stack = list_stack2, list_stack_top = 0, list_stack_size = list_stack2_size;
    if (!list2stack (tree_id, param_ptr [1])) return false;
    list_stack2 = list_stack, list_stack2_ptr = list_stack_ptr, l2 = list_stack_top-1,
    list_stack2_size = list_stack_size;

    list_stack_ptr = list_stack = list_stack3, list_stack_top = 0, list_stack_size = list_stack3_size;
    if (!list2stack (tree_id, param_ptr [2])) return false;
    list_stack3 = list_stack, list_stack3_ptr = list_stack_ptr, l3 = list_stack_top-1,
    list_stack3_size = list_stack_size;

    kind = (list_stack1_ptr->list_ref == EMPTY_LIST) << 2;
    kind += (list_stack2_ptr->list_ref == EMPTY_LIST) << 1;
    kind += (list_stack3_ptr->list_ref == EMPTY_LIST);

    switch (kind) {
    case 7:
	/* clos/clos/clos */
	if (l1+l2 != l3) return false;

	if (!unify_list (list_stack1+1, list_stack3+1, l1))
	    return false;

	return unify_list (list_stack2+1, list_stack3+l1+1, l2);


    case 6:
	/* clos/clos/open */
	if (l3 > l1+l2) return false;

	if (l3 >= l1) {
	    if (!unify_list (list_stack1+1, list_stack3+1, l1))
		return false;

	    if (!unify_list (list_stack2+1, list_stack3+l1+1, l3-l1))
		return false;

	    /* L3-tail = L2_middle */
	    list_stack2_ptr = list_stack2 + l3-l1+1;
	    create_substitution (list_stack3_ptr->tree_id, list_stack3_ptr->list_ref,
				 list_stack2_ptr->tree_id, list_stack2_ptr->list_ref);
	    return true;
	}
	/* l3 < l1 */
	if (!unify_list (list_stack1+1, list_stack3+1, l3))
	    return false;

	/* DV = L2 */
	DV = set_dynam_var ();
	create_substitution (glob_tree_top, DV, tree_id, param_ptr [1]);
	/* On cree une liste dynamique, copie de la fin de la liste L1 */
	/* dont la queue (liste vide) est remplacee par DV */
	DL = set_dynam_list (list_stack1+1+l3, l1-l3, DV);
	/* L3_tail = DV */
	create_substitution (list_stack3_ptr->tree_id, list_stack3_ptr->list_ref,
			     glob_tree_top, DL);

	return true;

    case 5:
	/* clos/open/clos */
	if (l1+l2 > l3) return false;

	if (!unify_list (list_stack1+1, list_stack3+1, l1))
	    return false;

	if (!unify_list (list_stack2+1, list_stack3+l1+1, l2))
	    return false;

	/* L2_tail = L3_middle */
	list_stack3_ptr = list_stack3 + l1+l2+1;
	create_substitution (list_stack2_ptr->tree_id, list_stack2_ptr->list_ref,
			     list_stack3_ptr->tree_id, list_stack3_ptr->list_ref);
	return true;
	
    case 4:
	/* clos/open/open */
	if (l3 < l1) {
	    if (!unify_list (list_stack1+1, list_stack3+1, l3))
		return false;

	    
	    /* DV = L2 */
	    DV = set_dynam_var ();
	    create_substitution (glob_tree_top, DV, tree_id, param_ptr [1]);
	    /* On cree une liste dynamique, copie de la fin de la liste L1 */
	    /* dont la queue (liste vide) est remplacee par DV */
	    DL = set_dynam_list (list_stack1+1+l3, l1-l3, DV);
	    /* L3_tail = DV */
	    create_substitution (list_stack3_ptr->tree_id, list_stack3_ptr->list_ref,
				 glob_tree_top, DL);
	    return true;
	}

	if (l3 < l1+l2) {
	    if (!unify_list (list_stack1+1, list_stack3+1, l1))
		return false;

	    if (!unify_list (list_stack2+1, list_stack3+l1+1, l3-l1))
		return false;

	    /* L3_tail = L2_middle */
	    list_stack2_ptr = list_stack2 + l3-l1+1;
	    create_substitution (list_stack3_ptr->tree_id, list_stack3_ptr->list_ref,
				 list_stack2_ptr->tree_id, list_stack2_ptr->list_ref);
	    return true;

	}

	/* l3 >= l1+l2 */
	if (!unify_list (list_stack1+1, list_stack3+1, l1))
	    return false;

	if (!unify_list (list_stack2+1, list_stack3+l1+1, l2))
	    return false;

	/* L2_tail = L3_middle */
	list_stack3_ptr = list_stack3 + l1+l2+1;
	create_substitution (list_stack2_ptr->tree_id, list_stack2_ptr->list_ref,
			     list_stack3_ptr->tree_id, list_stack3_ptr->list_ref);
	return true;

    case 3:
	/* open/clos/clos */
	if (l1+l2 > l3) return false;
	
	if (!unify_list (list_stack1+1, list_stack3+1, l1))
	    return false;

	if (!unify_list (list_stack2+1, list_stack3+l3-l2+1, l2))
	    return false;

	/* On cree une liste dynamique, copie du milieu de la liste L3 */
	/* dont la queue (liste non vide) est remplacee par la liste vide */
	DL = set_dynam_list (list_stack3+1+l1, l3-l1-l2, EMPTY_LIST);
	/* L1_tail = X */
	create_substitution (list_stack1_ptr->tree_id, list_stack1_ptr->list_ref,
			     glob_tree_top, DL);
	return true;

    case 2:
	/* open/clos/open */
	/* return false; */
	return true; /* contrainte verifiee + tard */

    case 1:
	/* open/open/clos */
	if (l1+l2 > l3) return false;

	if (!unify_list (list_stack1+1, list_stack3+1, l1)) return false;

	/* Les ppp peuvent se [re]executer de facon non synchrone : sub_tree_id peut ne pas
	   etre un fils direct de tree_id. Le clonage du sous-arbre de racine ppp et surtout
	   le clonage de ses ancetres pour remonter jusqu'a tree_id semble delicat. De plus
	   on peut adopter un autre point de vue: une unification qui reussit donne un resultat
	   unique (et non une disjonction). Donc on retourne vrai, la situation s'eclaircira peut_etre
	   plus tard. De plus quand l'arbre est totalement construit, s'il reste des variables
	   libres (a la racine!), on peut les affecter au cours d'une derniere phase, comme avec des
	   contraintes, en parcourant leur domaine de definition (ce n'est pas fait) */
	return true;

    case 0:
	/* open/open/open */
	/* return false; */
	return true; /* contrainte verifiee + tard */
    }
}

static bool
ffalse ()
{
    return false;
}


static struct {
    bool (*call) ();
} execute_prolog [] = {
    {ffalse},
    {concat},
    {member},
    {size},
    {nth},
    {reverse},
    {compare},
    {compare},
    {compare},
    {compare},
    {random_ppp},
    {is},
};

/* ********************************** END PREDEFINED PROLOG PREDICATES ********************************** */



static void
clear_vars (var_nb, var_ptr)
    SXINT var_nb, *var_ptr;
{
    while (var_nb--)
	*var_ptr-- = 0;
}




#if EBUG
static void print_term ();

static void
print_list (t, sub_tree)
    SXINT t, sub_tree;
{
    SXINT		y, node, local_var, bot, atom, top, kind;
    struct drcg_struct	*pdrcg;

    kind = REF2KIND (t);

    if (kind == VARIABLE) {
	/* C'est une variable */
	DRCGsubstitute (&sub_tree, &t);
	kind = REF2KIND (t);
    }

    t = REF2VAL (t);
    pdrcg = drcg+drcg_tree2attr [sub_tree].pid;

    switch (kind) {
    case VARIABLE:
	/* C'est une variable libre, on sort son nom. */
	/* Elle est sensee etre la liste suffixe, on la fait preceder de "|" */
	printf ("|_%i%s", sub_tree,
		pdrcg->var_str [pdrcg->gvar_name [pdrcg->gvar_disp [drcg_tree2attr [sub_tree].prod]+t-1]]);
	break;

#if EBUG
    case ATOM:
    case DYNAM_ATOM:
    case TERM:
    case INTEGER_CONSTANT:
    case INTEGER_OP:
    default:
	sxtrap (ME, "print_list");
#endif


    case STATIC_LIST:
	if (t > 0) {
	    fputs (", ", stdout);
	    print_term (pdrcg->list_list [t], sub_tree);
	    print_list (pdrcg->list_list [t+1], sub_tree);
	}
	break;

    case DYNAM_LIST:
	if (t > 0) {
	    fputs (", ", stdout);
	    print_term (XxY_X (dynam_list, t), sub_tree);
	    print_list (XxY_Y (dynam_list, t), sub_tree);
	}
	break;
    }
}


static void
print_term (t, sub_tree)
    SXINT t, sub_tree;
{
    SXINT		y, node, local_var, bot, atom, top, kind, op;
    struct drcg_struct	*pdrcg;

    kind = REF2KIND (t);

    if (kind == VARIABLE) {
	/* C'est une variable */
	if (t != DUM_VAR) {
	    DRCGsubstitute (&sub_tree, &t);
	    kind = REF2KIND (t);
	}
    }

    t = REF2VAL (t);
    pdrcg = drcg+drcg_tree2attr [sub_tree].pid;
    
    switch (kind) {
    case VARIABLE:
	/* C'est une variable libre, on sort son nom. */
#if EBUG
	if (sub_tree == 0)
	    /* Les variables dynamiques ne sont jamais libres! */
	    sxtrap (ME, "print_term");
#endif

	printf ("_%i%s", sub_tree,
		pdrcg->var_str [pdrcg->gvar_name [pdrcg->gvar_disp [drcg_tree2attr [sub_tree].prod]+t-1]]);
	break;

    case ATOM:
	printf ("%s", pdrcg->atom_str [t]);
	break;

    case DYNAM_ATOM:
	printf ("%s", sxstrget (t));
	break;

    case INTEGER_CONSTANT:
	printf ("%i", t);
	break;

    case TERM:
	bot = pdrcg->term_disp [t];
	atom = pdrcg->term_list [bot];

	printf ("%s", pdrcg->atom_str [REF2VAL (atom)]);

	top = pdrcg->term_disp [t+1];

	if (++bot < top) {
	    putchar ('(');

	    for (;;) {
		print_term (pdrcg->term_list [bot], sub_tree);

		if (++bot == top)
		    break;

		fputs (", ", stdout);
	    }

	    putchar (')');
	}
	break;

    case INTEGER_OP:
    {
	bool	is_val1, is_val2;
	SXINT	op_tree, op_ref, val1, val2;

	bot = pdrcg->term_disp [t];
	op = REF2VAL (pdrcg->term_list [bot]);

	op_tree = sub_tree;
	op_ref = pdrcg->term_list [bot+1];
	is_val1 = integer_val (&op_tree, &op_ref, &val1);

	if (op != UMINUS) {
	    op_tree = sub_tree;
	    op_ref = pdrcg->term_list [bot+2];
	    is_val2 = integer_val (&op_tree, &op_ref, &val2);
	}
	else {
	    if (is_val1) {
		printf ("(-%i)", val1);
	    }
	    else
	    {
		fputs ("(-", stdout);
		print_term (pdrcg->term_list [bot+1], sub_tree);
		putchar (')');
	    }

	    break;
	}

	if (is_val1 && is_val2) {
	    switch (op) {
	    case DIVIDE:
		printf ("%i", val1 / val2);
		break;

	    case MINUS:
		printf ("%i", val1 - val2);
		break;

	    case MODULO:
		printf ("%i", val1 % val2);
		break;

	    case MULTIPLY:
		printf ("%i", val1 * val2);
		break;

	    case PLUS:	
		printf ("%i", val1 + val2);
		break;

#if EBUG
	    default:
		sxtrap (ME, "integer_val");
#endif
	    }

	    break;
	}

	putchar ('(');

	if (is_val1)
	    printf ("%i", val1);
	else
	    print_term (pdrcg->term_list [bot+1], sub_tree);

	if (op == MODULO)
	    fputs (" mod ", stdout);
	else
	    printf ("%s", pdrcg->ppp2str [op]);

	if (is_val2)
	    printf ("%i", val2);
	else
	    print_term (pdrcg->term_list [bot+2], sub_tree);

	putchar (')');
    }

	break;

    case STATIC_LIST:
	putchar ('[');

	if (t > 0) {
	    print_term (pdrcg->list_list [t], sub_tree);
	    print_list (pdrcg->list_list [t+1], sub_tree);
	}

	putchar (']');
	break;

    case DYNAM_LIST:
	putchar ('[');

	if (t > 0) {
	    print_term (XxY_X (dynam_list, t), sub_tree);
	    print_list (XxY_Y (dynam_list, t), sub_tree);
	}

	putchar (']');
	break;

#if EBUG
    default:
	sxtrap (ME, "print_term");
#endif

    }
}

static void
print_rule (tree)
    SXINT tree;
{
    SXINT			prod, Aij, A, param_nb, pos, j, k, item, node, X, Blk, ste, ppp, son_tree;
    SXINT			*param_ptr, *node_ptr, *top_node_ptr;
    bool		is_first;
    struct Aij_struct	*Aij_struct_ptr;
    SXINT			param, *item_ptr;
    SXINT			local_pid;
    char		*Astr;
    struct drcg_struct	*pdrcg;
    struct drcg_tree2attr	*ptree;

    ptree = drcg_tree2attr+tree;
    Aij = ptree->Aij;
    local_pid = ptree->pid;
    pdrcg = drcg+local_pid;
    Aij_struct_ptr = (*Aij2struct [local_pid])(Aij);
    Astr = Aij_struct_ptr->A;
    prod = ptree->prod;
    printf ("%i: %s(", tree, Astr);

    for (param = 0; param < Aij_struct_ptr->arity; param++) {
	printf ("[%i..%i]", Aij_struct_ptr->lb [param], Aij_struct_ptr->ub [param]);

	if (param+1 < Aij_struct_ptr->arity)
	    fputs (", ", stdout);
    }
    
    putchar (')');

    param_nb = pdrcg->param_nb [prod][0];

    if (param_nb > 0) {
	putchar ('(');
	param_ptr = &(pdrcg->term [prod] [0] [0]);
	j = 0;

	for (;;) {
	    print_term (*param_ptr++, tree);

	    if (++j == param_nb)
		break;
		
	    fputs (", ", stdout);
	}

	putchar (')');
    }

    printf  ("\t(%i)--> ", prod);

    item_ptr = &(pdrcg->param_nb [prod][1]);
    node_ptr = &(node_list [ptree->node_list]);
    top_node_ptr = &(node_list [drcg_tree2attr [tree+1].node_list]);
    is_first = true;

    while (node_ptr < top_node_ptr) {
	son_tree = *node_ptr++;

	if (son_tree < 0) {
	    if (is_first)
		is_first = false;
	    else
		fputs (", ", stdout);

	    putchar ('*'); /* Echec */
	    son_tree = -son_tree;
	}

	printf ("%i ", son_tree);
    }

    if (has_prolog==1 && prod <= pdrcg->cst.maxpppprod) {
	SXINT 	ppp_item, ppp_item_nb;
	SXINT	*ppp_prolon, *pbot, *ptop;
	SXINT	*param_ptr;

	ppp_prolon = pdrcg->ppp_prolon + prod;

	if ((ppp_item_nb = ppp_prolon [1] - (ppp_item = *ppp_prolon)) > 0) {
	    /* Cette prod a des ppp */
	    pbot = pdrcg->ppp_lispro + ppp_item;
	    ptop = pbot + ppp_item_nb;

	    if (!is_first)
		fputs (", {", stdout);
	    else
		putchar ('{');

	    is_first = true;

	    do {
		if (is_first)
		    is_first = false;
		else
		    fputs (", ", stdout);

		param_ptr = &(pdrcg->ppp_term [pdrcg->ppp_term_hd [ppp_item]]);
		/* predef_prolog_predicate */
		ppp = *pbot;

		if (ppp == IS || ppp == LESS || ppp == LESS_EQUAL || ppp == SUP || ppp == SUP_EQUAL) {
		    /* infixe */
		    if (ppp == IS) {
			/* On imprime le nom de la variable */
			printf ("_%i%s is ", tree,
				pdrcg->var_str [pdrcg->gvar_name [pdrcg->gvar_disp [drcg_tree2attr [tree].prod]+
								  REF2VAL (*param_ptr++)-1]]);
		    }
		    else {
			print_term (*param_ptr++, tree);
			printf ("%s", pdrcg->ppp2str [ppp]);
		    }

		    print_term (*param_ptr++, tree);
		}
		else {
		    printf ("%s", pdrcg->ppp2str [ppp]);
		    param_nb = &(pdrcg->ppp_term [pdrcg->ppp_term_hd [ppp_item+1]])-param_ptr;

		    if (param_nb > 0) {
			putchar ('(');
			j = 0;

			for (;;) {
			    print_term (*param_ptr++, tree);

			    if (++j == param_nb)
				break;
		
			    fputs (", ", stdout);
			}

			putchar (')');
		    }
		}

		ppp_item++;
	    } while (++pbot < ptop);

	    putchar ('}');
	}
    }

    fputs (".\n", stdout);
}

static void
print_tree (tree)
    SXINT tree;
{
    SXINT		*param_ptr, *node_ptr, *top_node_ptr;

    if (tree < 0)
	tree = -tree;

    print_rule (tree);

    node_ptr = &(node_list [drcg_tree2attr [tree].node_list]);
    top_node_ptr = &(node_list [drcg_tree2attr [tree+1].node_list]);

    while (node_ptr < top_node_ptr) {
	print_tree (*node_ptr++);
    }
}
#endif



/*
static SXINT
DRCGsem_eval (prod, nbnt)
    SXINT prod, nbnt;
{
    a0(v0) --> a1(v1) ... ap(vp) .

    Si echec unification => retour

    succes unification

       si non-cyclique ... => retour

       si cyclique
          soit sub_tree le sous-arbre germe
	  On regarde si une des variables (instanciee) X de glob_tree_top contient une valeur
	  strictement plus petite que la variable X de sub_tree => non cyclique, on deplie
	  (attention aux interactions avec unfold)

	  Sinon cyclique:
	     on supprime glob_tree_top
	     germe est cyclique
	     changer le root_set de germe
	     on ajoute dans germe[*] les valeurs des variables calculees pour glob_tree_top
	     en les changeant en des ref cycliques (0 -> *)
	     Changer en cyclique tous les sous-arbres de root_set qui contiennent germe
	     (les variables de niveau 0 sont copiees ds *)
	     Tester si p1: a -> B et p2: B -> A on est appele de facon cyclique a la fois sur p1 ET sur p2


    Si un sub_tree est cyclique, disons ap et soit vp son vecteur de param
    On essaie une unification entre (glob_tree_top, 0, vp) et (sub_tree, 0, vp')
    Le loop_nb de sub_tree vaut 0
    Quand on entre ds un sub_tree, on incremente son loop_nb (pour le coup d'apres eventuel) et on le
    decremente en sortie.
    l'instanciation d'une variable libre d'un arbre cyclique se fait suivant le modele * correspondant
    On ne sait donc instancier une variable libre que par l'un de ses modeles au plus (pas par une variable)
    La premiere fois qu'on traite un niveau loop_nb, on RAZ toutes les variables de ce niveau
    Si une variable d'un niveau vaut 0, elle est libre. C'est son instanciation qui ne suit pas le schema
    habituel. Soit (t, n, X) la variable X du sous-arbre (cyclique) t de niveau loop_nb=n
    instanciation avec une variable => echec
    instanciation avec un terme/liste, alors on essaie d'unifier avec les modeles *
    Supposons que les modeles * soient {[], (t, *, [X|R])} et que la liste en main est (t', 0, [a, b])
    l'unification avec [] echoue
    Si l'unification entre (t', 0, [a, b]) et (t, loop_nb[t], [X|R]) reussit alors la variable
    (t, n, X) sera instanciee par (t, loop_nb[t], [X|R])

}
*/

/* A FAIRE: statiquement de l'inference de type et reperer les incoherences. Ex:
   a([X|R]) --> b(R).
   b(f(X)) --> ...
   R est a la fois une variable de terme et une variable de liste!!
   NON : l'echec doit etre dynamique a l'unification
   */

static SXINT
DRCGsem_eval (prod, nbnt)
    SXINT prod, nbnt;
{
    /* sons_stack [1..nbnt] contient les sous-arbres */
    SXINT 		sub_tree, sub_prod, param_nb, j, t1, t2, Aij, var_nb, failed_pos, indx, nb, son_nb;
    SXINT			*node_ptr, *var_ptr, *t1_ptr, *t2_ptr;
    struct drcg_tree2attr	*tree_ptr, *sub_tree_ptr, *next_tree_ptr;
    bool		done;
    SXINT			pos, sub_tree1, node, x, y;
    SXINT			old_glob_tree_top, old_node_list_top, old_var_hd_top;
    SXBA		old_tree_bag_top;
#if EBUG
    SXINT			old_var_list_top;
#endif

#if DRCG_is_cyclic==1
    SXINT			*son_ptr;
    struct cyclic_tree	*cyclic_tree_ptr;
#endif

    SXINT 		i, ste, dc, val, ref;
    SXINT			top, bot, couple, var, source_index;
    char		*str;
    struct sxtoken	*token;
    struct drcg_struct	*pdrcg, *pdrcg2;
    SXINT			lhs_pid;
#if EBUG
    bool		current_ppp_failed = false;
#endif

    old_glob_tree_top = glob_tree_top;
    old_tree_bag_top = tree_bag.pool_top;
    old_node_list_top = node_list_top;
    old_var_hd_top = var_hd_top;
#if EBUG
    old_var_list_top = var_list_top;
#endif

    /* On fabrique l'ensemble de ses sous arbres . */
    root_set = bag_get (&tree_bag, glob_tree_top+1);

    for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
	sub_tree = sons_stack [failed_pos];

	if (sub_tree > 0)
	    OR (root_set, drcg_tree2attr [sub_tree].tree_set);
    }

    SXBA_1_bit (root_set, glob_tree_top);

    Aij = Frhs_stack [0];

    if (Aij > Aij2tree_size) {
	SXINT	old_size = Aij2tree_size, *p, *q;

	while (Aij > (Aij2tree_size *= 2));

	Aij2tree = (SXINT*) sxrealloc (Aij2tree, Aij2tree_size+1, sizeof (SXINT));
	p = Aij2tree+Aij2tree_size;
	q = Aij2tree+old_size;

	while (p > q)
	    *p-- = 0;
    }

    lhs_pid = Fsons [0];
    pdrcg = drcg+lhs_pid;
    var_nb = pdrcg->var_nb [prod];

#if DRCG_is_cyclic==1
    /* Attention si A -> \alpha et A -> A on va fabriquer les arbres :
       1: A -> \alpha		puis
       2: A -> A -> \alpha
       Puis, on compose A -> A avec tous les arbres existants
       de sommet A, donc avec 1 et 2.
       La composition avec 2 est la detection de la circularite
       La composition avec 1 redonnerait 2, il faut pouvoir detecter ce cas. */

    if (is_cyclic_prod) {
	/* Recherche du sous-arbre germe. */
	/* A FAIRE: recuperation de root_set! */
#if EBUG
	if (var_nb == 0 || nbnt == 0)
	    sxtrap (ME, "DRCGsem_eval");
#endif
	/* On parcourt les sous-arbres qui ont Aij comme racine */
	sub_tree = Aij2tree [Aij];

	while (sub_tree != 0) {
	    sub_tree_ptr = &(drcg_tree2attr [sub_tree]);

	    if (sub_tree_ptr->prod == prod && SXBA_bit_is_set (root_set, sub_tree)) {
		son_ptr = &(node_list [sub_tree_ptr->node_list]);

		for (nb = 1; nb <= nbnt; nb++) {
		    if (sons_stack [nb] > 0 && drcg_tree2attr [sons_stack [nb]].Aij != drcg_tree2attr [*son_ptr].Aij)
			break;

		    son_ptr++;
		}

		if (nb > nbnt) {
		    /* sub_tree est le sous-arbre germe */
		    /* Il a bien sur les memes noms de variables que glob_tree_top. */
			
#if EBUG
		    /* Le germe n'est pas cyclique et il n'a encore jamais ete utilise */
		    if (sub_tree_ptr->cyclic_tree_id != 0)
			sxtrap (ME, "DRCGsem_eval");
#endif

		    /* C'est un germe qui reference les variables de glob_tree_top */
		    sub_tree_ptr->cyclic_tree_id = ++cyclic_glob_tree_top;

		    if (cyclic_glob_tree_top >= cyclic_tree_size)
			cyclic_tree_oflw ();

		    cyclic_tree_ptr = &(cyclic_tree [cyclic_glob_tree_top]);
		    cyclic_tree_ptr->loop_nb = 0;
		    /* sub_tree est le sous-arbre germe, il est unique */
		    break;
		}
	    }

	    sub_tree = sub_tree_ptr->next;
	}

	return failed_pos; /* nbnt+1 */
    }
#endif

    /* On prepare la zone des variables */
    if (var_nb > 0) {
	if ((var_hd_top += var_nb) >= var_hd_size)
	    var_hd_oflw ();

	var_ptr = var_hd + var_hd_top;
	clear_vars (var_nb, var_ptr);
    }

    tree_ptr = &(drcg_tree2attr [glob_tree_top]);
    /* Pour create substitution qui peut l'utiliser! */
    tree_ptr->prod = prod;
    tree_ptr->pid = lhs_pid;

    if (is_dynam_atom==1) {
	for (top = pdrcg->prod2da_disp [prod+1], bot = pdrcg->prod2da_disp [prod]; bot < top; bot++) {
	    /* On examine les &StrEq ("a", X) */
	    couple = pdrcg->prod2da [bot];
	    var = couple & ((1<<pdrcg->cst.logvar)-1);

	    if (var) {
		/* Si on a un &StrEq("a", X) et si X n'est pas une variable de la partie "Definite"
		   on ne fait rien */
		failed_pos = couple >> pdrcg->cst.logvar;
		source_index = -sons_stack [failed_pos];

#if EBUG
		if (source_index <= 0)
		    sxtrap (ME, "DRCGsem_eval");
#endif

		token = &(SXGET_TOKEN (source_index));
		ste = token->string_table_entry;

		if (ste == SXEMPTY_STE) {
		    str = pdrcg->t2str [token->lahead];
		    ste = sxstrsave (str);
		}
		
		ref = KV2REF (DYNAM_ATOM, ste);
	
		create_substitution (glob_tree_top, KV2REF (VARIABLE, var), glob_tree_top, ref);
	    }
	}
    }


    son_nb = 0;

    for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
	sub_tree = sons_stack [failed_pos];

	if (sub_tree > 0) {
	    son_nb++;
	    sub_tree_ptr = &(drcg_tree2attr [sub_tree]);
	    sub_prod = sub_tree_ptr->prod;

#if EBUG
	    if (sub_tree_ptr->pid != Fsons [failed_pos])
		sxtrap (ME, "DRCGsem_eval");
#endif
	    param_nb = drcg [sub_tree_ptr->pid].param_nb [sub_prod][0];

	    if (param_nb == pdrcg->param_nb [prod] [failed_pos]) {
		/* meme nb de param */
		if (param_nb > 0) {
		    t1_ptr = &(pdrcg->term [prod] [failed_pos] [0]);
		    t2_ptr = &(drcg [sub_tree_ptr->pid].term [sub_prod] [0] [0]);

		    for (j = 0; j < param_nb; j++) {
			/* jeme param du failed_pos eme nt de la regle prod */
			t1 = *t1_ptr++;
			t2 = *t2_ptr++;

			if (!unify (glob_tree_top, t1, sub_tree, t2))
			    break;
		    }

		    if (j < param_nb)
			break;
		}
	    }
	    else
		break;
	}
    }

    if (has_prolog==1) {
	SXINT 	ppp_item, ppp_item_nb;
	SXINT		*ppp_prolon, *pbot, *ptop;
	bool 	done;
	SXINT		x, sub_prod, cur_root, cur_ppp_item;
	SXINT		*param_ptr;

	/* Attention, il faut quand meme evaluer les ppp qui n'ont pas de variables, ou des variables
	   qui n'interviennent pas dans les predicats de la meme regle. */
	if (failed_pos > nbnt && prod <= pdrcg->cst.maxpppprod) {
	    ppp_prolon = pdrcg->ppp_prolon + prod;

	    if ((ppp_item_nb = ppp_prolon [1] - (glob_ppp_item = *ppp_prolon)) > 0) {
		/* Cette prod a des ppp */
		pbot = pdrcg->ppp_lispro + glob_ppp_item;
		ptop = pbot + ppp_item_nb;

		do {
		    /* Position du ppp dans prod */
		    /* seuls seront stockes ds pppredo les ppp_item < glob_ppp_item */
		    param_ptr = &(pdrcg->ppp_term [pdrcg->ppp_term_hd [glob_ppp_item]]);
		    /* predef_prolog_predicate */
		    ppp = *pbot;

		    if (!(*execute_prolog [ppp].call) (glob_tree_top, param_ptr)) {
			/* On simule un echec sur le dernier (vrai) nt de la production */
			failed_pos = nbnt;
			break;
		    }

		    glob_ppp_item++;
		} while (++pbot < ptop);
	    }
	}

	if (XxY_top (pppredo) > 0) {
	    if (failed_pos > nbnt) {
		/* l'unification du reste a marche et il y a des contraintes a verifier */
		/* Tous les ppp de la production courante seront stockes ds pppredo */
		do {
		    done = true;

		    for (x = 1; failed_pos > nbnt && x <= XxY_top (pppredo); x++) {
			if (!XxY_is_erased (pppredo, x)) {
			    /* Attention, le corps de la boucle peut rajouter des elements... */
			    done = false;

			    do {
				cur_root = XxY_X (pppredo, x);
				cur_ppp_item = XxY_Y (pppredo, x);

				if (cur_root == glob_tree_top) {
				    sub_prod = prod;
				    param_ptr = &(pdrcg->ppp_term [pdrcg->ppp_term_hd [cur_ppp_item]]);
				    ppp = pdrcg->ppp_lispro [cur_ppp_item];
				}
				else {
				    sub_prod = drcg_tree2attr [cur_root].prod;
				    pdrcg2 = drcg+drcg_tree2attr [cur_root].pid;
				    param_ptr = &(pdrcg2->ppp_term [pdrcg2->ppp_term_hd [cur_ppp_item]]);
				    ppp = pdrcg2->ppp_lispro [cur_ppp_item];
				}

				if (!(*execute_prolog [ppp].call) (cur_root, param_ptr)) {
				    /* Il faut savoir dans quel sous-arbre se trouve le sub_tree qui a fait echouer
				       les contraintes pour positionner failed_pos. */
				    if (cur_root == glob_tree_top) {
					failed_pos = nbnt;
#if EBUG
					current_ppp_failed = true;
#endif
				    }
				    else {
					for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
					    sub_tree = sons_stack [failed_pos];

					    if (sub_tree > 0 &&
						cur_root <= sub_tree &&
						SXBA_bit_is_set (drcg_tree2attr [sub_tree].tree_set, cur_root))
						break;
					}
#if EBUG
					if (failed_pos > nbnt)
					    sxtrap (ME, "DRCGsem_eval");
#endif
				    }

				    break;
				}

				/* a mettre apres l'execution du ppp */
				XxY_erase (pppredo, x);
			    } while (++x <= XxY_top (pppredo) && !XxY_is_erased (pppredo, x));
			}
		    }
		} while (failed_pos > nbnt && !done);
	    }

	    XxY_clear (&pppredo);
	}

	glob_ppp_item = 0;
    }

   
    if (failed_pos > nbnt) {
	tree_ptr->Aij = Aij;
	tree_ptr->next = Aij2tree [Aij];
	tree_ptr->tree_set = root_set;
#if DRCG_is_cyclic==1
	tree_ptr->cyclic_tree_id = 0;
#endif


	Aij2tree [Aij] = glob_tree_top;
	    
	if (son_nb > 0) {
	    if ((node_list_top += son_nb) >= node_list_size)
		node_list_oflw ();

	    node_ptr = &(node_list [node_list_top]);
	    nb = nbnt;
		
	    do {
		sub_tree = sons_stack [nb];

		if (sub_tree > 0)
		    *--node_ptr = sub_tree;
	    } while (--nb > 0);
	}

	if (++glob_tree_top >= tree_size)
	    tree_oflw ();

	next_tree_ptr = &(drcg_tree2attr [glob_tree_top]);
	next_tree_ptr->node_list = node_list_top;
	next_tree_ptr->var_hd = var_hd_top;

	is_unified = true;	/* DRCGsem_fun a marche (au moins) une fois */

#if EBUG
	if (debug_level & (8)) {
	    /* On doit imprimer qqchose */
	    fputs ("[SUCCESS]", stdout);

	    if (debug_level & 4)
		print_tree (glob_tree_top-1);
	    else
		print_rule (glob_tree_top-1);
	}
#endif

	/* A FAIRE :
	   Regarder si le nouveau sous-arbre n'est pas syntaxiquement et semantiquement equivalent
	   avec un autre sous-arbre deja construit. Auquel cas, il est inutile. On peut meme faire
	   de la subsomption!. */

	undo_stack_top = 0;
    }
    else {
	/* Echec de l'unification */
#if EBUG
	if (debug_level & (16)) {
	    /* On doit imprimer qqchose */
	    /* Les procedures d'impression ont besoin que les structures soient coherentes */
	    /* Le noeud imprime le + a droite est celui qui a produit l'echec de l'unification */
	    tree_ptr = &(drcg_tree2attr [glob_tree_top]);
	    tree_ptr->prod = prod;
	    tree_ptr->Aij = Aij;
	    
	    if (nbnt > 0) {
		if ((node_list_top + nbnt) >= node_list_size)
		    node_list_oflw ();

		node_ptr = &(node_list [node_list_top + nbnt]);
		nb = nbnt;
		
		do {
		    sub_tree = sons_stack [nb];

		    if (sub_tree < 0)
			/* &StrEq ("a", X) */
			sub_tree = 0;
		    else {
			if (nb == failed_pos && !current_ppp_failed)
			    /* fils sur lequel l'unification a echouee */
			    sub_tree = -sub_tree;
		    }

		    *--node_ptr = sub_tree;
		} while (--nb > 0);
	    }

	    if (glob_tree_top+1 >= tree_size)
		tree_oflw ();

	    next_tree_ptr = &(drcg_tree2attr [glob_tree_top+1]);
	    next_tree_ptr->node_list = node_list_top+nbnt;
	    next_tree_ptr->var_hd = var_hd_top;

	    fputs ("[FAILED]", stdout);

	    if (debug_level & 4)
		print_tree (glob_tree_top);
	    else
		print_rule (glob_tree_top);
	}	
#endif

	


	/* On remet en etat */
	/* On recupere aussi les "tree_bag" */
	/* Ca marche aussi s'il y a eu des clones de fabriques */
	glob_tree_top = old_glob_tree_top;
	bag_reuse (&tree_bag, old_tree_bag_top);
	node_list_top = old_node_list_top;
	/* Il y peut-etre eu creation de variables dynamiques => on ne peut utiliser var_nb */
	var_hd_top = old_var_hd_top;

	while (undo_stack_top > 0) {
	    indx = undo_stack [undo_stack_top--];

#if EBUG
	    if (var_hd [indx] != var_list_top)
		sxtrap (ME, "DRCGsem_eval");
#endif

	    var_hd [indx] = var_list [var_list_top].next;
	    var_list_top--;
	}

#if EBUG
	if (var_list_top != old_var_list_top)
	    sxtrap (ME, "DRCGsem_eval");
#endif
    }

    return failed_pos;
}






static SXINT
unfold (prod, i, nbnt)
    SXINT prod, i, nbnt;
{
    SXINT Bkl, sub_tree, failed_pos;

    if (i > nbnt) {
	failed_pos = DRCGsem_eval (prod, nbnt);
    }
    else {
	Bkl = Frhs_stack [i];

	if (Bkl > 0) {
	    for (sub_tree = Aij2tree [Bkl]; sub_tree > 0; sub_tree = drcg_tree2attr [sub_tree].next) {
		sons_stack [i] = sub_tree;

		failed_pos = unfold (prod, i+1, nbnt);
		/* if failed_pos > nbnt => l'unification a marche */
		/* if failed_pos <= nbnt => l'unification a echoue sur le traitement du failed_pos eme sous arbre */

		if (failed_pos < i)
		    break;
	    }
	}
	else {
	    /* On est sur un &StrEq ("a", X) */
	    sons_stack [i] = Bkl;
	    failed_pos = unfold (prod, i+1, nbnt);
	}
    }

    return failed_pos;
}



void
DRCGsem_fun_init (size)
    SXINT size;
{
    SXINT 		atom, pid, *ptr;
    struct drcg_struct	*pdrcg;

    if (Aij2tree == NULL) {
	/* Est appele sur chaque module, une seule allocation */
	for (pid = 0; pid < PID; pid++) {
	    pdrcg = drcg+pid;

	    if (max_var < pdrcg->cst.max_var) max_var = pdrcg->cst.max_var;
	    if (pdrcg->cst.has_prolog) has_prolog = 1;
	    if (pdrcg->cst.is_dynam_atom) is_dynam_atom = 1;
	    max_atom += pdrcg->cst.max_atom;
	    if (rhs_maxnt < pdrcg->cst.rhs_maxnt) rhs_maxnt = pdrcg->cst.rhs_maxnt;
	    max_term += pdrcg->cst.max_term;
	    max_list += pdrcg->cst.max_list;
	    if (pdrcg->cst.has_integer_op) has_integer_op = 1;
	}

	Aij2tree_size = size;
	Aij2tree = (SXINT*) sxcalloc (Aij2tree_size+1, sizeof (SXINT));
	sons_stack = (SXINT*) sxalloc (rhs_maxnt+1, sizeof (SXINT));

	tree_size = size;
	drcg_tree2attr = (struct drcg_tree2attr*) sxalloc (tree_size+1, sizeof (struct drcg_tree2attr));
    
	bag_alloc (&tree_bag, "tree_bag", (size + 1) * SXNBLONGS (size + 1));

	node_list_size = size;
	node_list = (SXINT*) sxalloc (node_list_size+1, sizeof (SXINT));
	var_hd_size = max_var*size;
	var_hd = (SXINT*) sxalloc (var_hd_size+1, sizeof (SXINT));
	var_list_size = max_var*size;
	var_list = (struct var_list*) sxalloc (var_list_size+1, sizeof (struct var_list));

	glob_tree_top = 1;

	drcg_tree2attr [1].node_list
	    /* = node_list_top */
	    = 0;
	drcg_tree2attr [0].var_hd = 0;
	/* Les variables sont numerotees a partir de 1 */
	drcg_tree2attr [1].var_hd = var_hd_top = 0;

	undo_stack = (SXINT*) sxalloc ((undo_stack_size = 20) + 1, sizeof (SXINT));

#if DRCG_is_cyclic==1
	cyclic_tree_size = n+1; /* tj > 0 pourquoi pas! */
	cyclic_tree = (struct cyclic_tree*) sxalloc (cyclic_tree_size+1, sizeof (struct cyclic_tree));
#endif

	if (has_prolog==1) {
	    /* size = n+1, tj > 0 pourquoi pas! */
	    list_stack1 = (struct list_struct*) sxalloc ((list_stack1_size = n+1) + 1,
							 sizeof (struct list_struct));
	    list_stack2 = (struct list_struct*) sxalloc ((list_stack2_size = n+1) + 1,
							 sizeof (struct list_struct));
	    list_stack3 = (struct list_struct*) sxalloc ((list_stack3_size = n+1) + 1,
							 sizeof (struct list_struct));

	    XxY_alloc (&dynam_list, "dynam_list", 10 /* !! */, 1, 0, 0, NULL,
#ifdef statistics
		       sxtty
#else
		       NULL
#endif
		       );

	    XxY_alloc (&pppredo, "pppredo", n+2 /* !! */, 1, 0, 0, NULL,
#ifdef statistics
		       sxtty
#else
		       NULL
#endif
		       );
	}

	if (is_dynam_atom==1 && max_atom > 0) {
	    ptr = dynam_atom_list = (SXINT*) sxalloc (max_atom+1, sizeof (SXINT));
	    *ptr = 0;

	    for (pid = 0; pid < PID; pid++) {
		pdrcg = drcg+pid;
		atom2dynam_atom [pid] = ptr;

		for (atom = 1; atom <= pdrcg->cst.max_atom; atom++)
		    *++ptr = sxstrsave (pdrcg->atom_str [atom]);
	    }
	}
    }
}


bool
DRCGsem_fun (rho, son_nb, sons, is_first)
    SXINT 	*rho, son_nb, sons[];
    /* rho est un tableau de taille son_nb+2
       rho[0] contient le numero p de la cause courante.  Si p est <0 la clause est -p et
       l'appel est cyclique
       rho[1] contient l'identifiant du vecteur de range de la LHS
       rho[2], ..., rho[son_nb+1] contient les identifiants des vecteurs de range de la RHS */
    /* sem_disp est un tableau global de taille PID dont chaque element est un pointeur
       vers un tableau local a chaque module ds lequel est stocke la semantique (element
       de type SEM_TYPE, c'est une union) */

    /* sons est un tableau de taille sons_nb+1, il matche clause.
       Si clause = A0 --> A1 ... Ap, sons[i] est l'identifiant du module ds lequel est defini Ai */

    /* Si is_first, c'est la premiere fois qu'on fait une reduc vers (A, rho[0]), clause est une A_clause */
    bool	is_first;
{
    SXINT		prod = rho [0];

    if (prod < 0) {
	fputs ("\nLogical terms are not (yet) evaluated upon cyclic backbones.\n", sxstderr);
	sxexit (SXSEVERITIES);
    }

    Frhs_stack = rho+1;
    Fsons = sons;

    is_unified = false;

    unfold (prod, 1, son_nb);

    return is_unified;
}



bool
DRCGsem_fun_post (S0n, start_symbol_pid, ret_val)
    SXINT 	S0n, start_symbol_pid;
    bool	ret_val;
{
    SXINT 	root, Aij, tree_nb;

    tree_nb = 0;

    if (ret_val) {
	for (root = Aij2tree [S0n]; root > 0; root = drcg_tree2attr [root].next) {
	    tree_nb++;

#if EBUG
	    root_set = drcg_tree2attr [root].tree_set;

	    if (debug_level & (4))
		print_tree (root);
	    else
		print_rule (root);

	    putchar ('\n');
#endif
	}
    }

    if (tree_nb == 0)
	/* L'unification a echouee */
	fputs ("\n**** Parsing or unification failed. ****\n\n", stdout);
    else if (tree_nb == 1)
	fputs ("\nThere is a single tree.\n\n", stdout);
    else
	printf ("\nThere are %i trees.\n\n", tree_nb);

    return ret_val;
}

void
DRCGsem_fun_final (size)
    SXINT size;
{
    if (Aij2tree != NULL) {
	sxfree (Aij2tree), Aij2tree = NULL;
	sxfree (sons_stack), sons_stack = NULL;
	sxfree (drcg_tree2attr), drcg_tree2attr = NULL;
	bag_free (&tree_bag);
	sxfree (node_list), node_list = NULL;
	sxfree (var_hd), var_hd = NULL;
	sxfree (var_list), var_list = NULL;
	sxfree (undo_stack), undo_stack = NULL;
#if DRCG_is_cyclic==1
	sxfree (cyclic_tree), cyclic_tree = NULL;
#endif

	if (has_prolog == 1) {
	    sxfree (list_stack1), list_stack1 = NULL;
	    sxfree (list_stack2), list_stack2 = NULL;
	    sxfree (list_stack3), list_stack3 = NULL;
	    XxY_free (&dynam_list);
	    XxY_free (&pppredo);
	}

	if (is_dynam_atom == 1 && max_atom > 0) {
	    sxfree (dynam_atom_list), dynam_atom_list = NULL;
	}
    }
}


