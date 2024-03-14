/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
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
/* 09-07-92 11:45 (pb):		Version pour test de l'analyse		*/
/*				incrementale non-deterministe		8?
/************************************************************************/
/* 13-04-92 16:50 (pb):		les (T|NT)_trans sont generees par	*/
/*				ordre croissant.			*/
/************************************************************************/
/* 19-10-90 16:05 (pb):		Modifs pour XNT en LHS			*/
/************************************************************************/
/* ??-09-90 ??:?? (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)LR0.c	- SYNTAX [unix] - 9 Juillet 1992";

static char	ME [] = "LR (0)";

#include "sxindparser.h"

#define max(x,y) (((x)>=(y))?(x):(y))

static int	limitem, limt, limnt;
static int	*tsymb1, *items_list, *nt_stack;
static SXBA	t_set, nt_set;
static bool	is_t_trans, is_nt_trans;


static void	oflw_nucleus (old_size, new_size)
    int		old_size, new_size;
{
    Q0 = (struct Q0*) sxrealloc (Q0, new_size + 1, sizeof (struct Q0));
}


static void	oflw_Q0xV (old_size, new_size)
    int		old_size, new_size;
{
    Q0xV_to_Q0 = (int*) sxrealloc (Q0xV_to_Q0, new_size + 1, sizeof (int));
}



static	mettre (item)
    register int	item;
{
    register int	x, tnt;
    register int	*q;

    q = tsymb1 + (tnt = bnf_ag.WS_INDPRO [item].lispro);

    if (tnt != 0) {
	if (*q == limitem) {
	    /* Nouvelle sous_liste */
	    if (tnt > 0) {
		if (SXBA_bit_is_reset_set (nt_set, tnt)) {
		    is_nt_trans = true;
		    nt_stack [(*nt_stack)++] = tnt;
		}
	    }
	    else {
		SXBA_1_bit (t_set, -tnt);
		is_t_trans = true;
	    }
	}
    }

    while ((x = *q) <= item)
	q = items_list + x;

    *q = item;
    items_list [item] = x;
}


