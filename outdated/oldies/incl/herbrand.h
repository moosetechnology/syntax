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

/*	TRAITEMENT DES DOMAINES DE HERBRAND		*/

/* POUR L'INSTANT */
#define DCG_is_cyclic	0

#ifdef is_sxndparser
/* Le parser est sxndparser, il interprete donc un automate LR ou LC */
#else
/* Le parser est sxparser_earley_lc, il interprete donc un LC a la mode Earley */

#if 0
#define Xpack	1
/* Les Aij sont definis par un X_header */
#include "semact.h"
/* Hubert : le fichier semact.h n'existe nulle part */
#endif

#include <stdlib.h> /* pour random() */

typedef struct {
    struct bag_disp_hd {
	SXINT		size;
	SXBA_ELT	*set;
    }		*hd;
    SXBA_ELT	*pool_top;
    SXINT		hd_size, hd_top, hd_high, pool_size, room;
    char	*name;
    SXINT		used_size, prev_used_size, total_size; /* #if statistics */
} bag_header;

extern void bag_alloc (bag_header *pbag, char *name, SXINT size);
extern SXBA bag_get (bag_header *pbag, ssize_t size);
extern void bag_reuse ();
extern void bag_free (bag_header *pbag);
extern void bag_clear (bag_header *pbag);
extern void OR ();
extern bool IS_AND ();

static void print_rule ();

static struct tree2attr {
    SXINT		prod, Aij, next, node_list, var_hd;
    SXBA	tree_set;
#if DCG_is_cyclic==1
    SXINT		cyclic_tree_id;
#endif
} *tree2attr;
static SXINT		glob_tree_top, tree_size;
static SXBA		root_set;

#if DCG_is_cyclic==1
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
    SXINT		next, sub_tree, val;
} *var_list;
static SXINT		var_list_top, var_list_size;
static SXINT		*var_hd, var_hd_top, var_hd_size;

static SXINT		*Aij2tree;
static SXINT		sons_stack [SXDCGrhs_maxnt+1];
static SXINT		*Frhs_stack;

static SXINT		*undo_stack, undo_stack_top, undo_stack_size;

static bool		is_unified;

#if SXDCGhas_prolog==1
#include "XxY.h"

static SXINT		ppp; /* valeur du predicat */

struct list_struct {
    SXINT tree_id, list_ref, elem_ref;
};

static struct list_struct	*list_stack, *list_stack_ptr, *list_stack1, *list_stack1_ptr,
                        *list_stack2, *list_stack2_ptr, *list_stack3, *list_stack3_ptr;
static SXINT		list_stack_top, list_stack_size, list_stack1_size, list_stack2_size, list_stack3_size;

static XxY_header	dynam_list;
#if 0
static SXINT		to_be_cloned [SXDCGrhs_maxnt+1];
#endif
static XxY_header	pppredo;
static SXINT		glob_ppp_item;

#endif

static SXINT		*clone_stack, *clone2node;
static SXINT		clone_stack_top, clone_stack_size, clone_root;
static SXINT		com_sub_tree, com_tree_set_nb, tree_clone_base;
static SXINT		clone2node_top, clone2node_size;
static SXBA		com_tree_set, sub_tree1_set, sub_tree2_set;

#if SXDCGis_dynam_constant==1
#if SXDCGmax_atom!=0
static SXINT atom2dynam_atom [SXDCGmax_atom+1];
#endif
#endif


static void
var_list_oflw ()
{
    var_list_size *= 2;
    var_list = (struct var_list*) sxrealloc (var_list, var_list_size+1, sizeof (struct var_list));
}

#if SXDCGhas_prolog==1
put_in_pppredo (sub_tree, item)
    SXINT sub_tree, item;
{
    /* On met (sub_tree, item) ds pppredo s'il ne s'y trouve pas deja */
    SXINT x;

    XxY_set (&pppredo, sub_tree, item, &x);
}
#endif

