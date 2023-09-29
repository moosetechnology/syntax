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
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from fsa_to_re.pl1      */
/*  on Monday the fourteenth of April, 1986, at 13:44:02,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 13:07 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* Ven 22 Nov 1996 17:25(pb):	Adaptation a LINUX			*/
/************************************************************************/
/* 15-04-92 14:20 (pb):		Utilisation des SXBA standards		*/
/************************************************************************/
/* 11-10-90 14:45 (pb):		L'etat "0" est accepte.			*/
/*				Optimisations (sur un exemple le temps	*/
/*				de construction d'ERs est passe de	*/
/*				80.7s a 14.4s!)				*/
/************************************************************************/
/* 11-10-90 14:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/




#include "sxunix.h"
#include "varstr.h"
#include "sxba.h"

char WHAT_FSA_TO_RE[] = "@(#)SYNTAX - $Id: fsa_to_re.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


#define leaf 1
#define concat 2
#define star 3
#define or 4
#define plus 5
#define option 6

#define head_s_no 67
#define node_number_per_area 20

static int	S [7] [7] = {{0, 0, 0, 0, 0, 0, 0},
			     { /* bidon */ 0, 1, 0, 2, 1, 1, 1},
			     { /* leaf */ 0, 0, 0, 0, 0, 0, 0},
			     { /* concat */ 0, 3, 0, 4, 3, 5, 6},
			     { /* star */ 0, 1, 0, 2, 1, 1, 1},
			     { /* or */ 0, 1, 0, 6, 1, 1, 6},
			     { /* plus */ 0, 1, 0, 5, 1, 5, 1} /* option */ };
static int	SIZE, INITIAL_S;
static SXBA	/* SIZE */ *M /* 0:SIZE-1 */ ;
struct node {
    struct node		*link;
    int		name;
    union {
	struct {
	    struct node		*left;
	    struct node		*right;
	}	brother;
	char	*string_ptr;
    }	body;
};
struct tree_area {
    struct tree_area	*lnk;
    struct node		node_s [node_number_per_area];
};
static struct tree_area		*tree_area_ptr;
static int	cur_node_no;
static struct node	***re_ptrs /* 0:SIZE-1, 0:SIZE-1 */ ;

/* re_ptrs(i,j) = pointeur vers l'arbre representant
l'expression reguliere entre les noeuds i et j si M (i, j) */

static int	*in, *out /* 1:SIZE */ ;

/* pour chaque noeud arite en entree et sortie */

static SXBA	/* SIZE */ to_be_processed, reached_set, loop_set;
static struct node	**P_S /* 1:P_S_size */ ;

struct tete {
    int size;
    struct node		**T;
};
static struct tete	*tete;
static int		size_of_tete, top_of_tete, x_of_tete;
#define tete_pop(n)	(x_of_tete -= n)

struct sets {
    int 	size;
    SXBA	set;	
};
static struct sets	*sets;
static int		size_of_sets, top_of_sets, x_of_sets;
#define sets_pop(n)	(x_of_sets -= n)

struct ints {
    int 	size;
    int		*T;	
};
static struct ints	*ints;
static int		size_of_ints, top_of_ints, x_of_ints;
#define ints_pop(n)	(x_of_ints -= n)

static int		*to_be_sorted /* 1:M */ ;
static int		size_of_to_be_sorted;

static SXBA		succ_set;

static int		prev, current;
static struct node	*head_s [head_s_no];
static struct node	*leafs_hd [head_s_no];
static struct node	*canon ();
static struct node	*reduce_or_son ();
static struct node	*gen_prefixe ();
static struct node	*gen_suffixe ();




static struct node **tete_open (size)
    int		size;
{
    register struct tete	*atete;

    if (tete == NULL) {
	tete = (struct tete*) sxalloc ((size_of_tete = 5) + 1, sizeof (struct tete));
	top_of_tete = x_of_tete = 0;
	/* x_of_tete <= top-of_tete */
    }
    else if (x_of_tete > size_of_tete) {
	tete = (struct tete*) sxrealloc (tete, (size_of_tete *= 2) + 1, sizeof (struct tete));
    }

    atete = tete + x_of_tete;

    if (x_of_tete == top_of_tete) {
	atete->T = (struct node**) sxalloc (size + 1, sizeof (struct node*));
	atete->size = size;
	top_of_tete++;
    }
    else if (size > atete->size) {
	    atete->T = (struct node**) sxrealloc (atete->T, size + 1, sizeof (struct node*));
	    atete->size = size;
	}

    x_of_tete++;
    return atete->T;
}

static VOID tete_free ()
{
    if (tete != NULL) {
	register struct tete	*atete = tete + top_of_tete;

	while (--atete >= tete)
	    sxfree (atete->T);

	sxfree (tete);
    }
}


static int *ints_open (size)
    int		size;
{
    register struct ints	*aints;

    if (ints == NULL) {
	ints = (struct ints*) sxalloc ((size_of_ints = 5) + 1, sizeof (struct ints));
	top_of_ints = x_of_ints = 0;
	/* x_of_ints <= top-of_ints */
    }
    else if (x_of_ints > size_of_ints) {
	ints = (struct ints*) sxrealloc (ints, (size_of_ints *= 2) + 1, sizeof (struct ints));
    }

    aints = ints + x_of_ints;

    if (x_of_ints == top_of_ints) {
	aints->T = (int*) sxalloc (size + 1, sizeof (int));
	aints->size = size;
	top_of_ints++;
    }
    else if (size > aints->size) {
	    aints->T = (int*) sxrealloc (aints->T, size + 1, sizeof (int));
	    aints->size = size;
	}

    x_of_ints++;
    return aints->T;
}

static VOID ints_free ()
{
    if (ints != NULL) {
	register struct ints	*aints = ints + top_of_ints;

	while (--aints >= ints)
	    sxfree (aints->T);

	sxfree (ints);
    }
}


static SXBA sets_open (size)
    int		size;
{
    register struct sets	*asets;

    if (sets == NULL) {
	sets = (struct sets*) sxalloc ((size_of_sets = 4) + 1, sizeof (struct sets));
	top_of_sets = x_of_sets = 0;
	/* x_of_sets <= top-of_sets */
    }
    else if (x_of_sets > size_of_sets) {
	sets = (struct sets*) sxrealloc (sets, (size_of_sets *= 2) + 1, sizeof (struct sets));
    }

    asets = sets + x_of_sets;

    if (x_of_sets == top_of_sets) {
	asets->set = sxba_calloc (size + 1);
	asets->size = size;
	top_of_sets++;
    }
    else {
	sxba_empty (asets->set);

	if (size != asets->size) {
	    asets->set = sxba_resize (asets->set, size + 1);
	    asets->size = size;
	}
    }

    x_of_sets++;
    return asets->set;
}

