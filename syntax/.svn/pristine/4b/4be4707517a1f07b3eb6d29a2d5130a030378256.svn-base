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
/* 10-05-90 17:07 (pb):		Changement de la structure des bfsa et	*/
/*				des pss lorsque le type est FSA_ :	*/
/*				init {liste de couples} final type	*/
/************************************************************************/
/* 07-05-90 11:21 (pb):		Le sommet de chaque pss est la h_value	*/
/*				courante				*/
/************************************************************************/
/* 25-04-90 11:32 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define WHAT	"@(#)ARP.c	- SYNTAX [unix] - 1 Août 1991"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "ARP";

#include "P_tables.h"

#include "rlr_optim.h"

#include "SS.h"
#include "RLR.h"

extern SXBOOLEAN	is_old_clone ();
extern SXBOOLEAN	foreach_LA ();
extern SXVOID	ARP_conflict_messages ();
extern SXBOOLEAN	bh_ARC_one_LA ();
extern SXBOOLEAN	bs_ARC_one_LA ();

static SXBA	ARP_items_set;
static SXBOOLEAN	is_ARP_state_ambig, is_ARP_opened;
static int	current_ARP_state;

static X_header	X_ARP_new_trans_hd;

static SXVOID	oflw_ARP_items (old_size, new_size)
    int		old_size, new_size;
{
    ARP_items_set = sxba_resize (ARP_items_set, new_size + 1);
}
static SXVOID	oflw_ARPxTRANS (old_size, new_size)
    int		old_size, new_size;
{
    ARPxTRANS_to_next = (int*) sxrealloc (ARPxTRANS_to_next, new_size + 1,
	 sizeof (int));
}



static SXVOID	ARP_open ()
{
    if (!is_ARP_opened) {
	is_ARP_opened = SXTRUE;
	XH_alloc (&ARP_states_hd, "ARP_states_hd", xac2, 4, 4, NULL, statistics_file);
	XH_alloc (&ARP_items_hd, "ARP_items_hd", xac2, 4, 2, NULL, statistics_file);
	XxY_alloc (&ARPxTRANS_hd, "ARPxTRANS_hd", xac2, 4, 1, 0, oflw_ARPxTRANS, statistics_file);
	ARPxTRANS_to_next = (int*) sxalloc (XxY_size (ARPxTRANS_hd) + 1, sizeof (int));
	XxY_alloc (&ARP_ItemxItem_hd, "ARP_ItemxItem_hd", xac2, 4, 0, 0, NULL, statistics_file);
	X_alloc (&X_ARP_new_trans_hd, "X_ARP_new_trans_hd", xac2, 4, NULL, statistics_file);
    }
    else {
	XH_clear (&ARP_states_hd);
	XH_clear (&ARP_items_hd);
	sxba_empty (ARP_items_set);
	XxY_clear (&ARPxTRANS_hd);
	XxY_clear (&ARP_ItemxItem_hd);
    }
}


static SXBOOLEAN	ARP_is_final_state (ARP_state)
    int		ARP_state;
{
    /* Si l'etat ARP_state contient (au moins) un item_set negatif,
       il est final. */
    register int	*bot, *top;

    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);

    while (bot < top) {
	if (*bot < 0)
	    return SXTRUE;

	bot += 2 /* On saute le type */ ;
    }

    return SXFALSE;
}