static void
create_substitution (sub_tree_var, x, sub_tree_term, t)
    SXINT sub_tree_var, x, sub_tree_term, t;
{
    /* On ajoute la substitution (x/t) a la base */
    /* x est une variable et t est quelconque */
    SXINT			*var_hd_ptr, indx, var_val;
    struct var_list	*var_list_ptr;
    struct tree2attr	*sub_tree_ptr;

#if EBUG
    if (sub_tree_var == 0)
	sxtrap (ME, "create_substitution");
#endif

    if (x == DUM_VAR || t == DUM_VAR || x == t && sub_tree_var == sub_tree_term) return;

    if (++var_list_top > var_list_size)
	var_list_oflw ();

    sub_tree_ptr = &(tree2attr [sub_tree_var]);
    var_val = REF2VAL (x);
    indx = sub_tree_ptr->var_hd + var_val;

    if (++undo_stack_top >= undo_stack_size)
	undo_stack = (SXINT*) sxrealloc (undo_stack, (undo_stack_size *= 2) + 1, sizeof (SXINT));
    
    undo_stack [undo_stack_top] = indx;

    var_hd_ptr = &(var_hd [indx]);
    var_list_ptr = &(var_list [var_list_top]);
    var_list_ptr->next = *var_hd_ptr;
    *var_hd_ptr = var_list_top;
    var_list_ptr->sub_tree = sub_tree_term;
    var_list_ptr->val = t;

#if SXDCGhas_prolog==1
{
    /* On vient de substituer une variable libre.
       On regarde si cette variable n'est pas utilisee dans un ppp
       qu'il faudra donc [re]executer, car l'info sur ses params a augmente. */
    SXINT			prod, gvar_val, ppp_item, init_ppp_item, next_ppp_item;
    SXBA		ppp_item_set;
    SXINT			*ppp_prolon;

    prod = sub_tree_ptr->prod;

    if (prod <= SXDCGmaxpppprod) {
	ppp_prolon = SXDCGppp_prolon + prod;

	if ((next_ppp_item = ppp_prolon [1]) - (init_ppp_item = *ppp_prolon) > 0) {
	    /* prod a des ppp */
	    gvar_val = SXDCGgvar_name [SXDCG2gvar_disp [prod]+var_val-1];

	    if (gvar_val <= SXDCGmax_gvarppp) {
		/* gvar_val intervient dans des ppp */
		ppp_item_set = SXDCGgvar2ppp_item_set [gvar_val];
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
#endif    
}


#if 0
static void
create_substitution (sub_tree_var, x, sub_tree_term, t)
    SXINT sub_tree_var, x, sub_tree_term, t;
{
    /* On ajoute la substitution (x/t) a la base */
    /* x est une variable et t est quelconque */
    SXINT			*var_hd_ptr, indx, var_val;
    struct var_list	*var_list_ptr;
    struct tree2attr	*sub_tree_ptr;
#if SXDCGhas_prolog==1
    SXINT			prod, gvar_val, item, next_item, pos;
    SXBA		item_set;
#endif

#if EBUG
    if (sub_tree_var == 0)
	sxtrap (ME, "create_substitution");
#endif

    if (x == DUM_VAR || t == DUM_VAR || x == t && sub_tree_var == sub_tree_term) return;

    if (++var_list_top > var_list_size)
	var_list_oflw ();

    sub_tree_ptr = &(tree2attr [sub_tree_var]);
    var_val = REF2VAL (x);
    indx = sub_tree_ptr->var_hd + var_val;

    if (++undo_stack_top >= undo_stack_size)
	undo_stack = (SXINT*) sxrealloc (undo_stack, (undo_stack_size *= 2) + 1, sizeof (SXINT));
    
    undo_stack [undo_stack_top] = indx;

    var_hd_ptr = &(var_hd [indx]);
    var_list_ptr = &(var_list [var_list_top]);
    var_list_ptr->next = *var_hd_ptr;
    *var_hd_ptr = var_list_top;
    var_list_ptr->sub_tree = sub_tree_term;
    var_list_ptr->val = t;

#if SXDCGhas_prolog==1
    /* On vient de substituer une variable libre.
       On regarde si cette variable n'est pas utilisee dans un ppp
       qu'il faudra donc [re]executer, car l'info sur ses params a augmente. */
    prod = sub_tree_ptr->prod;
    gvar_val = SXDCGgvar_name [SXDCG2gvar_disp [prod]+var_val-1];

    if (gvar_val <= SXDCGmax_gvarppp) {
	item_set = SXDCGgvar2pppitem_set [gvar_val];
	item = inputG.prolon [prod]-1;
	next_item = inputG.prolon [prod+1];

	while ((item = sxba_scan (item_set, item)) > 0 && item> 0 && item < next_item) {
	    pos = inputG.item2nbnt [item]+1;

	    if (sub_tree_var != glob_tree_top || pos < glob_where)
		put_in_pppredo (sub_tree_var, item);
	}
    }
#endif    
}
#endif

static void
substitute (sub_tree, var_ref)
    SXINT *sub_tree, *var_ref;
{
    /* var_ref est une variable de sub_tree, on regarde vers quoi elle pointe */
    SXINT			x, y, prev_sub_tree, var;
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
	if (prev_sub_tree == 0)
	    sxtrap (ME, "substitute");
#endif

	var_list_ptr = &(var_list [var_hd [tree2attr [prev_sub_tree].var_hd + var]]);

	while (var_list_ptr != var_list) {
	    y = var_list_ptr->sub_tree;

#if EBUG
	    if (y == 0)
		sxtrap (ME, "substitute");
#endif

	    if (y < root_set [0] && SXBA_bit_is_set (root_set, y))
		/* C'est un bon chemin */
		break;
	    
	    /* On pourrait faire var_list_ptr -= var_list_ptr->next;
	       en mettant dans next des increments */
	    var_list_ptr = var_list + var_list_ptr->next;
	}

	if  (var_list_ptr != var_list) {
	    /* on a trouve */
	    x = var_list_ptr->val;
#if EBUG
	    if (x == 0)
		sxtrap (ME, "substitute");
#endif
	    /* y est OK */
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

    kind = REF2KIND (*ref);

    if (kind == VARIABLE) {
	/* Variable */
	/* Est-elle liee ? */
	substitute (tree_id, ref);
	kind = REF2KIND (*ref);
    }

    if (kind == VARIABLE) return false;

    val1 = REF2VAL (*ref);

    if (kind == INTEGER_CONSTANT) {
	*val = val1;
	return true;
    }

    if (kind != INTEGER_OP) return false;

    bot = SXDCGterm_disp [val1];
    op_ref = SXDCGterm_list [bot];

    /* On prend le 1er operande */
    op1_tree = *tree_id;
    op1_ref = SXDCGterm_list [bot+1];

    if (!integer_val (&op1_tree, &op1_ref, &val1)) return false;

    int_op = REF2VAL (op_ref);

    if (int_op == UMINUS) {
	*val = -val1;
	return true;
    }

    /* On prend le 2eme operande */
    op2_tree = *tree_id;
    op2_ref = SXDCGterm_list [bot+2];

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


#if SXDCGterm_size!=0
#if occur_check==1

static bool
occur_body (tree_var, X, tree_t1, t1)
    SXINT tree_var, X, tree_t1, t1;
{
    SXINT	kind;

    kind = REF2KIND (t1);

    if (kind == VARIABLE) {
	/* Variable */
	/* Est-elle liee ? */
	substitute (&tree_t1, &t1);

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
    SXINT	bot, top, t1, val, tree_t1;

    kind = REF2KIND (t);
    
    if (kind == ATOM || kind == INTEGER_CONSTANT || kind == DYNAM_ATOM)
	/* clos */
	return false;

    t = REF2VAL (t);

    if (kind == TERM || kind == INTEGER_OP) {
	/* t est un terme ou une operation arithmetique */
	bot = SXDCGterm_disp [t];
	top = SXDCGterm_disp [t+1];

	/* On saute le foncteur */
	while (++bot < top) {
	    if (occur_body (tree_var, X, tree_term, SXDCGterm_list [bot]))
		return true;
	}

    }
    else {
	/* t est une liste non close */
	if (occur_body (tree_var, X, tree_term, SXDCGlist_list [t]))
	    return true;

	if (occur_body (tree_var, X, tree_term, SXDCGlist_list [t+1]))
	    return true;
    }

    return false;
}
#endif
#endif


#if SXDCGhas_integer_op==1
static bool
unify_int_expr (val, tree, expr)
    SXINT val, tree, expr;
{
    SXINT kind, bot, op1_tree, op1_ref, op1_kind, op2_tree, op2_ref, op2_kind, int_op, op_ref, val1, val2;

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

    bot = SXDCGterm_disp [REF2VAL (expr)];

    op1_tree = tree;
    op1_ref = SXDCGterm_list [bot+1];
    op1_kind = integer_val (&op1_tree, &op1_ref, &val1);

    op_ref = SXDCGterm_list [bot];
    int_op = REF2VAL (op_ref);

    if (int_op == UMINUS) {
#if EBUG
	if (op1_kind)
	    sxtrap (ME, "unify_int_expr");
#endif

	return unify_int_expr (-val, op1_tree, op1_ref);
    }

    op2_tree = tree;
    op2_ref = SXDCGterm_list [bot+2];
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
	sxtrap (ME, "integer_val");
#endif
    }
}
#endif



static bool
unify (sub_tree1, t1, sub_tree2, t2)
    SXINT sub_tree1, t1, sub_tree2, t2;
{
    SXINT		x, t, bot1, bot2, top1, sub_tree_var, sub_tree_term, what;
    SXINT 	t1_kind, t2_kind, t1_val, t2_val, t1_ref, t2_ref;
    bool	is_t1_var, is_t2_var, is_t_var;

    if (REF2KIND (t1) == VARIABLE) {
	/* variable */
	substitute (&sub_tree1, &t1);
	/* Ici, terme, liste, ... ou une variable libre */
	is_t1_var = (REF2KIND (t1) == VARIABLE);
    }
    else
	is_t1_var = false;

    if (REF2KIND (t2) == VARIABLE) {
	/* variable */
	substitute (&sub_tree2, &t2);
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
#if SXDCGmax_term!=0
#if occur_check==1
	    is_t_var = is_t2_var;
#endif
#endif

	}
	else {
	    x = t2, sub_tree_var = sub_tree2, t = t1, sub_tree_term = sub_tree1;
#if SXDCGmax_term!=0
#if occur_check==1
	    is_t_var = is_t1_var;
#endif
#endif
	}

#if SXDCGmax_term!=0
#if occur_check==1
	if (!is_t_var && occur (sub_tree_var, x, sub_tree_term, t)) {
	    /* La variable (sub_tree_var,x) occure ds t qui n'est pas une variable */
	    return false;
	}
#endif
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


    switch (t1_kind) {
#if SXDCGis_dynam_constant==1
    case ATOM:
    case DYNAM_ATOM:
#if SXDCGmax_atom!=0
	if (t1_kind == ATOM)
	    t1_val = atom2dynam_atom [t1_val];

	if (t2_kind == ATOM)
	    t2_val = atom2dynam_atom [t2_val];
#endif
	return t1_val == t2_val;
#endif

#if SXDCGmax_list!=0
#if SXDCGhas_prolog==1
    case STATIC_LIST:
    case DYNAM_LIST:
	if (t1_val == 0 || t2_val == 0)
	    /* unifiable ssi les 2 listes sont vides */
	    return t1_val == 0 && t2_val == 0;

	t1_ref = (t1_kind == DYNAM_LIST) ? XxY_X (dynam_list, t1_val) : SXDCGlist_list [t1_val];
	t2_ref = (t2_kind == DYNAM_LIST) ? XxY_X (dynam_list, t2_val) : SXDCGlist_list [t2_val];
	
	if (!unify (sub_tree1, t1_ref, sub_tree2, t2_ref))
	    return false;

	t1_ref = (t1_kind == DYNAM_LIST) ? XxY_Y (dynam_list, t1_val) : SXDCGlist_list [t1_val+1];
	t2_ref = (t2_kind == DYNAM_LIST) ? XxY_Y (dynam_list, t2_val) : SXDCGlist_list [t2_val+1];
	
	return unify (sub_tree1, t1_ref, sub_tree2, t2_ref);
#else
    case STATIC_LIST:
	if (t1_val == 0 || t2_val == 0)
	    /* l'un est la liste vide et pas l'autre */
	    return false;

	t1_ref = SXDCGlist_list [t1_val];
	t2_ref = SXDCGlist_list [t2_val];
	
	if (!unify (sub_tree1, t1_ref, sub_tree2, t2_ref))
	    return false;

	t1_ref = SXDCGlist_list [t1_val+1];
	t2_ref = SXDCGlist_list [t2_val+1];
	
	return unify (sub_tree1, t1_ref, sub_tree2, t2_ref);
#endif
#endif

#if SXDCGmax_term!=0
    case TERM:
	bot1 = SXDCGterm_disp [t1_val];
	bot2 = SXDCGterm_disp [t2_val];

	if (t1_val == t2_val) {
	    /* meme terme => meme foncteur et meme arite */
	    top1 = SXDCGterm_disp [t1_val+1];
	}
	else {
	    if (SXDCGterm_list [bot1] != SXDCGterm_list [bot2])
		/* foncteurs differents */
		return false;

	    top1 = SXDCGterm_disp [t1_val+1];

	    if (top1-bot1 != SXDCGterm_disp [t2_val+1]-bot2)
		/* arites differentes */
		return false;
	}

	while (++bot1 < top1) {
	    bot2++;

	    if (!unify (sub_tree1, SXDCGterm_list [bot1], sub_tree2, SXDCGterm_list [bot2]))
		return false;
	}

	return true;
#endif

#if SXDCGhas_integer_op==1
    case INTEGER_CONSTANT:
    case INTEGER_OP:
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
#endif

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
    tree2attr = (struct tree2attr*) sxrealloc (tree2attr, tree_size+1, sizeof (struct tree2attr));
}


static void
herbrand_semact_init (size)
    SXINT size;
{
    /* size == pAij_top */
    SXINT atom;

    Aij2tree = (SXINT*) sxcalloc (size+1, sizeof (SXINT));

    tree_size = size;
    tree2attr = (struct tree2attr*) sxalloc (tree_size+1, sizeof (struct tree2attr));
    
    bag_alloc (&tree_bag, "tree_bag", (size + 1) * SXNBLONGS (size + 1));

    node_list_size = size;
    node_list = (SXINT*) sxalloc (node_list_size+1, sizeof (SXINT));
    var_hd_size = SXDCGmax_var*size;
    var_hd = (SXINT*) sxalloc (var_hd_size+1, sizeof (SXINT));
    var_list_size = SXDCGmax_var*size;
    var_list = (struct var_list*) sxalloc (var_list_size+1, sizeof (struct var_list));

    glob_tree_top = 1;

    tree2attr [1].node_list
	/* = node_list_top */
	= 0;
    tree2attr [0].var_hd = 0;
    /* Les variables sont numerotees a partir de 1 */
    tree2attr [1].var_hd = var_hd_top = 0;

    undo_stack = (SXINT*) sxalloc ((undo_stack_size = 20) + 1, sizeof (SXINT));

#if DCG_is_cyclic==1
    cyclic_tree_size = sxplocals.atok_no; /* tj > 0 pourquoi pas! */
    cyclic_tree = (struct cyclic_tree*) sxalloc (cyclic_tree_size+1, sizeof (struct cyclic_tree));
#endif

#if SXDCGhas_prolog==1
    /* size = sxplocals.atok_no, tj > 0 pourquoi pas! */
    list_stack1 = (struct list_struct*) sxalloc ((list_stack1_size = sxplocals.atok_no) + 1,
						 sizeof (struct list_struct));
    list_stack2 = (struct list_struct*) sxalloc ((list_stack2_size = sxplocals.atok_no) + 1,
						 sizeof (struct list_struct));
    list_stack3 = (struct list_struct*) sxalloc ((list_stack3_size = sxplocals.atok_no) + 1,
						 sizeof (struct list_struct));

    XxY_alloc (&dynam_list, "dynam_list", 10 /* !! */, 1, 0, 0, NULL,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );

    XxY_alloc (&pppredo, "pppredo", sxplocals.atok_no+1 /* !! */, 1, 0, 0, NULL,
#ifdef statistics
	       sxtty
#else
	       NULL
#endif
	       );
#endif

    clone_stack = (SXINT*) sxalloc ((clone_stack_size = SXDCGrhs_maxnt+10) + 1, sizeof (SXINT));
    clone2node = (SXINT*) sxalloc ((clone2node_size = SXDCGrhs_maxnt+10) + 1, sizeof (SXINT));

#if SXDCGis_dynam_constant==1
#if SXDCGmax_atom!=0
    for (atom = 1; atom <= SXDCGmax_atom; atom++)
	atom2dynam_atom [atom] = sxstrsave (SXDCGatom_str [atom]);
#endif
#endif
}


static void
herbrand_semact_final (size)
    SXINT size;
{
    sxfree (Aij2tree);
    sxfree (tree2attr);
    bag_free (&tree_bag);
    sxfree (node_list);
    sxfree (var_hd);
    sxfree (var_list);
    sxfree (undo_stack);
#if DCG_is_cyclic==1
    sxfree (cyclic_tree);
#endif

#if SXDCGhas_prolog==1
    sxfree (list_stack1);
    sxfree (list_stack2);
    sxfree (list_stack3);
    XxY_free (&dynam_list);
    XxY_free (&pppredo);
#endif

    sxfree (clone_stack);
    sxfree (clone2node);
}



/*
static SXINT
DCGsem_eval (prod, nbnt)
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

#if SXDCGhas_prolog==1
static bool
list2stack (tree_id, list_ref)
    SXINT tree_id, list_ref;
{
    SXINT			kind, list_val, elem_ref;
    bool		end_reached = false;

    do {
	if ((kind = REF2KIND (list_ref)) == VARIABLE) {
	    /* Variable */
	    substitute (&tree_id, &list_ref);
	    kind = REF2KIND (list_ref);
	}

	if (kind == ATOM || kind == INTEGER_CONSTANT
#if SXDCGis_dynam_constant==1
	    || kind == DYNAM_ATOM
#endif
#if SXDCGmax_term!=0
	    || kind == TERM
#endif
#if SXDCGhas_integer_op==1
	    || kind == INTEGER_OP
#endif
	    )
	    return false;

	if (kind != VARIABLE && list_ref != EMPTY_LIST) {
	    list_val = REF2VAL (list_ref);
#if SXDCGhas_prolog==1
	    if (kind == DYNAM_LIST)
		elem_ref = XxY_X (dynam_list, list_val);
	    else
#endif
		/* kind == STATIC_LIST */
		elem_ref = SXDCGlist_list [list_val];
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
#if SXDCGhas_prolog==1
	    if (kind == DYNAM_LIST)
		list_ref = XxY_Y (dynam_list, list_val);
	    else
#endif
		/* kind == STATIC_LIST */
		list_ref = SXDCGlist_list [list_val+1];
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
    return KV2REF (VARIABLE, var_hd_top - tree2attr [glob_tree_top].var_hd);
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

    kind = REF2KIND (ref);

    if (kind == VARIABLE) {
	/* C'est une variable */
	substitute (&tree_id, &ref);
	kind = REF2KIND (ref);
    }

    val = REF2VAL (ref);

    switch (kind) {
    case VARIABLE:
	return false;
    case ATOM:
    case DYNAM_ATOM:
    case INTEGER_CONSTANT:
	return true;

#if SXDCGmax_term!=0
        case TERM:
	bot = SXDCGterm_disp [val];
	top = SXDCGterm_disp [val+1];

	while (++bot < top) {
	    if (!is_closed (tree_id, SXDCGterm_list [bot]))
		return false;
	}

	return true;

#if SXDCGhas_integer_op==1
        case INTEGER_OP:
	bot = SXDCGterm_disp [val];
	op = REF2VAL (SXDCGterm_list [bot]);

	if (op != UMINUS) {
	    if (!is_closed (tree_id, SXDCGterm_list [++bot]))
		return false;
	}

	return is_closed (tree_id, SXDCGterm_list [++bot]);
#endif
#endif

#if SXDCGmax_list!=0
    case STATIC_LIST:
	if (!is_closed (tree_id, SXDCGlist_list [val]))
	    return false;

	return is_closed (tree_id, SXDCGlist_list [val+1]);

#if SXDCGhas_prolog==1
    case DYNAM_LIST:
	if (!is_closed (tree_id, XxY_X (dynam_list, val)))
	    return false;

	return is_closed (tree_id, XxY_Y (dynam_list, val));
#endif
#endif

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

#if 0
	la1 = tree2attr [sub_tree_id].next;

	if (la1 >= l1) return false;

	list_stack1_ptr = list_stack1 + la1;
	done = false;

	do {
	    list_stack1_ptr++;
	    done = unify_int_expr (la1, Lgth_tree, Lgth_ref) &&
		unify (tree_id, param_ptr [2], list_stack1_ptr->tree_id, list_stack1_ptr->elem_ref);
	} while ((++la1 < l1) && !done);

	if (la1 >= l1) {
	    /* fini */
	    to_be_cloned [glob_cur_pos] = 0;
	    return done;
	}

	/* On prepare le coup suivant */
	to_be_cloned [glob_cur_pos] = la1;
#endif

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
#if SXDCGhas_integer_op==1
	return unify_int_expr (l1, Lgth_tree, Lgth_ref);
#else
	if (Lgth_kind != VARIABLE) return false;

	create_substitution (Lgth_tree, Lgth_ref, Lgth_tree, KV2REF (INTEGER_CONSTANT, l1));
	return true;
#endif
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

#if 0
    la1 = tree2attr [sub_tree_id].next;

    if (la1 >= l1) return false;

    list_stack1_ptr = list_stack1 + la1;
    done = false;

    do {
	list_stack1_ptr++;
	done = unify (tree_id, param_ptr [0], list_stack1_ptr->tree_id, list_stack1_ptr->elem_ref);
    } while ((++la1 < l1) && !done);

    if (la1 >= l1) {
	/* fini */
	to_be_cloned [glob_cur_pos] = 0;
	return done;
    }

    /* On prepare le coup suivant */
    to_be_cloned [glob_cur_pos] = la1;
#endif
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
#if 0
	/* Ici le resultat peut necessiter la fabrication de nouveaux sous-arbres "concat" */
	/* next est utilise pour stocker les la3, ca marche car un seul arbre concatii */
	la3 = tree2attr [sub_tree_id].next;

	/* On cree un nouveau sous-arbre, clone de sub_tree_id, que l'on insere ds la liste des
	   concatij, juste apres sub_tree_id afin qu'il soit examine la prochaine fois */

	/* On a l3 = l1+la3+l2+lb3 */

	/* On cherche le 1er la3 qui convient */
	lb3 = l3 - (l1+la3+l2);
	done = false;

	do {
	    if (unify_list (list_stack2+1, list_stack3+l1+la3+1, l2)) {
		/* ici on a un bon decoupage */

		/* On cree une liste dynamique, copie du milieu de la liste L3 */
		/* dont la queue (liste non vide) est remplacee par la liste vide */
		DL = set_dynam_list (list_stack3+1+l1, la3, EMPTY_LIST);
		/* L1_tail = DL */
		create_substitution (list_stack1_ptr->tree_id, list_stack1_ptr->list_ref,
				     glob_tree_top, DL);
		/* L2_tail = L3_middle */
		list_stack3_ptr = list_stack3 + l1+la3+l2+1;
		create_substitution (list_stack2_ptr->tree_id, list_stack2_ptr->list_ref,
				     list_stack3_ptr->tree_id, list_stack3_ptr->list_ref);

		done = true;
	    }
	} while (la3++, (--lb3 >= 0 && !done));

	if (lb3 < 0) {
	    /* fini */
	    to_be_cloned [glob_cur_pos] = 0;
	    return done;
	}

	/* On prepare le coup suivant */
	to_be_cloned [glob_cur_pos] = la3;
#endif
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


#endif

static SXINT
get_pos (node)
    SXINT node;
{
    /* On cherche la position de node ds clone2node */
    SXINT n;

    for (n = 0; n < clone2node_top; n++) {
	if (clone2node [n] == node)
	    return n;
    }

#if EBUG
    sxtrap (ME, "get_pos");
#endif
}

static SXINT
clone_node (node)
    SXINT 	node;
{
    SXINT 		clone_node, son_nb, nb, son_node, var_nb, var, next, pos_node;
    SXINT			*son_ptr, *var_hd_ptr, *clone_var_hd_ptr;
    struct var_list	*var_list_ptr, *clone_list_ptr;
    struct tree2attr	*node_ptr, *next_node_ptr, *clone_ptr, *next_clone_ptr;
    SXBA		clone_root_set;

    /* clone_stack contient les fils */

    clone_node = tree_clone_base + get_pos (node);
    clone_root_set = bag_get (&tree_bag, clone_node+1);
    SXBA_1_bit (clone_root_set, clone_node);

    node_ptr = &(tree2attr [node]);
    next_node_ptr = node_ptr+1;

    clone_ptr = &(tree2attr [clone_node]);

    next_clone_ptr = clone_ptr+1;
    
    son_nb = next_node_ptr->node_list - node_ptr->node_list;

    if (son_nb > 0) {
	if ((node_list_top += son_nb) >= node_list_size)
	    node_list_oflw ();

	son_ptr = &(node_list [node_list_top]);
	nb = son_nb;
		
	do {
	    son_node = clone_stack [clone_stack_top--];
	    *--son_ptr = son_node;
	    OR (clone_root_set, tree2attr [son_node].tree_set);
	} while (--nb > 0);
    }

    var_nb = next_node_ptr->var_hd - node_ptr->var_hd;

    if (var_nb > 0) {
	if ((var_hd_top += var_nb) >= var_hd_size)
	    var_hd_oflw ();

	var_hd_ptr = var_hd + next_node_ptr->var_hd;
	clone_var_hd_ptr = var_hd + var_hd_top;

	do {
	    *clone_var_hd_ptr = 0;

	    var = *var_hd_ptr;
	    var_list_ptr = var_list + var;
	    clone_list_ptr = &(var_list [var_list_top]);

	    while (var_list_ptr != var_list) {
		if (++var_list_top > var_list_size)
		    var_list_oflw ();

		if (++undo_stack_top >= undo_stack_size)
		    undo_stack = (SXINT*) sxrealloc (undo_stack, (undo_stack_size *= 2) + 1, sizeof (SXINT));
    
		undo_stack [undo_stack_top] = clone_var_hd_ptr - var_hd;

		clone_list_ptr++;
		clone_list_ptr->next = *clone_var_hd_ptr;
		*clone_var_hd_ptr = var_list_top;
		clone_list_ptr->val = var_list_ptr->val;

		if (var_list_ptr->sub_tree < com_tree_set [0]
		    && SXBA_bit_is_set (com_tree_set, var_list_ptr->sub_tree)
		    /* Cette variable reference la partie commune */
		    || var_list_ptr->sub_tree < sub_tree2_set [0]
		    && SXBA_bit_is_set (sub_tree2_set, var_list_ptr->sub_tree)
		    /* Cette variable reference le deuxieme sous_arbre (hors partie commune) */
		    && com_sub_tree <= var_list_ptr->sub_tree
		    && SXBA_bit_is_set (tree2attr [var_list_ptr->sub_tree].tree_set, com_sub_tree)
		    /* et se trouve sur le chemin menant a sub_tree2 */
		    ) {
		    /* elle doit referencer le clone */
		    pos_node = get_pos (var_list_ptr->sub_tree);
		    clone_list_ptr->sub_tree = tree_clone_base + pos_node;
		}
		else
		    clone_list_ptr->sub_tree = var_list_ptr->sub_tree;

		var_list_ptr = var_list + var_list_ptr->next;
	    }

	    var_hd_ptr--, clone_var_hd_ptr--;
	} while (--var_nb > 0);
    }

    clone_ptr->prod = node_ptr->prod;
    clone_ptr->Aij = node_ptr->Aij;
    clone_ptr->tree_set = clone_root_set;
    /* Pour le moment les clones sont "isoles", */
    clone_ptr->next = 0;

    next_clone_ptr->node_list = node_list_top;
    next_clone_ptr->var_hd = var_hd_top;

    return clone_node;
}


static SXINT
clone_tree (node)
    SXINT node;
{
    SXINT			son_nb, clone_son;
    SXINT			*son_ptr;
    struct tree2attr	*node_ptr, *next_node_ptr;

    node_ptr = &(tree2attr [node]);
    next_node_ptr = node_ptr+1;

    if ((son_nb = next_node_ptr->node_list - node_ptr->node_list) > 0) {
	son_ptr = node_list + node_ptr->node_list;

	do {
	    clone_son = clone_tree (*son_ptr++);

	    if (++clone_stack_top >= clone_stack_size)
		clone_stack = (SXINT*) sxrealloc (clone_stack, (clone_stack_size *= 2) + 1, sizeof (SXINT));

	    clone_stack [clone_stack_top] = clone_son;
	} while (--son_nb > 0);
    }

    return clone_node (node);
}


static SXINT
common_sub_tree (node)
    SXINT node;
{
    SXINT			son_nb, son, common_node, com_node;
    SXINT			*son_ptr;
    struct tree2attr	*node_ptr, *next_node_ptr;

    if (node < sub_tree1_set [0] && SXBA_bit_is_set (sub_tree1_set, node))
	return node;

    node_ptr = &(tree2attr [node]);
    next_node_ptr = node_ptr+1;

    if ((son_nb = next_node_ptr->node_list - node_ptr->node_list) > 0) {
	son_ptr = node_list + node_ptr->node_list;

	do {
	    /* On parcourt node haut-bas gauche-droite */
	    son = *son_ptr++;

	    if ((common_node = common_sub_tree (son)) > 0) {
		/* On compte les noeuds du chemin menant a la partie commune */
		com_tree_set_nb++;
		return common_node;
	    }
	} while (--son_nb > 0);
    }

    return 0;
}

static SXINT
fill_clone2node (node, bot)
    SXINT node, bot;
{
    SXINT			son_nb, son, n;
    SXINT			*son_ptr;
    struct tree2attr	*node_ptr, *next_node_ptr;

    if (node == com_sub_tree)
	return bot;

    clone2node [--bot] = node;
    node_ptr = &(tree2attr [node]);
    next_node_ptr = node_ptr+1;

    son_nb = next_node_ptr->node_list - node_ptr->node_list;
    son_ptr = node_list + node_ptr->node_list;

    for (n = 1; n <= son_nb; n++) {
	son = *son_ptr++;

	if (com_sub_tree <= son && SXBA_bit_is_set (tree2attr [son].tree_set, com_sub_tree))
	    return fill_clone2node (son, bot);
    }

#if EBUG
    sxtrap (ME, "fill_clone2node");
#endif
}


static SXINT
diff_sub_tree (node)
    SXINT node;
{
    /* On recherche les fils de "node" qui menent a com_sub_tree */
    /* com_sub_tree et com_tree_set sont globaux */
    SXINT			son_nb, son, n, marked_son, clone_son;
    SXINT			*son_ptr;
    struct tree2attr	*node_ptr, *next_node_ptr;

    node_ptr = &(tree2attr [node]);
    next_node_ptr = node_ptr+1;

    son_nb = next_node_ptr->node_list - node_ptr->node_list;
    son_ptr = node_list + node_ptr->node_list;

    for (n = 1; n <= son_nb; n++) {
	son = *son_ptr++;

	if (son == com_sub_tree) {
	    clone_son = clone_root;
	    marked_son = n;
	    break;
	}

	if (com_sub_tree <= son && SXBA_bit_is_set (tree2attr [son].tree_set, com_sub_tree)) {
	    clone_son = diff_sub_tree (son);
	    marked_son = n;
	    break;
	}
    }

#if EBUG
    if (n > son_nb || clone_stack_top != 0)
	sxtrap (ME, "diff_sub_tree");
#endif

    /* On remplit clone_stack */
    son_ptr = node_list + node_ptr->node_list;
    
    for (clone_stack_top = 1; clone_stack_top <= son_nb; clone_stack_top++) {
	clone_stack [clone_stack_top] = (clone_stack_top == marked_son) ? clone_son : *son_ptr;
	son_ptr++;
    }

    clone_stack_top = son_nb;

    return clone_node (node);
}


static SXINT
process_bi_epsilon (sub_tree1, sub_tree2)
    SXINT sub_tree1, sub_tree2;
{
    /* Les sous arbres sub_tree1 et sub_tree2 ont en commun le sous_arbre com_sub_tree.
       Ce sous-arbre est clone en clone_sub_tree.
       La valeur retournee est sub_tree2, sauf si sub_tree2 == com_sub_tree, ds ce cas, on retourne
       clone_sub_tree. */
    /* com_tree_set et sub_tree2_set sont globaux (utilises ds clone_node) */
    clone_root = clone_tree (com_sub_tree);

#if EBUG
    if (clone_stack_top != 0)
	sxtrap (ME, "clone_tree");
#endif

    if (com_sub_tree == sub_tree2)
	return clone_root;

    /* Clonage de sub_tree2 */
    return diff_sub_tree (sub_tree2);
}


static SXINT
DCGsem_eval (prod, nbnt)
    SXINT prod, nbnt;
{
    /* sons_stack [1..nbnt] contient les sous-arbres */
    SXINT 		sub_tree, sub_prod, param_nb, j, t1, t2, Aij, var_nb, failed_pos, indx, nb;
    SXINT			*node_ptr, *var_ptr, *t1_ptr, *t2_ptr;
    struct tree2attr	*tree_ptr, *sub_tree_ptr, *next_tree_ptr;
    bool		done;
    SXINT			pos, sub_tree1, node, x, y;
    SXINT			old_glob_tree_top, old_node_list_top, old_var_hd_top;
    SXBA		old_tree_bag_top;
#if EBUG
    SXINT			old_var_list_top;
#endif

#if DCG_is_cyclic==1
    SXINT			*son_ptr;
    struct cyclic_tree	*cyclic_tree_ptr;
#endif

#if SXDCGis_dynam_constant==1
    SXINT 		i, ste, dc, val, ref;
    char		*str;
    struct sxtoken	*token;
#endif

    old_glob_tree_top = glob_tree_top;
    old_tree_bag_top = tree_bag.pool_top;
    old_node_list_top = node_list_top;
    old_var_hd_top = var_hd_top;
#if EBUG
    old_var_list_top = var_list_top;
#endif

    /* On fabrique l'ensemble de ses sous arbres . */
    do {
	done = true;
	root_set = bag_get (&tree_bag, glob_tree_top+1);

	for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
	    sub_tree = sons_stack [failed_pos];
	    sub_tree2_set = tree2attr [sub_tree].tree_set;

	    if (IS_AND (root_set, sub_tree2_set)) {
		/* sxtrap (ME, "bi-epsilon grammars are not yet processed."); */

		/* traitement dynamique des grammaires bi_epsilon. */
		for (pos = 1; pos < failed_pos;pos++) {
		    sub_tree1 = sons_stack [pos];
		    sub_tree1_set = tree2attr [sub_tree1].tree_set;

		    if (IS_AND (sub_tree1_set, sub_tree2_set)) {
			com_tree_set_nb = 0;
			com_sub_tree = common_sub_tree (sub_tree);
			/* com_sub_tree est le sous-arbre commun entre sub_tree1 et sub_tree */

#if EBUG
			if (com_sub_tree == 0)
			    sxtrap (ME, "DCGsem_eval");
#endif

			if ((tree2attr [com_sub_tree+1].var_hd-tree2attr [com_sub_tree].var_hd) > 0) {
			    /* Il a des variables, on clone */
			    bag_reuse (&tree_bag, root_set);
			    com_tree_set = tree2attr [com_sub_tree].tree_set;
			    /* Les identifiants des clones seront des increments par rapport a tree_clone_base */
			    tree_clone_base = glob_tree_top;
			    com_tree_set_nb += sxba_cardinal (com_tree_set);

			    if ((glob_tree_top += com_tree_set_nb) >= tree_size)
				tree_oflw ();

			    if (com_tree_set_nb >= clone2node_size)
				clone2node = (SXINT*) sxrealloc (clone2node, (clone2node_size *= 2) + 1, sizeof (SXINT));

			    /* On remplit clone2node par les 2 bouts!! */
			    clone2node_top = com_tree_set_nb;

			    y = fill_clone2node (sub_tree, clone2node_top);

			    node = 0;
			    x = 0;

			    while ((node = sxba_scan (com_tree_set, node)) > 0)
				clone2node [x++] = node;

#if EBUG
			    if (x != y)
				sxtrap (ME, "DCGsem_eval");
#endif
			    /* Ce sont les sous-arbres sub_tree1 et sub_tree qui ont en
			       commun un sous-arbre. Attention, sub_tree1 et sub_tree
			       peuvent etre egaux. */
			    sons_stack [failed_pos] = process_bi_epsilon (sub_tree1, sub_tree);
			    /* PB: quel est le statut du clone par rapport a la liste des Bkk ? */
			    /* Pour le moment, on ne reutilise pas les clones! */
			    done = false;
			}

			break;
		    }
		}

#if EBUG
		if (pos == failed_pos)
		    sxtrap (ME, "DCGsem_eval");
#endif

		if (!done)
		    break;
	    }

	    OR (root_set, tree2attr [sub_tree].tree_set);
	}
    } while (!done);

    SXBA_1_bit (root_set, glob_tree_top);

    Aij = Frhs_stack [0];
    var_nb = SXDCG2var_nb [prod];

#if DCG_is_cyclic==1
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
	    sxtrap (ME, "DCGsem_eval");
#endif
	/* On parcourt les sous-arbres qui ont Aij comme racine */
	sub_tree = Aij2tree [Aij];

	while (sub_tree != 0) {
	    sub_tree_ptr = &(tree2attr [sub_tree]);

	    if (sub_tree_ptr->prod == prod && SXBA_bit_is_set (root_set, sub_tree)) {
		son_ptr = &(node_list [sub_tree_ptr->node_list]);

		for (nb = 1; nb <= nbnt; nb++) {
		    if (sons_stack [nb] > 0 && tree2attr [sons_stack [nb]].Aij != tree2attr [*son_ptr].Aij)
			break;

		    son_ptr++;
		}

		if (nb > nbnt) {
		    /* sub_tree est le sous-arbre germe */
		    /* Il a bien sur les memes noms de variables que glob_tree_top. */
			
#if EBUG
		    /* Le germe n'est pas cyclique et il n'a encore jamais ete utilise */
		    if (sub_tree_ptr->cyclic_tree_id != 0)
			sxtrap (ME, "DCGsem_eval");
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
#if 0
	/* Il y en a forcement un, mais peut etre pas sur ce "depliage" */
	if (sub_tree == 0)
	    sxtrap (ME, "DCGsem_eval");
#endif

	return failed_pos; /* nbnt+1 */
    }
#endif

    /* On prepare la zone des variables */
    if (var_nb > 0) {
	if ((var_hd_top += var_nb) >= var_hd_size)
	    var_hd_oflw ();

	var_ptr = var_hd + var_hd_top;
	/* Macro qui s'etend en un switch */
	SXDCGclear_vars (var_nb)
    }

    tree_ptr = &(tree2attr [glob_tree_top]);
    /* Pour create substitution qui peut l'utiliser! */
    tree_ptr->prod = prod;

    for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
	sub_tree = sons_stack [failed_pos];
	sub_tree_ptr = &(tree2attr [sub_tree]);
	sub_prod = sub_tree_ptr->prod;
	param_nb = SXDCG2param_nb [sub_prod][0];

	if (param_nb == SXDCG2param_nb [prod] [failed_pos]) {
	    /* meme nb de param */
	    if (param_nb > 0) {
		t1_ptr = &(SXDCG2term [prod] [failed_pos] [0]);
		t2_ptr = &(SXDCG2term [sub_prod] [0] [0]);

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

#if SXDCGhas_prolog==1
{
    SXINT 	ppp_item, ppp_item_nb;
    SXINT		*ppp_prolon, *pbot, *ptop;
    bool 	done;
    SXINT		x, sub_prod, cur_root, cur_ppp_item;
    SXINT		*param_ptr;

    /* Attention, il faut quand meme evaluer les ppp qui n'ont pas de variables, ou des variables
       qui n'interviennent pas dans les predicats de la meme regle. */
    if (failed_pos > nbnt && prod <= SXDCGmaxpppprod) {
	ppp_prolon = SXDCGppp_prolon + prod;

	if ((ppp_item_nb = ppp_prolon [1] - (glob_ppp_item = *ppp_prolon)) > 0) {
	    /* Cette prod a des ppp */
	    pbot = SXDCGppp_lispro + glob_ppp_item;
	    ptop = pbot + ppp_item_nb;

	    do {
		/* Position du ppp dans prod */
		/* seuls seront stockes ds pppredo les ppp_item < glob_ppp_item */
		param_ptr = &(SXDCGppp_term [SXDCGppp_term_hd [glob_ppp_item]]);
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
			    sub_prod = (cur_root == glob_tree_top) ? prod : tree2attr [cur_root].prod;
			    param_ptr = &(SXDCGppp_term [SXDCGppp_term_hd [cur_ppp_item]]);
			    ppp = SXDCGppp_lispro [cur_ppp_item];

			    if (!(*execute_prolog [ppp].call) (cur_root, param_ptr)) {
				/* Il faut savoir dans quel sous-arbre se trouve le sub_tree qui a fait echouer
				   les contraintes pour positionner failed_pos. */
				if (cur_root == glob_tree_top)
				    failed_pos = nbnt;
				else {
				    for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
					sub_tree = sons_stack [failed_pos];

					if (cur_root <= sub_tree &&
					    SXBA_bit_is_set (tree2attr [sub_tree].tree_set, cur_root))
					    break;
				    }
#if EBUG
				    if (failed_pos > nbnt)
					sxtrap (ME, "DCGsem_eval");
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
#endif
   
    if (failed_pos > nbnt) {
#if SXDCGis_dynam_constant==1
	if ((dc = SXDCGprod2dc [prod]) > 0) {
	    /* On est sur une production de la forme:
	       <Dcg_constant> = %Dcg_dynam_atom ;
	       <Dcg_constant> = %Dcg_dynam_int ;
	       <Dcg_constant> = ti ;
	       */
	    i = Aij2i (Aij);

#if EBUG
	    if (i+1 != Aij2j (Aij))
		sxtrap (ME, "DCGsem_eval");
#endif

	    token = &(SXGET_TOKEN (i));
	    ste = token->string_table_entry;

	    switch (dc) {
	    case INTEGER_CONSTANT:
		str = ste == SXEMPTY_STE ? inputG.tstring [token->lahead] : sxstrget (ste);
		val = atoi (str);
		ref = KV2REF (INTEGER_CONSTANT, val);

		break;
	    case ATOM:
		if (ste == SXEMPTY_STE) {
		    str = inputG.tstring [token->lahead];
		    ste = sxstrsave (str);
		}
		
		ref = KV2REF (DYNAM_ATOM, ste);
		break;
#if EBUG
	    default:
		sxtrap (ME, "DCGsem_eval");
#endif
	    }

	    /* SXDCG2term [prod] [0] [0] == (VARIABLE, 1) */
	    create_substitution (glob_tree_top, SXDCG2term [prod] [0] [0], glob_tree_top, ref);
	}
#endif

	tree_ptr->Aij = Aij;
	tree_ptr->next = Aij2tree [Aij];
	tree_ptr->tree_set = root_set;
#if DCG_is_cyclic==1
	tree_ptr->cyclic_tree_id = 0;
#endif


	Aij2tree [Aij] = glob_tree_top;
	    
	if (nbnt > 0) {
	    if ((node_list_top += nbnt) >= node_list_size)
		node_list_oflw ();

	    node_ptr = &(node_list [node_list_top]);
	    nb = nbnt;
		
	    do {
		*--node_ptr = sons_stack [nb];
	    } while (--nb > 0);
	}

	if (++glob_tree_top >= tree_size)
	    tree_oflw ();

	next_tree_ptr = &(tree2attr [glob_tree_top]);
	next_tree_ptr->node_list = node_list_top;
	next_tree_ptr->var_hd = var_hd_top;

	is_unified = true;	/* herbrand_semact a marche (au moins) une fois */

	if (debug_level & 4) {
	    print_rule (glob_tree_top-1);
	}

	/* A FAIRE :
	   Regarder si le nouveau sous-arbre n'est pas syntaxiquement et semantiquement equivalent
	   avec un autre sous-arbre deja construit. Auquel cas, il est inutile. On peut meme faire
	   de la subsomption!. */

	undo_stack_top = 0;
    }
    else {
	if (debug_level & 4) {
	    /* Pour l'impression */
	    tree_ptr = &(tree2attr [glob_tree_top]);
	    tree_ptr->prod = prod;
	    tree_ptr->Aij = Aij;
	    
	    if (nbnt > 0) {
		if ((node_list_top + nbnt) >= node_list_size)
		    node_list_oflw ();

		node_ptr = &(node_list [node_list_top + nbnt]);
		nb = nbnt;
		
		do {
		    *--node_ptr = sons_stack [nb];
		} while (--nb > 0);
	    }

	    if (glob_tree_top+1 >= tree_size)
		tree_oflw ();

	    next_tree_ptr = &(tree2attr [glob_tree_top+1]);
	    next_tree_ptr->node_list = node_list_top;
	    next_tree_ptr->var_hd = var_hd_top;

	    putchar ('*');	/* Unification failed */
	    print_rule (glob_tree_top);
	}

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
		sxtrap (ME, "DCGsem_eval");
#endif

	    var_hd [indx] = var_list [var_list_top].next;
	    var_list_top--;
	}

#if EBUG
	if (var_list_top != old_var_list_top)
	    sxtrap (ME, "DCGsem_eval");
#endif
    }

    return failed_pos;
}




#if 0
static SXINT
DCGsem_eval (prod, nbnt)
    SXINT prod, nbnt;
{
    /* sons_stack [1..nbnt] contient les sous-arbres */
    SXINT 		sub_tree, sub_prod, param_nb, j, t1, t2, Aij, var_nb, failed_pos, indx, nb;
    SXINT			*node_ptr, *var_ptr, *t1_ptr, *t2_ptr;
    struct tree2attr	*tree_ptr, *sub_tree_ptr, *next_tree_ptr;
    bool		done;
    SXINT			pos, sub_tree1, node, x, y;
    SXINT			old_glob_tree_top, old_node_list_top, old_var_hd_top;
    SXBA		old_tree_bag_top;
#if EBUG
    SXINT			old_var_list_top;
#endif


#if SXDCGhas_prolog==1
    SXINT			Bkl;
    bool		has_ppp;
#endif

#if DCG_is_cyclic==1
    SXINT			*son_ptr;
    struct cyclic_tree	*cyclic_tree_ptr;
#endif

#if SXDCGis_dynam_constant==1
    SXINT 		i, ste, dc, val, ref;
    char		*str;
    struct sxtoken	*token;
#endif

    old_glob_tree_top = glob_tree_top;
    old_tree_bag_top = tree_bag.pool_top;
    old_node_list_top = node_list_top;
    old_var_hd_top = var_hd_top;
#if EBUG
    old_var_list_top = var_list_top;
#endif

    /* On fabrique l'ensemble de ses sous arbres . */
    do {
	done = true;
	root_set = bag_get (&tree_bag, glob_tree_top+1);

	for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
	    sub_tree = sons_stack [failed_pos];
	    sub_tree2_set = tree2attr [sub_tree].tree_set;

	    if (IS_AND (root_set, sub_tree2_set)) {
		/* sxtrap (ME, "bi-epsilon grammars are not yet processed."); */

		/* traitement dynamique des grammaires bi_epsilon. */
		for (pos = 1; pos < failed_pos;pos++) {
		    sub_tree1 = sons_stack [pos];
		    sub_tree1_set = tree2attr [sub_tree1].tree_set;

		    if (IS_AND (sub_tree1_set, sub_tree2_set)) {
			com_tree_set_nb = 0;
			com_sub_tree = common_sub_tree (sub_tree);
			/* com_sub_tree est le sous-arbre commun entre sub_tree1 et sub_tree */

#if EBUG
			if (com_sub_tree == 0)
			    sxtrap (ME, "DCGsem_eval");
#endif

			if ((tree2attr [com_sub_tree+1].var_hd-tree2attr [com_sub_tree].var_hd) > 0) {
			    /* Il a des variables, on clone */
			    bag_reuse (&tree_bag, root_set);
			    com_tree_set = tree2attr [com_sub_tree].tree_set;
			    /* Les identifiants des clones seront des increments par rapport a tree_clone_base */
			    tree_clone_base = glob_tree_top;
			    com_tree_set_nb += sxba_cardinal (com_tree_set);

			    if ((glob_tree_top += com_tree_set_nb) >= tree_size)
				tree_oflw ();

			    if (com_tree_set_nb >= clone2node_size)
				clone2node = (SXINT*) sxrealloc (clone2node, (clone2node_size *= 2) + 1, sizeof (SXINT));

			    /* On remplit clone2node par les 2 bouts!! */
			    clone2node_top = com_tree_set_nb;

			    y = fill_clone2node (sub_tree, clone2node_top);

			    node = 0;
			    x = 0;

			    while ((node = sxba_scan (com_tree_set, node)) > 0)
				clone2node [x++] = node;

#if EBUG
			    if (x != y)
				sxtrap (ME, "DCGsem_eval");
#endif
			    /* Ce sont les sous-arbres sub_tree1 et sub_tree qui ont en
			       commun un sous-arbre. Attention, sub_tree1 et sub_tree
			       peuvent etre egaux. */
			    sons_stack [failed_pos] = process_bi_epsilon (sub_tree1, sub_tree);
			    /* PB: quel est le statut du clone par rapport a la liste des Bkk ? */
			    /* Pour le moment, on ne reutilise pas les clones! */
			    done = false;
			}

			break;
		    }
		}

#if EBUG
		if (pos == failed_pos)
		    sxtrap (ME, "DCGsem_eval");
#endif

		if (!done)
		    break;
	    }

	    OR (root_set, tree2attr [sub_tree].tree_set);
	}
    } while (!done);

    SXBA_1_bit (root_set, glob_tree_top);

    Aij = Frhs_stack [0];
    var_nb = SXDCG2var_nb [prod];

#if DCG_is_cyclic==1
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
	    sxtrap (ME, "DCGsem_eval");
#endif
	/* On parcourt les sous-arbres qui ont Aij comme racine */
	sub_tree = Aij2tree [Aij];

	while (sub_tree != 0) {
	    sub_tree_ptr = &(tree2attr [sub_tree]);

	    if (sub_tree_ptr->prod == prod && SXBA_bit_is_set (root_set, sub_tree)) {
		son_ptr = &(node_list [sub_tree_ptr->node_list]);

		for (nb = 1; nb <= nbnt; nb++) {
		    if (sons_stack [nb] > 0 && tree2attr [sons_stack [nb]].Aij != tree2attr [*son_ptr].Aij)
			break;

		    son_ptr++;
		}

		if (nb > nbnt) {
		    /* sub_tree est le sous-arbre germe */
		    /* Il a bien sur les memes noms de variables que glob_tree_top. */
			
#if EBUG
		    /* Le germe n'est pas cyclique et il n'a encore jamais ete utilise */
		    if (sub_tree_ptr->cyclic_tree_id != 0)
			sxtrap (ME, "DCGsem_eval");
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
#if 0
	/* Il y en a forcement un, mais peut etre pas sur ce "depliage" */
	if (sub_tree == 0)
	    sxtrap (ME, "DCGsem_eval");
#endif

	return failed_pos; /* nbnt+1 */
    }
#endif

    /* On prepare la zone des variables */
    if (var_nb > 0) {
	if ((var_hd_top += var_nb) >= var_hd_size)
	    var_hd_oflw ();

	var_ptr = var_hd + var_hd_top;
	/* Macro qui s'etend en un switch */
	SXDCGclear_vars (var_nb)
    }

#if SXDCGhas_prolog==1
    has_ppp = false;
#endif

    tree_ptr = &(tree2attr [glob_tree_top]);
    /* Pour create substitution qui peut l'utiliser! */
    tree_ptr->prod = prod;

    for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
#if SXDCGhas_prolog==1
	Bkl = Frhs_stack [failed_pos];

	if (SXDCGppp [Aij2A (Bkl)] != 0) {
	    /* On postpone l'execution des predicats prolog predefinis car on aura
	       peut-etre + d'info en fin de RHS! */
	    has_ppp = true;
	    continue;
	}
#endif

	sub_tree = sons_stack [failed_pos];
	sub_tree_ptr = &(tree2attr [sub_tree]);
	sub_prod = sub_tree_ptr->prod;
	param_nb = SXDCG2param_nb [sub_prod][0];

	if (param_nb == SXDCG2param_nb [prod] [failed_pos]) {
	    /* meme nb de param */
	    if (param_nb > 0) {
		t1_ptr = &(SXDCG2term [prod] [failed_pos] [0]);
		t2_ptr = &(SXDCG2term [sub_prod] [0] [0]);

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

#if SXDCGhas_prolog==1
    /* Attention, il faut quand meme evaluer les ppp qui n'ont pas de variables, ou des variables
       qui n'interviennent pas dans les predicats de la meme regle. */
    if (has_ppp && failed_pos > nbnt) {
	for (glob_where = 1; glob_where <= nbnt; glob_where++) {
	    Bkl = Frhs_stack [glob_where];

	    if ((ppp = SXDCGppp [Aij2A (Bkl)]) != 0) {
		/* predef_prolog_predicate */
		/* seuls seront stockes ds pppredo les pos < glob_where */
		if (!(*execute_prolog [ppp].call) (glob_tree_top,
						   /* sons_stack [glob_cur_pos], */
						   &(SXDCG2term [prod] [glob_where] [0])))
		    break;
	    }
	}

	failed_pos = glob_where;
    }

    if (XxY_top (pppredo) > 0) {
	if (failed_pos > nbnt) {
	    /* l'unification du reste a marche et il y a des contraintes a verifier */
	    bool 	done;
	    SXINT		x, /* sub_tree_id, */ sub_prod, cur_root, cur_pos, cur_item;
	    SXINT		*param_ptr;

	    /* Tous les ppp seront stockes ds pppredo */
	    glob_where = nbnt+1;

	    do {
		done = true;

		for (x = 1; failed_pos > nbnt && x <= XxY_top (pppredo); x++) {
		    if (!XxY_is_erased (pppredo, x)) {
			/* Attention, le corps de la boucle peut rajouter des elements... */
			done = false;

			do {
			    cur_root = XxY_X (pppredo, x);
			    cur_item = XxY_Y (pppredo, x);
			    cur_pos = inputG.item2nbnt [cur_item]+1;

			    if (cur_root == glob_tree_top) {
				/* sub_tree_id = sons_stack [cur_pos]; */
				sub_prod = prod;
			    }
			    else {
				sub_prod = tree2attr [cur_root].prod;
				/* sub_tree_id = node_list [tree2attr [cur_root].node_list+cur_pos-1]; */
			    }

			    param_ptr = &(SXDCG2term [sub_prod] [cur_pos] [0]);
			    ppp = SXDCGppp [inputG.lispro [cur_item]];

			    if (!(*execute_prolog [ppp].call) (cur_root, /* sub_tree_id, */ param_ptr)) {
				/* Il faut savoir dans quel sous-arbre se trouve le sub_tree qui a fait echouer
				   les contraintes pour positionner failed_pos. */
				if (cur_root == glob_tree_top)
				    failed_pos = cur_pos;
				else {
				    for (failed_pos = 1; failed_pos <= nbnt; failed_pos++) {
					if (SXBA_bit_is_set (tree2attr [sons_stack [failed_pos]].tree_set,
							     cur_root))
					    break;
				    }
#if EBUG
				    if (failed_pos > nbnt)
					sxtrap (ME, "DCGsem_eval");
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

    glob_where = 0;
#endif
   
    if (failed_pos > nbnt) {
#if SXDCGis_dynam_constant==1
	if ((dc = SXDCGprod2dc [prod]) > 0) {
	    /* On est sur une production de la forme:
	       <Dcg_constant> = %Dcg_dynam_atom ;
	       <Dcg_constant> = %Dcg_dynam_int ;
	       <Dcg_constant> = ti ;
	       */
	    i = Aij2i (Aij);

#if EBUG
	    if (i+1 != Aij2j (Aij))
		sxtrap (ME, "DCGsem_eval");
#endif

	    token = &(SXGET_TOKEN (i));
	    ste = token->string_table_entry;

	    switch (dc) {
	    case INTEGER_CONSTANT:
		str = ste == SXEMPTY_STE ? inputG.tstring [token->lahead] : sxstrget (ste);
		val = atoi (str);
		ref = KV2REF (INTEGER_CONSTANT, val);

		break;
	    case ATOM:
		if (ste == SXEMPTY_STE) {
		    str = inputG.tstring [token->lahead];
		    ste = sxstrsave (str);
		}
		
		ref = KV2REF (DYNAM_ATOM, ste);
		break;
#if EBUG
	    default:
		sxtrap (ME, "DCGsem_eval");
#endif
	    }

	    /* SXDCG2term [prod] [0] [0] == (VARIABLE, 1) */
	    create_substitution (glob_tree_top, SXDCG2term [prod] [0] [0], glob_tree_top, ref);
	}
#endif

	tree_ptr->Aij = Aij;
	tree_ptr->next = Aij2tree [Aij];
	tree_ptr->tree_set = root_set;
#if DCG_is_cyclic==1
	tree_ptr->cyclic_tree_id = 0;
#endif


	Aij2tree [Aij] = glob_tree_top;
	    
	if (nbnt > 0) {
	    if ((node_list_top += nbnt) >= node_list_size)
		node_list_oflw ();

	    node_ptr = &(node_list [node_list_top]);
	    nb = nbnt;
		
	    do {
		*--node_ptr = sons_stack [nb];
	    } while (--nb > 0);
	}

	if (++glob_tree_top >= tree_size)
	    tree_oflw ();

	next_tree_ptr = &(tree2attr [glob_tree_top]);
	next_tree_ptr->node_list = node_list_top;
	next_tree_ptr->var_hd = var_hd_top;

	is_unified = true;	/* herbrand_semact a marche (au moins) une fois */

	if (debug_level & 4) {
	    print_rule (glob_tree_top-1);
	}

	/* A FAIRE :
	   Regarder si le nouveau sous-arbre n'est pas syntaxiquement et semantiquement equivalent
	   avec un autre sous-arbre deja construit. Auquel cas, il est inutile. On peut meme faire
	   de la subsomption!. */

	undo_stack_top = 0;
    }
    else {
	if (debug_level & 4) {
	    /* Pour l'impression */
	    tree_ptr = &(tree2attr [glob_tree_top]);
	    tree_ptr->prod = prod;
	    tree_ptr->Aij = Aij;
	    
	    if (nbnt > 0) {
		if ((node_list_top + nbnt) >= node_list_size)
		    node_list_oflw ();

		node_ptr = &(node_list [node_list_top + nbnt]);
		nb = nbnt;
		
		do {
		    *--node_ptr = sons_stack [nb];
		} while (--nb > 0);
	    }

	    if (glob_tree_top+1 >= tree_size)
		tree_oflw ();

	    next_tree_ptr = &(tree2attr [glob_tree_top+1]);
	    next_tree_ptr->node_list = node_list_top;
	    next_tree_ptr->var_hd = var_hd_top;

	    putchar ('*');	/* Unification failed */
	    print_rule (glob_tree_top);
	}

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
		sxtrap (ME, "DCGsem_eval");
#endif

	    var_hd [indx] = var_list [var_list_top].next;
	    var_list_top--;
	}

#if EBUG
	if (var_list_top != old_var_list_top)
	    sxtrap (ME, "DCGsem_eval");
#endif
    }

    return failed_pos;
}
#endif


static SXINT
unfold (prod, i, nbnt)
    SXINT prod, i, nbnt;
{
    SXINT Bkl, sub_tree, failed_pos;

    if (i > nbnt) {
	failed_pos = DCGsem_eval (prod, nbnt);
    }
    else {
	Bkl = Frhs_stack [i];

	for (sub_tree = Aij2tree [Bkl]; sub_tree > 0; sub_tree = tree2attr [sub_tree].next) {
	    sons_stack [i] = sub_tree;

	    failed_pos = unfold (prod, i+1, nbnt);
	    /* if failed_pos > nbnt => l'unification a marche */
	    /* if failed_pos <= nbnt => l'unification a echoue sur le traitement du failed_pos eme sous arbre */

	    if (failed_pos < i)
		break;
	}

#if 0
#if SXDCGhas_prolog==1
    {
	SXINT 			la3, var_nb, old_glob_tree_top, old_var_list_top;
	SXINT			*var_ptr;
	struct tree2attr	*sub_tree_ptr, *tree_ptr, *next_tree_ptr;

	while (failed_pos >= i && (la3 = to_be_cloned [i]) > 0) {
	    old_glob_tree_top = glob_tree_top;

	    sub_tree = Aij2tree [Bkl];
	    /* il faut cloner sub_tree */
	    /* Aucun lien dans les next */
	    sub_tree_ptr = &(tree2attr [sub_tree]);
	    root_set = bag_get (&tree_bag, glob_tree_top+1);
	    SXBA_1_bit (root_set, glob_tree_top);

#if EBUG
	    /* Les ppp n'ont pas de param */
	    if (SXDCG2var_nb [sub_tree_ptr->prod] != 0)
		sxtrap (ME, "unfold");
#endif

#if 0
	    old_var_list_top = var_list_top;
	    var_nb = SXDCG2var_nb [sub_tree_ptr->prod];

	    /* On prepare la zone des variables */
	    if (var_nb > 0) {
		if ((var_hd_top += var_nb) >= var_hd_size)
		    var_hd_oflw ();

		var_ptr = var_hd + var_hd_top;
		/* Macro qui s'etend en un switch */
		SXDCGclear_vars (var_nb)
	    }
#endif
	    tree_ptr = &(tree2attr [glob_tree_top]);
	    tree_ptr->prod = sub_tree_ptr->prod;
	    tree_ptr->Aij = sub_tree_ptr->Aij;
	    tree_ptr->next = la3;
	    tree_ptr->tree_set = root_set;
#if DCG_is_cyclic==1
	    tree_ptr->cyclic_tree_id = 0;
#endif

	    sons_stack [i] = glob_tree_top;

	    if (++glob_tree_top >= tree_size)
		tree_oflw ();

	    next_tree_ptr = &(tree2attr [glob_tree_top]);
	    next_tree_ptr->node_list = node_list_top;
	    next_tree_ptr->var_hd = var_hd_top;

	    to_be_cloned [i] = 0;

	    failed_pos = unfold (prod, i+1, nbnt);
	    /* if failed_pos > nbnt => l'unification a marche */
	    /* if failed_pos <= nbnt => l'unification a echoue sur le traitement du failed_pos eme sous arbre */

	    if (failed_pos <= nbnt) {
		/* Echec, on recupere "glob_tree_top" */
		/* On recupere aussi les "tree_bag" */
		bag_reuse (&tree_bag, root_set);
#if EBUG
		if (old_glob_tree_top != glob_tree_top-1)
		    sxtrap (ME, "unfold");
#endif

		glob_tree_top--;
#if 0
		var_hd_top -= var_nb;
		var_list_top = old_var_list_top;
#endif
	    }
	}
    }
#endif
#endif
    }

    return failed_pos;
}

static SXINT
herbrand_semact (i, j, prod_core, rhs_stack)
    unsigned short	*prod_core;
    SXINT 	i, j, rhs_stack [];
{
    SXINT		prod = prod_core [0];
    SXINT		Aij;

#include <values.h>
    /* Definit la constante HIBITS == 1<<15 pour les mots de 32! */

    if (inputG.has_cycles && (prod & HIBITS)) {
	/* On est dans le cas cyclique,
	   on a deja ete appele avec cette production. */
	/* A FAIRE */

	prod &= ~(HIBITS);

#if DCG_is_cyclic==0
	sxtrap (ME, "Cyclic grammars are not (yet!) processed.");
#else

	if (SXDCG2var_nb [prod] == 0)
	    /* Pas de variables, on ne fait rien! */
	    return 0;

	is_cyclic_prod = true;
	/* Prevenir le parser que le point fixe est atteint. */
#endif	
    }
    else
#if DCG_is_cyclic==1
	is_cyclic_prod = false;
#endif

    Frhs_stack = rhs_stack;

    is_unified = false;

    unfold (prod, 1, inputG.prod2nbnt [prod]);

#if DCG_is_cyclic==1
    return is_cyclic_prod ? 0 : (SXINT)is_unified;
#else
    return (SXINT)is_unified;
#endif
}

static void
print_Aij (Aij)
    SXINT Aij;
{
    printf ("<%s[%i..%i]>", inputG.ntstring [Aij2A (Aij)], Aij2i (Aij), Aij2j (Aij));
}

#if SXDCGmax_list!=0
static void print_term ();

static void
print_list (t, sub_tree)
    SXINT t, sub_tree;
{
    SXINT		y, node, local_var, bot, atom, top, kind;

    kind = REF2KIND (t);

    if (kind == VARIABLE) {
	/* C'est une variable */
	substitute (&sub_tree, &t);
	kind = REF2KIND (t);
    }

    t = REF2VAL (t);
    
    switch (kind) {
    case VARIABLE:
	/* C'est une variable libre, on sort son nom. */
	/* Elle est sensee etre la liste suffixe, on la fait preceder de "|" */
	printf ("|_%i%s", sub_tree,
		SXDCGvar_str [SXDCGgvar_name [SXDCG2gvar_disp [tree2attr [sub_tree].prod]+t-1]]);
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
	    print_term (SXDCGlist_list [t], sub_tree);
	    print_list (SXDCGlist_list [t+1], sub_tree);
	}
	break;

#if SXDCGhas_prolog==1
    case DYNAM_LIST:
	if (t > 0) {
	    fputs (", ", stdout);
	    print_term (XxY_X (dynam_list, t), sub_tree);
	    print_list (XxY_Y (dynam_list, t), sub_tree);
	}
	break;
#endif
    }
}
#endif


static void
print_term (t, sub_tree)
    SXINT t, sub_tree;
{
    SXINT		y, node, local_var, bot, atom, top, kind, op;

    kind = REF2KIND (t);

    if (kind == VARIABLE) {
	/* C'est une variable */
	if (t != DUM_VAR) {
	    substitute (&sub_tree, &t);
	    kind = REF2KIND (t);
	}
    }

    t = REF2VAL (t);
    
    switch (kind) {
    case VARIABLE:
	/* C'est une variable libre, on sort son nom. */
#if EBUG
	if (sub_tree == 0)
	    /* Les variables dynamiques ne sont jamais libres! */
	    sxtrap (ME, "print_term");
#endif

	printf ("_%i%s", sub_tree,
		SXDCGvar_str [SXDCGgvar_name [SXDCG2gvar_disp [tree2attr [sub_tree].prod]+t-1]]);
	break;

    case ATOM:
	printf ("%s", SXDCGatom_str [t]);
	break;

    case DYNAM_ATOM:
	printf ("%s", sxstrget (t));
	break;

    case INTEGER_CONSTANT:
	printf ("%i", t);
	break;

#if SXDCGmax_term!=0
    case TERM:
	bot = SXDCGterm_disp [t];
	atom = SXDCGterm_list [bot];

	printf ("%s", SXDCGatom_str [REF2VAL (atom)]);

	top = SXDCGterm_disp [t+1];

	if (++bot < top) {
	    putchar ('(');

	    for (;;) {
		print_term (SXDCGterm_list [bot], sub_tree);

		if (++bot == top)
		    break;

		fputs (", ", stdout);
	    }

	    putchar (')');
	}
	break;

#if SXDCGhas_integer_op==1
    case INTEGER_OP:
    {
	bool	is_val1, is_val2;
	SXINT	op_tree, op_ref, val1, val2;

	bot = SXDCGterm_disp [t];
	op = REF2VAL (SXDCGterm_list [bot]);

	op_tree = sub_tree;
	op_ref = SXDCGterm_list [bot+1];
	is_val1 = integer_val (&op_tree, &op_ref, &val1);

	if (op != UMINUS) {
	    op_tree = sub_tree;
	    op_ref = SXDCGterm_list [bot+2];
	    is_val2 = integer_val (&op_tree, &op_ref, &val2);
	}
	else {
	    if (is_val1) {
		printf ("(-%i)", val1);
	    }
	    else
	    {
		fputs ("(-", stdout);
		print_term (SXDCGterm_list [bot+1], sub_tree);
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
	    print_term (SXDCGterm_list [bot+1], sub_tree);

	if (op == MODULO)
	    fputs (" mod ", stdout);
	else
	    printf ("%s", SXDCGppp2str [op]);

	if (is_val2)
	    printf ("%i", val2);
	else
	    print_term (SXDCGterm_list [bot+2], sub_tree);

	putchar (')');
    }

	break;
#endif
#endif

#if SXDCGmax_list!=0
    case STATIC_LIST:
	putchar ('[');

	if (t > 0) {
	    print_term (SXDCGlist_list [t], sub_tree);
	    print_list (SXDCGlist_list [t+1], sub_tree);
	}

	putchar (']');
	break;

#if SXDCGhas_prolog==1
    case DYNAM_LIST:
	putchar ('[');

	if (t > 0) {
	    print_term (XxY_X (dynam_list, t), sub_tree);
	    print_list (XxY_Y (dynam_list, t), sub_tree);
	}

	putchar (']');
	break;
#endif

#endif

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
    SXINT		prod, Aij, A, param_nb, pos, j, k, item, node, X, Blk, ste, ppp;
    SXINT		*param_ptr, *node_ptr;
    bool	is_first;

    Aij = tree2attr [tree].Aij;
    A = Aij2A (Aij);
    prod = tree2attr [tree].prod;
    printf ("%i:", tree);
    printf ("<%s[%i..%i]>", inputG.ntstring [A], k = Aij2i (Aij), Aij2j (Aij));

    param_nb = SXDCG2param_nb [prod][0];

    if (param_nb > 0) {
	putchar ('(');
	param_ptr = &(SXDCG2term [prod] [0] [0]);
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

    item = inputG.prolon [prod];
    pos = 0;
    node_ptr = &(node_list [tree2attr [tree].node_list]);
    is_first = true;

    while ((X = inputG.lispro [item]) != 0) {
	if (is_first)
	    is_first = false;
	else
	    fputs (", ", stdout);

	if (X > 0) {
	    pos++;
	    node = *node_ptr++;
	    printf ("%i", node);
	    Blk = tree2attr [node].Aij;
	    k = Aij2j (Blk);
	}
	else {
	    ste = SXGET_TOKEN (k).string_table_entry;
	    printf ("[%s]", ste == SXEMPTY_STE ? inputG.tstring [-X] : sxstrget (ste));
	    k++;
	}

	item++;
    }

#if SXDCGhas_prolog==1
    if (prod <= SXDCGmaxpppprod) {
	SXINT 	ppp_item, ppp_item_nb;
	SXINT		*ppp_prolon, *pbot, *ptop;
	SXINT		*param_ptr;

	ppp_prolon = SXDCGppp_prolon + prod;

	if ((ppp_item_nb = ppp_prolon [1] - (ppp_item = *ppp_prolon)) > 0) {
	    /* Cette prod a des ppp */
	    pbot = SXDCGppp_lispro + ppp_item;
	    ptop = pbot + ppp_item_nb;

	    do {
		if (is_first)
		    is_first = false;
		else
		    fputs (", ", stdout);

		param_ptr = &(SXDCGppp_term [SXDCGppp_term_hd [ppp_item]]);
		/* predef_prolog_predicate */
		ppp = *pbot;

		if (ppp == IS || ppp == LESS || ppp == LESS_EQUAL || ppp == SUP || ppp == SUP_EQUAL) {
		    /* infixe */
		    if (ppp == IS) {
			/* On imprime le nom de la variable */
			printf ("_%i%s is ", tree,
				SXDCGvar_str [SXDCGgvar_name [SXDCG2gvar_disp [tree2attr [tree].prod]+
							      REF2VAL (*param_ptr++)-1]]);
		    }
		    else {
			print_term (*param_ptr++, tree);
			printf ("%s", SXDCGppp2str [ppp]);
		    }

		    print_term (*param_ptr++, tree);
		}
		else {
		    printf ("%s", SXDCGppp2str [ppp]);
		    param_nb = &(SXDCGppp_term [SXDCGppp_term_hd [ppp_item+1]])-param_ptr;

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
	}
    }
#endif

    fputs (".\n", stdout);
}

static void
print_tree (tree)
    SXINT tree;
{
    SXINT		*param_ptr, *node_ptr, *top_node_ptr;

    print_rule (tree);

    node_ptr = &(node_list [tree2attr [tree].node_list]);
    top_node_ptr = &(node_list [tree2attr [tree+1].node_list]);

    while (node_ptr < top_node_ptr) {
	print_tree (*node_ptr++);
    }
}

static SXINT
herbrand_semantics (S0n)
    SXINT S0n;
{
    SXINT root, Aij, tree_nb;

    if (debug_level & (8+2+1)) {
	root = Aij2tree [S0n];
	tree_nb = 0;

	if (root > 0) {
	    do {
		/* root_set est l'ensemble des sous-arbres du pere
		   il faudrait ici faire l'union sur toutes les racines, ca ne semble pas necessaire
		   aucune info n'a pu passer de l'une a l'autre */
		tree_nb++;

		if (debug_level & (2+1)) {
		    root_set = tree2attr [root].tree_set;

		    if (debug_level & (2))
			print_tree (root);
		    else
			print_rule (root);

		    putchar ('\n');
		}
	    } while ((root = tree2attr [root].next) > 0);
	}
    }

    if (tree_nb == 0)
	/* L'unification a echouee */
	fputs ("\n**** Unification failed. ****\n\n", stdout);
    else
	if (debug_level & (8)) {
	    if (tree_nb == 1)
		fputs ("\nThere is a single tree.\n\n", stdout);
	    else
		printf ("\nThere are %i trees.\n\n", tree_nb);
	}

    return 1;
}


#if POST_SEM==1
/* DECOUPLAGE TOTAL ENTRE GENERATION DE LA FORET PARTAGEE ET UNIFICATION */

/* L'idee est que si l'on connait la foret partagee et si toutes les unifications qui
   calculent la semantique du symbole Aij ont echouees, alors ce n'est pas la peine
   d'executer les productions ou Aij intervient en rhs (elles sont supprimees) mais si
   de plus cette supression elimine toutes les occurrences en rhs de disons Bkl, alors
   ont peut supprimer (recursivement) les regles qui calculent Bkl. */

static struct {
    SXINT		rule_top, rule_size, item_top, item_size;

    SXINT		*prolon;
    SXINT		*lispro;
    SXINT		*next_item;

    struct pf_Aij {
	SXINT lhs, rhs;
    } *Aij;
} pf;

static void
pf_rule_oflw ()
{
    pf.prolon = (SXINT*) sxrealloc (pf.prolon, (pf.rule_size *=2)+1, sizeof (SXINT));
}


static void
pf_item_oflw ()
{
    pf.item_size *=2;
    pf.lispro = (SXINT*) sxrealloc (pf.lispro, pf.item_size+1, sizeof (SXINT));
    pf.next_item = (SXINT*) sxrealloc (pf.next_item, pf.item_size+1, sizeof (SXINT));
}


static void
herbrand_post_semact_init (size)
    SXINT size;
{
    /* size == pAij_top */
    pf.rule_size = size;
    pf.prolon = (SXINT*) sxalloc (pf.rule_size+1, sizeof (SXINT));

    pf.item_size = ((inputG.maxitem/inputG.maxprod)+2)*pf.rule_size;
    pf.lispro = (SXINT*) sxalloc (pf.item_size+1, sizeof (SXINT));
    pf.next_item = (SXINT*) sxalloc (pf.item_size+1, sizeof (SXINT));

    pf.prolon [0] = 0;
    pf.lispro [0] = pf.next_item [0] = 0;
    pf.prolon [pf.rule_top = 1] = pf.item_top = 1;

    pf.Aij = (struct pf_Aij*) sxcalloc (size+1, sizeof (struct pf_Aij));

    herbrand_semact_init (size);
}




static void
herbrand_post_semact_final (size)
    SXINT size;
{
    sxfree (pf.prolon);
    sxfree (pf.lispro);
    sxfree (pf.next_item);
    sxfree (pf.Aij);

    herbrand_semact_final (size);
}


static SXINT
herbrand_post_semact (i, j, prod_core, rhs_stack)
    unsigned short	*prod_core;
    SXINT 	i, j, rhs_stack [];
{
    SXINT		prod = prod_core [0];
    SXINT		Aij, Bkl, nbnt, x;

#include <values.h>
    /* Definit la constante HIBITS == 1<<15 pour les mots de 32! */

    if (inputG.has_cycles && (prod & HIBITS)) {
	/* On est dans le cas cyclique,
	   on a deja ete appele avec cette production. */
	/* A FAIRE */
	prod &= ~(HIBITS);
	sxtrap (ME, "Cyclic grammars are not (yet!) processed.");
    }

    nbnt = inputG.prod2nbnt [prod];

    if (pf.item_top + nbnt + 2 >= pf.item_size)
	pf_item_oflw ();

    pf.lispro [pf.item_top] = Aij = rhs_stack [0];
    pf.next_item [pf.item_top] = pf.Aij [Aij].lhs;
    pf.Aij [Aij].lhs = pf.item_top;

    for (x = 1; x <= nbnt; x++) {
	pf.item_top++;
	pf.lispro [pf.item_top] = Bkl = rhs_stack [x];
	pf.next_item [pf.item_top] = pf.Aij [Bkl].rhs;
	pf.Aij [Bkl].rhs = pf.item_top;
    }

    pf.item_top++;
    pf.lispro [pf.item_top] = -pf.rule_top;
    pf.next_item [pf.item_top] = prod;

    pf.item_top++;

    if (++pf.rule_top >= pf.rule_size)
	pf_rule_oflw ();

    pf.prolon [pf.rule_top] = pf.item_top;

    return true;
}


static SXINT
walk_next_rhs (it)
    SXINT it;
{
    SXINT it_top, next_it;

    while ((it_top = it) > 0) {
	while ((next_it = pf.lispro [++it_top]) > 0);

	if (pf.next_item [next_it] > 0)
	    /* production valide, on sort */
	    return it;

	it = pf.next_item [it];
    }

    /* fin de liste */
    return 0;
}


static SXINT
walk_next_lhs (it)
    SXINT it;
{
    /* it repere une lhs */
    while (it > 0 && pf.next_item [pf.prolon [1-pf.lispro [it-1]] - 1] < 0)
	/* production supprimee, on continue */
	it = pf.next_item [it];

    /* production valide */
    return it;
}


static void
erase_prod (Bkl)
    SXINT Bkl;
{
    SXINT it, pf_prod, it_top, it_cur, Cmn, new_it, prod;

    /* Supprime toutes les Bkl-productions */

    while ((it = pf.Aij [Bkl].lhs) > 0) {
	/* On ne reviendra jamais sur cette production par Bkl */
	pf.Aij [Bkl].lhs = pf.next_item [it];
	/* cet item est dand la regle pf_prod */
	pf_prod = 1-pf.lispro [it-1];
	it_top = pf.prolon [pf_prod+1]-1;

	if ((prod = pf.next_item [it_top]) > 0) {
	    if (debug_level & 16) {
		printf ("\t**** Suppression de %i\n", pf_prod);
	    }

	    pf.next_item [it_top] = -prod; /* supprimee */

	    /* On supprime sa partie droite */
	    it_cur = it;

	    while (++it_cur < it_top) {
		Cmn = pf.lispro [it_cur];

		if (pf.Aij [Cmn].rhs == it_cur) {
		/* C'est le premier de la liste, on le supprime */
		/* mais on peut "tomber" dans une production qui a ete supprimee
		   il faut donc le sauter */
		    pf.Aij [Cmn].rhs = new_it = walk_next_rhs (pf.next_item [it_cur]);

		    if (new_it == 0) {
			/* On peut supprimer toutes les Cmn-productions */
			erase_prod (Cmn);
		    }
		}
	    }
	}
    }
}

static void
clean_Bkl (item, item_top)
    SXINT item, item_top;
{
    SXINT Aij, it, it_top, Bkl, next_it;

    Aij = Frhs_stack [0];
    /* Supprime les productions de la forme : .. -> ... Aij ... Bkl ... et les Bkl */

    while ((it = pf.Aij [Aij].rhs) > 0) {
	/* On ne reviendra jamais sur cette production par Aij */
	pf.Aij [Aij].rhs = pf.next_item [it];
	/* On parcourt les occurrences de Aij en rhs. */
	/* On cherche le "top" de cette prod */
	it_top = it;

	while ((Bkl = pf.lispro [++it_top]) > 0) {
	    if (pf.Aij [Bkl].rhs == it_top) {
		/* C'est le premier de la liste, on le supprime */
		/* mais "next_item" peut "tomber" dans une production qui a ete supprimee
		   il faut donc le sauter */
		pf.Aij [Bkl].rhs = next_it = walk_next_rhs (pf.next_item [it_top]);

		if (next_it == 0) {
		    /* Il n'y a plus d'occurrences de Bkl en rhs */
		    /* On peut supprimer toutes les Bkl-productions */
		    erase_prod (Bkl);
		}
	    }
	}

	/* On supprime la production courante */
	pf.next_item [it_top] = -pf.next_item [it_top];
    }
}

static SXINT
herbrand_post_semantics (S0n)
    SXINT S0n;
{
    SXINT pf_prod, item_top, prod, item, Aij, next_item;

    for (pf_prod = 1; pf_prod < pf.rule_top; pf_prod++) {
	item_top = pf.prolon [pf_prod+1] - 1;
	prod = pf.next_item [item_top];

	if (prod > 0) {
	    /* pf_prod est valide */
	    item = pf.prolon [pf_prod];
	    Frhs_stack = pf.lispro + item;
	    is_unified = false;

	    unfold (prod, 1, item_top - item - 1);
	
	    if (!is_unified) {
		/* On "supprime" la regle courante */
		pf.next_item [item_top] = -prod;
		Aij = Frhs_stack [0];

		if (pf.Aij [Aij].lhs == item) {
		    /* C'est la derniere Aij-production */
		    pf.Aij [Aij].lhs = next_item = walk_next_lhs (pf.next_item [item]);

		    if (next_item == 0) {
			/* Il n'y a plus d'occurrences de Aij en lhs */
			/* On peut supprimer toutes productions qui ont Aij en rhs */
			if (debug_level & 16) {
			    printf ("**** Echec de %i\n", pf_prod);
			}

			clean_Bkl (item, item_top);
		    }
		}

	    }
	}
    }

    herbrand_semantics (S0n);

    if (is_print_time)
	sxtime (TIME_FINAL, "\tUnification");
}
#endif


SXINT
herbrand (what, arg)
    SXINT		what;
    char	*arg;
{
    struct for_semact	*for_semact;

    switch (what) {
    case SXOPEN:
    case SXINIT:
    case SXERROR:
    case SXFINAL:
    case SXCLOSE:
    case SXSEMPASS:
	/* INUTILISES */
	break;

    case SXACTION:
	for_semact = (struct for_semact*) arg;

	for_semact->sem_init =
#if POST_SEM==1
	    herbrand_post_semact_init;
#else
	herbrand_semact_init;
#endif
	for_semact->sem_final =
#if POST_SEM==1
	    herbrand_post_semact_final; 
#else
	herbrand_semact_final;
#endif
	for_semact->oflw = NULL;
	for_semact->semact =
#if POST_SEM==1
	    herbrand_post_semact;
#else
	herbrand_semact;
#endif
	for_semact->parsact = NULL;
	for_semact->prdct = NULL;
	for_semact->constraint = NULL;
	for_semact->sem_pass =
#if POST_SEM==1
	    herbrand_post_semantics;
#else
	herbrand_semantics;
#endif

#if 0
	/* Ds le cas des LIG, les Aij sont stockes ds le X_header Aij_hd */
	for_semact->need_Aij2A_i_j = false;
	for_semact->need_pack_unpack = false;
#endif

	break;

    default:
	fputs ("The function \"herbrand\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
#endif