static VOID sets_free ()
{
    if (sets != NULL) {
	register struct sets	*asets = sets + top_of_sets;

	while (--asets >= sets)
	    sxfree (asets->set);

	sxfree (sets);
    }
}



static struct node	*alloc_a_node ()
{
    struct tree_area	*p;

    if (cur_node_no == node_number_per_area) {
	p = tree_area_ptr;
	tree_area_ptr = (struct tree_area*) sxcalloc (1, sizeof (struct tree_area));
	tree_area_ptr->lnk = p;
	cur_node_no = 0;
    }

    return &(tree_area_ptr->node_s [cur_node_no++]);
}



static SXVOID	free_tree_areas ()
{
    register struct tree_area	*p;

    while ((p = tree_area_ptr)) {
	tree_area_ptr = p->lnk;
	sxfree (p);
    }
}



static struct node	*create_leaf (i, j, get_name_refs)
    int		i, j;
    int		(*get_name_refs) ();
{
    register struct node	*leaf_node_ptr, **orig;
    char			*p;

    (*get_name_refs) (i, j, &p);

    for (leaf_node_ptr = *(orig = leafs_hd + (((long) p) % head_s_no)); leaf_node_ptr != NULL; leaf_node_ptr = leaf_node_ptr->link) {
	if (p == leaf_node_ptr->body.string_ptr)
	    return (leaf_node_ptr);
    }

    leaf_node_ptr = alloc_a_node ();
    leaf_node_ptr->name = leaf;
    leaf_node_ptr->link = *orig;
    leaf_node_ptr->body.string_ptr = p;
    return *orig = leaf_node_ptr;
}



static struct node	*create_op (op, l, r)
    int		op;
    struct node		*l, *r;
{
    register struct node	*p, **orig;

    for (p = *(orig = head_s + ((op + (((long) (l) ^ (long) (r)) >> 1)) % head_s_no)); p != NULL; p = p->link) {
	if (p->name == op && p->body.brother.left == l && p->body.brother.right == r)
	    return (p);
    }

    p = alloc_a_node ();
    p->name = op;
    p->body.brother.left = l;
    p->body.brother.right = r;
    p->link = *orig;
    return *orig = p;
}



static struct node	*create_list (p_s, m, create_list_M, op)
    register struct node	**p_s;
    int				m, create_list_M, op;
{
    register int		x;
    register struct node	*p, *q;

    x = create_list_M;

    for (p = p_s [x]; p == NULL; p = p_s [x]) {
	x--;
    }

    for (x--; x >= m; x--) {
	q = p_s [x];

	if (q != NULL)
	    p = create_op (op, q, p);
    }

    return (p);
}



static BOOLEAN	is_tail (p1, p2)
    struct node		*p1, *p2;
{
    /* Si p2 est une concatenation, regarde si le dernier composant est p1 */
    register struct node	*p;

    if (p2->name != concat)
	return (FALSE);

    for (p = p2; p->name == concat; p = p->body.brother.right) {
    }

    return (p == p1);
}



static int	nb_op_sons (p, op)
    struct node		*p;
    int		op;
{
    if (p->name != op)
	return (1);

    return (1 + nb_op_sons (p->body.brother.right, op));
}



static SXVOID	gather_op_sons (p_s, xp, p, op)
    struct node		**p_s;
    int		*xp, op;
    struct node		*p;
{
    if (p->name != op) {
	p_s [++*xp] = p;
    }
    else {
	p_s [++*xp] = p->body.brother.left;
	gather_op_sons (p_s, xp, p->body.brother.right, op);
    }
}


static SXVOID	process_substr (p_s, xstar, xconc, process_substr_M)
    register struct node	**p_s;
    int		xstar, xconc, process_substr_M;
{
    struct node		*star_son_ptr;
    int		d, y;

    star_son_ptr = p_s [xstar]->body.brother.left;

    if (star_son_ptr == p_s [xconc]) {
	p_s [xconc] = NULL;
	p_s [xstar] = canon (plus, star_son_ptr, NULL);
    }
    else if (star_son_ptr->name == concat) {
	y = nb_op_sons (star_son_ptr, concat);
	d = xconc - ((xconc < xstar) ? y : 1);

	if (xconc < xstar || d + y <= process_substr_M) {
	    register struct node	**q_s, **r_s /* 1:y */ ;
	    register int		i, j;
	    int				x;

	    q_s = tete_open (y);
	    x = 0;
	    gather_op_sons (q_s, &x, star_son_ptr, concat);

	    if (xconc < xstar) {
		for (i = y; i >= 1; i--, xconc--) {
		    if (xconc == 0 || q_s [i] != p_s [xconc]) {
			if (i == y) {
			    tete_pop (1);
			    return;
			}

			/* egalite entre i+1 .. y;normalisation
			...UW{VW}X... => ...U{WV}WX... */

			r_s = tete_open (y);

			for (j = 1; j <= y; j++) {
			    r_s [j - i + ((j <= i) ? y : 0)] = q_s [j];
			}

			xconc++;

			for (; xstar > xconc; xstar--) {
			    p_s [xstar] = p_s [xstar - 1];
			}

			p_s [xconc] = canon (star, create_list (r_s, 1, y, concat), NULL);
			tete_pop (2);
			process_substr (p_s, xconc, xconc + 1, process_substr_M);
			return;
		    }
		}
	    }
	    else {
		for (i = 1; i <= y; i++) {
		    if (q_s [i] != p_s [d + i]) {
			tete_pop (1);
			return;
		    }
		}
	    }

	    for (i = d + 1; i <= d + y; i++) {
		p_s [i] = NULL;
	    }

	    tete_pop (1);
	    p_s [xstar] = canon (plus, star_son_ptr, NULL);
	}
    }
}



static BOOLEAN	is_an_element (head_ptr, elem_ptr, oper)
    struct node		*head_ptr, *elem_ptr;
    int		oper;
{
    register struct node	*p;

    for (p = head_ptr; p->name == oper; p = p->body.brother.right) {
	if (p->body.brother.left == elem_ptr)
	    return (TRUE);
    }

    return (p == elem_ptr);
}