static SXVOID	ARP_trans (item_init)
    int		item_init;
{
    /* Complete ARP_items_set avec la fermeture de la transition sur state de
       "item" (n, stateN) */
    register int	bfsa, *bot, *top, couple, p, q;
    int			n, state, item, final, unused;
    SXBOOLEAN		is_XQ0;

    n = XxY_X (ARP_items, item_init);
    state = XxY_Y (ARP_items, item_init);
    bfsa = XH_list_elem (bfsa_hd, n);
    bot = BFSA_BOT (bfsa);

    do {
	bfsa = *(++bot);
	final = *(bot + 1);

	if (bfsa != 0) {
	    /* bfsa non initial */
	    bot = BFSA_BOT (bfsa);
	    top = BFSA_TOP (bfsa);
	    
	    while (bot < top) {
		if (is_XQ0 = ((couple = *bot++) > Q0xQ0_top)) {
		    couple -= Q0xQ0_top;
		    q = XxY_Y (XQ0xXQ0_hd, couple);
		}
		else
		    q = XxY_Y (Q0xQ0_hd, couple);

		if (q == state) {
		    p = is_XQ0 ? XxY_X (XQ0xXQ0_hd, couple) : XxY_X (Q0xQ0_hd, couple);
		    XxY_set (&ARP_items, n, p, &item);
		    SXBA_1_bit (ARP_items_set, item);
		    XxY_set (&ARP_ItemxItem_hd, item_init, item, &unused);
		}
	    }
	}
	    
	if (final == state) {
	    /* L'etat sur lequel s'effectue la transition est un etat final du bfsa courant */
	    if ((bfsa = XH_list_elem (bfsa_hd, ++n)) <= 0) {
		/* item complet */
		XxY_set (&ARP_items, n, 0, &item);
		SXBA_1_bit (ARP_items_set, item);
		XxY_set (&ARP_ItemxItem_hd, item_init, item, &unused);
	    }
	    else {
		/* On change de "couche", final est de la forme (q, N), l'etat initial
		   de la nelle couche est de la forme (q, N') ou N peut etre different
		   de N' (on a meme q), on re'initialise donc state avec cette nelle valeur. */
		state = *(bot = BFSA_BOT (bfsa));
	    }
	}
	else
	    bfsa = 0;
    } while (bfsa != 0);	
}
    
    
    
static int	ARP_install_items_set ()
{
   /* ARP_items_set contient un ensemble d'items, on les stocke dans ARP_hd */
    /* Un resultat negatif indique la presence d'un item final */
    register int	item = 0;
    register SXBOOLEAN	is_final = SXFALSE;
    int		result;

    while ((item = sxba_scan_reset (ARP_items_set, item)) > 0) {
	XH_push (ARP_items_hd, item);

	if (XxY_Y (ARP_items, item) == 0)
	    is_final = SXTRUE;
    }

    XH_set (&ARP_items_hd, &result);
    return is_final ? -result : result;
}





static	SXVOID ARP_next (ARP_state, trans)
    int		ARP_state, trans;
{
    /* Calcule l'etat atteint depuis ARP_state par transition sur trans. */
    register int	*bot, *ibot;
    register int	is, type_nb, item, q;
    int			*top, *itop, type, next_is, next_ARP_state, xtrans;
    SXBOOLEAN		ARP_trans_called;

    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);
    type_nb = 0;

    while (bot < top) {
	is = *bot++;
	is = abs (is);
	ibot = ARP_ITEMS_BOT (is);
	itop = ARP_ITEMS_TOP (is);
	ARP_trans_called = SXFALSE;

	while (ibot < itop) {
	    if ((q = XxY_Y (ARP_items, item = *ibot++)) > xac2)
		q = XxY_X (Q0xN_hd, q - xac2);

	    if (q == trans) {
		ARP_trans (item);
		ARP_trans_called = SXTRUE;
	    }
	}

	if (ARP_trans_called) {
	    if (++type_nb >= 2) {
		/* On empile le precedent */
		XH_push (ARP_states_hd, next_is);
		XH_push (ARP_states_hd, type);
	    }

	    /* On calcule le suivant */
	    next_is = ARP_install_items_set ();
	    /* Next_is < 0 => contient un item final */
	    type = *bot;
	}

	bot++;
    }
    
    /* On empile les derniers */
    if (type_nb >= 2)
	XH_push (ARP_states_hd, next_is);

    XH_push (ARP_states_hd, type); /* Unique si type unique. */
    XH_set (&ARP_states_hd, &next_ARP_state);
    XxY_set (&ARPxTRANS_hd, ARP_state, trans, &xtrans);
    ARPxTRANS_to_next [xtrans] = next_ARP_state;
}