void	LR0 ()
{
    limitem = bnf_ag.WS_TBL_SIZE.indpro + 1;
    limt = -bnf_ag.WS_TBL_SIZE.xtmax - 1;
    limnt = bnf_ag.WS_TBL_SIZE.xntmax + 1;

    {
	register int	INDPRO, INDPROt4;

	INDPRO = bnf_ag.WS_TBL_SIZE.indpro;
	INDPRO = max (10, INDPRO);
	INDPROt4 = 4 * INDPRO;
	XH_alloc (&nucleus_hd, "nucleus", INDPRO, 2, 4, oflw_nucleus, statistics_file);
	Q0 = (struct Q0*) sxalloc (XH_size (nucleus_hd) + 1, sizeof (struct Q0));
	XxY_alloc (&Q0xV_hd, "Q0xV", INDPROt4, 2, 0, 0, oflw_Q0xV, statistics_file);
	Q0xV_to_Q0 = (int*) sxalloc (XxY_size (Q0xV_hd) + 1, sizeof (int));
	XxY_alloc (&Q0xQ0_hd, "Q0xQ0", INDPROt4, 2, 0, 1, NULL, statistics_file);
    }

    items_list = (int*) sxalloc (limitem, sizeof (int));
    tsymb1 = (int*) sxalloc (limnt - limt, sizeof (int)) - limt;
    /* limt est negatif (-xtmax-1) */
    nt_stack = (int*) sxcalloc (limnt, sizeof (int));
    nt_stack [0] = 1;
    nt_set = sxba_calloc (limnt);
    t_set = sxba_calloc (-limt);

    {
	register int	i;

	for (i = -bnf_ag.WS_TBL_SIZE.xtmax; i < limnt; i++) {
	    tsymb1 [i] = limitem;
	}
    }

    XH_push (nucleus_hd, 0);
    /* initialisation */
    XH_set (&nucleus_hd, &xac2);
    /* xac2 = 1 */
    Q0 [1].in = 0;
    Q0 [0].xla = 0;

    {
	register int	xac1;

	for (xac1 = 1; xac1 < XH_top (nucleus_hd); xac1++) {
	    is_t_trans = is_nt_trans = false;
	    
	    {
		/* Fermeture. */
		register int	x, lim;

		for (lim = XH_X (nucleus_hd, xac1 + 1), x = XH_X (nucleus_hd, xac1); x < lim; x++) {
		    mettre (XH_list_elem (nucleus_hd, x) + 1);
		}
	    }

	    {
		register int	xnt_stack, x, i;
		int	lim;

		for (xnt_stack = 1; xnt_stack < *nt_stack; xnt_stack++) {
		    x = nt_stack [xnt_stack];

		    for (lim = bnf_ag.WS_NTMAX [x + 1].npg, i = bnf_ag.WS_NTMAX [x].npg; i < lim; i++) {
			mettre (bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [i].numpg].prolon);
		    }
		}
	    }

	    Q0 [xac1].bot = XxY_top (Q0xV_hd) + 1;
	    Q0 [xac1].xla = Q0 [0].xla;


/* Traitement des transitions terminales. */

	    {
		register int	xt, item, red_no;
		int	next_state, indice, nb = 0;

		if (is_t_trans) {
		    /* Il y a des transitions terminales */
		    xt = 0;

		    while ((xt = sxba_scan_reset (t_set, xt)) > 0) {
			if (items_list [item = tsymb1 [-xt]] == limitem /* un seul item */ &&
			    bnf_ag.WS_INDPRO [item + 1].lispro == 0 /* en position pre_reduce */)
			    next_state = -item;
			else {
			    do {
				XH_push (nucleus_hd, item);
			    } while ((item = items_list [item]) != limitem);

			    XH_set (&nucleus_hd, &next_state);
			}

			nb++;
			XxY_set (&Q0xV_hd, xac1, -xt, &indice);
			Q0xV_to_Q0 [indice] = next_state;

			if (next_state > 0) {
			    XxY_set (&Q0xQ0_hd, xac1, next_state, &indice);
			    Q0 [next_state].in = -xt;
			}

			tsymb1 [-xt] = limitem;
		    }
		}

		Q0 [xac1].t_trans_nb = nb; /* Attention Q0 a pu etre realloue! */
	    }


/* Traitement des transitions non-terminales. */

	    {
		register int	xnt, item, red_no;
		int	next_state, indice, nb = 0;

		if (is_nt_trans) {
		    /* Il y a des transitions non-terminales */
		    xnt = 0;

		    while ((xnt = sxba_scan_reset (nt_set, xnt)) > 0) {
			if (items_list [item = tsymb1 [xnt]] == limitem /* un seul item */ &&
			    bnf_ag.WS_INDPRO [item + 1].lispro == 0 /* en position pre_reduce */) {
			    next_state = -item;
			}
			else {
			    do {
				XH_push (nucleus_hd, item);
			    } while ((item = items_list [item]) != limitem);

			    XH_set (&nucleus_hd, &next_state);
			}

			nb++;
			XxY_set (&Q0xV_hd, xac1, xnt, &indice);
			Q0xV_to_Q0 [indice] = next_state;

			if (next_state > 0) {
			    XxY_set (&Q0xQ0_hd, xac1, next_state, &indice);
			    Q0 [next_state].in = xnt;
			}

			tsymb1 [xnt] = limitem;
		    }

		    /* Pour le coup suivant. */
		    nt_stack [0] = 1;
		}

		Q0 [xac1].nt_trans_nb = nb; /* Attention Q0 a pu etre realloue! */
	    }


/* Traitement des reductions. */

	    {
		register int	item, nb = 0;
		int	indice, red_no;

		if ((item = tsymb1 [0]) != limitem) {
		    do {
			/* L'utilisation de V = XNTMAX + red_no permet de calculer
			   directement Q0xV a partir de l'etat et de la reduction. */
			XxY_set (&Q0xV_hd,
				 xac1,
				 bnf_ag.WS_TBL_SIZE.xntmax +
				 (red_no = bnf_ag.WS_INDPRO [item].prolis),
				 &indice);
			Q0xV_to_Q0 [indice] = -item;
			/* pourquoi pas? */
			reduce_item_no++;
			nb++;

			if (bnf_ag.WS_TBL_SIZE.xnbpro > bnf_ag.WS_TBL_SIZE.actpro) {
			    /* has_xprod */
			    /* On regarde s'il n'existe pas d'autres reductions ayant
			       meme representant afin de les stocker sur des valeurs
			       consecutives de StNt. */
			    register int	next_item, prev_item;
			    int			repr_red_no;

			    repr_red_no = PROD_TO_RED (red_no);
			    next_item = item;

			    while ((next_item = items_list [prev_item = next_item]) != limitem) {
				red_no = bnf_ag.WS_INDPRO [next_item].prolis;

				if (PROD_TO_RED (red_no) == repr_red_no) {
				    XxY_set (&Q0xV_hd,
					     xac1,
					     bnf_ag.WS_TBL_SIZE.xntmax + red_no,
					     &indice);
				    Q0xV_to_Q0 [indice] = -next_item;
				    /* pourquoi pas? */
				    reduce_item_no++;
				    nb++;
				    items_list [prev_item] = items_list [next_item];
				}
			    }
			}
		    } while ((item = items_list [item]) != limitem);

		    max_red_per_state = max (max_red_per_state, nb);
		    tsymb1 [0] = limitem;
		    Q0 [0].xla += nb;
		}

		Q0 [xac1].red_trans_nb = nb; /* Attention Q0 a pu etre realloue! */
	    }
	}

	xac2 = XH_top (nucleus_hd);
	sxfree (nt_stack), nt_stack = NULL;
	sxfree (nt_set), nt_set = NULL;
	sxfree (t_set), t_set = NULL;
	sxfree (tsymb1 + limt), tsymb1 = NULL;
	sxfree (items_list), items_list = NULL;
    }
}