static BOOLEAN	is_a_subset (p_s, m1, M1, q_s, m2, M2)
    register struct node	**p_s, **q_s;
    int				m1, m2, M1, M2;
{
    register int		x1, x2;
    register struct node	*p2;

    x1 = m1 - 1;

    for (x2 = m2; x2 <= M2; x2++) {
	if ((p2 = q_s [x2]) != NULL) {
	    for (x1++; x1 <= M1; x1++) {
		if (p2 == p_s [x1])
		    break;
	    }

	    if (x1 > M1)
		return FALSE;
	}
    }

    return TRUE;
}



static BOOLEAN	is_a_member (p1, p2)
    struct node		*p1, *p2;
{
    /* Check if the left son of p1 is
	 - the left son of p2
	 - or a [sub-]alternative of p2 */
    register struct node		*p1_son, *p2_son;
    int		xp1, xp2;

    if ((p1_son = p1->body.brother.left) == (p2_son = p2->body.brother.left))
	return (TRUE);

    if (p2_son->name != or)
	return (FALSE);

    if (p1_son->name != or)
	return (is_an_element (p2_son, p1_son, or));

    if ((xp1 = nb_op_sons (p1_son, or)) > (xp2 = nb_op_sons (p2_son, or)))
	return (FALSE);
    
    {
	register struct node	**p1_s /* 1:xp1 */ , **p2_s /* 1:xp2 */ ;
	int		x;
	BOOLEAN	subsetp;
	
	p1_s = tete_open (xp1);
	p2_s = tete_open (xp2);
	x = 0;
	gather_op_sons (p1_s, &x, p1_son, or);
	x = 0;
	gather_op_sons (p2_s, &x, p2_son, or);
	subsetp = is_a_subset (p2_s, 1, xp2, p1_s, 1, xp1);
	tete_pop (2);
	return subsetp;
    }
}



static BOOLEAN	is_a_staror_elem (node_ptr, result_ptr)
    struct node		*node_ptr, **result_ptr;
{
    int			concat_no;

    *result_ptr = NULL;

    if (node_ptr->name != concat)
	return FALSE;

    concat_no = nb_op_sons (node_ptr, concat);

    {
	register struct node	**p_s /* 1:concat_no */, *or_ptr, *p ;
	int	x, or_no;

	p_s = tete_open (concat_no);
	x = 0;
	gather_op_sons (p_s, &x, node_ptr, concat);
	p = p_s [concat_no];

	if (p->name != star) {
	    tete_pop (1);
	    return (FALSE);
	}

	or_ptr = p->body.brother.left;

	if (or_ptr->name != or) {
	    tete_pop (1);
	    return (FALSE);
	}

	p = create_list (p_s, 1, concat_no - 1, concat);
	or_no = nb_op_sons (or_ptr, or);

	{
	    register struct node	**q_s /* 1:or_no */ ;

	    q_s = tete_open (or_no);
	    x = 0;
	    gather_op_sons (q_s, &x, or_ptr, or);

	    for (x = 1; x <= or_no; x++) {
		if (q_s [x] == p) {
		    q_s [x] = NULL;
		    goto next;
		}
	    }

	    tete_pop (2);
	    return (FALSE);

next:	    *result_ptr = create_op (concat, p, canon (star, create_list (q_s, 1, or_no, or), NULL));
	    tete_pop (2);
	    return (TRUE);
	}
    }
}



static BOOLEAN	is_a_sublanguage (left, right)
    struct node		*left, *right;
{
    register struct node	*left_son, *right_son;
    int				or_no, concat_no;

    left_son = left->body.brother.left;
    right_son = right->body.brother.left;

    if (left_son->name != or || right_son->name != concat)
	return (FALSE);

    or_no = nb_op_sons (left_son, or);
    concat_no = nb_op_sons (right_son, concat);

    {
	struct node		**or_son_ptrs, **concat_son_ptrs;
	register struct node	*p;
	int	x, cn;
	BOOLEAN		elementp;

	or_son_ptrs = tete_open (or_no);
	concat_son_ptrs = tete_open (concat_no);
	x = 0;
	gather_op_sons (or_son_ptrs, &x, left_son, or);
	x = 0;
	gather_op_sons (concat_son_ptrs, &x, right_son, concat);

	for (x = concat_no; x >= 1; x--) {
	    p = concat_son_ptrs [x];

	    if (((p->name != star) && (p->name != option)) || !is_an_element (left_son, p->body.brother.left, or))
		break;
	}

	if (x == concat_no) {
	    tete_pop (2);
	    return (FALSE);
	}

	if (x == 0) {
	    tete_pop (2);
	    return (TRUE);
	}

	if (x == 1) {
	    p = concat_son_ptrs [1];

	    if (p->name == or) {
		cn = nb_op_sons (p, or);

		{
		    struct node		**c_s /* 1:cn */ ;
		    int		y;
		    BOOLEAN	subsetp;

		    c_s = tete_open (cn);
		    y = 0;
		    gather_op_sons (c_s, &y, p, or);
		    subsetp = is_a_subset (or_son_ptrs, 1, or_no, c_s, 1, cn);
		    tete_pop (3);
		    return subsetp;
		}
	    }
	    else {
		elementp = is_an_element (left_son, p, or);
		tete_pop (2);
		return elementp;
	    }
	}
	else {
	    /* x>1 */
	    elementp = is_an_element (left_son, create_list (concat_son_ptrs, 1, x, concat), or);
	    tete_pop (2);
	    return elementp;
	}
    }
}



static SXVOID	transform (left, right)
    struct node		**left, **right;
{
    register struct node	*left_son, *right_son;
    int				cn1, cn2;

    left_son = (*left)->body.brother.left;
    right_son = (*right)->body.brother.left;

    if (left_son->name != concat || right_son->name != concat)
	return;

    cn1 = nb_op_sons (left_son, concat);
    cn2 = nb_op_sons (right_son, concat);