static	SXVOID ARP_sort (ARP_state)
    int		ARP_state;
{
    /* Cette procedure regarde les transitions possibles depuis ARP_state.
       Lorsqu'elle en a troube une nouvelle, elle appelle ARP_trans. */
    register int	*bot, *ibot;
    register int	is, trans;
    int			*top, *itop, unused;

    X_clear (&X_ARP_new_trans_hd);
    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);

    while (bot < top) {
	is = *bot++;
	is = abs (is);
	bot++;
	ibot = ARP_ITEMS_BOT (is);
	itop = ARP_ITEMS_TOP (is);

	while (ibot < itop) {
	    if ((trans = XxY_Y (ARP_items, *ibot++)) != 0) {
		/* Suppression des items finals. */
		if (trans > xac2)
		    trans = XxY_X (Q0xN_hd, trans - xac2);

		if (!X_set (&X_ARP_new_trans_hd, trans, &unused)) {
		    /* C'est la premiere fois qu'on voit trans. */
		    ARP_next (ARP_state, trans);
		}
	    }
	}
    }
}




static SXVOID	ARP_init (XARC, XARC_state)
    struct ARC_struct	*XARC;
    int		XARC_state;
{
    register int	x, y, fork, xk;
    register SXBA	pss_set;
    int		pss, ARP_state, bot, top, lim, is, bfsa, item;

    ARP_open ();

    for (xk = 0, lim = XH_X (XARC->Q_hd, XARC_state + 1), x = XH_X (XARC->Q_hd, XARC_state); x <
	 lim; x++, xk++) {
	y = XH_list_elem (XARC->Q_hd, x++);
	pss_set = pss_sets [xk];

	for (top = XH_X (fks_hd, y + 1), y = XH_X (fks_hd, y); y < top; y++) {
	    pss = XxY_Y (forks, fork = XH_list_elem (fks_hd, y));

	    if (SXBA_bit_is_set (pss_set, pss)) {
		bfsa = XxY_X (forks, fork);
		bot = XH_X (bfsa_hd, bfsa);
		bfsa = XH_list_elem (bfsa_hd, bot);
		/* etat initial du [sous-]bfsa en fond de pile */
		XxY_set (&ARP_items, bot, XH_list_elem (bfsa_hd, XH_X (bfsa_hd, bfsa)), &item);
		SXBA_1_bit (ARP_items_set, item);
	    }
	}

	is = ARP_install_items_set ();
	XH_push (ARP_states_hd, is);
	XH_push (ARP_states_hd, XH_list_elem (XARC->Q_hd, x));
    }

    XH_set (&ARP_states_hd, &ARP_state);
}


static SXVOID	PARTIAL_FSA_ARP_init (bfsa1, bfsa2)
    int		bfsa1, bfsa2;
{
    register int	bot, is, bfsa;
    int			item, ARP_state;

    ARP_open ();
    bot = XH_X (bfsa_hd, bfsa1);
    bfsa = XH_list_elem (bfsa_hd, bot);
    /* etat initial du [sous-]bfsa en fond de pile */
    XxY_set (&ARP_items, bot, XH_list_elem (bfsa_hd, XH_X (bfsa_hd, bfsa)), &item);
    SXBA_1_bit (ARP_items_set, item);
    is = ARP_install_items_set ();
    XH_push (ARP_states_hd, is);
    XH_push (ARP_states_hd, bfsa1);
    bot = XH_X (bfsa_hd, bfsa2);
    bfsa = XH_list_elem (bfsa_hd, bot);
    /* etat initial du [sous-]bfsa en fond de pile */
    XxY_set (&ARP_items, bot, XH_list_elem (bfsa_hd, XH_X (bfsa_hd, bfsa)), &item);
    SXBA_1_bit (ARP_items_set, item);
    is = ARP_install_items_set ();
    XH_push (ARP_states_hd, is);
    XH_push (ARP_states_hd, bfsa2);
    XH_set (&ARP_states_hd, &ARP_state);    
}



