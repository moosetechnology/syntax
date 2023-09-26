static SXBA	item_set;
static int	xla_items, la_items_size;
static int	*la_items;
static int	hash_la_items [HASH_SIZE];
static int	xxor_item, xor_items_size, xor_hash_size;
static struct xor_item {
	   int	lnk, state, orig;
       }	*xor_items;
static int	*xor_hash;

#include "sxcommon.h"
char WHAT_LASET[] = "@(#)SYNTAX - $Id: la_set.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


#define INFINI 0x3FFFFFFF

#define MAX(x,y)	(((x)>(y))?(x):(y)))

static int	xor_search (state, item)
    int		state, item;
{
    /* Retourne l'index dans le tableau xor_items du couple (state, item).
       Si ce couple n'existait pas, il est insere */

    register int	x, y, hash;
    register struct xor_item	*aitem;

    for (x = xor_hash [hash = state ^ item], y = 0; x != 0; x = xor_items [y = x].lnk) {
	if (xor_items [x].state == state)
	    return x;
    }

    if (++xxor_item > xor_items_size) {
	xor_items_size *= 2;
	xor_items = (struct xor_item*) sxrealloc (xor_items, xor_items_size + 1, sizeof (struct xor_item));
    }

    aitem = xor_items + xxor_item;
    aitem->lnk = 0;
    aitem->state = state;
    aitem->orig = 0;

    if (y == 0)
	xor_hash [hash] = xxor_item;
    else
	xor_items [y].lnk = xxor_item;

    return xxor_item;
}



static int	state_nt_to_items (state, nt)
    int		state, nt;
{
    /* Recherche dans l'etat "state" la liste des items dont le marqueur LR repere
       le non-terminal "nt" */
    /* Recherche par dichotomie */
    /* Suppose que "nt" existe */

    register int	min, max, xtnt;

    min = S1 [xtnt = S1 [state].shftnt].ptnt;
    max = S1 [xtnt + 1].ptnt - 1;

    while (min < max) {
	xtnt = (min + max) >> 1;

	if (nt <= bnf_ag.WS_INDPRO [tant [S1 [tnt [xtnt]].ptant]].lispro)
	    max = xtnt;
	else
	    min = xtnt + 1;
    }

    return tnt [max];
}



static int	install_in_la_items (item_set)
    register SXBA	item_set;
{
    register int	xlnk, lnk, size;
    int		hash, prev_lnk;

    xlnk = xla_items;
    hash = 0;
    size = 0;
    xla_items += 2;

    {
	register int	item;

	for (item = sxba_scan (item_set, 0); item != -1; sxba_scan (item_set, item)) {
	    if (xla_items >= la_items_size) {
		la_items = (int*) sxrealloc (la_items, (la_items_size *= 2) + 1, sizeof (int));
	    }

	    size++;
	    la_items [xla_items++] = item;
	    hash += item;
	}
    }

    la_items [xlnk + 1] = size++;
    /* On regarde si la sequence n'existait pas deja */
    hash %= HASH_SIZE;

    for (lnk = hash_la_items [hash], prev_lnk = 0; lnk != 0; lnk = la_items [prev_lnk = lnk]) {
	register int	i;

	for (i = 1; i <= size; i++) {
	    if (la_items [lnk + i] != la_items [xlnk + i])
		break;
	}

	if (i > size /* la sequence existe deja */ ) {
	    xla_items = xlnk;
	    return lnk + 1 /* index du compte */ ;
	}
    }


/* On installe la nouvelle sequence */

    la_items [xlnk] = 0;

    if (prev_lnk == 0)
	hash_la_items [hash] = xlnk;
    else
	la_items [prev_lnk] = xlnk;

    return xlnk + 1;
}