    {
	register struct node	**c1_s /* 1:cn1 */ , **c2_s /* 1:cn2 */ ;
	int	x, y;
	struct node	*p, *or_ptr;

	c2_s = tete_open (cn2);
	x = 0;
	gather_op_sons (c2_s, &x, right_son, concat);

	if (c2_s [cn2]->name != star) {
	    tete_pop (1);
	    return;
	}

	or_ptr = c2_s [cn2]->body.brother.left;

	if (or_ptr->name != or) {
	    tete_pop (1);
	    return;
	}

	c1_s = tete_open (cn1);
	x = 0;
	gather_op_sons (c1_s, &x, left_son, concat);

	if (c1_s [cn1]->name != star) {
	    tete_pop (2);
	    return;
	}

	for (x = cn1 - 1; x >= 1; x--) {
	    p = c1_s [x];

	    if (p->name != star)
		break;
	}

	if (x == 0) {
	    tete_pop (2);
	    return;
	}


/* garde_fou */

	if (nb_op_sons (or_ptr, or) != cn1 - x + 1) {
            tete_pop (2);
	    return;
	}

	for (y = x + 1; y <= cn1; y++) {
	    if (!is_an_element (or_ptr, c1_s [y]->body.brother.left, or)) {
		tete_pop (2);
		return;
	    }
	}

	p = create_list (c1_s, 1, x, concat);

	if (!is_an_element (or_ptr, p, or)) {
	    tete_pop (2);
	    return;
	}

	*right = NULL;
	c1_s [x] = canon (or, p, create_list (c2_s, 1, cn2 - 1, concat));
	*left = canon (star, create_list (c1_s, x, cn1, concat), NULL);
	tete_pop (2);
    }
}



static BOOLEAN	less (i, j)
    int		i, j;
{
    return (long) (P_S [i]) < (long) (P_S [j]);
}



static struct node	*create_or_list (p_s, m, create_or_list_M)
    struct node		**p_s;
    int		m, create_or_list_M;
{
    register int	x;
    struct node		*v, *w;

    P_S = p_s;

    if (to_be_sorted == NULL)
	to_be_sorted = (int*) sxalloc ((size_of_to_be_sorted = create_or_list_M) + 1, sizeof (int));
    else if (create_or_list_M > size_of_to_be_sorted)
	to_be_sorted = (int*) sxrealloc (to_be_sorted, (size_of_to_be_sorted = create_or_list_M) + 1, sizeof (int));
    
    for (x = 1; x <= create_or_list_M; x++) {
	to_be_sorted [x] = x;
    }

    sort_by_tree (to_be_sorted, m, create_or_list_M, less);
    /* Suppression des alternatives identiques et construction du nouvel arbre */
    v = p_s [create_or_list_M];

    for (x = create_or_list_M - 1; x >= m; x--) {
	w = p_s [x];

	if (v != w)
	    v = w;
	else
	    p_s [x] = NULL;
    }

    return (create_list (p_s, m, create_or_list_M, or));
}



static struct node	*canon (oper, left, right)
    int		oper;
    struct node		*left, *right;
{
    int		concat_no, or_no, left_name, cn;
    struct node		*p;

    switch (oper) {
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("fsa_to_re","unknown switch case #1");
#endif
      break;
    case leaf:
	/* leaf (error) */
	return NULL;

    case concat:
	/* op = concat */
/* On canonicalise */
	concat_no = nb_op_sons (left, concat) + nb_op_sons (right, concat);

	{
	    register struct node	**p_s /* 1:concat_no */ ;
	    int		x, xp, l_name, r_name, canon_cn;
	    struct node		*l_ptr, *r_ptr, *rl_ptr;

	    p_s = tete_open (concat_no);
	    x = 0;
	    gather_op_sons (p_s, &x, left, concat);
	    xp = x;
	    gather_op_sons (p_s, &x, right, concat);
	    l_ptr = p_s [xp];
	    r_ptr = p_s [xp + 1];
	    l_name = l_ptr->name;
	    r_name = r_ptr->name;

	    switch (S [l_name] [r_name]) {
	    case 0:
		/* error */
		fprintf (sxstderr, "Constructor error in fsa_to_re: unexpected node operator.");
		tete_pop (1);
		exit (1);

	    case 1:
		/* general case */
		break;

	    case 2:
		/* XYZ{YZ}V => X{YZ}+V */
		process_substr (p_s, xp + 1, xp, concat_no);
		break;

	    case 3:
		/* V{XY}XYZ => V{XY}+Z */
		process_substr (p_s, xp, xp + 1, concat_no);
		break;

	    case 4:
		/* ...{X}{Y}... */
		if (is_a_member (l_ptr, r_ptr) /* {U} {U|V} => {U|V} */ )
		    p_s [xp] = NULL;
		else if (is_a_member (r_ptr, l_ptr) /* {U|V} {U} => {U|V} */ )
		    p_s [xp + 1] = NULL;
		else if (is_tail (l_ptr, r_ptr->body.brother.left)
			 /* {U} {V{U}} => {U|V} */
								  ) {
		    rl_ptr = r_ptr->body.brother.left;
		    canon_cn = nb_op_sons (rl_ptr, concat);

		    {
			register struct node	**q_s /* 1:canon_cn */ ;
			int	canon_x;

			q_s = tete_open (canon_cn);
			canon_x = 0;
			gather_op_sons (q_s, &canon_x, rl_ptr, concat);
			p_s [xp] = NULL;
			p_s [xp + 1] = create_op (star, canon (or, l_ptr->body.brother.left, create_list (q_s, 1, canon_cn - 1,
			     concat)), NULL);
			tete_pop (1);
		    }
		}
		else if (is_tail (r_ptr, l_ptr->body.brother.left)
			 /* {U{V}}{V} => {U{V}} */
								  )
		    p_s [xp + 1] = NULL;
		else if (is_a_sublanguage (l_ptr, r_ptr)
			 /* {X|Y|Z|T}{X[Z]{T}} => {X|Y|Z|T} */
							)
		    p_s [xp + 1] = NULL;
		else
		    transform (&(p_s [xp]), &(p_s [xp + 1]));
		/* {X{Y}{Z}}{T{X|Y|Z}} => {(X|T){Y}{Z}} */

		break;

	    case 5:
		/* [X] {Y} or {X} {Y}+ or [X] {Y}+ */
		if (is_a_member (l_ptr, r_ptr))
		    p_s [xp] = NULL;

		break;

	    case 6:
		/* {X} [Y] or {X}+ {Y} or {X}+ [Y] */
		if (is_a_member (l_ptr, r_ptr))
		    p_s [xp + 1] = NULL;

		break;
	    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
	      sxtrap("fsa_to_re","unknown switch case #2");
#endif
	      break;
	    }

	    l_ptr = create_list (p_s, 1, concat_no, concat);
	    tete_pop (1);
	    return l_ptr;
	}

    case star:
	/* star */
	left_name = left->name;

	if (left_name == star /* {{X}} => {X} */ )
	    return (left);

	if (left_name == option /* {[X]} => {X} */  || left_name == plus
	    /* {{X}+} => {X} */
									)
	    return (create_op (star, left->body.brother.left, NULL));

	if (is_a_staror_elem (left, &p) /* {X{X|Y}} => {X{Y}} */ )
	    return (canon (star, p, NULL));

	if (left_name == concat) {
	    /* {[X] {Y}} => {X|Y} */
	    cn = nb_op_sons (left, concat);

	    {
		register struct node	**q_s /* 1:cn */ ;
		int	x;
		struct node	*l_ptr;

		q_s = tete_open (cn);
		x = 0;
		gather_op_sons (q_s, &x, left, concat);

		for (x = 1; x <= cn; x++) {
		    if (q_s [x]->name == star || q_s [x]->name == option)
			q_s [x] = q_s [x]->body.brother.left;
		    else {
			tete_pop (1);
			return (create_op (oper, left, right));
		    }
		}

		l_ptr = create_op (star, create_or_list (q_s, 1, cn), NULL);
		tete_pop (1);
		return l_ptr;
	    }
	}

	break;

    case or:
	/* or */
	{
	    register struct node	**p_s /* 1:or_no */ ;
	    struct node		*l_ptr;
	    int		x;
	    BOOLEAN is_option = FALSE;

	    /* [X]|Y or X|[Y] or [X]|[Y] => [X|Y] */
	    if (left->name == option) {
		left = left->body.brother.left;
		is_option = TRUE;
	    }

	    if (right->name == option) {
		right = right->body.brother.left;
		is_option = TRUE;
	    }

	    or_no = nb_op_sons (left, or) + nb_op_sons (right, or);
	    p_s = tete_open (or_no);
	    x = 0;
	    gather_op_sons (p_s, &x, left, or);
	    gather_op_sons (p_s, &x, right, or);
	    l_ptr = create_or_list (p_s, 1, or_no);
	    tete_pop (1);
	    return is_option ? create_op (option, l_ptr, NULL) : l_ptr;
	}

    case plus:
	/* plus */
	left_name = left->name;

	if (left_name == plus /* {{X}+}+ => {X}+ */  || left_name == star
	    /* {{X}}+ => {X} */
									 )
	    return (left);

	if (left_name == option /* {[X]}+ => {X} */ )
	    return (create_op (star, left->body.brother.left, NULL));

	break;

    case option:
	/* option */
	left_name = left->name;

	if (left_name == star /* [{X}] => {X} */  || left_name == option
	    /* [[X]] => [X] */
									)
	    return (left);

	if (left_name == plus /* [{X}+] => {X} */ )
	    return (create_op (star, left->body.brother.left, NULL));

	if (is_a_staror_elem (left, &p) /* [X{X|Y}] => {X{Y}} */ )
	    return (canon (star, p, NULL));

	break;
    }

