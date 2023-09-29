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

/*   I N C L U D E S   */

#include "sxversion.h"
#include "sxunix.h"
#include "P_tables.h"
#include "csynt_optim.h"

char WHAT_XPRDCT_MNGR[] = "@(#)SYNTAX - $Id: xprdct_mngr.c 3179 2023-05-06 08:32:16Z garavel $" WHAT_DEBUG;

struct xprdct {
    SXINT		lnk, hash, size, head;
};

struct node {
    SXINT		brother, son;
    SXINT		prdct, action;
};

static struct node	*tree;
static SXINT	tree_size, xnode;
static struct node	empty_node;
static SXINT	xprdct_hash [HASH_SIZE];
static SXINT	*prdcts_seq;
static SXINT	xprdcts_size, ind_xprdcts, prdcts_seq_size, ind_prdcts_seq;
static struct xprdct	*xprdcts;


#define error_node 1


static SXINT	install_prdct_seq (struct state *install_prdct_seq_predicates, 
				   struct P_prdct *install_prdct_seq_prdcts, 
				   SXINT *hashs, 
				   SXINT *lnks, 
				   SXINT install_prdct_seq_xpredicates, 
				   SXINT hash_code)
{

/* Look wether the sequence of predicates item started at "install_prdct_seq_xpredicates" in "install_prdct_seq_predicates"
   is already defined in "install_prdct_seq_predicates" at another index.
   Returns this index if the answer is yes otherwise install_prdct_seq_xpredicates.
   The length of the input sequence is in "install_prdct_seq_predicates [install_prdct_seq_xpredicates].lgth".
   This sequence starts at  "install_prdct_seq_predicates [install_prdct_seq_xpredicates].start" in "install_prdct_seq_prdcts".
   "lnks [install_prdct_seq_xpredicates]" is used to hold the link between sequences with
   same hash_code. New sequences are sorted by decreasing values of the field
   "prdct" in "install_prdct_seq_prdcts". */


    struct P_prdct	*aprdct, *xaprdct, *lim, *start;
    SXINT	lgth, hd, prev;

    lgth = install_prdct_seq_predicates [install_prdct_seq_xpredicates].lgth;
    lim = (start = install_prdct_seq_prdcts + install_prdct_seq_predicates [install_prdct_seq_xpredicates].start) + lgth;

    for (hd = hashs [hash_code], prev = 0; hd != 0; hd = lnks [prev = hd]) {
	if (install_prdct_seq_predicates [hd].lgth == lgth) {
	    for (aprdct = start, xaprdct = install_prdct_seq_prdcts + install_prdct_seq_predicates [hd].start; aprdct < lim; aprdct++, xaprdct++) {
		if (xaprdct->prdct != aprdct->prdct || xaprdct->action != aprdct->action)
		    break;
	    }

	    if (aprdct == lim)
		return hd;
	}
    }


/* New sequence */

    lnks [install_prdct_seq_xpredicates] = 0 /* link tail */ ;

    if (prev == 0)
	/* new bucket */
	hashs [hash_code] = install_prdct_seq_xpredicates;
    else
	lnks [prev] = install_prdct_seq_xpredicates;

    return install_prdct_seq_xpredicates;
}



static SXINT	gen_brothers (SXINT left_son, struct floyd_evans *fe)
{
    /* Genere une sequence de predicats correspondant aux feuilles (filles) terminales d'un noeud */
    SXINT	son, hash_code, x;
    struct node	*node;


/* Traitement des predicats */


    predicates [++xpredicates].lgth = 0;
    predicates [xpredicates].start = (x = xprdct_items) + 1;
    hash_code = 0;

    for (son = left_son; son != 0; son = node->brother) {
	node = tree + son;
	hash_code += (prdcts [++xprdct_items].prdct = node->prdct);
	fe_to_equiv (tnt_fe, (node->son == 0 ? fe : tnt_fe) + node->action);
	hash_code += (prdcts [xprdct_items].action = put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe));
	predicates [xpredicates].lgth++;
    }

    if ((tnt_fe->next = install_prdct_seq (predicates, prdcts, prdct_hash, prdct_lnks, xpredicates, hash_code % HASH_SIZE
	 )) != xpredicates) {
	/* La sequence existait deja, on l'utilise */
	xpredicates--;
	xprdct_items = x;
    }

    tnt_fe->codop = Prdct;
    tnt_fe->reduce = tnt_fe->pspl = 0;
    return put_in_fe (tnt_fe, tnt_hash, tnt_lnks, tnt_fe);
}



SXINT	get_xprdct (SXINT prdct1, SXINT prdct2)
{
    /* Retourne un nouveau predicat etendu qui correspond a la concatenation (et logique)
       de prdct1 et prdct2 */

    SXINT	x, y, z, t;
    SXINT		hash_code, size, prev;


#define HASH(p) (((p) >= 0) ? (p) : (((p) == -1) ? HASH_SIZE - 1 : xprdcts [-(p)].hash))
#define SIZE(p) (((p) >= -1) ? 1 : xprdcts [-(p)].size)

    if (xprdcts == NULL) {
	xprdcts = (struct xprdct*) sxalloc ((xprdcts_size = 20) + 1, sizeof (struct xprdct));
	ind_xprdcts = 1;
	prdcts_seq = (SXINT*) sxalloc ((prdcts_seq_size = 2 * xprdcts_size) + 1, sizeof (SXINT));
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
	prdcts_seq = (SXINT*) sxrealloc (prdcts_seq, (prdcts_seq_size *= 2) + 1, sizeof (SXINT));

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



SXBOOLEAN		prdct_equal (SXINT xprdct, SXINT prdct1, SXINT prdct2)
{
    /* Regarde si le predicat etendu prdct1 && prdct2 est xprdct */

    if (xprdct >= -1)
	return SXFALSE;

    return xprdct == get_xprdct (prdct1, prdct2);
}



void build_branch (SXINT xprdct, SXINT action, SXINT father)
{
    /* Construit la branche qui s'epelle "xprdct" sous "father" */

    SXINT	prdct, son, prev, x;
    struct node	*node = NULL /* Pour un compilateur croisé pour l'architecture itanium64 qui est gcc version 3.4.5
						et qui dit "warning: 'node' might be used uninitialized in this function" */;

    sxinitialise(x); /* pour faire taire gcc -Wuninitialized */
    if (action == 0)
	action = t_error;

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
	node->action = 0;
	node->prdct = prdct;
	son = xnode;
    }

    if (xprdct >= -1) {
	if (node->son == 0)
	    node->action = action;
	else
	    sxexit(1);
    }
    else
	build_branch (prdcts_seq [x + 1], action, son);

    return;
}



SXINT	gen_tree (SXINT father, struct floyd_evans *fe)
{
    /* Genere les sequences de predicats du [sous_arbre] father */

    SXINT	son, first_son;
    struct node	*node;

    for (son = (first_son = tree [father].son); son != 0; son = node->brother) {
	node = tree + son;

	if (node->son != 0)
	    node->action = gen_tree (son, fe);
    }

    son = gen_brothers (first_son, fe);

    if (father == 0 /* pour build_branch suivant */) {
	tree [0] = empty_node;
	xnode = 0;
    }

    return son;
}



void	xprdct_mngr (SXINT entry)
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

    fputs ("The function \"xprdct_mngr\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}