bool		pred_card (q)
    int		q;
{
    /* Retourne true ss1 le nombre de predecesseurs de q dans l'automate LR(0) est
       superieur a 1 */
    register int	x, n = 0;

    XxY_Yforeach (Q0xQ0_hd, q, x) {
	if (++n > 1)
	    return true;
    }

    return false;
}



int	Q0_V_to_next (state, v)
    int		state, v;
{
    /* Retourne l'etat successeur de "state" par transition sur "v". */
    int		indice;

    return (indice = XxY_is_set (&Q0xV_hd, state, v)) ? Q0xV_to_Q0 [indice] : 0;
}


int	nucl_i (state, i)
    register int	state, i;
{
    /* Retourne le predecesseur (item - 1) du ieme item s'il existe (sinon 0) du nucleus
       de l'etat state. */
    register int	x;

    if (state < 0) {
	if (i == 1) /* Un seul item code en negatif */
	    return -state;
    }
    else {
	if ((x = XH_X (nucleus_hd, state) + i - 1) < XH_X (nucleus_hd, state + 1))
	    return XH_list_elem (nucleus_hd, x);
    }

    return 0;
}


bool		t_in_deriv (t, item)
    register int	t, item;
{
    /* Teste si le terminal "t" appartient a S avec
       S = FIRST (beta) et item == A->alpha . beta */

    register int	tnt;

    while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) > 0) {
	/* Non terminal */
	if (SXBA_bit_is_set (bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)], -t))
	    return true;

	if (!SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
	    break;
    }

    return tnt == t;
}



bool ante (state, item, f)
    int		state, item;
    bool	(*f)();
{
    /* item : A -> alpha . beta est dans state.
       Appelle la fonction f pour chaque etat q tel que goto*(q, alpha) = state.
       f a la possibilite d'interrompre le parcourt en retournant true */
    register int	x;

    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0) {
	/* item du kernel: A -> alpha X . beta */
	XxY_Yforeach (Q0xQ0_hd, state, x) {
	    /* Calcul des etats predecesseurs de state (sur X) */
	    if (ante (XxY_X (Q0xQ0_hd, x), item - 1, f))
		return true;
	}
    }
    else /* item de la fermeture: A -> . beta */
	return f (state, item);

    return false;
}

int	gotostar (state, start, goal)
    register int	state, start, goal;
{
    /* calcule goto*(state,X1 X2 ... Xn) avec
       start : A -> alpha .X1 X2 ... Xn beta
       goal :  A -> alpha X1 X2 ... Xn .beta */

    while (start < goal) {
	state = Q0_V_to_next (state, bnf_ag.WS_INDPRO [start++].lispro);
    }

    return state;
}