    return (create_op (oper, left, right));
}



static int	get_a_pred (n)
    int		n;
{
    register int	i = -1;

    while ((i = sxba_scan (to_be_processed, i)) >= 0) {
	if (SXBA_bit_is_set (M [i], n))
	    break;
    }

    return i;
}



static SXVOID	gen_a_loop (pred, next, loop)
    int		pred, next, loop;
{
    /* Il y a une boucle elementaire sur l'etat loop
soit t=trans(pred,next) et l=trans(loop,loop)
si t=l alors 
    si pred=next=loop alors trans(pred,next)={t}
    sinon trans(pred,next)={t}+
sinon si next=loop alors trans(pred,next)=t {l}
      sinon si pred=loop alors trans(pred,next)= {l} t
            sinon erreur
*/
    struct node		*t, *l;

    if ((l = re_ptrs [loop] [loop]) == (t = re_ptrs [pred] [next]))
	if (pred == next && next == loop) {
	    re_ptrs [pred] [next] = canon (star, l, NULL);
	}
	else {
	    re_ptrs [pred] [next] = canon (plus, l, NULL);
	}
    else if (next == loop)
	re_ptrs [pred] [next] = canon (concat, t, canon (star, l, NULL));
    else
	re_ptrs [pred] [next] = canon (concat, canon (star, l, NULL), t);
}


static SXVOID	gen_splits (pred, next)
    int		pred, next;
{
    /* Calcule les chemins entre pred et succ(next)*/
    register int		succ;
    register struct node	*p1, *p2, *q;

    if (succ_set == NULL)
	succ_set = sxba_calloc (SIZE);

    sxba_copy (succ_set, M [next]);
    /* M [next] est peut etre change dans la boucle.. */
    p1 = re_ptrs [pred] [next];

    succ = -1;

    while ((succ = sxba_scan (succ_set, succ)) >= 0) {
	p2 = re_ptrs [next] [succ];

	if (SXBA_bit_is_set (M [pred], succ)) {
	    /* Il y a deja une ER */
	    if ((q = re_ptrs [pred] [succ]) == p2)
		re_ptrs [pred] [succ] = canon (concat, canon (option, p1, NULL), q);
	    else
		re_ptrs [pred] [succ] = canon (or, canon (concat, p1, p2), q);
	}
	else {
	    re_ptrs [pred] [succ] = canon (concat, p1, p2);
	    SXBA_1_bit (M [pred], succ);
	    ++(out [pred]);
	    ++(in [succ]);
	}

	if (pred == succ)
	    if (pred != INITIAL_S)
		SXBA_1_bit (loop_set, pred);
    }
}



static VOID	erase (pred, next)
    int		pred, next;
{
    /* Supprime la transition entre pred et next */
    SXBA_0_bit (M [pred], next);
    --(out [pred]);
    --(in [next]);
}



static int	get_current_node (get_current_node_to_be_processed)
register SXBA	get_current_node_to_be_processed;
{
    register int	s, i, n, cur;
    int			e;

    s = e = SIZE;
    cur = -1;
    i = -1;

    while ((i = sxba_scan (get_current_node_to_be_processed, i)) >= 0) {
	if ((n = in [i]) < e) {
	    cur = i;
	    e = n;
	}
	else if (n == e)
	    if ((n = out [i]) < s) {
		cur = i;
		s = n;
	    }
    }

    return (cur);
}