static int	la_set (state, item, item_set, level)
    int		state, item, level;
    register SXBA	item_set;
{
    /* Cette fonction recursive calcule le "LALR(1) look-ahead set" du couple (state,item)
       Ce look-ahead set est un ensemble d'items; le vrai look-ahead set peut s'en deduire
       en prenant l'union des First1 de tous ses elements.
       Methode:
       On considere l'"arbre" des predecesseurs de (state,item) et pour chaque noeud de cet
       arbre on essaie de calculer et de stocker (pour reutilisation eventuelle ulterieure)
       le look-ahead set associe.
       Les look-ahead set calcules sont stockes dans le tableau la_items.
       Si on tombe sur un noeud calcule on remplit item_set avec le contenu de la_items et on
       retourne l'index correspondant (>0) dans la_items.
       Si on tombe sur un noeud qui existe deja sur le chemin menant a la racine, on retourne
       le niveau initial (<0) de ce noeud (item_set = vide).
       Sinon c'est un noeud non calcule qui n'existe pas sur le chemin menant a la racine.
       Un noeud est calcule si tous ses predecesseurs sont calcules ou si les chemins
       incomplets se sont arretes sur son propre calcul.
       Sinon il est partiellement calcule.
       Le look-ahead set d'un sous arbre partiellement calcule est utilise mais sa racine
       est marquee non calculee en sortant de la_set.
    */

    register struct xor_item	*axor_item;

    axor_item = xor_items + xor_search (state, item);

    {
	register int	orig;

	if ((orig = axor_item->orig) < 0)
	    /* En cours de calcul sur le chemin menant a la racine */
	    return orig;

	if (orig > 0 /* deja calcule */ ) {
	    if (item_set != NULL) {
		/* On recopie dans item_set */
		register int	*x, *lim;

		x = la_items + orig;
		lim = la_items + *x /* size */ ;

		while (++x <= lim)
		    sxba_1_bit (item_set, *x /* item */ );
	    }

	    return orig;
	}
    }

    {
	register int	xti;
	register SXBA	cur_item_set;
	int	max_level, times;
	BOOLEAN		is_computed;

	axor_item->orig = level /* on marque le noeud en cours de traitement */ ;
	cur_item_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
	is_computed = TRUE;
	max_level = -INFINI;
	times = 0;

	if (bnf_ag.WS_INDPRO [item - 1].lispro != 0 /* item du kernel A -> alpha X .beta */ ) {
	    /* Calcul des etats predecesseurs de state (sur X) */
	    register int	x;

	    for (x = S1 [state].pprd; x != 0; x = lstprd [x].f2) {
		times++;

		if ((xti = la_set (lstprd [x].f1, item - 1, cur_item_set, level - 1)) < 0 /* non calcule */ ) {
		    is_computed = FALSE;
		    max_level = MAX (max_level, xti);
		}
	    }
	}
	else /* item de la fermeture: A -> . beta */  {
	    register int	xtant, lim;

	    {
		register int	xpt;

		xpt = state_nt_to_items (state, bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc /* nt */ );
		xtant = S1 [xpt].ptant;
		lim = S1 [xpt + 1].ptant;
	    }

	    {
		register int	tnt, start_item;

		while (xtant < lim) {
		    start_item = item = tant [xtant++] /* B -> alpha .A gamma */ ;

		    if ((tnt = bnf_ag.WS_INDPRO [++item].lispro) != 0) {
			times = 2 /* Pas de simple propagation */ ;
			sxba_1_bit (cur_item_set, item);
		    }


/* gamma derive-t-il dans le vide? */

		    while (tnt > 0) {
			/* non terminal */
			tnt = XNT_TO_NT_CODE (tnt);

			if (!sxba_bit_is_set (bnf_ag.BVIDE, tnt))
			    /* non-terminal ne derivant pas dans le vide */
			    break;

			tnt = bnf_ag.WS_INDPRO [++item].lispro;
		    }

		    if (tnt == 0 /* gamma =>* epsilon */ ) {
			times++;

			if ((xti = la_set (state, start_item, cur_item_set, level - 1)) < 0 /* non calcule */ ) {
			    is_computed = FALSE;
			    max_level = MAX (max_level, xti);
			}
		    }
		}
	    }
	}

	if (is_computed && times == 1)
	    /* simple propagation du la-set */
	    max_level = axor_item->orig = xti;
	else if (is_computed || max_level == level)
	    /* On installe cur_item_set dans la_items */
	    max_level = axor_item->orig = install_in_la_items (cur_item_set);
	else
	    /* la_set partiellement calcule, le sous-arbre est considere comme n'etant pas calcule */
	    axor_item->orig = 0;

	if (item_set != NULL)
	    sxba_or (item_set, cur_item_set);

	sxfree (cur_item_set);
	return max_level;
    }
}



initialize ()
{
    register int	i;

    xor_hash_size = xac2 + bnf_ag.WS_TBL_SIZE.indpro + 1;
    xor_items_size = xor_hash_size;
    xor_hash = (int*) sxcalloc (xor_hash_size, sizeof (int));
    xor_items = (struct xor_item*) sxalloc (xor_items_size + 1, sizeof (struct xor_item));
    xxor_item = 0;

    for (i = 0; i < HASH_SIZE; i++)
	hash_la_items = 0;

    la_items_size = xor_hash_size;
    la_items = (int*) sxalloc (la_items_size + 1, sizeof (int));
    xla_items = 1;
    item_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
}



action ()
{
    x = la_set (xac1, tr [xtr1], item_set, -1);
}



finalize ()
{
    sxfree (item_set);
    sxfree (la_items);
    sxfree (xor_items);
    sxfree (xor_hash);
}
