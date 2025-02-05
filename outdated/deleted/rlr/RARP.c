/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1989 by Institut National de Recherche *
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
/* 01-08-91 14:45 (pb):		Utilisation des XxYxZ			*/
/************************************************************************/
/* 25-04-90 11:32 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

char WHAT_RARP[] = "@(#)RARP.c	- SYNTAX [unix] - 21 octobre 1991";

static char	ME [] = "RARP";

#include "P_tables.h"

#include "rlr_optim.h"

#include "SS.h"
#include "RLR.h"

#define RARP_STATES_BOT(x)	(RARP_states_hd.list + RARP_states_hd.display [x].X)
#define RARP_STATES_TOP(x)	(RARP_states_hd.list + RARP_states_hd.display [(x)+1].X)
#define RARP_ITEMS_BOT(x)	(RARP_items_hd.list + RARP_items_hd.display [x].X)
#define RARP_ITEMS_TOP(x)	(RARP_items_hd.list + RARP_items_hd.display [(x)+1].X)

static int		ARP_states_set_size;
static SXBA		ARP_states_set;
static int		UARP_states_set_size;
static SXBA		UARP_states_set;
static XxYxZ_header	UARPxTRANSxUARP_hd;
static int		*ARP_to_UARP;

static XH_header	RARP_states_hd, RARP_items_hd;
static XxYxZ_header	RARPxTRANSxRARP_hd;

static X_header		X_RARP_new_trans_hd;

static int		last_UARP_state;

static bool		is_alloc_UARP_called;

static void	RARP_open ()
{
    static int	RARPxTRANSxRARP_foreach [] = {1, 0, 1, 0, 0, 0};
    register int l, i;
    
    l = last_UARP_state;

    if (UARP_states_set == NULL) {
	UARP_states_set = sxba_calloc ((UARP_states_set_size = l) + 1);
	XH_alloc (&RARP_states_hd, "RARP_states_hd", xac2, 4, 4, NULL, statistics_file);
	XH_alloc (&RARP_items_hd, "RARP_items_hd", xac2, 4, 2, NULL, statistics_file);
	XxYxZ_alloc (&RARPxTRANSxRARP_hd, "RARPxTRANSxRARP", xac2, 4, RARPxTRANSxRARP_foreach, NULL, statistics_file);
	X_alloc (&X_RARP_new_trans_hd, "X_RARP_new_trans_hd", xac2, 4, NULL, statistics_file);
    }
    else {
	XH_clear (&RARP_states_hd);
	XH_clear (&RARP_items_hd);
	sxba_empty (UARP_states_set);
	XxY_clear (&RARPxTRANSxRARP_hd);

	if (UARP_states_set_size < l) {
	    UARP_states_set = sxba_resize (UARP_states_set, l + 1);
	    UARP_states_set_size = l;
	}
    }
}



static void UARP_gather ()
{
    /* On rassemble dans ws les feuilles de UARP. */
    /* On en profite pour RAZer Q0_to_occur_nb. */
    register int x, UARP_state;

    sxba_fill (UARP_states_set);

    for (x = XxYxZ_top (UARPxTRANSxUARP_hd); x > 0; x--) {
	UARP_state = XxYxZ_X (UARPxTRANSxUARP_hd, x);
	Q0_to_occur_nb [XxYxZ_Y (UARPxTRANSxUARP_hd, x)] = 0;
	/* permettra de compter le nb d'occur d'un etat du LR (0) dans les chemins. */
	SXBA_0_bit (UARP_states_set, UARP_state);
    }

    UARP_state = 0;

    while ((UARP_state = sxba_scan_reset (UARP_states_set, UARP_state)) > 0)
	SS_push (ws, UARP_state);
}

static int	RARP_install_items_set ()
{
    /* UARP_states_set contient un ensemble d'items, on les stocke dans RARP_hd */
    /* Un resultat negatif indique la presence d'un item final */
    register int	item = 0;
    int			result;
    bool		is_final = true;

    while ((item = sxba_scan_reset (UARP_states_set, item)) > 0) {
	if (item != 1)
	    is_final = false;

	XH_push (RARP_items_hd, item);
    }

    XH_set (&RARP_items_hd, &result);
    return is_final ? -result : result;
}