static	void chose_larger_set ();
static struct node	*process (p_s, set, or_no, min_s, MAX_s)
    struct node		***p_s;
    SXBA	set;
    int		or_no;
    int		*min_s, *MAX_s;
{
    register SXBA	/* or_no */ pre_set, post_set, middle_set;
    int		pre_card, post_card;
    struct node		*pre_p, *post_p, *ret_ptr;

    pre_set = sets_open (or_no);
    post_set = sets_open (or_no);
    middle_set = sets_open (or_no);
    chose_larger_set (p_s, set, min_s, &pre_card, pre_set, &pre_p, or_no);
    chose_larger_set (p_s, set, MAX_s, &post_card, post_set, &post_p, or_no);

    if (pre_card >= post_card) {
	sxba_minus (sxba_copy (middle_set, set), pre_set);
	pre_p = gen_prefixe (p_s, pre_set, pre_p, or_no, min_s, MAX_s);

	if (!sxba_is_empty (middle_set))
	    ret_ptr = canon (or, pre_p, process (p_s, middle_set, or_no, min_s, MAX_s));
	else
	    ret_ptr = pre_p;
    }
    else {
	sxba_minus (sxba_copy (middle_set, set), post_set);
	post_p = gen_suffixe (p_s, post_set, post_p, or_no, min_s, MAX_s);

	if (!sxba_is_empty (middle_set))
	    ret_ptr = canon (or, process (p_s, middle_set, or_no, min_s, MAX_s), post_p);
	else
	    ret_ptr = post_p;
    }

    sets_pop (3);
    return ret_ptr;
}



static struct node	*factorize (or_ptr)
    struct node		*or_ptr;
{
    int		or_no;

    or_no = nb_op_sons (or_ptr, or);

    {
	register int	*min_s, *MAX_s /* 1:or_no */ ;
	register int	i;
	int		concat_no, j;
	register struct node	*p;

	min_s = ints_open (or_no);
	MAX_s = ints_open (or_no);
	i = concat_no = 0;

	for (p = or_ptr; p->name == or; p = p->body.brother.right) {
	    min_s [++i] = 1;
	    MAX_s [i] = j = nb_op_sons (p->body.brother.left, concat);
	    concat_no = (j > concat_no) ? j : concat_no;
	}

	min_s [++i] = 1;
	MAX_s [i] = j = nb_op_sons (p, concat);
	concat_no = (j > concat_no) ? j : concat_no;

	{
	    register struct node		***p_s /* 1:or_no, 1:concat_no */ ;
	    SXBA	/* or_no */ set;
	    int		x;
	    struct node		*ret_ptr;

	    p_s = (struct node***) sxcalloc ((unsigned long int) (or_no + 1), sizeof (struct node**));
	    p_s [0] = NULL;

	    for (i = 1; i <= or_no; i++)
		p_s [i] = tete_open (concat_no);

	    i = 0;

	    for (p = or_ptr; p->name == or; p = p->body.brother.right) {
		x = 0;
		gather_op_sons (p_s [++i], &x, p->body.brother.left, concat);
	    }

	    x = 0;
	    gather_op_sons (p_s [++i], &x, p, concat);
	    set = sets_open (or_no);
	    sxba_fill (set), SXBA_0_bit (set, 0);
	    ret_ptr = process (p_s, set, or_no, min_s, MAX_s);
	    sets_pop (1);
	    tete_pop (or_no);
	    sxfree (p_s);
	    ints_pop (2);
	    return ret_ptr;
	}
    }
}



static struct node	*reduce (visited)
    struct node		*visited;
{
    switch (visited->name) {
    case or:
	return (factorize (canon (or, reduce (visited->body.brother.left), reduce_or_son (visited->body.brother.right))))
	     ;

    case concat:
	return (canon (concat, reduce (visited->body.brother.left), reduce (visited->body.brother.right)));

    case star:
    case plus:
    case option:
	return (canon (visited->name, reduce (visited->body.brother.left), NULL));

    case leaf:
	return (visited);
    default:
      return (struct node*)NULL; /* Pour compilos tatillons... */
    }
}



static struct node	*reduce_or_son (visited)
    struct node		*visited;
{
    if (visited->name == or)
	return (canon (or, reduce (visited->body.brother.left), reduce_or_son (visited->body.brother.right)));
    else
	return (reduce (visited));
}



static	void chose_larger_set (p_s, set, mM, best_card, best_set, best_p, or_no)
    struct node		***p_s /* 1:or_no, 1:concat_no */ ;
    SXBA	set, best_set;
    int		*mM;
    int		*best_card, or_no;
    struct node		**best_p;
{
    register SXBA	/* or_no */ working_set, c_set;
    register int	i, j, c_card;
    struct node		*c_p;

    working_set = sets_open (or_no);
    c_set = sets_open (or_no);
    sxba_copy (working_set, set);
    *best_card = 0;
    *best_p = NULL;
    sxba_empty (best_set);
    i = 0;

    while ((i = sxba_scan_reset (working_set, 0)) > 0) {
	sxba_empty (c_set);
	SXBA_1_bit (c_set, i);
	c_card = 1;
	c_p = p_s [i] [mM [i]];
	j = i;

	while ((j = sxba_scan (working_set, j)) >= 0) {
	    if (c_p == p_s [j] [mM [j]]) {
		SXBA_1_bit (c_set, j);
		c_card++;
		SXBA_0_bit (working_set, j);
	    }
	}

	if (c_card > *best_card) {
	    *best_card = c_card;
	    *best_p = c_p;
	    sxba_copy (best_set, c_set);
	}
    }

    sets_pop (2);
}