static SXBOOLEAN	ARP_is_ambiguous (XARC, XARC_state, ARP_state)
    struct ARC_struct	*XARC;
    int		ARP_state, XARC_state;
{
    /* ARP_state est conflictuel. S'il existe deux bs complets (de
       types differents) associes a des pss egaux, il y a ambiguite. */
    register int	item, pss, x, wst;
    register int	*bot, *top, *bot1, *top1;
    int		type, wsb, n_0;
    SXBOOLEAN	is_first_time, is_ambig;
    SXBA	or_set;

    n_0 = 0;
    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);

    while (bot < top) {
	if (*bot < 0 && ++n_0 > 1) {
	    break /* au moins deux */ ;
	}

	bot += 2 /* On saute le type */ ;
    }

    if (n_0 < 2)
	return SXFALSE;

    sxba_empty (and_set);
    sxba_empty (or_set = pss_sets [work_set_no]);
    is_first_time = SXTRUE;
    is_ambig = SXFALSE;
    bot = ARP_STATES_BOT (ARP_state);

    while (bot < top) {
	if (*bot < 0) {
	    type = *(bot + 1);
	    bot1 = ARP_ITEMS_BOT (-*bot);
	    top1 = ARP_ITEMS_TOP (-*bot);
	    SS_open (ws);

	    while (bot1 < top1) {
		item = *bot1++;

		if (XxY_Y (ARP_items, item) == 0)
		    n_to_forks (XARC, XARC_state, XxY_X (ARP_items, item) /* n */
			 , type);
	    }

	    wsb = SS_bot (ws);
	    wst = SS_top (ws);

	    if (!is_first_time) {
		while (--wst >= wsb) {
		    pss = XxY_Y (forks, SS_get (ws, wst));


/* Attention, pour un meme type, des forks differents peuvent
   avoir memes pss. */

		    if (SXBA_bit_is_set (or_set, pss)) {
			SXBA_1_bit (and_set, pss);
			is_ambig = SXTRUE;
		    }
		}

		wst = SS_top (ws);
	    }

	    while (--wst >= wsb) {
		pss = XxY_Y (forks, SS_get (ws, wst));
		SXBA_1_bit (or_set, pss);
	    }

	    is_first_time = SXFALSE;
	    SS_close (ws);
	}

	bot += 2 /* On saute le type */ ;
    }

    return is_ambig;
}




SXVOID	ARP_alloc ()
{
    /* Les "ARP_items" doivent survivre jusqu'a la sortie des messages. */
    XxY_alloc (&ARP_items, "ARP_items", 4 * xac2, 4, 0, 0, oflw_ARP_items, statistics_file);
    ARP_items_set = sxba_calloc (XxY_size (ARP_items) + 1);
}



SXVOID	ARP_realloc ()
{
    /* On peut etre sur ARP_realloc sans que ARP_items ait ete alloue */
    if (ARP_items.display != NULL)
	XxY_clear (&ARP_items);
    else
	ARP_alloc ();
}



SXVOID	ARP_free ()
{
    if (ARP_items_set != NULL) {
	XxY_free (&ARP_items);
	sxfree (ARP_items_set), ARP_items_set = NULL;
    }

    if (is_ARP_opened) {
	XH_free (&ARP_states_hd);
	XH_free (&ARP_items_hd);
	XxY_free (&ARPxTRANS_hd);
	sxfree (ARPxTRANS_to_next);
	XxY_free (&ARP_ItemxItem_hd);
	X_free (&X_ARP_new_trans_hd);
    }

    RARP_free ();
}


SXVOID call_ARP_conflict_messages (XARC, prev_XARC_state, t, next_XARC_state)
    struct ARC_struct	*XARC;
    int			prev_XARC_state, t, next_XARC_state;
{
    ARP_conflict_messages (XARC, prev_XARC_state, t, next_XARC_state, current_ARP_state,
			   is_ARP_state_ambig, and_set);

}



