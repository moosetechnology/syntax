/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1990 by Institut National de Recherche *
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
/* 01-08-91 14:45 (pb):		Utilisation des XxYxZ			*/
/************************************************************************/
/* 26-10-90 15:15 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT[] = "@(#)super_FSA.c	- SYNTAX [unix] - 1 Aout 1991";

static char	ME [] = "super_FSA";


#include "sxba.h"
#include "XxYxZ.h"

#define PUSH(T,x)		T [x] = T [0], T [0] = x

static XxYxZ_header	*QxTxQ_hd;
static X_header		*Q_hd;
static int		*Q_to_classes, *trans, *to_next;
static SXBA		*classes;


static void	CLEAR (T)
    register int	*T;
{
    register int	x, y;

    for (x = T [y = 0]; x > 0; x = T [y = x])
	T [y] = 0;

    T [y] = 0;
    T [0] = -1;
}

static void install (r)
    int r;
{
    register int	rts, t;

    XxYxZ_Xforeach (*QxTxQ_hd, r, rts) {
	if (trans [t = XxYxZ_Y (*QxTxQ_hd, rts)] == 0) {
	    PUSH (trans, t);
	    to_next [t] = XxYxZ_Z (*QxTxQ_hd, rts);
	}
    }
}


static bool check (r)
    int r;
{
    register int	rts, t, s;
    int			next;

    XxYxZ_Xforeach (*QxTxQ_hd, r, rts) {
	if (trans [t = XxYxZ_Y (*QxTxQ_hd, rts)] != 0) {
	    if ((s = XxYxZ_Z (*QxTxQ_hd, rts)) > 0)
		s = Q_to_classes [s];

	    if ((next = to_next [t]) > 0)
		next = Q_to_classes [next];

	    if (next != s)
		return false;
	}
    }

    return true;
}



int	super_FSA (X_hd, XxYxZ_hd, AxOxV_hd, Tcard, dont_reuse_initial_state)
    X_header	*X_hd;
    XxYxZ_header	*XxYxZ_hd, *AxOxV_hd;
    int		Tcard;
    bool	dont_reuse_initial_state;
{
    /* This procedure computes from a given FSA :
          M = (Q, T, delta, i, F)
       a [super-]FSA :
          M' = (Q', T, delta', i', F')
       such that:
          - if x is a string of M then x is also a string of M'
	  - for each (x1, x2) in T*xT* such
	       delta (i, x1) = f1,
	       delta (i, x2) = f2,
	       (f1, f2) in FxF and
	       f1 # f2
	    we have :
	       delta' (i', x1) = f1',
	       delta' (i', x2) = f2',
	       (f1', f2') in F'xF' and
	       f1' # f2'
	  - it does not exist another super-FSA M" of M with |Q"| < |Q'|
    */
    /* Final states are negative integers, non final are positive. */
    /* The initial state is the integer 1. */
    /* dont_reuse_initial_state prohibits (even if possible) the [re-]usage
       of the initial state. */
    register int	top_classe;
    int			Qcard;

    Q_hd = X_hd;
    QxTxQ_hd = XxYxZ_hd;
    Qcard = X_top (*Q_hd);
    classes = sxbm_calloc (Qcard + 1, Qcard + 1);
    Q_to_classes = (int*) sxalloc (Qcard + 1, sizeof (int));
    trans = (int*) sxcalloc (Tcard + 1, sizeof (int));
    trans [0] = -1;
    to_next = (int*) sxalloc (Tcard + 1, sizeof (int));

    {
	/* 0_equivalence computation. */
	/* Partitioning into |F| + 1 classes ; {f1}, ..., {fn}, Q-F
	   where fi in F */

	/* Q - F computation */
	register int	x, r, x_classe;
	register SXBA	classe, new_classe;
	bool		is_stable, is_OK;

	top_classe = dont_reuse_initial_state ? 2 : 1;
	classe =  classes [top_classe];
	SXBA_1_bit (classes [1], 1);
	Q_to_classes [1] = 1;

	for (x = 2; x <= Qcard; x++) {
	    Q_to_classes [x] = top_classe;
	}


	/* k_equivalence computation. */
	/* q1 and q2 are k-equivalent iff they are k-1 equivalent and
	   foreach t delta (q1, t) and delta (q2, t) are also k-1 equivalent
	*/

	is_stable = false;
	
	while (!is_stable) {
	    is_stable = true;
	    x_classe = 0;

	    while (++x_classe <= top_classe) {
		classe = classes [x_classe];
		r = sxba_scan (classe, 0);

		if (sxba_scan (classe, r) > 0) {
		    /* two elements at least. */
		    install (r);
		    new_classe = classes [top_classe + 1];
		    is_OK = true;

		    while ((r = sxba_scan (classe, r)) > 0) {
			if (check (r))
			    install (r);
			else {
			    is_OK = false;
			    SXBA_1_bit (new_classe, r);
			}
		    }

		    if (!is_OK) {
			/* Mandatory global processing of incompatible states. */
			is_stable = false;
			top_classe++;
			sxba_minus (classe, new_classe);
			r = 0;

			while ((r = sxba_scan (new_classe, r)) > 0) {
			    Q_to_classes [r] = top_classe;
			}
		    }

		    CLEAR (trans);
		}
	    }
	}
    }

    sxfree (classes);
    sxfree (trans);
    sxfree (to_next);

    {
	/* Super_FSA is stored. */
	register int	rts, rts_top;
	int		s, aov;

	XxYxZ_clear (AxOxV_hd);
	rts_top = XxYxZ_top (*QxTxQ_hd);

	for (rts = 1; rts <= rts_top;rts++) {
	    if ((s = XxYxZ_Z (*QxTxQ_hd, rts)) > 0)
		s = Q_to_classes [s];

	    XxYxZ_set (AxOxV_hd, Q_to_classes [XxYxZ_X (*QxTxQ_hd, rts)], XxYxZ_Y (*QxTxQ_hd, rts), s, &aov);
	}
    }

    sxfree (Q_to_classes);
    return top_classe;
}