static struct node	*gen_prefixe (p_s, set, node_ptr, or_no, min_s, MAX_s)
    struct node		***p_s;
    SXBA	set;
    struct node		*node_ptr;
    int		or_no;
    int		*min_s, *MAX_s;
{
    register int		i, j;
    BOOLEAN	is_option, are_the_same;
    struct node		*c_ptr, *ret_ptr;
    register SXBA	/* or_no */ c_set;

    c_set = sets_open (or_no);
    is_option = FALSE;
    sxba_copy (c_set, set);
    i = sxba_scan (set, 0); /* Element 0 non utilise. */
    j = ++(min_s [i]);

    if (j > MAX_s [i]) {
	is_option = TRUE;
	SXBA_0_bit (c_set, i);
	c_ptr = NULL;
    }
    else
	c_ptr = p_s [i] [j];

    are_the_same = TRUE;

    while ((i = sxba_scan (c_set, i)) >= 0) {
	j = ++(min_s [i]);

	if (j > MAX_s [i]) {
	    is_option = TRUE;
	    SXBA_0_bit (c_set, i);
	}

	if (c_ptr != p_s [i] [j])
	    are_the_same = FALSE;
    }

    if (is_option) {
	if (!sxba_is_empty (c_set))
	    ret_ptr = canon (concat, node_ptr, canon (option, process (p_s, c_set, or_no, min_s, MAX_s), NULL));
	else
	    ret_ptr = node_ptr;
    }
    else if (are_the_same)
	ret_ptr = canon (concat, node_ptr, gen_prefixe (p_s, set, c_ptr, or_no, min_s, MAX_s));
    else
	ret_ptr = canon (concat, node_ptr, process (p_s, c_set, or_no, min_s, MAX_s));

    sets_pop (1);
    return ret_ptr;
}



static struct node	*gen_suffixe (p_s, set, node_ptr, or_no, min_s, MAX_s)
    struct node		***p_s;
    SXBA	set;
    struct node		*node_ptr;
    int		or_no;
    int		*min_s, *MAX_s;
{
    register int	i, j;
    BOOLEAN	is_option, are_the_same;
    struct node		*c_ptr, *ret_ptr;
    register SXBA	/* or_no */ c_set;

    c_set = sets_open (or_no);
    is_option = FALSE;
    sxba_copy (c_set, set);
    i = sxba_scan (set, -1);
    j = --(MAX_s [i]);

    if (j < min_s [i]) {
	is_option = TRUE;
	SXBA_0_bit (c_set, i);
	c_ptr = NULL;
    }
    else
	c_ptr = p_s [i] [j];

    are_the_same = TRUE;

    while ((i = sxba_scan (c_set, i)) >= 0) {
	j = --(MAX_s [i]);

	if (j < min_s [i]) {
	    is_option = TRUE;
	    SXBA_0_bit (c_set, i);
	}

	if (c_ptr != p_s [i] [j])
	    are_the_same = FALSE;
    }

    if (is_option) {
	if (!sxba_is_empty (c_set))
	    ret_ptr = canon (concat, canon (option, process (p_s, c_set, or_no, min_s, MAX_s), NULL), node_ptr);
	else
	    ret_ptr = node_ptr;
    }
    else if (are_the_same)
	ret_ptr = canon (concat, gen_suffixe (p_s, set, c_ptr, or_no, min_s, MAX_s), node_ptr);
    else
	ret_ptr = canon (concat, process (p_s, c_set, or_no, min_s, MAX_s), node_ptr);

    sets_pop (1);
    return ret_ptr;
}




static struct node	*cut (node_ptr)
    struct node		*node_ptr;
{
    if (node_ptr == NULL) {
	/* sous arbre deja coupe par ailleurs */
	return (struct node*)NULL;
    }

    switch (node_ptr->name) {
    case leaf:
	return (*(node_ptr->body.string_ptr) == NUL) ? (NULL) : (node_ptr);

    case concat:
    case or:
	node_ptr->body.brother.left = cut (node_ptr->body.brother.left);
	node_ptr->body.brother.right = cut (node_ptr->body.brother.right);

	return (node_ptr->body.brother.left == NULL && node_ptr->body.brother.right == NULL) ? (NULL) : (node_ptr);

    case star:
    case plus:
    case option:
	node_ptr->body.brother.left = cut (node_ptr->body.brother.left);

	return (node_ptr->body.brother.left == NULL) ? (NULL) : (node_ptr);
    default:
      return (struct node*)NULL; /* Pour compilos tatillons... */
    }
}



static VARSTR	tree_to_string (varstr_ptr, node_ptr)
    VARSTR	varstr_ptr;
    struct node		*node_ptr;
{
  register VARSTR	ret_ptr;
  struct node           *left, *right;

  if (node_ptr == NULL)
    return (varstr_ptr);

  switch (node_ptr->name) {
  case leaf:
    ret_ptr = varstr_catenate (varstr_ptr, node_ptr->body.string_ptr);
    break;

  case concat:
    /* version du 17/01/06 */
    left = node_ptr->body.brother.left;
    right = node_ptr->body.brother.right;

    if (left) {
      if (left->name == or)
	varstr_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr,
								       "("),
						      left),
				      ")");
      else
	varstr_ptr = tree_to_string (varstr_ptr, left);

      if (right)
	varstr_ptr = varstr_catchar (varstr_ptr, ' ');
    }

    if (right) {
      if (right->name == or)
	varstr_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr,
								       "("),
						      right),
				      ")");
      else
	varstr_ptr = tree_to_string (varstr_ptr, right);
    }

    return varstr_ptr;
      
#if 0
    /* ancienne version */
    if (node_ptr->body.brother.left == NULL)
      ret_ptr = tree_to_string (varstr_ptr, node_ptr->body.brother.right);
    else if (node_ptr->body.brother.right == NULL)
      ret_ptr = tree_to_string (varstr_ptr, node_ptr->body.brother.left);
    else if (node_ptr->body.brother.left->name == or)
      if (node_ptr->body.brother.right->name == or)
	ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr,
												     "("), node_ptr->body.brother.left), ") ("), node_ptr->body.brother.right), ")");
      else
	ret_ptr = tree_to_string (varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, "("), node_ptr->
								   body.brother.left), ") "), node_ptr->body.brother.right);
    else if (node_ptr->body.brother.right->name == or)
      ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (tree_to_string (varstr_ptr, node_ptr->body.
										  brother.left), " ("), node_ptr->body.brother.right), ")");
    else
      ret_ptr = tree_to_string (varstr_catenate (tree_to_string (varstr_ptr, node_ptr->body.brother.left), " "),
				node_ptr->body.brother.right);