static SXVOID	ARP_construction ()
{
   /* Soit Li le langage defini par les bfsa associes au conflit de type typei
       sur Q0* (les elements du vocabulaire sont les ETATS de l'automate LR(0)).
       On peut cloner ssi il n'existe pas de chaine s de Li qui est un prefixe
       de Lj (i != j et i, j dans {1, 2, ...}). */

    /* DETECTION DU CLONAGE
       Chaque bfsa est une Expression Reguliere
       On construit l'AFD (appele ARP --- Automate Regulier des Piles)
       reconnaissant simultanement les Li (technique analogue a ce qui se
       fait dans LECL).
           - tout etat ne reconnaissant que des items de Li est un etat
	     final en IMPASSE
	   - tout etat comportant un item final de Li (etat 0 d'un bfsa
	     de Li) est final

       Si tout item final est en impasse => on peut cloner
       S'il existe un etat final qui n'est pas en impasse => non LR */

    /* Retourne le type du conflit */
    register int	ARP_state = 0;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1) {
	    /* Etat multiple */
	    if (!ARP_is_final_state (ARP_state))
		ARP_sort (ARP_state);
	}
    }
}    
    
    
int UNBOUNDED_ARP_construction (XARC, qtq, message_needed)
    struct ARC_struct	*XARC;
    int		qtq;
    SXBOOLEAN	message_needed;
{
    register int	XARC_state, ARP_state, x, fsa;
    int			conflict_kind, la_lgth;
    
    XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    XARC->is_ARP = SXTRUE;
    ARP_init (XARC, XARC_state);
    ARP_construction ();
    ARP_state = 0;
    conflict_kind = NO_CONFLICT_;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1 &&
	    ARP_is_final_state (ARP_state)) {
	    /* Non clonable, on prepare eventuellement les messages denoncant le conflit */
	    XARC->is_clonable = SXFALSE;
	    la_lgth = XARC->attr [XARC_state].lgth;

	    if (is_ARP_state_ambig = ARP_is_ambiguous (XARC, XARC_state, ARP_state))
		/* and_set contient la liste des pss incrimines */
		conflict_kind = AMBIGUOUS_;
	    else
		conflict_kind |= (la_lgth == -1 ? NOT_LR_ : NOT_LRk_);
	    
	    if (message_needed && should_print_conflict (conflict_kind)) {
		messages_mngr_alloc ();
		current_ARP_state = ARP_state;
		
		if (la_lgth == -1)
		    la_lgth = k_value;
		
		SS_open (ws);
		SS_push (ws, la_lgth);
		ARC_walk_backward (XARC, qtq, bh_ARC_one_LA, bs_ARC_one_LA);
		SS_close (ws);
		
		if (is_unique_derivation_per_conflict)
		    break;
	    }
	    else
		break;
	}
    }

    if (XARC->is_clonable)
	build_UARP ();

    return conflict_kind;
}


int TOTAL_FSA_ARP_construction (XARC, qtq)
    struct ARC_struct	*XARC;
    int		qtq;
{
    register int	XARC_state, ARP_state, x, fsa;
    int			conflict_kind, la_lgth;
    
    XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    XARC->is_ARP = SXTRUE;
    ARP_init (XARC, XARC_state);
    ARP_construction ();
    ARP_state = 0;
    conflict_kind = NO_CONFLICT_;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1 &&
	    ARP_is_final_state (ARP_state)) {
	    XARC->is_clonable = SXFALSE;
	    return NOT_RoxoLRk_;
	}
    }

    if (XARC->is_clonable)
	build_UARP ();

    return conflict_kind;
}


SXBOOLEAN PARTIAL_FSA_ARP_construction (XARC, bfsa1, bfsa2)
    struct ARC_struct	*XARC;
    int		bfsa1, bfsa2;
{
    register int	ARP_state, fsa;
    
    XARC->is_ARP = SXTRUE;
    PARTIAL_FSA_ARP_init (bfsa1, bfsa2);
    ARP_construction ();
    ARP_state = 0;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1 &&
	    ARP_is_final_state (ARP_state)) {
	    XARC->is_clonable = SXFALSE;
	    return SXTRUE;
	}
    }

    if (XARC->is_clonable)
	build_UARP ();

    return SXFALSE;
}
