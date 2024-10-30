/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 30-04-92 15:25 (pb):		Utilise egalement pour generer le	*/
/*				non-determinisme.			*/
/************************************************************************/
/* 24-08-88 18:53 (bl):		Ajout de #ifdef MSDOS			*/
/************************************************************************/
/* 28-03-88 09:55 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT_XPRDCT_MNGR[] = "@(#)xprdct_mngr.c\t- SYNTAX [unix] - 30 avril 1992";

static char	ME [] = "xprdct_mngr";

/*   I N C L U D E S   */
#include "P_tables.h"

#include "rlr_optim.h"

struct xprdct {
    int		lnk, hash, size, head;
};
struct node {
    int			brother, son;
    int			prdct;
    struct floyd_evans	*fe;
};
static struct node	*tree;
static int	tree_size, xnode;
static struct node	empty_node;
static	xprdct_hash [HASH_SIZE];
static int	*prdcts_seq;
static int	xprdcts_size, ind_xprdcts, prdcts_seq_size, ind_prdcts_seq;
static struct xprdct	*xprdcts;


#define error_node 1


static int	install_prdct_seq (predicates, prdcts, hashs, lnks, xpredicates, hash_code)
    struct state	*predicates;
    struct P_prdct	*prdcts;
    int		*hashs, *lnks, hash_code;
    register int	xpredicates;
{

/* Look wether the sequence of predicates item started at "xpredicates" in "predicates"
   is already defined in "predicates" at another index.
   Returns this index if the answer is yes otherwise xpredicates.
   The length of the input sequence is in "predicates [xpredicates].lgth".
   This sequence starts at  "predicates [xpredicates].start" in "prdcts".
   "lnks [xpredicates]" is used to hold the link between sequences with
   same hash_code. New sequences are sorted by decreasing values of the field
   "prdct" in "prdcts". */


    register struct P_prdct	*aprdct, *xaprdct, *lim, *start;
    register int	lgth, hd, prev;

    lgth = predicates [xpredicates].lgth;
    lim = (start = prdcts + predicates [xpredicates].start) + lgth;

    for (hd = hashs [hash_code], prev = 0; hd != 0; hd = lnks [prev = hd]) {
	if (predicates [hd].lgth == lgth) {
	    for (aprdct = start, xaprdct = prdcts + predicates [hd].start; aprdct < lim; aprdct++, xaprdct++) {
		if (xaprdct->prdct != aprdct->prdct || xaprdct->action != aprdct->action)
		    break;
	    }

	    if (aprdct == lim)
		return hd;
	}
    }


/* New sequence */

    lnks [xpredicates] = 0 /* link tail */ ;

    if (prev == 0)
	/* new bucket */
	hashs [hash_code] = xpredicates;
    else
	lnks [prev] = xpredicates;

    return xpredicates;
}



static int	gen_brothers (codop, left_son)
    int		codop, left_son;
{
    /* Utilise pour les sequences de predicats et le non_determinisme.
       codop == Prdct ou NonDeter. */
    /* Genere une sequence de predicats correspondant aux feuilles (filles) terminales d'un noeud */
    register int	son, hash_code, action, x;
    register struct node	*node;


/* Traitement des predicats */


    predicates [++xpredicates].lgth = 0;
    predicates [xpredicates].start = (x = xprdct_items) + 1;
    hash_code = 0;

    for (son = left_son; son != 0; son = node->brother) {
	node = tree + son;
	hash_code += (prdcts [++xprdct_items].prdct = node->prdct);
	fe_to_equiv (tnt_fe, node->fe);
	hash_code += (prdcts [xprdct_items].action = put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe));
	predicates [xpredicates].lgth++;
    }

    if ((tnt_fe->next = install_prdct_seq (predicates, prdcts, prdct_hash, prdct_lnks, xpredicates, hash_code % HASH_SIZE
	 )) != xpredicates) {
	/* La sequence existait deja, on l'utilise */
	xpredicates--;
	xprdct_items = x;
    }

    tnt_fe->codop = codop;
    tnt_fe->reduce = tnt_fe->pspl = 0;
    return put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe);
}