#endif /* 0 */

    /* NOTREACHED break; */

  case star:
    ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, "{"), node_ptr->body.brother.left), "}");
    break;

  case or:
    if (node_ptr->body.brother.left == NULL)
      ret_ptr = tree_to_string (varstr_ptr, node_ptr->body.brother.right);
    else if (node_ptr->body.brother.right == NULL)
      ret_ptr = tree_to_string (varstr_ptr, node_ptr->body.brother.left);
    else
      ret_ptr = tree_to_string (varstr_catenate (tree_to_string (varstr_ptr, node_ptr->body.brother.left), " | "),
				node_ptr->body.brother.right);

    break;

  case plus:
    ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, "{"), node_ptr->body.brother.left), "}+")
      ;
    break;

  case option:
    ret_ptr = varstr_catenate (tree_to_string (varstr_catenate (varstr_ptr, "["), node_ptr->body.brother.left), "]");
    break;
  default: /* pour faire taire gcc -Wswitch-default */
    sxinitialise(ret_ptr); /* pour faire taire gcc -Wuninitialized */			      
#if EBUG
    sxtrap("fsa_to_re","unknown switch case #3");
#endif
    break;
  }

  return ret_ptr;
}




VARSTR	fsa_to_re (varstr_ptr, R, R_plus, size, initial_s, final_s, get_name_refs)
    VARSTR	varstr_ptr;
    SXBA	*R, *R_plus;
    int		size, initial_s, final_s;
    int		(*get_name_refs) ();
{
    register int	i, j;
    register SXBA	and_set;
    struct node		**re_ptrs_base;

/* Cette fonction transforme l'ensemble des chemins entre initial_s et final_s
   de l'automate R en une expression reguliere. Le nom de la transition
   entre l'etat i et l'etat j est donne par la procedure
   utilisateur get_name_refs (i, j, string_ptr)
*/
/* Les etats de l'automate sont dans [0..size]. */
 /* Automate sous forme de vecteur de bits et sa fermeture transitive */

    if (!SXBA_bit_is_set (R_plus [initial_s], final_s))
	return varstr_ptr;

    SIZE = size + 1;
    INITIAL_S = initial_s;
    tree_area_ptr = NULL;
    cur_node_no = node_number_per_area;
    M = sxbm_calloc (SIZE, SIZE);
    re_ptrs = (struct node***) sxalloc ((int) (SIZE), sizeof (struct node**));

    {
	register struct node	**p;

	re_ptrs_base = p = (struct node**) sxcalloc ((unsigned long int) (SIZE * SIZE), sizeof (struct node*));

	for (i = 0; i <= size; i++) {
	    re_ptrs [i] = p;
	    p += SIZE;
	}
    }

    in = (int*) sxcalloc ((unsigned long int) SIZE, sizeof (int));
    out = (int*) sxcalloc ((unsigned long int) SIZE, sizeof (int));
    ints = NULL;
    sets = NULL;
    to_be_processed = sets_open (size);
    loop_set = sets_open (size);
    and_set = sets_open (size);
    tete = NULL;
    to_be_sorted = NULL;
    succ_set = NULL;

    for (i = 0; i < head_s_no; i++)
	leafs_hd [i] = head_s [i] = NULL;

    SXBA_1_bit (to_be_processed, initial_s);
    SXBA_1_bit (to_be_processed, final_s);

    for (i = 0; i <= size; i++) {
	if (SXBA_bit_is_set (R_plus [initial_s], i) && SXBA_bit_is_set (R_plus [i], final_s))
	    SXBA_1_bit (to_be_processed, i);
    }

    i = -1;

    while ((i = sxba_scan (to_be_processed, i)) >= 0) {
	sxba_and (sxba_copy (M [i], R [i]), to_be_processed);
	reached_set = M [i];
	j = -1;

	while ((j = sxba_scan (reached_set, j)) >= 0) {
	    re_ptrs [i] [j] = create_leaf (i, j, get_name_refs);
	    (out [i])++;
	    (in [j])++;

	    if (i == j)
		SXBA_1_bit (loop_set, i);
	}
    }

    SXBA_0_bit (loop_set, initial_s);

    sxba_copy (and_set, to_be_processed);
    SXBA_0_bit (and_set, initial_s);
    SXBA_0_bit (and_set, final_s);

    while (!sxba_is_empty (and_set)) {
	i = -1;

	while ((i = sxba_scan_reset (loop_set, i)) >= 0) {
	    j = -1;

	    while ((j = sxba_scan (to_be_processed, j)) >= 0) {
		if (SXBA_bit_is_set (M [j], i) && i != j)
		    gen_a_loop (j, i, i);
	    }

	    erase (i, i);
	}

	current = get_current_node (and_set);
	prev = get_a_pred (current);
	gen_splits (prev, current);
	erase (prev, current);

	if (in [current] == 0) {
	    SXBA_0_bit (to_be_processed, current);
	    out [current] = 0;
	    reached_set = M [current];
	    i = -1;

	    while ((i = sxba_scan (reached_set, i)) >= 0)
		(in [i])--;

	    sxba_empty (M [current]);
	}

	sxba_copy (and_set, to_be_processed);
	SXBA_0_bit (and_set, initial_s);
	SXBA_0_bit (and_set, final_s);
    }


/* Cas particulier de boucle sur l'etat final */

    if (SXBA_bit_is_set (M [final_s], final_s)) {
	gen_a_loop (initial_s, final_s, final_s);
	erase (final_s, final_s);
    }


/* Cas particulier de la transition de l'etat final a l'etat initial */

    if (SXBA_bit_is_set (M [final_s], initial_s))
	gen_splits (initial_s, final_s);


/* Cas particulier de boucle sur l'etat initial */

    if (SXBA_bit_is_set (M [initial_s], initial_s)) {
	gen_a_loop (initial_s, final_s, initial_s);
	erase (initial_s, initial_s);
    }


/* Factorisation */

    re_ptrs [initial_s] [final_s] = reduce (re_ptrs [initial_s] [final_s]);
    /* on supprime les sous arbres ne produisant que la chaine vide */
    re_ptrs [initial_s] [final_s] = cut (re_ptrs [initial_s] [final_s]);
    varstr_ptr = tree_to_string (varstr_ptr, re_ptrs [initial_s] [final_s]);
    sxfree (out);
    sxfree (in);
    tete_free ();
    sets_free ();
    ints_free ();

    if (to_be_sorted != NULL)
	sxfree (to_be_sorted);

    if (succ_set != NULL)
	sxfree (succ_set);

    sxfree (re_ptrs_base);
    sxfree (re_ptrs);
    sxbm_free (M);
    free_tree_areas ();
    return varstr_ptr;
}
