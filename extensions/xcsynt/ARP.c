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

#if 0
static char	ME [] = "ARP";
#endif

#include "P_tables.h"
#include "csynt_optim.h"

char WHAT_XCSYNTARP[] = "@(#)SYNTAX - $Id: ARP.c 3652 2023-12-24 09:43:15Z garavel $" WHAT_DEBUG;

#include "SS.h"
#include "RLR.h"

extern void	ARP_conflict_messages (struct ARC_struct *XARC, 
				       SXINT prev_XARC_state, 
				       SXINT t, 
				       SXINT next_XARC_state, 
				       SXINT ARP_state,
				       bool is_ambig, 
				       SXBA ARP_conflict_messages_and_set);
extern bool	bh_ARC_one_LA (struct ARC_struct *ARC, SXINT qtq);
extern bool	bs_ARC_one_LA (struct ARC_struct *ARC, SXINT qtq);

static SXBA	ARP_items_set;
static bool	is_ARP_state_ambig, is_ARP_opened;
static SXINT	current_ARP_state;

static X_header	X_ARP_new_trans_hd;


static void	oflw_ARP_items (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    ARP_items_set = sxba_resize (ARP_items_set, new_size + 1);
}

static void	oflw_ARPxTRANS (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    ARPxTRANS_to_next = (SXINT*) sxrealloc (ARPxTRANS_to_next, new_size + 1,
	 sizeof (SXINT));
}