int	get_xprdct (prdct1, prdct2)
    int		prdct1, prdct2;
{
    /* Retourne un nouveau predicat etendu qui correspond a la concatenation (et logique)
       de prdct1 et prdct2 */

    register int	x, y, z, t;
    int		hash_code, size, prev;


#define HASH(p) (((p) >= 0) ? (p) : (((p) == -1) ? HASH_SIZE - 1 : xprdcts [-(p)].hash))
#define SIZE(p) (((p) >= -1) ? 1 : xprdcts [-(p)].size)

    if (xprdcts == NULL) {
	xprdcts = (struct xprdct*) sxalloc ((xprdcts_size = 20) + 1, sizeof (struct xprdct));
	ind_xprdcts = 1;
	prdcts_seq = (int*) sxalloc ((prdcts_seq_size = 2 * xprdcts_size) + 1, sizeof (int));
	ind_prdcts_seq = 0;

	/* Executions multiples dans le meme process */
	for (x = 0; x < HASH_SIZE ; x++)
	    xprdct_hash [x] = 0;
    }

    hash_code = (HASH (prdct1) + HASH (prdct2)) % HASH_SIZE;
    size = SIZE (prdct1) + SIZE (prdct2);

    for (x = xprdct_hash [hash_code], prev = 0; x != 0; x = xprdcts [prev = x].lnk) {
	if (size == xprdcts [x].size) {
	    y = xprdcts [x].head;

	    if (prdct1 >= -1) {
		if (prdcts_seq [y++] != prdct1)
		    continue;
	    }
	    else {
		for (t = (z = xprdcts [-prdct1].head) + xprdcts [-prdct1].size; z < t; z++) {
		    if (prdcts_seq [y++] != prdcts_seq [z])
			break;
		}

		if (z < t)
		    continue;
	    }

	    if (prdct2 >= -1) {
		if (prdcts_seq [y] != prdct2)
		    continue;
	    }
	    else {
		for (t = (z = xprdcts [-prdct2].head) + xprdcts [-prdct2].size; z < t; z++) {
		    if (prdcts_seq [y++] != prdcts_seq [z])
			break;
		}

		if (z < t)
		    continue;
	    }

	    return -x;
	}
    }


/* Nouvel element, on le met */

    if (++ind_xprdcts > xprdcts_size)
	xprdcts = (struct xprdct*) sxrealloc (xprdcts, (xprdcts_size *= 2) + 1, sizeof (struct xprdct));

    if (ind_prdcts_seq + size > prdcts_seq_size)
	prdcts_seq = (int*) sxrealloc (prdcts_seq, (prdcts_seq_size *= 2) + 1, sizeof (int));

    if (prev == 0)
	xprdct_hash [hash_code] = ind_xprdcts;
    else
	xprdcts [prev].lnk = ind_xprdcts;

    xprdcts [ind_xprdcts].lnk = 0;
    xprdcts [ind_xprdcts].hash = hash_code;
    xprdcts [ind_xprdcts].size = size;
    xprdcts [ind_xprdcts].head = ind_prdcts_seq + 1;

    if (prdct1 >= -1)
	prdcts_seq [++ind_prdcts_seq] = prdct1;
    else
	for (t = (z = xprdcts [-prdct1].head) + xprdcts [-prdct1].size; z < t; z++)
	    prdcts_seq [++ind_prdcts_seq] = prdcts_seq [z];

    if (prdct2 >= -1)
	prdcts_seq [++ind_prdcts_seq] = prdct2;
    else
	for (t = (z = xprdcts [-prdct2].head) + xprdcts [-prdct2].size; z < t; z++)
	    prdcts_seq [++ind_prdcts_seq] = prdcts_seq [z];

    return -ind_xprdcts;
}



bool		prdct_equal (xprdct, prdct1, prdct2)
    int		xprdct, prdct1, prdct2;
{
    /* Regarde si le predicat etendu prdct1 && prdct2 est xprdct */

    if (xprdct >= -1)
	return false;

    return xprdct == get_xprdct (prdct1, prdct2);
}



void build_branch (xprdct, fe, father)
    int		xprdct, father;
    struct floyd_evans	*fe;
{
    /* Construit la branche qui s'epelle "xprdct" sous "father" */

    register int	prdct, son, prev, x;
    register struct node	*node;

    if (fe == NULL)
	fe = tnt_fe + t_error;


    if (tree == NULL) {
	tree = (struct node*) sxalloc ((tree_size = 20) + 1, sizeof (struct node));
	tree [0] = empty_node;
	xnode = 0;
    }

    if (xprdct >= -1)
	prdct = xprdct;
    else
	prdct = prdcts_seq [x = xprdcts [-xprdct].head];

    for (prev = 0, son = tree [father].son; son != 0; prev = son, son = node->brother) {
	if ((node = tree + son)->prdct <= prdct)
	    break;
    }

    if (son == 0 || node->prdct < prdct) {
	if (++xnode > tree_size)
	    tree = (struct node*) sxrealloc (tree, (tree_size *= 2) + 1, sizeof (struct node));

	if (prev == 0)
	    tree [father].son = xnode;
	else
	    tree [prev].brother = xnode;

	node = tree + xnode;
	node->brother = son;
	node->son = 0;
	node->fe = NULL;
	node->prdct = prdct;
	son = xnode;
    }

    if (xprdct >= -1) {
	if (node->son == 0)
	    node->fe = fe;
	else
	    sxtrap (ME, "build_branch");
    }
    else
	build_branch (prdcts_seq [x + 1], fe, son);

    return;
}



int	gen_tree (codop, father)
    int		codop, father;
{
    /* Genere les sequences de predicats du [sous_arbre] father */

    register int	son, first_son;
    register struct node	*node;

    for (son = (first_son = tree [father].son); son != 0; son = node->brother) {
	node = tree + son;

	if (node->son != 0)
	    node->fe = tnt_fe + gen_tree (codop, son);
    }

    son = gen_brothers (codop, first_son);

    if (father == 0 /* pour build_branch suivant */) {
	tree [0] = empty_node;
	xnode = 0;
    }

    return son;
}



int	xprdct_mngr (entry)
    int		entry;
{
    switch (entry) {
    case SXCLOSE:
	if (tree != NULL)
	    sxfree (tree);

	if (xprdcts != NULL)
	    sxfree (xprdcts);

	if (prdcts_seq != NULL)
	    sxfree (prdcts_seq);

	return;

    case SXINIT:
	tree = NULL;
	xprdcts = NULL;
	prdcts_seq = NULL;
	return;

    default:
	break;
    }

    sxtrap (ME, "xprdct_mngr");
}