static	void RARP_next (RARP_state, trans)
    int		RARP_state, trans;
{
    /* Calcule l'etat atteint depuis RARP_state par transition sur trans. */
    register int	*bot, *ibot;
    register int	is, type_nb, x;
    int			*top, *itop, type, next_is, next_RARP_state, unused, UARP_state, pred,
                        new_trans;
    bool		not_empty;

    bot = RARP_STATES_BOT (RARP_state);
    top = RARP_STATES_TOP (RARP_state);
    type_nb = 0;

    while (bot < top) {
	is = *bot++;
	is = abs (is);
	ibot = RARP_ITEMS_BOT (is);
	itop = RARP_ITEMS_TOP (is);
	not_empty = false;

	while (ibot < itop) {
	    if ((UARP_state = *ibot++) != 1) {
		XxYxZ_Zforeach (UARPxTRANSxUARP_hd, UARP_state, x) {
		    if (XxYxZ_Y (UARPxTRANSxUARP_hd, x) == trans) {
			pred = XxYxZ_X (UARPxTRANSxUARP_hd, x);
			SXBA_1_bit (UARP_states_set, pred);
			not_empty = true;
		    }
		}
	    }
	}

	if (not_empty) {
	    if (++type_nb >= 2) {
		/* On empile le precedent */
		XH_push (RARP_states_hd, next_is);
		XH_push (RARP_states_hd, type);
	    }

	    /* On calcule le suivant */
	    next_is = RARP_install_items_set ();
	    /* Si next_is < 0, pour le type courant on n'a que l'etat initial de l'ARP. */
	    type = *bot;
	}

	bot++;
    }
    
    /* On empile les derniers */
    if (type_nb >= 2 || next_is > 0)
	XH_push (RARP_states_hd, next_is);

    XH_push (RARP_states_hd, type); /* Unique si type unique. */
    XH_set (&RARP_states_hd, &next_RARP_state);
    /* En fait dans RARP, les transitions sont des clones. */
    new_trans = Q0_to_occur_nb [trans]++ == 0 ? trans : new_clone (trans);
    XxYxZ_set (&RARPxTRANSxRARP_hd, RARP_state, new_trans, next_RARP_state, &unused);
}


static	void RARP_sort (RARP_state)
    int		RARP_state;
{
    /* Cette procedure regarde les transitions possibles depuis RARP_state.
       Lorsqu'elle en a trouve une nouvelle, elle appelle RARP_next. */
    register int	*bot, *ibot;
    register int	is, trans, x;
    int			*top, *itop, unused, UARP_state;

    X_clear (&X_RARP_new_trans_hd);
    bot = RARP_STATES_BOT (RARP_state);
    top = RARP_STATES_TOP (RARP_state);

    while (bot < top) {
	is = *bot++;
	is = abs (is);
	bot++;
	ibot = RARP_ITEMS_BOT (is);
	itop = RARP_ITEMS_TOP (is);

	while (ibot < itop) {
	    if ((UARP_state = *ibot++) != 1) {
		XxYxZ_Zforeach (UARPxTRANSxUARP_hd, UARP_state, x) {
		    trans = XxYxZ_Y (UARPxTRANSxUARP_hd, x);

		    if (!X_set (&X_RARP_new_trans_hd, trans, &unused)) {
			/* C'est la premiere fois qu'on voit trans. */
			RARP_next (RARP_state, trans);
		    }
		}
	    }
	}
    }
}


void	RARP_free ()
{
    /* Libere les structures UARP. */
    if (is_alloc_UARP_called) {
	XxY_free (&UARPxTRANSxUARP_hd);
    }

    if (ARP_to_UARP != NULL) {
	sxfree (ARP_states_set);
	sxfree (ARP_to_UARP), ARP_to_UARP = NULL;
    }

    if (UARP_states_set != NULL) {
	sxfree (UARP_states_set), UARP_states_set = NULL;
	XH_free (&RARP_states_hd);
	XH_free (&RARP_items_hd);
	XxYxZ_free (&RARPxTRANSxRARP_hd);
	X_free (&X_RARP_new_trans_hd);
    }
}



static void	RARP_init ()
{
    register int	x, UARP_state, is, lim;
    int			RARP_state;

    /* Les feuilles de UARP sont rassemblees dans ws . */
    for (lim = SS_top (ws), x = SS_bot (ws); x < lim; x++) {
	UARP_state = SS_get (ws, x);
	SXBA_1_bit (UARP_states_set, UARP_state);
	is = RARP_install_items_set ();
	XH_push (RARP_states_hd, is);
	XH_push (RARP_states_hd, UARP_state);
    }

    SS_clear (ws);
    XH_set (&RARP_states_hd, &RARP_state);
}



static void	RARP_construction ()
{
    register int	RARP_state;

    RARP_state = 0;

    while (++RARP_state < XH_top (RARP_states_hd)) {
	if (XH_X (RARP_states_hd, RARP_state + 1) - XH_X (RARP_states_hd, RARP_state) > 1) {
	    /* Pas etat terminal */
	    RARP_sort (RARP_state);
	}
    }
}    