static void	ARP_open (void)
{
    if (!is_ARP_opened) {
	is_ARP_opened = true;
	XH_alloc (&ARP_states_hd, "ARP_states_hd", xac2, 4, 4, NULL, statistics_file);
	XH_alloc (&ARP_items_hd, "ARP_items_hd", xac2, 4, 2, NULL, statistics_file);
	XxY_alloc (&ARPxTRANS_hd, "ARPxTRANS_hd", xac2, 4, 1, 0, oflw_ARPxTRANS, statistics_file);
	ARPxTRANS_to_next = (SXINT*) sxalloc (XxY_size (ARPxTRANS_hd) + 1, sizeof (SXINT));
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


static bool	ARP_is_final_state (SXINT ARP_state)
{
    /* Si l'etat ARP_state contient (au moins) un item_set negatif,
       il est final. */
    SXINT	*bot, *top;

    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);

    while (bot < top) {
	if (*bot < 0)
	    return true;

	bot += 2 /* On saute le type */ ;
    }

    return false;
}




static void	ARP_trans (SXINT item_init)
{
    /* Complete ARP_items_set avec la fermeture de la transition sur state de
       "item" (n, stateN) */
    SXINT	bfsa, *bot, *top, couple, p, q;
    SXINT			n, state, item, final, unused;
    bool		is_XQ0;

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
		if ((is_XQ0 = ((couple = *bot++) > Q0xQ0_top))) {
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
    
    
    
static SXINT	ARP_install_items_set (void);


static	void ARP_next (SXINT ARP_state, SXINT trans)
{
    /* Calcule l'etat atteint depuis ARP_state par transition sur trans. */
    SXINT	*bot, *ibot;
    SXINT	is, type_nb, item, q;
    SXINT			*top, *itop, type, next_is, next_ARP_state, xtrans;
    bool		ARP_trans_called;

    sxinitialise(next_is); /* pour faire taire gcc -Wuninitialized */
    sxinitialise(type); /* pour faire taire gcc -Wuninitialized */
    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);
    type_nb = 0;

    while (bot < top) {
	is = *bot++;
	is = labs (is);
	ibot = ARP_ITEMS_BOT (is);
	itop = ARP_ITEMS_TOP (is);
	ARP_trans_called = false;

	while (ibot < itop) {
	    if ((q = XxY_Y (ARP_items, item = *ibot++)) > xac2)
		q = XxY_X (Q0xN_hd, q - xac2);

	    if (q == trans) {
		ARP_trans (item);
		ARP_trans_called = true;
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


static	void ARP_sort (SXINT ARP_state)
{
    /* Cette procedure regarde les transitions possibles depuis ARP_state.
       Lorsqu'elle en a troube une nouvelle, elle appelle ARP_trans. */
    SXINT	*bot, *ibot;
    SXINT	is, trans;
    SXINT			*top, *itop, unused;

    X_clear (&X_ARP_new_trans_hd);
    bot = ARP_STATES_BOT (ARP_state);
    top = ARP_STATES_TOP (ARP_state);

    while (bot < top) {
	is = *bot++;
	is = labs (is);
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




static SXINT	ARP_install_items_set (void)
{
   /* ARP_items_set contient un ensemble d'items, on les stocke dans ARP_hd */
    /* Un resultat negatif indique la presence d'un item final */
    SXINT	item = 0;
    bool	is_final = false;
    SXINT		result;

    while ((item = sxba_scan_reset (ARP_items_set, item)) > 0) {
	XH_push (ARP_items_hd, item);

	if (XxY_Y (ARP_items, item) == 0)
	    is_final = true;
    }

    XH_set (&ARP_items_hd, &result);
    return is_final ? -result : result;
}





static void	ARP_init (struct ARC_struct *XARC, SXINT XARC_state)
{
    SXINT	x, y, ARP_init_fork, xk;
    SXBA	pss_set;
    SXINT		pss, ARP_state, bot, top, lim, is, bfsa, item;

    ARP_open ();

    for (xk = 0, lim = XH_X (XARC->Q_hd, XARC_state + 1), x = XH_X (XARC->Q_hd, XARC_state); x <
	 lim; x++, xk++) {
	y = XH_list_elem (XARC->Q_hd, x++);
	pss_set = pss_sets [xk];

	for (top = XH_X (fks_hd, y + 1), y = XH_X (fks_hd, y); y < top; y++) {
	    pss = XxY_Y (forks, ARP_init_fork = XH_list_elem (fks_hd, y));

	    if (SXBA_bit_is_set (pss_set, pss)) {
		bfsa = XxY_X (forks, ARP_init_fork);
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


static void	PARTIAL_FSA_ARP_init (SXINT bfsa1, SXINT bfsa2)
{
    SXINT	bot, is, bfsa;
    SXINT			item, ARP_state;

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



static bool	ARP_is_ambiguous (struct ARC_struct *XARC, 
				  SXINT XARC_state, 
				  SXINT ARP_state)
{
    /* ARP_state est conflictuel. S'il existe deux bs complets (de
       types differents) associes a des pss egaux, il y a ambiguite. */
    SXINT	item, pss, wst;
    SXINT	*bot, *top, *bot1, *top1;
    SXINT		type, wsb, n_0;
    bool	is_first_time, is_ambig;
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
	return false;

    sxba_empty (and_set);
    sxba_empty (or_set = pss_sets [work_set_no]);
    is_first_time = true;
    is_ambig = false;
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
			is_ambig = true;
		    }
		}

		wst = SS_top (ws);
	    }

	    while (--wst >= wsb) {
		pss = XxY_Y (forks, SS_get (ws, wst));
		SXBA_1_bit (or_set, pss);
	    }

	    is_first_time = false;
	    SS_close (ws);
	}

	bot += 2 /* On saute le type */ ;
    }

    return is_ambig;
}




void	ARP_alloc (void)
{
    /* Les "ARP_items" doivent survivre jusqu'a la sortie des messages. */
    XxY_alloc (&ARP_items, "ARP_items", 4 * xac2, 4, 0, 0, oflw_ARP_items, statistics_file);
    ARP_items_set = sxba_calloc (XxY_size (ARP_items) + 1);
}



void	ARP_realloc (void)
{
    /* On peut etre sur ARP_realloc sans que ARP_items ait ete alloue */
    if (ARP_items.display != NULL)
	XxY_clear (&ARP_items);
    else
	ARP_alloc ();
}



void	ARP_free (void)
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


void call_ARP_conflict_messages (struct ARC_struct *XARC,
				 SXINT prev_XARC_state, 
				 SXINT t, 
				 SXINT next_XARC_state)
{
    ARP_conflict_messages (XARC, prev_XARC_state, t, next_XARC_state, current_ARP_state,
			   is_ARP_state_ambig, and_set);

}



static void	ARP_construction (void)
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
    SXINT	ARP_state = 0;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1) {
	    /* Etat multiple */
	    if (!ARP_is_final_state (ARP_state))
		ARP_sort (ARP_state);
	}
    }
}    
    
    
SXINT UNBOUNDED_ARP_construction (struct ARC_struct *XARC,
				  SXINT	qtq,
				  bool message_needed)
{
  SXINT	XARC_state, ARP_state;
    SXINT			conflict_kind, la_lgth;
    
    XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    XARC->is_ARP = true;
    ARP_init (XARC, XARC_state);
    ARP_construction ();
    ARP_state = 0;
    conflict_kind = NO_CONFLICT_;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1 &&
	    ARP_is_final_state (ARP_state)) {
	    /* Non clonable, on prepare eventuellement les messages denoncant le conflit */
	    XARC->is_clonable = false;
	    la_lgth = XARC->attr [XARC_state].lgth;

	    if ((is_ARP_state_ambig = ARP_is_ambiguous (XARC, XARC_state, ARP_state)))
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


SXINT TOTAL_FSA_ARP_construction (struct ARC_struct *XARC, SXINT qtq)
{
  SXINT	XARC_state, ARP_state;
  SXINT			conflict_kind;
    
    XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    XARC->is_ARP = true;
    ARP_init (XARC, XARC_state);
    ARP_construction ();
    ARP_state = 0;
    conflict_kind = NO_CONFLICT_;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1 &&
	    ARP_is_final_state (ARP_state)) {
	    XARC->is_clonable = false;
	    return NOT_RoxoLRk_;
	}
    }

    if (XARC->is_clonable)
	build_UARP ();

    return conflict_kind;
}


bool PARTIAL_FSA_ARP_construction (struct ARC_struct *XARC, SXINT bfsa1, SXINT bfsa2)
{
  SXINT	ARP_state;
    
    XARC->is_ARP = true;
    PARTIAL_FSA_ARP_init (bfsa1, bfsa2);
    ARP_construction ();
    ARP_state = 0;

    while (++ARP_state < XH_top (ARP_states_hd)) {
	if (XH_X (ARP_states_hd, ARP_state + 1) - XH_X (ARP_states_hd, ARP_state) > 1 &&
	    ARP_is_final_state (ARP_state)) {
	    XARC->is_clonable = false;
	    return true;
	}
    }

    if (XARC->is_clonable)
	build_UARP ();

    return false;
}