void cloning ()
{
    /* UARP contient l'union des ARP. */
    /* On construit RARP le DFSA inverse correspondant. */
    register int	RARP_state, p, q, x, y;

    RARP_open ();
    SS_open (ws);
    UARP_gather ();
    RARP_init ();
    RARP_construction ();
    /* RARP contient le DFSA inverse de l'UARP pour les conflits d'interet. */
    /* Les transitions sont des clones. */
    /* On regarde tous les couples consecutifs de transitions du RARP.
       Normalement, l'ordre n'a pas d'importance.
       p designe un etat non clone a cette etape (ca peut etre un clone d'un
       coup precedent) et pi designe le ieme clone de p.
       ->X q est la transition entrante ds q.

                 couples                          action
		 (p, q)				  rien (normalement on a pi ->X q)
		 (p, qj)	  soit ->X q alors p ->X r est change en p ->X qj
		 (pi, qj)	  soit ->X q alors pi ->X r est change en pi ->X qj
		 (pi, q)			  rien (normalement on a pi ->X q)
    */

    RARP_state = 0;

    while (++RARP_state < XH_top (RARP_states_hd)) {
	XxYxZ_Xforeach (RARPxTRANSxRARP_hd, RARP_state, x) {
	    q = XxYxZ_Y (RARPxTRANSxRARP_hd, x);

	    XxYxZ_Zforeach (RARPxTRANSxRARP_hd, RARP_state, y) {
		p = XxYxZ_Y (RARPxTRANSxRARP_hd, y);
		create_clone (p, q);
	    }
	}
    }
}


void alloc_UARP ()
{
    static int	UARPxTRANSxUARP_foreach [] = {0, 0, 1, 0, 0, 0};

    /* Alloue ou raz les structures UARP (Union of ARPs). */
    if (!is_alloc_UARP_called) {
	is_alloc_UARP_called = true;
	XxYxZ_alloc (&UARPxTRANSxUARP_hd, "UARPxTRANSxUARP_hd", xac2, 4, UARPxTRANSxUARP_foreach, NULL, statistics_file);
    }
    else {
	XxY_clear (&UARPxTRANSxUARP_hd);
    }

    last_UARP_state = 1; /* etat initial. */
}


static void open_UARP ()
{
    /* Alloue ou raz les structures UARP (Union of ARPs) qui dependent de l'ARP courant. */
    register int l, i;
    
    l = XH_top (ARP_states_hd);

    if (ARP_to_UARP == NULL) {
	ARP_to_UARP = (int*) sxcalloc (l + 1, sizeof (int));
	ARP_states_set = sxba_calloc ((ARP_states_set_size = l) + 1);
    }
    else {
	sxba_empty (ARP_states_set);

	if (ARP_states_set_size < l) {
	    ARP_states_set = sxba_resize (ARP_states_set, l + 1);
	    ARP_to_UARP = (int*) sxrealloc (ARP_to_UARP, i + 1, sizeof (int));
	    ARP_states_set_size = l;
	}

	for (i = l ;i > 1; i--)
	    ARP_to_UARP [i] = 0;
    }
}


void build_UARP ()
{
    register int	i, ARP_state, next_ARP_state, x;
    int			trans, UARP_state, next_UARP_state, unused;

    /* UARP est l'automate qui represente l'union des langages definis par
       les ARP le constituant. Si M et M' sont deux ARP (donc deterministes)
       je ne sais pas, en "completant" M par M' fabriquer un automate de'terministe
       reconnaissant L(M) U L(M').
       Exemple L(M) = a {a} b et L(M') = a a a c
       Donc UARP est non deterministe (sans transition vide).
       Il est simplement forme par partage de l'etat initial entre les
       differents ARP le constituant et en renommant les autres etats. */

    open_UARP ();
    SS_open (ws);
    SXBA_1_bit (ARP_states_set, 1);
    SS_push (ws, 1);
    ARP_to_UARP [1] = 1; /* etats initiaux de l'ARP et de l'UARP. */

    while (!SS_is_empty (ws)) {
	ARP_state = SS_pop (ws);
	UARP_state = ARP_to_UARP [ARP_state];

	XxY_Xforeach (ARPxTRANS_hd, ARP_state, x) {
	    trans = XxY_Y (ARPxTRANS_hd, x);
	    next_ARP_state = ARPxTRANS_to_next [x];

	    if ((next_UARP_state = ARP_to_UARP [next_ARP_state]) == 0)
		next_UARP_state = ARP_to_UARP [next_ARP_state] = ++last_UARP_state;

	    XxYxZ_set (&UARPxTRANSxUARP_hd, UARP_state, trans, next_UARP_state, &unused);

	    if (SXBA_bit_is_reset_set (ARP_states_set, next_ARP_state))
		SS_push (ws, next_ARP_state);
	}
    }

    SS_close (ws);
}

