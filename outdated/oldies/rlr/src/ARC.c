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
/* 22-03-93 14:45 (pb):		Modif de germe_to_gr_act ds le cas du	*/
/*				non determinisme.			*/
/************************************************************************/
/* 21-04-92 15:12 (pb):		Ajout de "Q_set" dans "ARC_struct"	*/
/************************************************************************/
/* 04-02-92 11:20 (pb):		Bug dans complete_germe_to_gr_act	*/
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

char WHAT[] = "@(#)ARC.c	- SYNTAX [unix] - 23 mars 1993";

static char	ME [] = "ARC";

#include "P_tables.h"

#include "rlr_optim.h"

#include "SS.h"
#include "RLR.h"


extern bool		erase_quads ();
extern bool		open_listing ();
extern void		solved_by_ARC ();
extern void		mm_process_one_LA ();

static SXBA		xpss_set;
static int		xpss_set_size;

static bool		is_cycle_on_reads_completed;
static int		*out_cycles;
static XH_header	out_cycle_hd;

static SXBA		is_cycle_on_pss_computed, is_cycle_on_pss;

static SXBA		*BS, *BS_plus;
static int		*BS_couples, BS_top;
static X_header		Q0_to_short_hd;

static int		StNt_orig, max_la, XE1_init;

static int		*arc_path_stack;
static SXBA		arc_path_set;
static int		QxTxQ_foreach [6], AxOxV_foreach [6];
static XxYxZ_header	QxTxQ_hd, AxOxV_hd;

static X_header		Q_hd;

static int		pss_work_set_m, pss_work_set_M;


static int		*arc, *arc_trans;
static XH_header	pss_list_hd;
static bool		IS_SHIFT_REDUCE, THERE_IS_AN_XPROD;;
static SXBA		CONFLICT_T_SET;

#define RLR_maj(x, n)	(++*(x) > lgt [n] ? RLR_sature (n) : 0)

#define wl_maj(p)	if(++((*(p))[0].lnk) > (*(p))[0].ss) \
                          *(p) = (struct wl*) sxrealloc (*(p),((*(p))[0].ss *= 2) + 1, sizeof (struct wl))

#define PssToIndex(s)	((pss_kind != FSA_) ? pss_hd.display [s + 1].X - pss_hd.display [s].X - 2: 0)


static void ARC_UNBOUNDED_Q_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARCs [ARC_UNBOUNDED].attr = (struct ARC_ATTR*) sxrealloc (ARCs [ARC_UNBOUNDED].attr, new_size + 1, sizeof (struct ARC_ATTR));
    ARCs [ARC_UNBOUNDED].Q_set = sxba_resize (ARCs [ARC_UNBOUNDED].Q_set, new_size + 1);
}
static void ARC_BOUNDED_or_FSA_Q_oflw (old_size, new_size)
    int		old_size, new_size;
{
    ARCs [ARC_BOUNDED_or_FSA].attr = (struct ARC_ATTR*) sxrealloc (ARCs [ARC_BOUNDED_or_FSA].attr, new_size + 1, sizeof (struct ARC_ATTR));
    ARCs [ARC_BOUNDED_or_FSA].Q_set = sxba_resize (ARCs [ARC_BOUNDED_or_FSA].Q_set, new_size + 1);
}

static void	oflw_pss_to_Next (old_size, new_size)
    int		old_size, new_size;
{
    pss_to_Next = (int*) sxrealloc (pss_to_Next, new_size + 1, sizeof (int));
}


static void	oflw_ibfsa (old_size, new_size)
    int		old_size, new_size;
{
    ibfsa_to_ate = (int*) sxrealloc (ibfsa_to_ate, new_size + 1, sizeof (int));
}


static void	oflw_fknot (old_size, new_size)
    int		old_size, new_size;
{
    fknot_to_ate = (int*) sxrealloc (fknot_to_ate, new_size + 1, sizeof (int));
}


static void	oflw_PSSxT (old_size, new_size)
    int		old_size, new_size;
{
    PSSxT_to_xpts = (struct PSSxT_to_xpts*) sxrealloc (PSSxT_to_xpts, new_size +
	 1, sizeof (struct PSSxT_to_xpts));
}



static void	oflw_pss (old_size, new_size)
    int		old_size, new_size;
{
    register int	i = new_size;

    pss_check_stack = (int*) sxrealloc (pss_check_stack, i + 1, sizeof (int));
    back_head = (int*) sxrealloc (back_head, i + 1, sizeof (int));

    while (i > old_size) {
	back_head [i] = 0;
	pss_check_stack [i--] = 0;
    }

    for (i = MAX_RED_PER_STATE + 2; i >= 0; i--)
	if (work_set == pss_sets [i])
	    break;

    pss_sets = sxbm_resize (pss_sets, MAX_RED_PER_STATE + 6, MAX_RED_PER_STATE +
	 6, new_size + 1);

    if (i >= 0)
	work_set = pss_sets [i];

    for (i = max_red_per_state + 1; i >= 0; i--)
	kas [i].pss_set = pss_sets [i];

    and_set = pss_sets [MAX_RED_PER_STATE + 2];
    is_cycle_on_pss_computed = pss_sets [MAX_RED_PER_STATE + 3];
    is_cycle_on_pss = pss_sets [MAX_RED_PER_STATE + 4];
    pss_work_set = pss_sets [MAX_RED_PER_STATE + 5];
}


void	ARC_allocate (ARC, kind, F_oflw1)
    struct ARC_struct	*ARC;
    int			kind;
    void		(*F_oflw1)();
{
    static int	ARCxTxARC_foreach [] = {1, 0, 1, 0, 0, 0};

    XH_alloc (&(ARC->Q_hd), "ARC_hd", xac2, 4, 4, F_oflw1, statistics_file);
    ARC->Q_set = sxba_calloc (XH_size (ARC->Q_hd) + 1);
    XxYxZ_alloc (&(ARC->QxTxQ_hd), "ARCxTxARC", xac2, 4, ARCxTxARC_foreach, NULL, statistics_file);
    ARC->Q_stack = SS_alloc (xac2);
    ARC->attr = (struct ARC_ATTR*) sxalloc (XH_size (ARC->Q_hd) + 1, sizeof (struct ARC_ATTR));
    ARC->is_XARC = kind == XARC_UNBOUNDED || kind == XARC_FSA ;

    if (ARC->is_XARC)
	ARC->wl = NULL;
    else {
	ARC->wl = (struct wl*) sxalloc (xac2 + 1, sizeof (struct wl));
	ARC->wl [0].lnk = 0;
	ARC->wl [0].ss = xac2;
    }

    ARC->whd = (struct whd*) sxcalloc (XTMAX + 1, sizeof (struct whd));
    ARC->t_set = sxba_calloc (XTMAX + 1);
    ARC->kind = kind;
    ARC->is_allocated = true;
}


static void	ARC_clear (ARC)
    struct ARC_struct	*ARC;
{
    XH_clear (&(ARC->Q_hd));
    sxba_empty (ARC->Q_set);
    XxYxZ_clear (&(ARC->QxTxQ_hd));
    SS_clear_ss (ARC->Q_stack);
    sxba_empty (ARC->t_set);
    ARC->is_initiated = false;
    ARC->conflict_t_set = NULL;
    ARC->ARC = NULL;
    ARC->is_ARP = false;
}


static void	ARC_deallocate (ARC)
    struct ARC_struct	*ARC;
{
    XH_free (&(ARC->Q_hd));
    sxfree (ARC->Q_set);
    XxYxZ_free (&(ARC->QxTxQ_hd));
    SS_free (ARC->Q_stack);
    sxfree (ARC->attr);

    if (ARC->wl != NULL)
	sxfree (ARC->wl), ARC->wl = NULL;

    sxfree (ARC->whd);
    sxfree (ARC->t_set);
    ARC->is_allocated = false;
}


static void pss_clear ()
{
    register int i;

    XxY_clear (&PSSxT_hd);
    XH_clear (&pss_hd);
    XH_clear (&pss_list_hd);

    for (i = MAX_RED_PER_STATE + 5; i >= 0; i--)
	sxba_empty (pss_sets [i]);

    XH_clear (&Next_hd);
    X_clear (&pss_to_Next_hd);

    for (i = XH_size (pss_hd); i >= 0; i--) {
	back_head [i] = 0;
    }
}

static void	ARC_reinit ()
{
    register int	i;

    if (couples_set != NULL) /* infinity */  {
	sxba_empty (couples_set);
    }

    back [0].lnk = 0;

    if (ibfsa_needed) {
	XH_clear (&ibfsa_hd);
	SS_clear_ss (ibfsa_stack);
    }

/*    pss_clear (); Fait actuellement ds tous les cas. */

    if (reads_cycle_sets != NULL)
	XH_clear (&fknot_hd);
}



static void	undo (old_xac2)
    int	old_xac2;
{
    /* Du fait du clonage, certaines transitions de l'automate LR(0) ont ete
       supprimees. Ces transitions supprimees sont dans couples_set */
    /* Toutes les structures doivent donc etre modifiees en consequence.
       Ca semble tres complique.
       Dans un premier temps on est brutal: tout est reinitialise!! */
    /* Les clones peuvent etre utilises lors de la construction a venir
       (sans repasser par LALR1()), les structures qui dependent de
       xac2 sont donc reallouees en consequence. XARC_resize sera peut
       etre aussi rappelle depuis LALR1() pour changer la taille de arc. */
    XARC_resize (old_xac2, 0);
}



static	free_1 ()
{
    if (ibfsa_stack != NULL) {
	SS_free (ibfsa_stack), ibfsa_stack = NULL;
	sxfree (ibfsa_to_ate);
	XH_free (&ibfsa_hd);
    }

    if (couples_set != NULL) {
	sxfree (couples_set), couples_set = NULL;
    }

    XARC_free ();
    messages_mngr_free ();
    earley_free ();
    first_free ();

    sxfree (back);
    XxY_free (&PSSxT_hd);
    sxfree (PSSxT_to_xpts);
    sxfree (back_head);
    sxfree (pss_check_stack);
    SS_free (ws);
    sxbm_free (pss_sets);
    XH_free (&pss_hd);
    XH_free (&Next_hd);
    sxfree (pss_to_Next);
    X_free (&pss_to_Next_hd);

    if (xpss_set != NULL) {
	sxfree (xpss_set);
    }

    if (BS != NULL) {
	sxbm_free (BS_plus);
	sxbm_free (BS), BS = NULL;
	X_free (&Q0_to_short_hd);
    }

    if (reads_cycle_sets != NULL) {
	sxfree (fknot_to_ate);
	XH_free (&fknot_hd);
    }

    if (is_cycle_on_reads_completed) {
	sxfree (out_cycles);
	XH_free (&out_cycle_hd);
    }

    XH_free (&pss_list_hd);
}



static int	RLR_sature (nbt)
    register int	nbt;
{
    register int	old_size, new_size;

    if (nbt > 0) {
	lgt [nbt] = new_size = 2 * (old_size = lgt [nbt]);

	if (statistics_file != NULL) {
	    sat_mess (ME, nbt, old_size, new_size);
	}
    }

    switch (nbt) {
    case 0:
	/* initialisations */
	back = (struct back*) sxalloc (lgt [3] + 1, sizeof (struct back));
	back [0].lnk = 0;
	break;

    case 3:
	back = (struct back*) sxrealloc (back, new_size + 1, sizeof (struct back
	     ));
	break;

    default:
	sxtrap (ME, "sature");
    }

    return nbt;
}



bool check_level_conformity (item, level)
    int item, level;
{
    /* Appele depuis earley.
       On s'apprete a stocker item (il repere le terminal t, look-ahead de niveau level).
       Regarde si item est l'un des items des etats sommet de pile des pss (eventuellement)
       stockes dans ws (voir mm_process...). */
    register int	x, xnucl, pssl, lim;
    register int	*bot, *top;

    if (!XX_is_pss_check)
	/* aucune verification de prevue... */
	return true;

    pssl = SS_get (ws, SS_top (ws) - level);

    for (lim = XH_X (pss_list_hd, pssl + 1), x = XH_X (pss_list_hd, pssl); x < lim ; x++) {
	if ((xnucl = Q0xV_to_Q0 [XxY_is_set (&Q0xV_hd, TopState (XH_list_elem (pss_list_hd, x)), bnf_ag.WS_INDPRO [item].lispro)]) < 0) {
	    if (-xnucl == item)
		return true;
	}
	else {
	    xnucl = XQ0_TO_Q0 (xnucl);
	    bot = XH_BOT (nucleus_hd, xnucl);
	    top = XH_TOP (nucleus_hd, xnucl);

	    do {
		if (*bot == item)
		    return true;
	    } while (++bot < top);
	}
    }

    return false;
}

bool check_bot_state_conformity (state, level)
    int state, level;
{
    /* Appele depuis earley.
       On vient de "remonter" dans l'automate LR(0) sur state. Cette remontee est compatible
       seulement si il existe un pss dont l'etat initial est state. */
    register int	x, pssl, lim;

    if (!XX_is_pss_check)
	/* aucune verification de prevue... */
	return true;

    pssl = SS_get (ws, SS_top (ws) - level);

    for (lim = XH_X (pss_list_hd, pssl + 1), x = XH_X (pss_list_hd, pssl); x < lim ; x++) {
	if (BotState (XH_list_elem (pss_list_hd, x)) == state)
	    return true;
    }

    return false;
}

static bool	is_new_pss (pss, state, index, next_state, new_pss)
    int		pss, state, index, next_state, *new_pss;
{
    /* Fabrique un pss a partir d'un ancien et next_state et le retourne
       dans new_pss.
       Retourne vrai si le stack_suffixe fabrique n'existait pas deja.
       une pile de hauteur h_value+1 doit etre utilisee si possible. */
    bool	is_knot = false;

    if (pss_kind == FSA_) {
	register int	bot;
	int	new_couple, start;
	bool		new_couple_pushed = false;

	start = pss == 0 ? state : XH_list_elem (pss_hd, bot = XH_X (pss_hd, pss
	     ));
	XH_push (pss_hd, start);
	new_couple = XxY_is_set (&Q0xQ0_hd, state, next_state);

	if (pss != 0) {
	    /* Il faut supprimer de pss tous les couples qui n'atteignent pas
	       state et ajouter (state, next_state) */
	    register int	x, top = XH_X (pss_hd, pss + 1) - 2;
	    int		y, couple, X, Y;

	    if ((x = top - (bot += 1)) > 0) {
		bool		done;

		sxba_empty (xpss_set);

		if (x > xpss_set_size) {
		    while ((xpss_set_size *= 2) < x)
			;

		    xpss_set = sxba_resize (xpss_set, xpss_set_size);
		}

		SXBA_1_bit (sxba_empty (LR0_states_set), state);

		do {
		    done = true;

		    for (x = bot; x < top; x++) {
			y = x - bot;

			if (!SXBA_bit_is_set (xpss_set, y)) {
			    couple = XH_list_elem (pss_hd, x);
			    Y = XxY_Y (Q0xQ0_hd, couple);

			    if (SXBA_bit_is_set (LR0_states_set, Y)) {
				SXBA_1_bit (xpss_set, y);
				X = XxY_X (Q0xQ0_hd, couple);
				SXBA_1_bit (LR0_states_set, X);
				done = false;
			    }
			}
		    }
		} while (!done);

		x = -1;

		while ((x = sxba_scan (xpss_set, x)) != -1) {
		    top = XH_list_elem (pss_hd, x + bot);

		    if (!new_couple_pushed && top > new_couple) {
			new_couple_pushed = true;
			XH_push (pss_hd, new_couple);
		    }

		    if (top != new_couple)
			XH_push (pss_hd, top);
		}
	    }
	}

	if (!new_couple_pushed)
	    XH_push (pss_hd, new_couple);
    }
    else /* pss_kind != FSA_ */	 {
	if (pss == 0) {
	    XH_push (pss_hd, state);
	}
	else {
	    register int	i, apss, knot;

	    if (pss_kind == UNBOUNDED_)
		i = 0;
	    else if ((i = index - h_value + 2) < 0)
		i = 0;
	    else if (i > 0)
		i--;

	    for (apss = XH_X (pss_hd, pss) + i; i <= index; i++) {
		if ((knot = XH_list_elem (pss_hd, apss++)) < 0)
		    is_knot = true;

		XH_push (pss_hd, knot);
	    }
	}
    }

    XH_push (pss_hd, next_state);
    XH_push (pss_hd, is_knot ? -2 : h_value);
    return !XH_set (&pss_hd, new_pss);
}



static void	complete_cycle_on_reads ()
{
   /* Construit les structures :
      out_cycles et out_cycle_hd qui, a chaque cycle nb associe
      out_cycles [nb] qui est l'identifiant dans la XH list out_cycle_hd
      du sous_ensemble de Q0xV designant les "sorties" de ce cycle. */
    /* Une sortie est une transition (element de Q0xV) hors du cycle,
       soit terminale, soit sur un non-terminal
       derivant dans le vide vers un etat non nul.
       Si si ->t, on memorise pss1,
       si si ->nt sj et sj non nul et nt =>+ epsilon */
    register int	x, StNt, StNt1, nt;
    int			lim;
    register SXBA	cycle_set, Q0xV_set;
    register struct Q0	*aQ0;

    out_cycles = (int*) sxalloc (reads_cycle_sets_line_nb, sizeof (int));
    XH_alloc (&out_cycle_hd, "out_cycle", reads_cycle_sets_line_nb, 1, XxY_top (Q0xV_hd) / xac2, NULL, statistics_file);
    Q0xV_set = sxba_calloc (XxY_size (Q0xV_hd) + 1);

    for (x = 1; x < reads_cycle_sets_line_nb; x++) {
	cycle_set = reads_cycle_sets [x];
	StNt = 0;

	while ((StNt = sxba_scan (cycle_set, StNt)) > 0) {
	    aQ0 = Q0 + XxY_X (Q0xV_hd, StNt);

	    if ((lim = aQ0->t_trans_nb) > 0) {
		/* transitions terminales */
		lim += (StNt1 = aQ0->bot);

		do {
		    SXBA_1_bit (Q0xV_set, StNt1);
		} while (++StNt1 < lim);
	    }

	    if ((lim = aQ0->nt_trans_nb) > 0) {
		/* transitions non_terminales */
		lim += (StNt1 = aQ0->bot + aQ0->t_trans_nb);

		do {
		    if (!SXBA_bit_is_set (cycle_set, StNt1)) {
			/* On ne conserve que les transitions hors du cycle. */
			nt = XxY_Y (Q0xV_hd, StNt1);

			if (SXBA_bit_is_set (bnf_ag.BVIDE, nt))
			    SXBA_1_bit (Q0xV_set, StNt1);
		    }
		} while (++StNt1 < lim);
	    }
	}

	StNt1 = 0;

	while ((StNt1 = sxba_scan_reset (Q0xV_set, StNt1)) > 0) {
	    XH_push (out_cycle_hd, StNt1);
	}

	XH_set (&out_cycle_hd, out_cycles + x);
    }

    sxfree (Q0xV_set);
}



static void	Next_with_fknot ();

static void	Next (top_state, pss)
    int		top_state, pss;
{
    /* pss est un suffixe de pile d'analyse */
    /* La pile parametre est memorisee, meme si son sommet n'a pas
       de transitions terminales work_set etant aussi utilise lors
       de sa construction pour detecter les chemins deja (ou en
       cours) traites */
    /* De plus, pour tous les items de topstate de la forme
       B -> phi . A psi et A =>+ vide et psi non vide, on calcule
       Next(goto(topstate, A)) */

    register int	StNt, nt, next_state, fknot;
    register struct Q0	*aQ0;
    int			lim;

    if (SXBA_bit_is_reset_set (pss_work_set, pss)) {
	pss_work_set_m = min (pss_work_set_m, pss);
	pss_work_set_M = max (pss_work_set_M, pss);
	
	if ((lim = (aQ0 = Q0 + top_state)->nt_trans_nb) > 0) {
	    /* Il y a des transitions non terminales */
	    lim += (StNt = aQ0->bot + aQ0->t_trans_nb);
	    
	    do {
		if ((next_state = Q0xV_to_Q0 [StNt]) > 0) {
		    /* vers un etat non vide. */
		    nt = XxY_Y (Q0xV_hd, StNt);
		    
		    if (SXBA_bit_is_set (bnf_ag.BVIDE, nt)) {
			
			
/* Si pss est UNBOUNDED_, il faut tester si la transition non terminale
   (top_state, nt) est impliquee dans un cycle de reads. Si oui soit
   fknot le cycle correspondant. On fabrique les piles pss1 = pss || -fknot || si
   ou si est un etat "sortie" de fknot. Une sortie est un etat du fknot ayant des
   transitions a l'exterieur du fknot, soit terminales, soit sur un non-terminal
   derivant dans le vide vers un etat non nul.
   Si si ->t, on memorise pss1,
   si si ->nt sj et sj non nul et nt =>+ epsilon, on appelle Next (pss1 || sj). */
			
			if (reads_cycle_sets != NULL && *(PSS_TOP (pss)) < 0 && (fknot = StNt_to_reads_cycle_nb [X_is_set (&StNt_reads_cycle_hd, StNt)]) != 0)
			    Next_with_fknot (pss, fknot);
			else
			    Next (next_state, make_new_pss (pss, next_state));
		    }
		}
	    } while (++StNt < lim);
	}
    }
}



static void	Next_with_fknot (pss, fknot)
    int pss, fknot;
{
    register int	bot1, x, next_state;
    register SXBA	out_set;
    int			out, pss1, bot, top, kn, top1, Q0V, state, V;

    if (!is_cycle_on_reads_completed) {
	complete_cycle_on_reads ();
	is_cycle_on_reads_completed = true;
    }

    bot = XH_X (pss_hd, pss);
    top = XH_X (pss_hd, pss + 1) - 1;

    if (top - bot > 2 && (kn = XH_list_elem (pss_hd, top - 2)) < 0) {
	/* On a un knot en sommet de pss, ce doit etre fknot */
	if (-kn != fknot)
	    sxtrap (ME, "Next_with_fknot");

	/* fusion */
	top -= 2;
    }

    out = out_cycles [fknot];
    bot1 = XH_X (out_cycle_hd, out);
    top1 = XH_X (out_cycle_hd, out + 1);

    while (bot1 < top1) {
	Q0V = XH_list_elem (out_cycle_hd, bot1++);
	state = XxY_X (Q0xV_hd, Q0V);
	V = XxY_Y (Q0xV_hd, Q0V);
	x = bot;

	while (x < top)
	    XH_push (pss_hd, XH_list_elem (pss_hd, x++));

	XH_push (pss_hd, -fknot);
	XH_push (pss_hd, state);

	if (V > 0) {
	    next_state = Q0xV_to_Q0 [Q0V];
	    XH_push (pss_hd, next_state);
	}

	XH_push (pss_hd, -2);
	XH_set (&pss_hd, &pss1);

	if (V < 0) {
	    if (SXBA_bit_is_reset_set (pss_work_set, pss1)) {
		pss_work_set_m = min (pss_work_set_m, pss1);
		pss_work_set_M = max (pss_work_set_M, pss1);
	    }
	}
	else
	    Next (next_state, pss1);
    }
}


static void call_Next (top_state, pss)
    int top_state;
    register int	pss;
{
    /* On memorise les resultats de l'application de Next sur un pss. */
    register int	x;
    int			lim, ptn, n;

    if (SXBA_bit_is_set (Next_states_set, top_state)) {
	if (X_set (&pss_to_Next_hd, pss, &ptn)) {
	    /* deja_calcule */
	    n = pss_to_Next [ptn];
	}
	else {
	    sxba_empty (pss_work_set);
	    pss_work_set_m = maxint;
	    pss_work_set_M = 0;
	    Next (top_state, pss);

	    for (pss = pss_work_set_m; pss != -1; pss = sxba_scan (pss_work_set, pss)) {
		XH_push (Next_hd, pss);

		if (pss == pss_work_set_M)
		    break;
	    }

	    XH_set (&Next_hd, &n);
	    pss_to_Next [ptn] = n;
	}

	for (lim = XH_X(Next_hd, n + 1), x = XH_X (Next_hd, n); x < lim; x++) {
	    pss = XH_list_elem (Next_hd, x);

	    if (SXBA_bit_is_reset_set (work_set, pss)) {
		work_kas->m = min (work_kas->m, pss);
		work_kas->M = max (work_kas->M, pss);
	    }
	}
    }
    else {
	if (SXBA_bit_is_reset_set (work_set, pss)) {
	    work_kas->m = min (work_kas->m, pss);
	    work_kas->M = max (work_kas->M, pss);
	}
    }
}


static int	ibfsa_do ()
{
    /* Fabrique un IncrementalBackwardFSA a partir de ibfsa_stack et
       retourne son identifiant. */
    register int	state, bot, top;
    int		result;

    bot = SS_bot (ibfsa_stack) - 1;
    top = SS_top (ibfsa_stack);

    while (bot < top) {
	state = SS_get (ibfsa_stack, bot++);
	XH_push (ibfsa_hd, state);
    }

    if (!XH_set (&ibfsa_hd, &result))
	ibfsa_to_ate [result] = 0;

    return result;
}



static void	put_newr_in_back (X1, X2)
    int		X1, X2;
{
    /* Ajoute en tete l'element (X1, X2) a la relation back
       et lie entre eux les elements ayant meme X1 */
    /* Il n'existe pas deja et X2 vient d'etre fabrique, il a donc une
       valeur superieure a tous les autres et peut donc etre mis en
       tete de la chaine des X2 sans verification */
    register int		xback, *aX1;
    register struct back	*aelem;

    RLR_maj (&(back [0].lnk), 3);
    aelem = back + (xback = back [0].lnk);
    aelem->X2 = X2;
    aelem->ibfsa = ibfsa_needed ? (SS_is_empty (ibfsa_stack) ? 0 : ibfsa_do ()) : 0;
    aelem->lnk = *(aX1 = back_head + X1);
    *aX1 = xback;
}



static void	put_r_in_back (X1, X2)
    int		X1;
    register int	X2;
{
    /* Ajoute l'element (X1, X2) --s'il n'existe pas deja-- a la relation back
       Les deuxiemes champs sont tries par valeur decroissante */

    register int	x, F2, y, *aX1;
    register struct back	*aelem;

    for (x = *(aX1 = back_head + X1), y = 0; x != 0; y = x, x = aelem->lnk) {
	if ((F2 = (aelem = back + x)->X2) <= X2) {
	    if (F2 == X2)
		return;

	    break;
	}
    }

    RLR_maj (&(back [0].lnk), 3);
    aelem = back + (back [0].lnk);
    aelem->X2 = X2;
    aelem->lnk = x;
    aelem->ibfsa = ibfsa_needed ? (SS_is_empty (ibfsa_stack) ? 0 : ibfsa_do ()) : 0;

    if (y == 0)
	*aX1 = back [0].lnk;
    else
	back [y].lnk = back [0].lnk;
}



static bool is_new_knoted_pss (pss, pred, index, next_state, pss1)
    int	pss, pred, index, next_state, *pss1;
{
    /* Fabrique un pss de type -2 (UNBOUNDED_ avec knot) */
    register int bot, top;

    bot = XH_X (pss_hd, pss);
    top = bot + index;

    while (bot <= top)
	XH_push (pss_hd, XH_list_elem (pss_hd, bot++));

    if (pred != 0)
	XH_push (pss_hd, pred);

    XH_push (pss_hd, next_state);
    XH_push (pss_hd, -2);
    return !XH_set (&pss_hd, pss1);
}


static void	compute_back (ppss, state, index, item, pss)
    int		ppss, state, index, item, pss;
{
    /* Cette fonction recursive calcule la relation binaire back entre
       des couples (pss, pss1) ou chaque pss est un suffixe de pile
       d'analyse.
       pss = (s1 ... si sj) back pss1 = (p1 ... pk pl) ou
       "nt -> . alpha" et "nt1 -> delta . nt gamma" sont dans si sous-sommet
       de pss si = goto*(pk, delta) "utilise" (s1 ... si),
       pl = goto (pk, nt1) et gamma =>* epsilon */
    /* Si h_value	ppss		index
            >0		0		etat
	    >0		>0		indice du sommet
	    0		0		etat initial = etat final
	    0		>0		etat final	*/

    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0
	 /* item du kernel A -> X1 ... Xn . alpha */ ) {
	/* Calcul des etats predecesseurs de state (sur Xn) */
	register int	x;
	int	start;

	if (ppss > 0) {
	    switch (*(PSS_TOP (ppss))) {
	    case 0 : /* FSA_ */
	        {
		    register int	bot, top, couple;
		    
		    top = XH_X (pss_hd, ppss + 1) - 2;
		    start = XH_list_elem (pss_hd, bot = XH_X (pss_hd, ppss));
		    
		    for (x = bot + 1; x < top; x++) {
			if (XxY_Y (Q0xQ0_hd, couple = XH_list_elem (pss_hd, x)) == state) {
			    compute_back (ppss, XxY_X (Q0xQ0_hd, couple),
							 0, item - 1, pss);
			}
		    }
		    
		    if (start == state)
			ppss = 0;
		}

		break;

	    case -2 : /* UNBOUNDED_ avec (peut etre) des knots */
	        {
		    register int	StNt;
		    register int	*as = PSS_BOT (ppss) + index;
		    register SXBA	cycle_set;

		    if (*as < 0 || index > 0 && *(as - 1) < 0) {
			if (*as > 0) {
			    /* index repere la sortie du knot */
			    --index;
			    --as;
			}
		    

/* On cherche tous les predecesseurs de state dans knot */
/* Quand les StNt seront des Q0xV, on pourra faire mieux. */
			cycle_set = reads_cycle_sets [-*as];
			StNt = 0;

			while ((StNt = sxba_scan (cycle_set, StNt)) > 0) {
			    if (Q0xV_to_Q0 [StNt] == state) {
				compute_back (ppss, XxY_X (Q0xV_hd, StNt), index, item
							     - 1, pss);
			    }
			}


			if (*(as - 1) != state)
			    break;
			
			/* On sort egalement du knot */
			index--;
			/* Ce ne peut etre la sortie d'un autre knot
			   on continue en sequence. */
		    }
		}

		/* pas de knot dans le coup on continue en sequence */

	    case -1 : /* UNBOUNDED_ sans knots */
	    default : /* BOUNDED_ */

		if (index > 0) {
		    state = XH_list_elem (pss_hd, XH_X (pss_hd, ppss) + --index);

		    compute_back (ppss, state, index, item
			 - 1, pss);
		}
		else
		    /* On est sorti de ppss */
		    ppss = 0;

		break;
	    }
	}

	if (ppss == 0) {
	    /* On est [egalement] sorti de ppss */
	    register int	pred;

	    XxY_Yforeach (Q0xQ0_hd, state, x) {
		pred = XxY_X (Q0xQ0_hd, x);

		if (ibfsa_needed)
		    SS_push (ibfsa_stack, pred);

		compute_back (0, pred, 0, item - 1, pss);

		if (ibfsa_needed)
		   SS_decr (ibfsa_stack);
	    }
	}
    }
    else /* item de la fermeture: A -> . beta */  {
	/* On ajoute (pss, pss1) a la relation back */
	register int *as;
	int	StNt, pss1, xp, next_state, nt, new_item;
	bool brand_new_pss;

	StNt = XxY_is_set (&Q0xV_hd, state, nt = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [
	     item].prolis].reduc/* A */);

/* ATTENTION, CHANGEMENT, si next_state est < 0, il designe un item. */
	next_state = Q0xV_to_Q0 [StNt];

	if (ppss > 0 && *(PSS_TOP (ppss)) == -2 && (*(as = PSS_BOT (ppss) + index) < 0 || index > 0 && *(as - 1) < 0)) {
	    /* index repere un knot ou sa sortie */
	    int pred;

	    if (*as > 0) {
		/* index repere la sortie du knot */
		--index;
		--as;
	    }

	    /* transition a l'interieur du knot */
	    pred = SXBA_bit_is_set (reads_cycle_sets [-*as], StNt) ? 0 : state;
	    brand_new_pss = is_new_knoted_pss (ppss, pred, index, next_state, &pss1);
	}
	else
	    brand_new_pss = is_new_pss (ppss, state, index, next_state, &pss1);

	if (brand_new_pss) {
	    /* C'est la premiere fois qu'on rencontre pss1
	       (pss, pss1) est obligatoirement nouveau
	       on calcule donc ses predecesseurs */
	    /* Recherche des items de l'etat state ayant le marqueur LR devant A */
	    register int	n = 0;

	    while ((new_item = nucl_i (next_state, ++n)) > 0) {
		/* nieme item du nucleus */
		new_item++;/* B -> delta A. gamma */

		if (SXBA_bit_is_set (bnf_ag.NULLABLE, new_item)) {
		    int		old_bot;

		    if (ibfsa_needed) {
			/* On initialise une nouvelle ibfsa_stack */
			old_bot = SS_bot (ibfsa_stack);
			SS_bot (ibfsa_stack) = SS_top (ibfsa_stack);
		    }

		    compute_back (ppss, state, index, new_item
			 - 1, pss1);

		    if (ibfsa_needed) {
			SS_top (ibfsa_stack) = SS_bot (ibfsa_stack);
			SS_bot (ibfsa_stack) = old_bot;
		    }
		}
	    }

	    put_newr_in_back (pss, pss1);
	}
	else
	    /* pss1 existe, ses "descendants" ont donc deja ete calcules
	       (ou sont en cours de calcul). On se contente de memoriser
	       l'element (pss, pss1) de la relation back. */
	    put_r_in_back (pss, pss1);
    }
}



static void	call_compute_back (ppss, state, index, item, pss)
    int		ppss, state, index, item, pss;
{
    if (ibfsa_needed) {
	SS_get (ibfsa_stack, 2) = ppss == 0 ? state : BotState (ppss);
	SS_top (ibfsa_stack) = SS_bot (ibfsa_stack) = 3;
    }

    compute_back (ppss, state, index, item, pss);
}



void	back_depth_first_traversal (X1)
    int		X1;
{
    /* Rappelons qu'un noeud est un suffixe de pile d'analyse. */
    /* Cette procedure recursive traverse en profondeur d'abord
       une fois et une seule chaque noeud du graphe induit par
       la relation back.
       Appelle Next sur chaque noeud non encore examine. */
    register int	x, X2, ts;

    for (x = back_head [X1]; x != 0; x = back [x].lnk) {
	if (pss_check_stack [X2 = back [x].X2] == 0
	     /* X2 n'est pas encore examine */ ) {
	    PUSH (pss_check_stack, X2);

	    if ((ts = TopState (X2)) > 0) {
		/* Transition vers un etat non nul */
		call_Next (ts, X2);
	    }

	    back_depth_first_traversal (X2);
	}
    }
}



bool is_pss_include (bot1, top1, bot2, top2)
    register int	*bot1, *bot2;
    int			*top1, *top2;
{
    /* On est dans le cas UNBOUNDED_ */
    /* boti..topi designent des pss */
    /* [bot1..top1[ ne contient pas de fknot
       [bot2..top2[ contient eventuellement des fknot
       Retourne true ssi 1 est reconnu par 2 */
    /* On suppose que si (... in fknot out q ...) est une portion de [bot2..top2[
       alors le couple (out, q) ne figure pas ds fknot. */
    register int	state, Q0V;
    register SXBA	cycle_set;

    while (bot2 < top2 && bot1 < top1) {
	if ((state = *bot2) < 0) {
	    /* fknot, -state designe un cycle de la relation reads */
	    cycle_set = reads_cycle_sets [-state];
	    state = *(bot2 - 1); /* == *(bot1 -1) */
	    /* On regarde si (state, ps_state) est une transition du fknot. */
	    Q0V = XxY_is_set (&Q0xV_hd, state, Q0_to_tnt_trans (*bot1));
	    
	    while (SXBA_bit_is_set (cycle_set, Q0V)) {
		/* On donne priorite au cycle (c'est peut etre suspect)
		   Faut-il a la fois rester ds le cycle et essayer d'en sortir? */
		state = *bot1;
		
		if (++bot1 == top1)
		    break;
		
		Q0V = XxY_is_set (&Q0xV_hd, state, Q0_to_tnt_trans (*bot1));
	    }
	    
	    if (state != *++bot2)
		return false;
	}
	else {
	    if (*bot1++ != state)
		return false;
	}
	
	bot2++;
    }
    
    if (bot2 == top2)
	return bot1 == top1;

    /* pss1 termine' et pss2 en cours.
       OK si in == out pour tous les fknot */
    state = *(bot1 - 1);

    do {
	if (*bot2 > 0 || *++bot2 != state)
	    return false;
    } while (++bot2 < top2);

    return true;
}



static void	pss_set_reduction (pss_set, m, M)
    register SXBA	pss_set;
    register int	*m, *M;
{
    /* On est ds le cas UNBOUNDED_ */
    /* card (pss_set) >= 2 */
    /* Supprime de pss_set les pss inclus dans les autres */
    /* On en profite pour supprimer les pss qui n'ont pas de transition terminale */
    /* Ca ne peut arriver que s'il y a des fknot */
    register int	pss, look_at = 0, low = 0, high;
    register SXBA	wset;
    int			wl, wh, wpss;

    for (pss = *m; pss != -1; pss = sxba_scan (pss_set, pss)) {
	if (Q0 [TopState (pss)].t_trans_nb != 0) {
	    /* Il y a des transitions terminales. */
	    /* il y a des fknot ds le coup si top == -2 */
	    look_at |= *(PSS_TOP (pss)) == -2 ? 1 : 2;
	    high = pss;

	    if (low == 0)
		low = pss;
	}
	else
	    SXBA_0_bit (pss_set, pss);

	if (pss == *M)
	    break;
    }

    *m = low;
    *M = high;

    if (look_at != 3)
	/* Pas de melange => pas de pb */
	return;

    /* On partitionne en deux */
    wset = pss_sets [work_set_no];
    sxba_empty (wset);
    wl = 0;
    low = 0;

    for (pss = *m; pss != -1; pss = sxba_scan (pss_set, pss)) {
	if (*(PSS_TOP (pss)) == -1) {
	    SXBA_0_bit (pss_set, pss);
	    SXBA_1_bit (wset, pss);
	    wh = pss;

	    if (wl == 0)
		wl = pss;
	}
	else {
	    high = pss;

	    if (low == 0)
		low = pss;
	}

	if (pss == *M)
	    break;
    }

    *m = 0;

    for (wpss = wl; wpss != -1; wpss = sxba_scan (wset, wpss)) {
	for (pss = low; pss != -1; pss = sxba_scan (pss_set, pss)) {
	    if (is_pss_include (PSS_BOT (wpss), PSS_TOP (wpss), PSS_BOT (pss), PSS_TOP (pss))) {
		SXBA_0_bit (wset, wpss);
		break;
	    }

	    if (pss == high) {
		/* On conserve wpss */
		if (*m == 0)
		    *m = wpss;

		*M = wpss;
		break;
	    }
	}

	if (wpss == wh)
	    break;
    }

    if (*m == 0) {
	*m = low;
	*M = high;
    }
    else {
	*m = min (*m, low);
	*M = max (*M, high);
	sxba_or (pss_set, wset);
    }
}


static void	put_in_work_set (xc)
    int		xc;
{
    SXBA_1_bit (work_set, xc);
    work_kas->m = min (work_kas->m, xc);
    work_kas->M = max (work_kas->M, xc);
}



static void	sim_a_red (pss, t)
    int		pss, t;
{
    /* pss designe un suffixe de pile d'analyse */
    /* Cette fonction simule toutes les reductions possibles depuis pss par
       transition sur t et memorise dans work_set les etats atteints */
    register int	next_state;
    int		indice, top_state;

    if (-t == bnf_ag.WS_TBL_SIZE.tmax) /* "END OF FILE" */  {
	/* On "retourne" le pss parametre */
	put_in_work_set (pss);
	return;
    }


/* On calcule cet ensemble dans work_set */

    indice = XxY_is_set (&Q0xV_hd, top_state = TopState (pss), -t);
    next_state = Q0xV_to_Q0 [indice];


/* On a une transition terminale sur t depuis top_state sur next_state, item
   est un item de top_state de la forme A -> alpha . t beta
   Si next_state==0 alors beta est vide et item est le seul de top_state de
   cette forme.
   Cette procedure va simuler (recursivement) toutes les reductions
   (eventuelles) qu'il est possible d'effectuer depuis next_state avec
   top_state comme sous sommet de pile, sans transition terminale et en
   memorisant dans work_set les etats atteints par ces reductions. */

    if (back_head [0] == 0)
	if (next_state > 0) {
	    register int	StNt;
	    register struct Q0	*aQ0 = Q0 + next_state;
	    int		lim, new_pss;

	    if ((lim = aQ0->red_trans_nb) > 0) {
		/* Ici on est sur que next_state est un etat conflictuel
		   au sens LR(0) du terme:
		   il y a au moins un reduce et autre chose. */
		register int	item;
		int		new_index;
		bool		is_first = true;


		lim += (StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);

		do {
		    item = -Q0xV_to_Q0 [StNt];
		    /* Deux cas possibles:
		       - item = A -> alpha t .
		       - item = A -> .
		    */

		    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0) {
			/* item du kernel A -> alpha t . */
			call_compute_back (pss, top_state, PssToIndex (pss
			     ), item - 1, 0);
		    }
		    else {
			if (is_first) {
			    is_first = false;
			    new_pss = make_new_pss (pss, next_state);
			    new_index = PssToIndex (new_pss);
			}

			call_compute_back (new_pss, next_state,
			     new_index, item, 0);
		    }
		} while (++StNt < lim);
	    }
	}
	else {
/* next_state < 0 . C'est un etat reduce ne comprenant que l'item
   A -> alpha t . */
	    call_compute_back (pss, top_state, PssToIndex (pss), -next_state, 0);
	}

    if (next_state > 0 && Q0 [next_state].t_trans_nb > 0)
	/* Il y a [au moins] une transition terminale depuis next_state */
	put_in_work_set (make_new_pss (pss, next_state));

    if (back_head [0] != 0) {
	CLEAR (pss_check_stack);
	back_depth_first_traversal (0);
    }
}


static void	couple_name (x, y, name)
    int		x, y;
    char	**name;
{
    /* Cette procedure "retourne" dans name le nom de la transition entre
       les etats (short) x et y.
       Si on a une transition de x a y, le nom de la transition est y.
       Le meme nom doit retourner le meme pointeur. */
    register int	ate;
    char	s [8];

    sprintf (s, "%d", X_X (Q0_to_short_hd, y));
    ate = sxstrsave (s);
    *name = sxstrget (ate);
}

static int ARC_store (arc_no, t_set, QxTxQ_hd)
    int		arc_no;
    SXBA	t_set;
    register XxYxZ_header	*QxTxQ_hd;
{
    /* arc_no est la racine d'un ARC
       on extrait de cet automate un automate dont les transitions
       initiales sont celles de t_set.
       Cet automate (p ->t q) est range dans des XxYxZ apres renommage
       des etats dans un X_header (Q_hd)
       L'etat initial est 1
       Les etats finals sont codes en negatif.
       L'etat initial est duplique s'il est reference car il n'est pas
       identique a l'etat LR(0) correspondant. */
    
    register int	xs, arc_s, x, arc_state;
    int			next, r, s, rts, xt, xs_top, top;

    X_clear (&Q_hd);
    XxYxZ_clear (QxTxQ_hd);
    X_set (&Q_hd, arc_no, &r); /* etat initial => r = 1. */
    SXBA_1_bit (arc_path_set, r);
    xs = 0;
    arc_path_stack [xs_top = 1] = arc_no;

    while (++xs <= xs_top) {
	arc_state = arc_path_stack [xs];
	r = X_is_set (&Q_hd, arc_state);
	arc_s = arc_state == grand ? arc_no : arc_state;

	for (top = arc [arc_s + 1], x = arc [arc_s]; x < top; x++) {
	    xt = SS_get (arc_trans, x++);
	    next = SS_get (arc_trans, x);

	    if (arc_state != arc_no || SXBA_bit_is_set (t_set, xt)) {
		if (next == arc_no)
		    /* reference a l'etat initial */
		    /* impossible ds la nelle implantation!. */
		    next = grand;

		if (next > 0)
		    X_set (&Q_hd, next, &s);
		else
		    s = next /* etat final */;

		XxYxZ_set (QxTxQ_hd, r, xt, s, &rts);

		if (next > 0 && SXBA_bit_is_reset_set (arc_path_set, s)) {
		    arc_path_stack [++xs_top] = next;
		}
	    }
	}
    }

    sxba_empty (arc_path_set);
    return xs_top;
}


static int complete_germe_to_gr_act (xac1)
    int xac1;
{
    /* L'etat xac1 n'a pas de reduce, on regarde recursivement
       ses successeurs a la recherche d'une action de reduction. */
    register int		next, gr_act;
    register struct P_item	*aitem, *aitem_top;
    register struct state	*astate;

    germe_to_gr_act [xac1] = -1; /* en cours d'examen */
    astate = t_states + xac1;
    aitem_top = (aitem = t_items + astate->start) + astate->lgth - 1 /* suppression du defaut */;

    for (; aitem < aitem_top; aitem++) {
	/* On n'a pas d'action reduce. */
	if (t_fe [aitem->action].codop == LaScanShift) {
	    next = t_fe [aitem->action].next;

	    if ((gr_act = germe_to_gr_act [next]) == 0)
		gr_act = complete_germe_to_gr_act (next);

	    if (gr_act > 0)
		break;
	}
    }

    return germe_to_gr_act [xac1] = gr_act;
}


static void	ARC_cg (xac1_init, new_state_nb, plulong)
    int		xac1_init, new_state_nb, plulong;
{
    /* Generation du code pour l'automate AxOxV
       L'etat initial de cet automate est 1, il sera code par la ligne xac1_init
       Les etats successeurs seront codes a partir de la ligne XE1. */
    /* Cet automate a new_state_nb etats significatifs.
       Dans l'etat initial, on ne genere pas la reduction plulong. */
    /* Le traitement d'erreur differe suivant que l'option is_non_deterministic_automaton
       est ou n'est pas positionnee (l'automate pouvant etre deterministe).
       Si is_non_deterministic_automaton, a chaque ARC, dans germe_to_gr_act, on associe
       une "ref" qui designe une sequence d'actions qui regroupe toutes les possibilites
       discriminees par cet ARC. Lorsqu'une erreur est detectee ds un ARC, le traitement
       consiste a retourner cette "ref" non-deterministe, le traitement se poursuit donc
       en exploitant en parallele les differentes possibilites. */

    /* Afin de permettre de la correction d'erreur en look-ahead, on ne genere
       pas d'instruction la plus populaire sur un ARC. */
    register int		x, a, next, xac1;
    int				xt, aov, pied, gr_act;
    register struct P_item	*aitem;
    register struct state	*astate;
    bool			is_germe_to_gr_act_completed = true;

    for (xac1 = xac1_init, a = 1; a <= new_state_nb; a++, xac1 = XE1 + a - 2) {
	astate = t_states + xac1;

	if (a > 1)
	    astate->start = xt_items;

	aitem = t_items + xt_items;
	gr_act = 0;

	XxYxZ_Xforeach (AxOxV_hd, a, aov) {
	    if ((xt = XxYxZ_Y (AxOxV_hd, aov)) > -bnf_ag.WS_TBL_SIZE.tmax) {
		xprdct_items++;
		xpredicates++ /* Approximatif ! */ ;
		    
		if (xt_state_set == NULL)
		    xt_state_set = sxba_calloc (xe1 + 1);
		    
		SXBA_1_bit (xt_state_set, xac1);
	    }

	    if ((next = XxYxZ_Z (AxOxV_hd, aov)) > 0 /* LaScanShift */ ) {
		next = next == 1 ? xac1_init : XE1 + next - 2;

		if (a == 1 && is_non_deterministic_automaton) {
		    /* Pour la recuperation d'erreur. */
		    /* Etat successeur d'une transition initiale depuis xac1_init. */
		    /* germe_to_gr_act [xac1] contiendra une ref vers la sequence
		       non deterministe de toutes les actions valides pour ce sous-ARC. */
		    germe_to_gr_act [next] = xac1_init;
		    xpredicates++;
		}

		t_fe->codop = LaScanShift;
		t_fe->reduce = 0;
		t_fe->pspl = 0;
		t_fe->next = next;
		aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
		}
	    else {
		if (is_non_deterministic_automaton)
		    xprdct_items++; /* pour la recuperation d'erreurs. */

		if ((next = -next) < grand) {
		    /* Resolution en faveur de Shift */
		    /* next est un StNt */
		    /* pied est l'etat LR(0) atteint depuis germe par transition sur t1 */
		    if ((pied = Q0xV_to_Q0 [next]) < 0) {
			if ((t_fe->reduce = bnf_ag.WS_INDPRO [-pied].prolis) == 0) {
			    /* Generation de l'instruction Halt */
			    t_fe->codop = Reduce;
			    t_fe->pspl = 2;
			    t_fe->next = 1;
			}
			else {
			    t_fe->codop = ScanReduce;
			    t_fe->pspl = LGPROD (t_fe->reduce) - 1;
			    t_fe->next = bnf_ag.WS_NBPRO [t_fe->reduce].reduc;
			    t_fe->next = XNT_TO_NT_CODE (t_fe->next);
			    t_fe->reduce = PROD_TO_RED (t_fe->reduce);
			    SXBA_0_bit (not_used_red_set, t_fe->reduce);
			}
		    }
		    else {
			t_fe->codop = ScanShift;
			t_fe->reduce = 0;
			t_fe->next = pied;
			t_fe->pspl = Q0 [pied].nt_trans_nb == 0 ? 0 : pied;
		    }
		}
		else {
		    /* Resolution en faveur de Reduce */
		    next -= grand;
			
		    if (a == 1 && next == plulong)
			    continue;
			
		    t_fe->codop = Reduce;
		    t_fe->pspl = LGPROD (next);
		    t_fe->next = bnf_ag.WS_NBPRO [next].reduc;
		    t_fe->next = XNT_TO_NT_CODE (t_fe->next);
		    t_fe->reduce = PROD_TO_RED (next);
		    SXBA_0_bit (not_used_red_set, t_fe->reduce);
		}
		    
		aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);

		if (t_fe->codop == Reduce)
		    gr_act = aitem->action;
	    }
		
	    aitem->check = xt;
	    aitem++;
	    xt_items++;
	    astate->lgth++;
	}

		
	/* Generation du defaut */
	
	if (a > 1) {
	    aitem->check = any;
	    aitem->action = t_error;

	    if (!is_non_deterministic_automaton && (germe_to_gr_act [xac1] = gr_act) == 0)
		is_germe_to_gr_act_completed = false;
	}
	/* else
	   germe_to_gr_act [xac1] = 0; par defaut */

	xt_items++;
	astate->lgth++;
    }

    if (!is_germe_to_gr_act_completed) {
	/* is_non_deterministic_automaton == false */
	for (x = xac1 - 1; x >= XE1; x--) {
	    if (germe_to_gr_act [x] == 0) {
		complete_germe_to_gr_act (x);
	    }
	}
    }

    XE1 = xac1;
}


static bool ARC_max_la (state, lgth)
    int state;
{
    /* Calcul de la longueur du plus long chemin. */
    /* La longueur du chemin courant entre l'etat initial et state est lgth. */
    /* Retourne true ssi une boucle a ete trouvee. */
    /* Les etats du chemin courant sont stockes dans arc_path_set. */
    register int aov, next;
    register bool is_loop = false;
   
    SXBA_1_bit (arc_path_set, state);
    lgth++;

    XxYxZ_Xforeach (AxOxV_hd, state, aov) {
	if ((next = XxYxZ_Z (AxOxV_hd, aov)) > 0) {
	    if (SXBA_bit_is_set (arc_path_set, next) || ARC_max_la (next, lgth)) {
		is_loop = true;
		break;
	    }
	}
	else /* Feuille */
	    max_la = max (max_la, lgth);
    }

    SXBA_0_bit (arc_path_set, state);
    return is_loop;
}



static bool	BS_couple (X, Y, n)
    int	*X, *Y, n;
{
    /* BS_couple est appele depuis XxY_to_SXBM pour traiter la fermeture
       transitive d'un sous-automate de Q0. */
    /* Remplit X et Y avec les elements du nieme couple de BS_couples. */
    /* BS_couples et BS_top ont ete initialises dans fill_BS_plus. */
    register int	couple;

    if (n > BS_top)
	return false;

    couple = BS_couples [n];
    *Y = XxY_Y (Q0xQ0_hd, couple);
    *X = XxY_X (Q0xQ0_hd, couple);
    return true;
}


static VARSTR	couples_to_re (bot, top, entree, sortie, vstr)
    register int	*bot, *top, entree, sortie;
    VARSTR		vstr;
{
    /* On suppose que bot < top (automate non vide) */
    /* On commence par remplir BS */
    register int	i;
    int			old_size, new_size, real_size;

    /* En faire une option (format liste de couples ou ER). */
    if (false) {
	BS_couples = bot - 1, XxY_to_SXBM (NULL, BS_couple, BS_top = top - bot, &Q0_to_short_hd, &BS);
	real_size = X_top (Q0_to_short_hd);
	
	if ((new_size = BASIZE (BS [0])) > (old_size = BS_plus == NULL ? 0 : BASIZE (BS_plus [0])))
	    if (old_size == 0)
		BS_plus = sxbm_calloc (new_size, new_size);
	    else
		BS_plus = sxbm_resize (BS_plus, old_size, new_size, new_size);
	
	for (i = 1; i <= real_size; i++)
	    sxba_copy (BS_plus [i], BS [i]);
	
	fermer (BS_plus, real_size + 1);
	return fsa_to_re (vstr, BS, BS_plus, real_size,
			  X_is_set (&Q0_to_short_hd, entree),
			  X_is_set (&Q0_to_short_hd, sortie),
			  couple_name);
    }
    else {
	register int couple;
	char	s [12];

	varstr_catenate (vstr, "{");

	do {
	    couple = *bot;
	    sprintf (s, "%d %d", XxY_X (Q0xQ0_hd, couple), XxY_Y (Q0xQ0_hd, couple));
	    varstr_catenate (vstr, s);

	    if (++bot < top)
		varstr_catenate (vstr, "|");
	    
	} while (bot < top);

	varstr_catenate (vstr, "} ");
	sprintf (s, "%d", sortie);
	return varstr_catenate (vstr, s);
    }
}



static bool	is_same_pss (xkas, pss_sets)
    int		xkas;
    SXBA	*pss_sets;
{
    /* Au retour and_set est l'union des intersections 2 a 2. */
    register int	x, y;
    register SXBA	pss_set, or_set;

    if (xkas > 2) {
	sxba_empty (and_set);
	or_set = pss_sets [work_set_no];

	for (x = xkas - 1; x > 0; x--) {
	    pss_set = pss_sets [x];

	    for (y = x - 1; y >= 0; y--) {
		sxba_or (and_set, sxba_and (sxba_copy (or_set, pss_set),
					   pss_sets [y]));
	    }
	}
    }
    else {
	sxba_and (sxba_copy (and_set, pss_sets [0]), pss_sets [1]);
    }

    return sxba_scan (and_set, 0) > 0;
}



bool	make_inter_pss (pss1, pss2)
    int pss1, pss2;
{
    /* On regarde si pss1 et pss2 de type FSA_ ont une intersection non vide. */
    register int	init, final;
    register int	*bot1, *top1, *bot2, *top2;

    /* On commence par regarder s'ils ont meme etat initial et meme etat final. */
    if (*(bot2 = PSS_BOT (pss2)) != (init = *(bot1 = PSS_BOT (pss1))) ||
	*(top2 = PSS_TOP (pss2) - 1) != (final = *(top1 = PSS_TOP (pss1) - 1)))
	return false;

    sxba_empty (LR0_states_set);
    SXBA_1_bit (LR0_states_set, init);
    make_inter (bot1 + 1, top1, bot2 + 1, top2, true, LR0_states_set);
    return SXBA_bit_is_set (LR0_states_set, final);
}


static bool is_cycle_on_fsa (pss)
    int	pss;
{
    /* pss est de type FSA_. */
    /* Si pss est sans cycle, alors il est equivalent a un nombre fini
       de pss UNBOUNDED_. */

    register int	*bot, *top;
    register int	i, real_size;
    register bool	is_cycle;

    if (SXBA_bit_is_set (is_cycle_on_pss_computed, pss))
	return SXBA_bit_is_set (is_cycle_on_pss, pss);


    /* On teste d'abord une condition suffisante :
          - pas de cycle trivial (s, s) et
	  - pas de couple (s, s1) et (s', s1). */
    bot = PSS_BOT (pss);
    top = PSS_TOP (pss) - 1;
    sxba_empty (LR0_states_set);
    SXBA_1_bit (LR0_states_set, *bot);

    while (++bot < top) {
	i = XxY_Y (Q0xQ0_hd, *bot);

	if (!SXBA_bit_is_reset_set (LR0_states_set, i))
	    break;
    }

    is_cycle = false;

    if (bot != top) {
	BS_couples = PSS_BOT (pss), XxY_to_SXBM (NULL, BS_couple, BS_top = (int)(top - BS_couples) - 1, &Q0_to_short_hd, &BS_plus);
	real_size = X_top (Q0_to_short_hd) + 1;
	fermer (BS_plus, real_size);

	for (i = 1; i < real_size;i++) {
	    if (SXBA_bit_is_set (BS_plus [i], i)) {
		is_cycle = true;
		break;
	    }
	}
    }
	
    SXBA_1_bit (is_cycle_on_pss_computed, pss);

    if (is_cycle)
	SXBA_1_bit (is_cycle_on_pss, pss);

    return is_cycle;
}



static int	is_inter_pss (xkas, pss_sets, is_all, is_cycle_needed)
    int		xkas;
    SXBA	*pss_sets;
    bool	is_all, is_cycle_needed;
{
    /* On est dans le cas pss_kind == FSA_.
       On regarde s'il existe deux pss associes a deux types differents
       ayant une intersection non vide. L'egalite a deja ete testee. */
    /* Si is_all, les tests 2 a 2 sont tous effectues et pss_sets est modifie
       en consequence. */
    /* Si is_cycle_needed, on teste en plus s'il existe un couple a intersection
       non vide dont aucun des composant n'a de cycle. */
    /* Valeur retournee :
          - 0	=> intersection vide
	  - 1	=> intersection non vide et pas de cycle.
	  - 2	=> intersection non vide. */
    register int	x, y, pss1, pss2;
    register SXBA	pss_set, pss_set1, pss_set2;
    bool		found, empty = true, is_couple_without_cycle = false, is_cycle_on_pss1;

    if (is_all)
	sxba_empty (pss_set = pss_sets [work_set_no]);

    /* Les candidats au test sont ceux ayant meme etat initial et meme etat final. */
    for (x = 0; x < xkas - 1;x++) {
	pss_set1 = pss_sets [x];
	pss1 = 0;

	while ((pss1 = sxba_scan (pss_set1, pss1)) > 0) {
	    found = false;

	    if (is_cycle_needed && !is_couple_without_cycle)
		is_cycle_on_pss1 = is_cycle_on_fsa (pss1);

	    for (y = x + 1; y < xkas;y++) {
		pss_set2 = pss_sets [y];
		pss2 = 0;

		while ((pss2 = sxba_scan (pss_set2, pss2)) > 0) {
		    if (make_inter_pss (pss1, pss2)) {
			if (!is_all) {
			    if (is_cycle_needed && (is_cycle_on_pss1 || is_cycle_on_fsa (pss2)))
				return 2;

			    return 1;
			}

			if (is_cycle_needed && !is_couple_without_cycle && !is_cycle_on_pss1 && !is_cycle_on_fsa (pss2))
			    is_couple_without_cycle = true;

			found = true;
			SXBA_1_bit (pss_set, pss2);
		    }
		}
	    }
		
	    if (found) {
		empty = false;
		SXBA_1_bit (pss_set, pss1);
	    }
	}
    }

    if (!is_all || empty)
	return 0;

    for (x = 0; x < xkas;x++)
	sxba_and (pss_sets [x], pss_set);

    return is_couple_without_cycle ? 1 : 2;
}



bool ARC_walk_backward (ARC, qtq, bh, bs)
    struct ARC_struct	*ARC;
    int		qtq;
    bool	(*bh)(), (*bs)();
{
    /* Cette fonction parcourt l'ARC "ARC" dans le sens retrograde.
       Elle appelle "bh" en visite heritee et "bs" en visite synthetisee.
       Si "bh" retourne "true", la visite du sous graphe continue.
       Si "bs" retourne faux, la visite est interrompue. */

    register int x = 0;

    if (bh (ARC, qtq)) {
	XxYxZ_Zforeach (ARC->QxTxQ_hd, XxYxZ_X (ARC->QxTxQ_hd, qtq), x) {
	    if (!ARC_walk_backward (ARC, x, bh, bs))
		break;
	}
    }

    return (bs == NULL || bs (ARC, qtq)) && x == 0;
}

static bool ARC_walk_forward (ARC, qtq, fh, fs)
    struct ARC_struct	*ARC;
    int			qtq;
    bool		(*fh)(), (*fs)();
{
    /* Cette fonction parcourt l'ARC "ARC" dans le sens usuel.
       Elle appelle "fh" en visite heritee et "fs" en visite synthetisee.
       Si "fh" retourne "true", la visite du sous graphe continue.
       Si "fs" retourne faux, la visite est interrompue. */

    register int x = 0;

    if (fh (ARC, qtq)) {
	XxYxZ_Xforeach (ARC->QxTxQ_hd, XxYxZ_Z (ARC->QxTxQ_hd, qtq), x) {
	    if (!ARC_walk_forward (ARC, x, fh, fs))
		break;
	}
    }

    return (fs == NULL || fs (ARC, qtq)) && x == 0;
}


static bool fh_propagate_cycle (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    register struct ARC_ATTR	*attr = ARC->attr + XxYxZ_Z (ARC->QxTxQ_hd, qtq);

    if (attr->lgth == -1)
	return false;

    attr->lgth = -1;
    return true;
}



static bool fh_propagate_lgth (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    register int		new_lgth, old_lgth;
    register struct ARC_ATTR	*attr = ARC->attr + XxYxZ_Z (ARC->QxTxQ_hd, qtq);

    /* is_marked est utilise comme un (petit) compteur.
       Les fonctions down... et up... doivent absolument faire des actions
       inverses. Comme up... decremente ds tous les cas, down... doit
       incrementer. Sinon(avec des booleens) ca ne marche pas sur les doubles
       boucles. */
    if (attr->is_marked++) {
	/* On vient de detecter un cycle */
	if (pss_kind == UNBOUNDED_ && attr->lgth != -1)
	    ARC_walk_forward (ARC, qtq, fh_propagate_cycle, NULL);

	return false;
    }

    new_lgth = ARC->attr [XxYxZ_X (ARC->QxTxQ_hd, qtq)].lgth;
    new_lgth = L_SUCC (new_lgth);
    old_lgth = attr->lgth;

    if (pss_kind != UNBOUNDED_ && old_lgth > new_lgth /* On propage la longueur minimale */ ||
	pss_kind == UNBOUNDED_ && old_lgth != -1 && old_lgth < new_lgth /* On propage la longueur maximale */) {
	attr->lgth = new_lgth;
	return true; /* On continue */
    }

    return false;
}

bool fs_unmark (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    --(ARC->attr [XxYxZ_Z (ARC->QxTxQ_hd, qtq)].is_marked);
    return true; /* On continue */
}


int ARC_UNBOUNDED_get_conflict_kind (ARC, ARC_state, is_new_arc_state, t)
    struct ARC_struct	*ARC;
    int			ARC_state, t;
    bool		is_new_arc_state;
{
    /* Soit S+ un pss commun a deux types differents (si pss_kind == FSA_,
       ce peut etre un chemin commun a deux FSA). On est sur qu'il existe une
       chaine c de T* qui est un suffixe correct d'un element du langage.
       Pour chacun des types, S+ va evidemment evoluer d'une maniere identique
       en reconnaissant c et, par consequent c appartient a l'intersection des
       deux automates et l'etat est impur et la grammaire non RLALR.
       Si pss_kind == UNBOUNDED_ || FSA_ la grammaire est donc non R(oxo) LALR. */

    register int	lgth, ck;
    register struct ARC_ATTR	*attr;

    attr = ARC->attr + ARC_state;
    ck = attr->conflict_kind;

    if (t == bnf_ag.WS_TBL_SIZE.tmax) {
	/* ARC_state est mixte et atteint par transition sur eof, il est donc
	   conflictuel. */
	ck |= NOT_RLALR_;
    }
    else
     {
	/* Si on est en LR(k), l'etat courant peut ne pas etre LALR(h), h < k car il
	   possede deux pss identiques, il faut cependant continuer si possible (le
	   nouvel etat n'est pas atteint par transition sur eof) la generation de
	   l'ARC jusqu'a atteindre la longueur k, sinon on pourrait ne detecter que
	   du non LR(h). */
	/* En fait, si 2 pss sont identiques, les suffixes des bfsa construits a partir
	   de la (si on poursuit la construction) seraient identiques donc si
	   non LR(h) => non LR (k). */
	/* Apres reflexion, dans le cas UNBOUNDED_ et is_lr_constructor, on continue
	   jusqu'a la longueur k.
	   Cette partie de la procedure detecte les conflits du LALR, c'est donc de la
	   responsabilite de la boucle de construction de l'ARC de savoir si'il faut
	   continuer a calculer les descendants des etats impurs. */
	/* Si les bfsa associes aux pss non egaux ne peuvent
	   pas se cloner, pas la peine de continuer, meme si les bfsa associes aux
	   pss egaux sont clonables. */
	/* fill_kas : quand ne conserve-t-on que les pss identiques en UNBOUNDED_ ?
	   - si not is_lr_constructor (qq soit h). */

	register int	lgth = attr->lgth;

	if (lgth == -1)
	    ck |= NOT_LALR_;
	else if (lgth >= k_value)
	    ck |= NOT_LALRk_;
	
	if (is_same_pss (XH_list_lgth (ARC->Q_hd, ARC_state) >> 1, pss_sets))
	    ck |= NOT_RLALR_;
    }

    return attr->conflict_kind = ck;
}


static int	ARC_BOUNDED_or_FSA_get_conflict_kind (ARC, ARC_state, is_new_arc_state, t)
    struct ARC_struct	*ARC;
    int			ARC_state, t;
    bool		is_new_arc_state;
{
    /* Soit S+ un pss commun a deux types differents (si pss_kind == FSA_,
       ce peut etre un chemin commun a deux FSA). On est sur qu'il existe une
       chaine c de T* qui est un suffixe correct d'un element du langage.
       Pour chacun des types, S+ va evidemment evoluer d'une maniere identique
       en reconnaissant c et, par consequent c appartient a l'intersection des
       deux automates et l'etat est impur et la grammaire non RLALR.
       Si pss_kind == UNBOUNDED_ || FSA_ la grammaire est donc non R(oxo) LALR. */

    register int	lgth, ck, type_nb, pss;
    register struct ARC_ATTR	*attr;

    attr = ARC->attr + ARC_state;
    ck = attr->conflict_kind;

    if (!is_new_arc_state)
	return ck;

    if (t == bnf_ag.WS_TBL_SIZE.tmax) {
	/* ARC_state est mixte et atteint par transition sur eof, il est donc
	   conflictuel. */
	/* Dans le cas FSA_, on pourrait etre plus precis en utilisant is_same_pss
	   ou is_inter_pss. */
	ck |= (pss_kind == BOUNDED_ ? NOT_RhLALR0_ : NOT_RoxoLALR0_);
    }
    else {
	if (is_same_pss (type_nb = XH_list_lgth (ARC->Q_hd, ARC_state) >> 1, pss_sets)) {
	    if (pss_kind == FSA_) {
		/* On regarde si un des pss communs n'a pas de cycle. */
		/* Serait-il fin de noter l'existence de cycles au fur et a
		   mesure de la construction des pss? */
		pss = 0;
		
		while ((pss = sxba_scan (and_set, pss)) > 0) {
		    if (!is_cycle_on_fsa (pss)) {
			ck |= NOT_RLALR_;
			break;
		    }
		}
	    }
	    else
		ck |= (pss_kind == BOUNDED_ ? NOT_RhLALR0_ : NOT_RoxoLALR0_);
	}
	else
	    if (pss_kind == FSA_ && (pss = is_inter_pss (type_nb, pss_sets, false, true)))
		/* Valeur retournee :
		   - 0	=> intersection vide
		   - 1	=> intersection non vide et pas de cycle.
		   - 2	=> intersection non vide. */
		ck |= (pss == 1 ? NOT_RLALR_ : NOT_RoxoLALR0_);
    }
    
    if (ck == NO_CONFLICT_ && k_value > 0) {
	/* On regarde si la distance min n'a pas depassee le max autorise. */
	if (attr->lgth >= k_value)
	    ck |= pss_kind == FSA_ ? NOT_RoxoLALRk_ : NOT_RhLALRk_;
    }

    return attr->conflict_kind = ck;
}


void	ts_to_kas (xpts, akas)
    int			xpts;
    register struct kas	*akas;
{
    register int	x, lim, V;
    register SXBA	pss_set = akas->pss_set;

    lim = XH_X (pss_list_hd, xpts + 1);
    V = XH_list_elem (pss_list_hd, x = XH_X (pss_list_hd, xpts));
    akas->m = min (akas->m, V);
    SXBA_1_bit (pss_set, V);

    while (++x < lim) {
	V = XH_list_elem (pss_list_hd, x);
	SXBA_1_bit (pss_set, V);
    }

    akas->M = max (akas->M, V);
}

void	print_bfsa (bfsa, ate)
    int		bfsa, ate;
{
    char	s [8];

    sprintf (s, "<%d>=", bfsa);
    put_edit_apnnl (s);
    put_edit_ap (sxstrget (ate));
}


static bool	make_fknot (e, s, nb, result)
    int		e, s, nb, *result;
{
    /* Fabrique un triplet (e, s, nb) ou e et s sont
       l'entree et la sortie du fknot nb. */
    /* Retourne true ssi c'est un nouveau fknot */

    XH_push (fknot_hd, e);
    XH_push (fknot_hd, s);
    XH_push (fknot_hd, nb);
    return !XH_set (&fknot_hd, result);
}



static VARSTR	fknot_to_vstr (vstr, entree, sortie, nb)
    VARSTR	vstr;
    int		entree, sortie, nb;
{
    /* nb designe un cycle de la relation reads (entree, nb, sortie)
       ou nb est un ibfsa dont les elements sont des couples. */
    register int	*bot;
    register SXBA	cycle_set;
    register int	StNt, prev_lgth;
    int		indice, fknot;

    if (make_fknot (entree, sortie, nb, &fknot)) {
	/* Nouveau fknot */
	prev_lgth = varstr_length (vstr);


/* Pour l'instant, on en fait des couples. */
	cycle_set = reads_cycle_sets [nb];
	StNt = 0;

	if (couples_set == NULL) {
	    couples_set = sxba_calloc (Q0xQ0_top + 1);
	    LR0_states_set = LR0_sets [5];
	}

	while ((StNt = sxba_scan (cycle_set, StNt)) > 0) {
	    indice = XxY_is_set (&Q0xQ0_hd, XxY_X (Q0xV_hd, StNt), Q0xV_to_Q0 [StNt]);
	    XH_push (pss_hd, indice);
	}

	XH_set (&pss_hd, &indice);
	vstr = couples_to_re (PSS_BOT (indice), PSS_TOP (indice) + 1, entree, sortie, vstr);
	fknot_to_ate [fknot] = sxstrsave (varstr_tostr (vstr) + prev_lgth);
    }
    else
	vstr = varstr_catenate (vstr, sxstrget (fknot_to_ate [fknot]));

    return vstr;
}



void	print_pss (pss)
    int		pss;
{
    register int	*bot, *top;
    char	s [8];

    sprintf (s, "%d=", pss);
    put_edit_apnnl (s);
    varstr_raz (vstr);
    bot = PSS_BOT (pss);
    top = PSS_TOP (pss);
    
    if (*top == 0) {
	register int	init, final;
	/* cas FSA_ */
	top--;
	
	init = *bot;
	final = *top;
	sprintf (s, "%d", init);
	varstr_catenate (vstr, s);
	
	if (++bot < top) {
	    /* Bfsa_Or_Pss non trivial */
	    varstr_catenate (vstr, " ");
	    vstr = couples_to_re (bot, top, init, final, vstr);
	}
    }
    else /* pss_kind != FSA_ */  {
	register int	state;
	while (bot < top) {
	    if ((state = *bot) < 0) {
		/* [f]knot */
		fknot_to_vstr (vstr, *(bot - 1), *(bot + 1), -state);
		bot++;
	    }
	    else {
		sprintf (s, "%d", *bot);
		varstr_catenate (vstr, s);
	    }
	    
	    if (++bot < top)
		varstr_catenate (vstr, *bot < 0 ? " " : "-");
	}
    }
    
    put_edit_ap (varstr_tostr (vstr));
}

static void	ARC_print (ARC)
    struct ARC_struct	*ARC;
{
    register int	ARC_state, xas, xts, pss;
    int			lim1, lim2, lim3, xpts, t, type, next_ARC_state;
    static char	fsa_ [] = "fsa";
    static char	bounded_ [] = "bounded";
    static char	unbounded_ [] = "unbounded";
    char	*s, string [8], c_name [24];

    if (ARC->is_XARC) {
	XARC_print (ARC);
	return;
    }

    if (reads_cycle_sets != NULL) {
	XH_alloc (&fknot_hd, "fknot_hd", xac2, 4, 3, oflw_fknot, statistics_file);
	fknot_to_ate = (int*) sxalloc (XH_size (fknot_hd) + 1, sizeof (int));
    }

    put_edit_nl (3);

    get_constructor_name (c_name, true, ARC->pss_kind != UNBOUNDED_, ARC->h_value, ARC->k_value);
    put_edit_nnl (32, c_name);
    put_edit_ap ("   C O N S T R U C T I O N");
    put_edit_nl (1);
    put_edit_nnl (32, "*** ");
    put_edit_apnnl (ARC->conflict_kind == NO_CONFLICT_ ? "Pure " : "Mixed ");

    if (ARC->pss_kind == FSA_)
	s = fsa_;
    else if (ARC->pss_kind == BOUNDED_)
	s = bounded_;
    else
	s = unbounded_;

    put_edit_apnnl (s);
    put_edit_apnnl ("-ARC whose seed is ");
    put_edit_apnb (germe);
    put_edit_ap (" ***");
    put_edit_nl (2);
    put_edit (1, "Parse Stack Suffixes list");
    sxba_empty (and_set);

    /* On parcourt tous les pss de l'ARC courant */
    SS_clear (ARC->Q_stack);
    ARC->attr [1].is_marked = true;
    SS_push (ARC->Q_stack, 1); /* Etat initial */
    
    while (!SS_is_empty (ARC->Q_stack)) {
	ARC_state = SS_pop (ARC->Q_stack);

	if ((lim2 = XH_X (ARC->Q_hd, ARC_state + 1)) - (xas = XH_X (ARC->Q_hd, ARC_state)) > 1) {
	    do {
		xpts = XH_list_elem (ARC->Q_hd, xas++); /* On saute le type. */
		lim3 = XH_X (pss_list_hd, xpts + 1);
		
		if (ARC_state == 1)
		    lim3--; /* On a ajoute un pss bidon (0) ds l'etat initial */
		
		for (xts = XH_X (pss_list_hd, xpts); xts < lim3; xts++) {
		    pss = XH_list_elem (pss_list_hd, xts);
		    SXBA_1_bit (and_set, pss);
		}
	    } while (++xas < lim2);

	    XxYxZ_Xforeach (ARC->QxTxQ_hd, ARC_state, xts) {
		next_ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, xts);

		XxYxZ_Xforeach (ARC->QxTxQ_hd, next_ARC_state, pss) {
		    break;
		}

		if ((ARC->attr [next_ARC_state].conflict_kind != NO_CONFLICT_ || pss != 0) &&
		    !ARC->attr [next_ARC_state].is_marked) {
		    ARC->attr [next_ARC_state].is_marked = true;
		    SS_push (ARC->Q_stack, next_ARC_state);
		}
	    }
	}
    }

    pss = 0;

    while ((pss = sxba_scan (and_set, pss)) > 0) {
	print_pss (pss);
    }

    for (lim1 = XH_top (ARC->Q_hd), ARC_state = 1; ARC_state < lim1; ARC_state++) {
	if (ARC->attr [ARC_state].is_marked) {
	    lim2 = XH_X (ARC->Q_hd, ARC_state + 1);
	    xas = XH_X (ARC->Q_hd, ARC_state);
	    put_edit_nl (3);
	    put_edit_nnl (1, "STATE ");
	    put_edit_apnb (ARC_state);
	    
	    if (ARC->attr [ARC_state].conflict_kind != NO_CONFLICT_)
		put_edit_apnnl (" (conflict)");
	    
	    do {
		xpts = XH_list_elem (ARC->Q_hd, xas++); /* On saute le type. */
		lim3 = XH_X (pss_list_hd, xpts + 1);
		
		if (ARC_state == 1)
		    lim3--; /* On a ajoute un pss bidon (0) ds l'etat initial */
		
		if ((type = -XH_list_elem (ARC->Q_hd, xas) - grand) <= 0)
		    put_edit_nnl (1, "Shift");
		else {
		    put_edit_nnl (1, "Red ");
		    put_edit_apnb (type);
		}
		
		put_edit_apnnl (" : {");
		
		for (xts = XH_X (pss_list_hd, xpts); xts < lim3;) {
		    pss = XH_list_elem (pss_list_hd, xts);
		    sprintf (string, "%d", pss);
		    put_edit_apnnl (string);
		    
		    if (++xts < lim3)
			put_edit_apnnl (", ");
		}
		
		put_edit_ap ("}");
	    } while (++xas < lim2);
	    
	    XxYxZ_Xforeach (ARC->QxTxQ_hd, ARC_state, xts) {
		put_edit_nnl (11, xt_to_str (-XxYxZ_Y (ARC->QxTxQ_hd, xts)));
		put_edit_apnnl (" ---> ");
		next_ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, xts);
		
		if (XH_X (ARC->Q_hd, next_ARC_state + 1) -
		    (xas = XH_X (ARC->Q_hd, next_ARC_state)) == 1) {
		    if ((type = -XH_list_elem (ARC->Q_hd, xas) - grand) <= 0)
			put_edit_apnnl ("Shift");
		    else {
			put_edit_apnnl ("Red ");
			put_edit_apnb (type);
		    }
		}
		else {
		    put_edit_apnb (next_ARC_state);
		    
		    if (!ARC->attr [next_ARC_state].is_marked)
			put_edit_apnnl (" (unprocessed)");
		}
		
		put_edit_nl (1);
	    }
	}
    }
}


bool		X_sees_pss (X, pss)
    int		X, pss;
{
    /* X et pss sont des suffixes de pile d'analyse */
    /* On regarde si pss peut se former a partir de X par des transitions
       non-terminales derivant dans le vide. */
    /* Les boucles eventuelles ont deja ete testees. */

    register int	StNt, nt, new_pss, next_state;
    register struct Q0	*aQ0;
    int			lim;

    if ((lim = (aQ0 = Q0 + TopState (X))->nt_trans_nb) > 0) {
	/* Il y a des transitions non terminales */
	lim += (StNt = aQ0->bot + aQ0->t_trans_nb);

	do {
	    if ((next_state = Q0xV_to_Q0 [StNt]) > 0) {
		/* vers un etat non vide */
		nt = XxY_Y (Q0xV_hd, StNt);

		if (SXBA_bit_is_set (bnf_ag.BVIDE, nt)) {
		    new_pss = make_new_pss (X, next_state);

		    if (new_pss == pss || X_sees_pss (new_pss, pss))
			return true;
		}
	    }
	} while (++StNt < lim);
    }

    return false;
}


static int max_min_lgth (ARC)
    struct ARC_struct *ARC;
{
    int ARC_state, lgth = 0;

    for (ARC_state = XH_top (ARC->Q_hd) - 1; ARC_state > 0; ARC_state--) {
	if (ARC->attr [ARC_state].conflict_kind != NO_CONFLICT_)
	    lgth = max (lgth, ARC->attr [ARC_state].lgth);
    }

    return lgth;
}


static bool	check_lc (max_min_lgth)
    int	max_min_lgth;
{
    /* Un ARC (impur) vient d'etre construit.
       Pour chaque etat conflictuel s de l'ARC on calcule d(s),
       distance minimale entre l'etat initial et s.
       Soit h = MAX d(s). Si h est > k_value on positionne NEW_K_VALUE a h
       et on retourne true, sinon on retourne false. */
    /* Une detection de non LALR a pu se produire anterieurement. Dans ce
       cas, la detection (egalite des pss) s'est produite a une distance <=
       k_value et la detection de non R(oxo)LALR se produit a la meme distance
       => traite par le cas precedent. */
    return max_min_lgth > ORIG_K_VALUE ? (NEW_K_VALUE = max_min_lgth, true) : false;
}



int orig_item_i (red_no, i)
    int red_no, i;
{
    /* Retourne le ieme item origine d'une reduction dont le representant est red_no
       sinon 0. */
    register int	cur_red, xred_no, n = 0;

    for (cur_red = 0; cur_red < RED_NO; cur_red++) {
	xred_no = XxY_Y (Q0xV_hd, StNt_orig + cur_red) - bnf_ag.WS_TBL_SIZE.xntmax;

	if (PROD_TO_RED (xred_no) == red_no && ++n == i)
	    return bnf_ag.WS_NBPRO [xred_no + 1].prolon - 1;
    }

    return 0;
}



SXBA	fill_pss_set (ARC, pss_set, ARC_state, t, type, set)
    struct ARC_struct	*ARC;
    register SXBA	pss_set, set;
    int			ARC_state, t, type;
{
    /* met dans pss_set les pss de l'etat ARC_state de l'ARC de type "type" qui
       par transition sur t donnent un pss de set. */
    register int	xts1, xts, xas, pss;
    int			lim, lim1, topas, xpts, xpts1, pss1;

    sxba_empty (pss_set);

    for (topas = XH_X (ARC->Q_hd, ARC_state + 1), xas = XH_X (ARC->Q_hd, ARC_state);xas < topas; xas++) {
	if ((xts = -XH_list_elem (ARC->Q_hd, ++xas)) == type ||
	    xts < grand && type < grand /* shift (etat 1) */) {
	    xpts = XH_list_elem (ARC->Q_hd, xas - 1);
	    lim = XH_X (pss_list_hd, xpts + 1);

	    if (ARC_state == 1)
		lim--;

	    for (xts = XH_X (pss_list_hd, xpts);xts < lim;xts++) {
		pss = XH_list_elem (pss_list_hd, xts);
		xpts1 = PSSxT_to_xpts [XxY_is_set (&PSSxT_hd, pss, -t)].xpts;

		for (lim1 = XH_X (pss_list_hd, xpts1 + 1), xts1 = XH_X (pss_list_hd, xpts1);xts1 < lim1;xts1++) {
		    pss1 = XH_list_elem (pss_list_hd, xts1);

		    if (SXBA_bit_is_set (set, pss1))
			SXBA_1_bit (pss_set, pss);
		}
	    }

	    break;
	}
    }

    return pss_set;
}


void	fill_kas (ARC, ARC_state, kas, pss_sets, xkas, conflict_kind)
    struct ARC_struct	*ARC;
    int		ARC_state, *xkas, conflict_kind;
    struct kas	*kas;
    SXBA	*pss_sets;
{
    register int	xk;
    register SXBA	pss_set;

    {
	register int	xts, xas, pss;
	register struct kas	*akas;
	int	lim, xpts, topas;

	for (xk = 0, topas = XH_X (ARC->Q_hd, ARC_state + 1), xas = XH_X (ARC->Q_hd, ARC_state);
	     xas < topas; xas++, xk++) {
	    akas = kas + xk;
	    sxba_empty (pss_set = pss_sets [xk]);
	    akas->xpts = xpts = XH_list_elem (ARC->Q_hd, xas++);
	    akas->type = XH_list_elem (ARC->Q_hd, xas);
	    xts = XH_X (pss_list_hd, xpts);
	    pss = XH_list_elem (pss_list_hd, xts);
	    SXBA_1_bit (pss_set, pss);
	    akas->m = pss;
	    lim = XH_X (pss_list_hd, xpts + 1);

	    if (ARC_state == 1)
		lim--; /* On a ajoute un pss bidon (0) ds l'etat initial */

	    while (++xts < lim) {
		pss = XH_list_elem (pss_list_hd, xts);
		SXBA_1_bit (pss_set, pss);
	    }

	    akas->M = pss;
	}

	*xkas = xk;
    }

    if (conflict_kind == NO_CONFLICT_)
	return;

    /* Il serait peut etre fin, pour les etats conflictuels, de ne stocker
       que les pss d'interet, ca eviterait de les recalculer ici.
       Pb: dans l'impression de l'automate de l'ARC il manquerait des items
       dans les etats en conflits . */

    if (conflict_kind == NOT_LALRk_ ||
	conflict_kind == NOT_LALR_ ||
	conflict_kind == NOT_RhLALRk_ ||
	conflict_kind == NOT_RoxoLALRk_)
	/* Dans ce cas, tous les pss sont conserves (aucune verification ds earley autre
	   que sur la chaine de pre-vision) */
	return;

    if ((pss_kind != UNBOUNDED_ || !is_lr_constructor) && is_same_pss (xk, pss_sets)) {
	register int	x;

	for (x = 0; x < xk; x++) {
	    sxba_and (pss_sets [x], and_set);
	}
    }
    else if (pss_kind == FSA_) {
	/* On ne conserve que les pss ayant des chemins communs. */
	is_inter_pss (xk, pss_sets, true, false);
    }
    /* else garde-fou ou on conserve tout ex transition sur eof... */
}

int	make_new_pss (pss, next_state)
    int		pss, next_state;
{
    /* Fabrique un pss a partir d'un ancien et next_state et le retourne. */

    register int	bot, top;
    int		result;
    bool	is_knot = false;

    bot = XH_X (pss_hd, pss);
    top = XH_X (pss_hd, pss + 1) - 1;

    if (XH_list_elem (pss_hd, top) == 0) {
	/* pss de type FSA_ */
	register int couple;
	int	new_couple;
	bool		new_couple_pushed = false;

	XH_push (pss_hd, XH_list_elem (pss_hd, bot));

/* Fabrique le couple (final, next_state) dans couples. */

	new_couple = XxY_is_set (&Q0xQ0_hd, TopState (pss), next_state);
	top--;

	while (++bot < top) {
	    couple = XH_list_elem (pss_hd, bot);

	    if (!new_couple_pushed && couple > new_couple) {
		new_couple_pushed = true;
		XH_push (pss_hd, new_couple);
	    }

	    if (couple != new_couple)
		XH_push (pss_hd, couple);
	}

	if (!new_couple_pushed)
	    XH_push (pss_hd, new_couple);
    }
    else /* pss_kind != FSA_ */	 {
	register int	i, knot;

	if (pss_kind == BOUNDED_ && (i = top - bot - h_value + 1) > 0)
	    bot += i;

	while (bot < top) {
	    if ((knot = XH_list_elem (pss_hd, bot++)) < 0)
		is_knot = true;

	    XH_push (pss_hd, knot);
	}
    }

    XH_push (pss_hd, next_state);
    XH_push (pss_hd, is_knot ? -2 : h_value);
    XH_set (&pss_hd, &result);
    return result;
}



void	ARC_init (print_conflicts, kind, max_red_per_state, arcs_size)
    bool	print_conflicts;
    int		kind, max_red_per_state, arcs_size;
{
    /* pss_kind :	UNBOUNDED_	FSA_	BOUNDED_
       h_value	:	   -1	   	0	  >0
    */
    register int	i;
    int			to;
    bool		first_call;

    /* print_conflicts indique qu'il faudra peut etre imprimer des conflits. */
    conflict_message = print_conflicts;
    pss_kind = kind;
    ibfsa_needed = is_lr_constructor /* || conflict_message && pss_kind != BOUNDED_ */;

/* Attention, conflict_message, pss_kind et ibfsa_needed peuvent changer
   d'un appel sur l'autre. */
    if (kas == NULL) {
	/* Premier appel de ARC_init */
	to = h_value <= 0 ? 6 : (h_value > 4 ? 4 : h_value);

	XE1 = XE1_init = 0;
	MAX_RED_PER_STATE = max_red_per_state;
	XTMAX = bnf_ag.WS_TBL_SIZE.xtmax;
	compute_Next_states_set ();
	lgt [1] = arcs_size * to * xac2;
	lgt [2] = lgt [1];
	lgt [3] = lgt [2];
	lgt [4] = xac2;
	/* lgt [5] est disponible */
	/* lgt [6] = 10; initialise dans XARC */
	/* lgt [8] est disponible */
	RLR_sature (0);
	arc = SS_alloc (arcs_size + 2);
	arc_trans = SS_alloc (xac2);
	SS_push (arc, SS_top (arc_trans));
	XH_alloc (&pss_list_hd, "pss_list_hd", to * xac2, 4, 2, NULL, statistics_file);

	XxY_alloc (&PSSxT_hd, "PSSxT", to * xac2, 4, 0, 0, oflw_PSSxT, statistics_file);
	PSSxT_to_xpts = (struct PSSxT_to_xpts*) sxalloc (XxY_size (PSSxT_hd) + 1
	     , sizeof (struct PSSxT_to_xpts));
	XH_alloc (&pss_hd, "pss_hd", to * xac2, 4, to, oflw_pss, statistics_file);
	pss_check_stack = (int*) sxcalloc (XH_size (pss_hd) + 1, sizeof (int));
	pss_check_stack [0] = -1;
	X_alloc (&pss_to_Next_hd, "pss_to_Next", xac2, 4, oflw_pss_to_Next, statistics_file);
	pss_to_Next = (int*) sxalloc (X_size (pss_to_Next_hd) + 1, sizeof (int));
	XH_alloc (&Next_hd, "Next_hd", xac2, 4, to, NULL, statistics_file);
	ws = SS_alloc (xac2);
	back_head = (int*) sxcalloc (XH_size (pss_hd) + 1, sizeof (int));
	pss_sets = sxbm_calloc (MAX_RED_PER_STATE + 6, XH_size (pss_hd) + 1);
	work_set_no = MAX_RED_PER_STATE + 1;
	and_set = pss_sets [MAX_RED_PER_STATE + 2];
	is_cycle_on_pss_computed = pss_sets [MAX_RED_PER_STATE + 3];
	is_cycle_on_pss = pss_sets [MAX_RED_PER_STATE + 4];
	pss_work_set = pss_sets [MAX_RED_PER_STATE + 5];
	kas = (struct kas*) sxalloc (max_red_per_state + 2, sizeof (struct kas))
	     ;

	for (i = max_red_per_state + 1; i >= 0; i--)
	    kas [i].pss_set = pss_sets [i];

	init_state_attr = (struct init_state_attr*) sxalloc (max_red_per_state +
	     2, sizeof (struct init_state_attr));

	BS = NULL;
	bfsa_to_ate = NULL;
	couples_set = NULL;
	xpss_set = NULL;
	ibfsa_stack = NULL;
	first_call = true;
    }
    else
	first_call = false;


    if ((ibfsa_needed || kind == FSA_) && couples_set == NULL) /* infinity */  {
	couples_set = sxba_calloc (Q0xQ0_top + 1);
	LR0_states_set = LR0_sets [5];
    }
    
    
    if (kind == FSA_ && xpss_set == NULL) {
	xpss_set = sxba_calloc (xpss_set_size = 32);
    }
    
    if (ibfsa_needed && ibfsa_stack == NULL) {
	XH_alloc (&ibfsa_hd, "ibfsa_hd", 2 * xac2, 4, 2, oflw_ibfsa, statistics_file);
	ibfsa_to_ate = (int*) sxalloc (XH_size (ibfsa_hd) + 1, sizeof (int));
	ibfsa_stack = SS_alloc (xac2);
    }
    
    if (!first_call) {
/* Le premier appel a deja ete effectue, on se contente de reinitialise les
   structures */
/* Normalement "arcs" contient suffisamment de place:
      - Au plus deux ARCs par etat conflictuel
      - "arc" a pu etre realloue par un appel depuis LALR1 a XARC_resize.
      - ARCS_SIZE a la bonne valeur. */

/* MAX_RED_PER_STATE = max_red_per_state; inutile, les clones eventuels
   ne l'ont pas change. */
	ARC_reinit ();
    }
}



int	ARC_final ()
{
    register int i;

    for (i = 3; i >= 0; i--)
	if (ARCs [i].is_allocated)
	    ARC_deallocate (&(ARCs [i]));
    
    if (arc_path_set != NULL) {
	sxfree (arc_path_set), arc_path_set = NULL;
	sxfree (arc_path_stack);

	if (is_super_arc) {
	    XxYxZ_free (&QxTxQ_hd);
	}

	X_free (&Q_hd);
	XxYxZ_free (&AxOxV_hd);
    }

    SS_free (arc);
    SS_free (arc_trans);
    return XE1 > XE1_init ? XE1 : xe1;
}



void	ARC_free ()
{
    sxfree (kas), kas = NULL;
    sxfree (init_state_attr);
    free_1 ();
}


void	ARC_get_sizes (arc_state_nb, arc_trans_nb)
    int		*arc_state_nb, *arc_trans_nb;
{
    register int l, m;

    m = *arc_state_nb = SS_top (arc) - SS_bot (arc) - 1;
    l = *arc_trans_nb = (SS_top (arc_trans) - SS_bot (arc_trans)) / 2;
    X_alloc (&Q_hd, "Q", l + 1, 4, NULL, statistics_file);

    if (is_super_arc) {
	QxTxQ_foreach [XYZ_X] = 1;
	XxYxZ_alloc (&QxTxQ_hd, "QxTxQ", l + 1, 4, QxTxQ_foreach, NULL, statistics_file);
    }

    AxOxV_foreach [XYZ_X] = 1;
    XxYxZ_alloc (&AxOxV_hd, "AxOxV", l + 1, 4, AxOxV_foreach, NULL, statistics_file);
    arc_path_set = sxba_calloc (m + 1);
    arc_path_stack = (int*) sxalloc (m + 1, sizeof (int));
    XE1_init = XE1 = xe1;
}



static int solve_something (ARC)
    struct ARC_struct	*ARC;
{
    /* Range les [sous-]automates de ARC qui correspondent a une resolution totale
       des sous-arbres init_state ->t q ->* r avec r pur.
       Note dans t_set tous les t. */
    register int	i, x, ARC_state, t, next_ARC_state;
    int			arc_no, arc_no_init, next_arc_no;
    register SXBA	t_set = ARC->t_set;
    register int	*tQ = NULL;

    arc_no_init = arc_no = SS_top (arc) - 1;
/*    sxba_empty (t_set); inutile */
    SS_clear_ss (ARC->Q_stack);
    SS_push (ARC->Q_stack, 1); /* Etat initial */
    
    for (i = SS_top (ARC->Q_stack) - 1;i < SS_top (ARC->Q_stack); i++) {
	/* Q_stack doit etre exploiter ds le sens ou il est remplit. */
	ARC_state = SS_get (ARC->Q_stack, i);

	XxYxZ_Xforeach (ARC->QxTxQ_hd, ARC_state, x) {
	    t = XxYxZ_Y (ARC->QxTxQ_hd, x);
	    next_ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, x);
	    
	    if (ARC_state == 1) {
		if (ARC->attr [next_ARC_state].conflict_kind == NO_CONFLICT_ &&
		    ARC->attr [next_ARC_state].is_pure_tree)
		    SXBA_1_bit (t_set, t);
		else
		    next_ARC_state = 0;
	    }
	    
	    if (next_ARC_state != 0) {
		SS_push (arc_trans, t);
		
		if (XH_list_lgth (ARC->Q_hd, next_ARC_state) == 1) {
		    SS_push (arc_trans, XH_list_elem (ARC->Q_hd, XH_X (ARC->Q_hd, next_ARC_state)));
		}
		else {
		    if (tQ == NULL)
			tQ = (int*) sxcalloc (XH_top (ARC->Q_hd), sizeof (int));
		    
		    if ((next_arc_no = tQ [next_ARC_state]) == 0) {
			next_arc_no = tQ [next_ARC_state] = ++arc_no;
			SS_push (ARC->Q_stack, next_ARC_state);
		    }
		    
		    SS_push (arc_trans, next_arc_no);
		}
	    }
	}
	
	if (arc_no != arc_no_init)
	    SS_push (arc, SS_top (arc_trans));
    }
    
    if (tQ != NULL)
	sxfree (tQ);
    
    return arc_no != arc_no_init ? arc_no_init : 0;
}

int	pss_set_to_pss_list (pss_set)
    register SXBA	pss_set;
{
    register int pss = 0;
    int		result;
    
    while ((pss = sxba_scan (pss_set, pss)) > 0)
	XH_push (pss_list_hd, pss);
    
    XH_set (&pss_list_hd, &result);
    return result;
}


static int	install_a_pss_list (akas, is_init_state)
    struct kas	*akas;
    bool is_init_state;
{
    /* On supprime de work_set toutes les stack_suffixes dont le sommet
       n'a pas de transition terminale */
    register int	pss;
    int			MIN = akas->m, MAX = akas->M;
    register SXBA	pss_set = akas->pss_set;
    int		result;

    if (pss_kind == UNBOUNDED_ && reads_cycle_sets != NULL && MIN != MAX)
	pss_set_reduction (pss_set, &MIN, &MAX);

    for (pss = MIN; pss != -1; pss = sxba_scan (pss_set, pss)) {
	if (Q0 [TopState (pss)].t_trans_nb == 0)
	    /* Il n'y a pas de transition terminale */
	    SXBA_0_bit (pss_set, pss);
	else
	    XH_push (pss_list_hd, pss);

	if (pss == MAX)
	    break;
    }

    if (is_init_state)
	/* On ajoute un pss bidon (0) pour etre sur de ne jamais retrouver l'etat initial
	   lors de la construction. */
	XH_push (pss_list_hd, 0);

    XH_set (&pss_list_hd, &result);
    return result;
}



static bool	install_arc_state (ARC, kas, xkas, new_arc)
    struct ARC_struct	*ARC;
    struct kas	*kas;
    int		xkas, *new_arc;
{
    /* Un etat de l'ARC a ete calcule dans kas, on l'installe dans
       la structure definitive s'il ne s'y trouvait pas deja.
       new_arc contient l'identifiant de cet etat.
       Retourne true si c'est un nouvel etat. */

    register int	x, ol;
    register struct kas		*akas;
    register bool	is_old;

    for (x = 0; x <= xkas; x++) {
	XH_push (ARC->Q_hd, (akas = kas + x)->xpts);
	XH_push (ARC->Q_hd, akas->type);
    }

    return !XH_set (&(ARC->Q_hd), new_arc);
}

static void	ARC_gather_trans (ARC, ARC_state)
    struct ARC_struct	*ARC;
    int		ARC_state;
{
    /* On est sur un [X]ARC.
       Remplit ARC->t_set avec les transitions terminales valides.
       2 cas possibles :
          - ARC->ARC est NULL alors on filtre les transitions initiales par conflict_t_set
	  - sinon on utilise les transitions marquees de l'ARC origine. */
    register SXBA		t_set = ARC->t_set;
    register struct ARC_struct	*PARC = ARC->ARC;
    register struct whd		*awhd;

    if (PARC != NULL) {
	register int	x, t, next_PARC_state;
    
	XxYxZ_Xforeach (PARC->QxTxQ_hd, ARC->attr [ARC_state].from, x) {
	    t = XxYxZ_Y (PARC->QxTxQ_hd, x);

	    if (PARC->attr [next_PARC_state = XxYxZ_Z (PARC->QxTxQ_hd, x)].is_marked) {
		SXBA_1_bit (t_set, t);
		
		if (ARC->is_XARC) {
		    (awhd = ARC->whd + t)->lnk = next_PARC_state;
		    awhd->type_nb = (XH_list_lgth (PARC->Q_hd, next_PARC_state) + 1) >> 1;
		}
	    }
	}

	if (ARC->is_XARC)
	    return;
    }

    {
	register int		xts, xas, StNt;
	register struct wl	*awl;
	register struct Q0	*aQ0;
	register SXBA		conflict_t_set = ARC->conflict_t_set;
	int			t, lim1, pss, lim3, xpts, trans_no, botas, type;
	
	botas = XH_X (ARC->Q_hd, ARC_state);
	/* sxba_empty (t_set); suppose inutile */
	
	for (xas = XH_X (ARC->Q_hd, ARC_state + 1) - 1; xas >= botas; xas--)
	{
	    type = XH_list_elem (ARC->Q_hd, xas--);
	    xpts = XH_list_elem (ARC->Q_hd, xas);
	    lim3 = XH_X (pss_list_hd, xpts + 1);
	    
	    if (ARC_state == 1)
		lim3--; /* On a ajoute un pss bidon (0) ds l'etat initial */
	    
	    for (xts = XH_X (pss_list_hd, xpts); xts < lim3; xts++) {
		/* Il y a des transitions terminales */
		pss = XH_list_elem (pss_list_hd, xts);
		lim1 = (StNt = (aQ0 = Q0 + TopState (pss))->bot) + aQ0->t_trans_nb;
		
		do {
		    /* Il existe une transition depuis TopState
		       sur t */
		    t = -XxY_Y (Q0xV_hd, StNt);
		    
		    /* On a evite artificiellement les boucles eventuelles sur l'etat
		       initial en ajoutant un pss bidon (0) a la liste des pss. */
		    if (PARC == NULL && (ARC_state > 1 || SXBA_bit_is_set (conflict_t_set, t)) ||
			PARC != NULL && SXBA_bit_is_set (t_set, t)) {
			SXBA_1_bit (t_set, t);
			/* ajoute en tete le doublet (pss, type) a la liste des t */
			wl_maj (&(ARC->wl));
			awl = ARC->wl + ARC->wl [0].lnk;
			awl->lnk = (awhd = ARC->whd + t)->lnk;
			awl->ss = pss;
			awl->type = type;
			
			if (awhd->lnk == 0)
			    awhd->type_nb = 1;
			else if (ARC->wl [awhd->lnk].type != awl->type)
			    (awhd->type_nb)++;
			
			awhd->lnk = ARC->wl [0].lnk;
		    }
		} while (++StNt < lim1);
	    }
	}
	
	ARC->wl [0].lnk = 0; /* locked */
    }
}



static bool ARC_trans (ARC, ARC_state, t, next_ARC_state)
    struct ARC_struct	*ARC;
    int		ARC_state, t, *next_ARC_state;
{
    /* Calcul de la transition (mixte) depuis ARC_state sur t. */
    register int			x, xkas;
    register struct whd			*awhd = ARC->whd + t;
    register struct wl			*awl;
    register struct PSSxT_to_xpts	*aelem;
    struct kas				*akas;
    int					indice, ck, type, pss;
    bool				is_old_aelem, is_first, is_brand_new_state;
    
    /* Pour chaque doublet ({pss}, type) de l'etat courant xpas de l'arc */
    /* Pour chaque pss de pile de l'automate LR(0) associe a type */
    
    if (awhd->type_nb == 1) {
	if ((type = ARC->wl [awhd->lnk].type) > 0)
	    type = -XxY_is_set (&Q0xV_hd, type, -t);
	
	XH_push (ARC->Q_hd, type);
	is_brand_new_state = !XH_set (&(ARC->Q_hd), next_ARC_state);
    }
    else {
	for (x = awhd->lnk, xkas = 0, type = 0; x != 0;) {
	    awl = ARC->wl + x;
	    
	    if (awl->type != type) {
		/* Changement de type */
		(akas = kas + xkas)->type = ((type = awl->
					      type) > 0) ? -XxY_is_set (&Q0xV_hd, type, -t)
						  : type;
		is_first = true;
		sxba_empty (pss_sets [xkas++]);
		akas->m = maxint;
		akas->M = 0;
	    }
	    
	    is_old_aelem = XxY_set (&PSSxT_hd, pss = awl->ss, t, &indice);
	    aelem = PSSxT_to_xpts + indice;
	    
	    if (is_old_aelem && aelem->xpts != 0) {
		/* L'ensemble des etats atteint depuis pss
		   par transition sur t puis simulation
		   de toutes les reductions possibles a deja
		   ete calcule, on l'utilise. */
		ts_to_kas (aelem->xpts, akas);
	    }
	    else {
		back_head [0] = is_old_aelem ? aelem->back_head : 0;
		
		if (is_first) {
		    work_set = (work_kas = akas)->pss_set;
		}
		else {
		    work_kas = kas + work_set_no;
		    sxba_empty (work_set = pss_sets [work_set_no]);
		    work_kas->m = maxint;
		    work_kas->M = 0;
		}
		
		sim_a_red (pss, t);
		
		/* On supprime de work_set les etats n'ayant pas de transitions terminales */
		/* On installe work_set dans ts et on le memorise dans PSSxT_to_xpts */
		
		aelem->xpts = install_a_pss_list (work_kas, false);
		aelem->back_head = back_head [0];
		
		if (is_first)
		    is_first = false;
		else
		    ts_to_kas (aelem->xpts, akas);
	    }
	    
	    if ((x = awl->lnk) == 0 || ARC->wl [x].type != type) {
		akas->xpts = install_a_pss_list (akas, false);
	    }
	}
	
	is_brand_new_state = install_arc_state (ARC, kas, xkas - 1, next_ARC_state);
    }

    if (is_brand_new_state) {
	if (ARC->ARC == NULL)
	    ARC->attr [*next_ARC_state].from = 0;
	else {
	    XxYxZ_Xforeach (ARC->ARC->QxTxQ_hd, ARC->attr [ARC_state].from, x) {
		if (XxYxZ_Y (ARC->ARC->QxTxQ_hd, x) == t) {
		    ARC->attr [*next_ARC_state].from = XxYxZ_Z (ARC->ARC->QxTxQ_hd, x);
		    break;
		}
	    }
	}

	ARC->attr [*next_ARC_state].primes_nb = 0;
    }

    return is_brand_new_state;
}
    

bool bh_ARC_one_LA (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    /* Recherche dans l'ARC un chemin (look_ahead) de longueur donnee.
       Le chemin partiel est stocke dans ws, le fond de ws est la longueur restant
       a calculer. */
    register int		ARC_state, lgth, la_lgth;
    int				lim;
    register struct ARC_ATTR	*attr;

    SS_push (ws, qtq);
    ARC_state = XxYxZ_X (ARC->QxTxQ_hd, qtq);
    lgth = ARC->attr [ARC_state].lgth;
    la_lgth = --SS_get_bot (ws);

    if (la_lgth == 0 && ARC_state == 1) {
	/* On tient un chemin */
	mm_process_one_LA (ARC);
	return false;
    }

    if (la_lgth == 0 || ARC_state == 1)
	return false;

    return pss_kind == UNBOUNDED_ && (la_lgth <= lgth || lgth == -1) /* lgth est la longueur du plus long chemin. */ ||
	pss_kind != UNBOUNDED_ && la_lgth >= lgth /* lgth est la longueur du plus court chemin. */;
}


bool bs_ARC_one_LA (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    SS_pop (ws);
    ++SS_get_bot (ws);
    return !is_unique_conflict_per_state;
}


static void ARC_conflict_processing (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    /* Un conflit vient d'etre detecte lors de la transition qtq.
       On calcule le message d'erreur correspondant. */
    register int		ARC_state, la_lgth;
    int				xkas;
    register struct ARC_ATTR	*attr;

    attr = ARC->attr + (ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, qtq));

    if (should_print_conflict (attr->conflict_kind)) {
	messages_mngr_alloc ();
	/* On remplit kas */
	fill_kas (ARC, ARC_state, kas, pss_sets, &xkas, attr->conflict_kind);

	if ((la_lgth = attr->lgth) == -1)
	    la_lgth = k_value;
	    
	SS_open (ws);
	SS_push (ws, la_lgth);
	ARC_walk_backward (ARC, qtq, bh_ARC_one_LA, bs_ARC_one_LA);
	SS_close (ws);
    }
}


bool bh_mark (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    register struct ARC_ATTR	*attr = ARC->attr + XxYxZ_X (ARC->QxTxQ_hd, qtq);

    if (attr->is_marked)
	return false; /* sous-graphe deja traite (ou en cours de traitement). */

    attr->is_marked = true;
    return true; /* On continue */
}


bool bh_unmark (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    register struct ARC_ATTR	*attr = ARC->attr + XxYxZ_X (ARC->QxTxQ_hd, qtq);

    if (!attr->is_marked)
	return false; /* sous-graphe deja traite (ou en cours de traitement). */

    attr->is_marked = false;
    return true; /* On continue */
}

static void ARC_init_state_building (ARC)
    struct ARC_struct	*ARC;
{
    /* Calcul de l'etat initial dans les cas d'un ARC (FSA_, BOUNDED_ ou UNBOUNDED_). */
    
    register int			cur_red, xkas;
    register struct init_state_attr	*pinit;
    int					item, pss, lim, xred_no, repr_red_no, type, ARC_state;
    
    if (IS_SHIFT_REDUCE) {
	xkas = RED_NO;
    }
    else {
	xkas = RED_NO - 1;
	init_state_attr [RED_NO].type = 0;
    }
    
    lim = xkas + 1;
    
    for (cur_red = 0; cur_red < lim; cur_red++) {
	pinit = init_state_attr + cur_red;
	work_kas = kas + cur_red;
	sxba_empty (work_set = pss_sets [cur_red]);
	
	if (cur_red < RED_NO) {
	    /* Cas Reduce */
	    xred_no = XxY_Y (Q0xV_hd, StNt_orig + cur_red) - bnf_ag.WS_TBL_SIZE.xntmax;
	    item = bnf_ag.WS_NBPRO [xred_no + 1].prolon - 1;
	    type = -grand - xred_no;
	    work_kas->m = maxint;
	    work_kas->M = 0;
	    back_head [0] = 0;
	    call_compute_back (0, germe, 0, item, 0);
	    CLEAR (pss_check_stack);
	    back_depth_first_traversal (0);
	    /* work_set contient la portion du germe de type cur_red */
	    pinit->back_head = back_head [0];
	}
	else {
	    /* Cas Shift */
	    type = germe; /* Shift (valeur initiale) */
	    XH_push (pss_hd, germe);
	    
	    if (pss_kind == FSA_)
		XH_push (pss_hd, germe);
	    
	    XH_push (pss_hd, h_value);
	    XH_set (&pss_hd, &pss);
	    SXBA_1_bit (work_set, pss);
	    work_kas->m = work_kas->M = pss;
	}
	
	/* On supprime les etats n'ayant pas de transitions
	   terminales */
	pinit->type = work_kas->type = type;
	pinit->pss_list = work_kas->xpts = install_a_pss_list (work_kas, true);
	pinit->h_value = h_value;
	pinit->fork_list = 0;
    }
    
    has_several_equiv_reductions = false;
    
    if (THERE_IS_AN_XPROD) {
	int	x, y, nb;
	register struct kas	*akas;
	
	/* On "regroupe" eventuellement les reduce ayant meme representant. */
	xkas = -1;
	
	for (cur_red = 0; cur_red < RED_NO; cur_red++) {
	    xred_no = XxY_Y (Q0xV_hd, StNt_orig + cur_red) - bnf_ag.WS_TBL_SIZE.xntmax;
	    work_kas = kas + cur_red;
	    repr_red_no = PROD_TO_RED (xred_no);
	    
	    for (nb = 0, x = cur_red + 1; x < RED_NO; x++, nb++) {
		y = XxY_Y (Q0xV_hd, StNt_orig + x) - bnf_ag.WS_TBL_SIZE.xntmax;
		
		if (PROD_TO_RED (y) != repr_red_no)
		    break;
		
		has_several_equiv_reductions = true;
		akas = kas + x;
		sxba_or (work_kas->pss_set, akas->pss_set);
		work_kas->m = min (work_kas->m, akas->m);
		work_kas->M = max (work_kas->M, akas->M);
	    }
	    
	    work_kas->type = -(grand + repr_red_no);
	    
	    if (nb > 0)
		work_kas->xpts = install_a_pss_list (work_kas, true);
	    
	    if (++xkas < cur_red)
		kas [xkas] = *work_kas;
	    
	    cur_red += nb;
	}
    }
    
    /* kas contient le germe */
    install_arc_state (ARC, kas, xkas, &ARC_state); /* ARC_state == 1 */
    
    if (ARC_state != 1)
	sxtrap (ME, "ARC_construction");
    
    ARC->attr [0].lgth = ARC->attr [1].lgth = 0;
    ARC->attr [0].is_marked = ARC->attr [1].is_marked = false;
    ARC->conflict_kind = ARC->attr [1].conflict_kind = NO_CONFLICT_;
    ARC->attr [1].is_tree_orig = false;
    ARC->attr [0].primes_nb = ARC->attr [1].primes_nb = 0;
    ARC->attr [1].from = ARC->ARC == NULL ? 0 : 1;
    ARC->is_initiated = true;
}


static struct ARC_struct *call_ARC_building (XARC, qtq)
    struct ARC_struct	*XARC;
    int			qtq;
{
    /* L'appelant est un XARC de type FSA_.
       On vient de detecter un etat impur dans l'XARC courant.
       On relance la construction en LR pour sortir les messages
       (ou eventuellement detecter du clonage.) */
    register int	XARC_state;
    struct ARC_struct	*ARC;
    
    XARC_state = XxYxZ_Z (XARC->QxTxQ_hd, qtq);
    pss_kind = UNBOUNDED_;
    k_value = XARC->attr [XARC_state].lgth;
    h_value = -1;
    ARC = &(ARCs [ARC_UNBOUNDED]);
    
    if (!ARC->is_allocated) {
	ARC_allocate (ARC, ARC_UNBOUNDED, ARC_UNBOUNDED_Q_oflw);
    }

    if (!ARC->is_initiated) {
	ARC->ARC = XARC;
	ARC_init_state_building (ARC);
	ARC->is_clonable = true;
	ARC->pss_kind = pss_kind;
	ARC->h_value = h_value;
	ARC->k_value = k_value;
    }

    /* On remonte dans l'XARC en marquant les etats accessibles. */
    XARC->attr [XARC_state].is_marked = true;
    ARC_walk_backward (XARC, qtq, bh_mark, NULL);

    ARC_building (ARC, ARC_trans, ARC_UNBOUNDED_get_conflict_kind);
    
    /* On enleve les marques. */
    ARC_walk_backward (XARC, qtq, bh_unmark, NULL);
    XARC->attr [XARC_state].is_marked = false;
    ARC->is_clonable = XARC->is_clonable;
    pss_kind = FSA_;
    k_value = ORIG_K_VALUE;
    h_value = 0;

    return ARC;
}


static bool ARC_conflicting_node_processing (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    /* Traite un etat conflictuel :
          - prepare les messages d'erreur
	  - regarde s'il est clonable
	  - etc...
       Retourne true ssi le conflit a pu etre resolu. */
    register int	ARC_state, ck;
    int			xkas;
    bool		local_conflict_has_been_solved = false;

    ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, qtq);
    
    if (ARC->is_XARC) {
	/* Si c'est un XARC, on regarde si c'est localement clonable. */
	fill_kas (ARC->ARC, ARC->attr [ARC_state].from, kas, pss_sets, &xkas,
		  ARC->attr [ARC_state].conflict_kind);
	ARC->attr [ARC_state].conflict_kind = ck = is_LRpi (ARC, qtq);

	if (ck != NO_CONFLICT_) {
	    /* non clonable */
	    /* Dans le cas LR les messages ont ete prepares. */
	    if (conflict_message && pss_kind == FSA_ &&
		should_print_conflict (NOT_RoxoLR0_)) {
		/* On tente de sortir les messages en LR!.
		   On relance le tout. */
		struct ARC_struct *XARC;
		
		XARC = call_ARC_building (ARC, qtq);
		
		if (XARC->conflict_kind == NO_CONFLICT_ ||
		    /* Probleme, le conflit a ete resolu en LALR. */
		    XARC->is_clonable)
		    /* les clones ont ete ajoutes a la liste. */
		    local_conflict_has_been_solved = true;
		/* sinon, les messages on ete prepares en LR. */
	    }
	}
	else
	    local_conflict_has_been_solved = true;
    }
    else {
	/* C'est un ARC. */
	if (is_lr_constructor) {
	    if (call_XARC_building (ARC, qtq) == NO_CONFLICT_)
		local_conflict_has_been_solved = true;
	}
	else {
	    /* Preparation du message de non-conformite */
	    if (conflict_message) {
		ARC_conflict_processing (ARC, qtq);
	    }
	}
    }

    return local_conflict_has_been_solved;
}


static bool fh_remote_conflict (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    register int		next_ARC_state, t;
    register struct ARC_ATTR	*attr;

    attr = ARC->attr + (next_ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, qtq));

    if ((attr->is_marked)++)
	return false; /* sous-graphe deja traite (ou en cours de traitement). */

    t = XxYxZ_Y (ARC->QxTxQ_hd, qtq);

    if (attr->conflict_kind != NO_CONFLICT_ &&
	(ARC->is_XARC || pss_kind != UNBOUNDED_ || !is_lr_constructor ||
	 attr->lgth == -1 || attr->lgth >= k_value ||
	 t == -bnf_ag.WS_TBL_SIZE.tmax)) {

	if (erase_quads (ARC->kind, qtq))
	    ARC_conflicting_node_processing (ARC, qtq);
    }

    return true; /* On continue */
}

static bool bh_not_pure_tree (ARC, qtq)
    struct ARC_struct	*ARC;
    int			qtq;
{
    register struct ARC_ATTR	*attr = ARC->attr + XxYxZ_X (ARC->QxTxQ_hd, qtq);

    if (!attr->is_pure_tree)
	return false; /* sous-graphe deja traite (ou en cours de traitement). */

    attr->is_pure_tree = false;
    return true; /* On continue */
}


int	ARC_building (ARC, F_trans, F_get_conflict_kind)
    struct ARC_struct	*ARC;
    bool	(*F_trans)();
    int		(*F_get_conflict_kind) ();
{
    register int	ARC_state, t;
    register struct ARC_ATTR	*attr;
    int			next_ARC_state, qtq, ck, x_tree_orig, lgth, type_nb, new_lgth;
    bool		give_up, is_new_ARC_state, local_conflict_has_been_solved;

    sxba_empty (ARC->Q_set);
    SS_clear (ARC->Q_stack);
    SS_push (ARC->Q_stack, 1), SXBA_1_bit (ARC->Q_set, 1); /* Etat initial */
    
    while (!SS_is_empty (ARC->Q_stack)) {
	ARC_state = SS_pop (ARC->Q_stack);

	if (ARC->attr [ARC_state].is_tree_orig) {
	    x_tree_orig = SS_top (ARC->Q_stack);
	}

	ARC_gather_trans (ARC, ARC_state);
	give_up = false;
	t = 0;
	
	while ((t = sxba_scan_reset (ARC->t_set, t)) > 0) {
	    if (!give_up) {
		XxYxZ_Xforeach (ARC->QxTxQ_hd, ARC_state, qtq) {
		    if (XxYxZ_Y (ARC->QxTxQ_hd, qtq) == t)
			break;
		}

		if (qtq != 0) {
		    /* La transition existe deja */
		    next_ARC_state = XxYxZ_Z (ARC->QxTxQ_hd, qtq);
		    
		    if (XH_list_lgth (ARC->Q_hd, next_ARC_state) > 1 /* non final */ &&
			(ARC->attr [next_ARC_state].conflict_kind == NO_CONFLICT_ ||
			 !ARC->is_XARC && pss_kind == UNBOUNDED_ && is_lr_constructor &&
			 attr->lgth > 0 && attr->lgth < k_value &&
			 t != -bnf_ag.WS_TBL_SIZE.tmax) &&
			!SXBA_bit_is_set (ARC->Q_set, next_ARC_state)) {
			SS_push (ARC->Q_stack, next_ARC_state), SXBA_1_bit (ARC->Q_set, next_ARC_state);
		    }
		}
		else {
		    /* On la calcule. */
		    /* Calcul de next_ARC_state */
		    is_new_ARC_state = F_trans (ARC, ARC_state, t, &next_ARC_state);
		    attr = ARC->attr + next_ARC_state;
		    XxYxZ_set (&(ARC->QxTxQ_hd), ARC_state, t, next_ARC_state, &qtq);

		    if (is_new_ARC_state) {
			new_lgth = ARC->attr [ARC_state].lgth;
			attr->lgth = L_SUCC (new_lgth);
			attr->is_pure_tree = true;
			attr->is_marked = false;
			attr->conflict_kind = NO_CONFLICT_;
		    }
		    else
			ARC_walk_forward (ARC, qtq, fh_propagate_lgth, fs_unmark);
		    
		    if (ARC_state == 1)
			attr->is_tree_orig = true;
		    
		    if ((type_nb = ARC->whd [t].type_nb) > 1) {
			/* Etat mixte */
			ck = F_get_conflict_kind (ARC, next_ARC_state, is_new_ARC_state, -t);
			
			if (!attr->is_pure_tree && conflict_message) {
			    int	x;

			    /* Cet etat est ancien et l'un de ses descendants est conflictuel.
			       On vient de rajouter un chemin de look-ahead, le traitement est
			       donc incomplet, on recommence. */
			    XxYxZ_Xforeach (ARC->QxTxQ_hd, next_ARC_state, x) {
				if (!ARC_walk_forward (ARC, x, fh_remote_conflict, fs_unmark))
				    break;
			    }
			}

			if (ck != NO_CONFLICT_)
			    ARC_walk_backward (ARC, qtq, bh_not_pure_tree, NULL);
			
			if (ck == NO_CONFLICT_ ||
			    !ARC->is_XARC && pss_kind == UNBOUNDED_ && is_lr_constructor &&
			    attr->lgth > 0 && attr->lgth < k_value &&
			    t != -bnf_ag.WS_TBL_SIZE.tmax) {
			    /* On continue la generation d'un ARC en LALR lorsque la distance
			       n'est pas bonne et que la construction LR est demandee. */
			    if (is_new_ARC_state) {
				SS_push (ARC->Q_stack, next_ARC_state), SXBA_1_bit (ARC->Q_set, next_ARC_state);
			    }
			}
			else if (!ARC_conflicting_node_processing (ARC, qtq)) {
			    /* Pbs: generation de code (cg) et messages (m).
			       (cg) : soit init_state ->t1 q
			       pour tous les q tels que q ->* next_ARC_state, le sous-arbre
			       issu de q est inutile
			       (m) :  depend de is_unique_conflict_per_state
			       On pourrait supprimer de Q_stack les etats des sous-arbres. */
			    if (!conflict_message) {
				/* On abandonne tout! */
				    give_up = true;
				    SS_clear (ARC->Q_stack);
			    }
			    else if (is_unique_conflict_per_state) {
				/* Abandon de l'"arbre" courant => CLEAR des structures. */
				if (ARC_state != 1) {
				    give_up = true;
				    SS_top (ARC->Q_stack) = x_tree_orig;
				}
			    }
			}
			
			/* ARC_conflicting_node_processing a pu modifier conflict_kind. */
			ARC->conflict_kind |= attr->conflict_kind;
		    }
		    else
			attr->conflict_kind = NO_CONFLICT_;
		}
	    }
	    
	    ARC->whd [t].lnk = 0;
	}
    }
    
    return ARC->conflict_kind;
}

bool ARC_construction (failed_kind, xac1, conflict_t_set, is_shift_reduce, StNt
			  , red_no)
    int		xac1, StNt, red_no, *failed_kind;
    SXBA	conflict_t_set;
    bool	is_shift_reduce;
{
    /* Il y a conflit (LALR(1) ou LR(1)) dans l'etat xac1.
       Cette procedure calcule l'ARC reduit de germe xac1.
       is_shift_reduce est vrai ssi on a un conflit Shift/Reduce
       Il y a red_no reduce dans le coup.
       Les nt_trans originelles des reduces sont StNt, StNt+1, ..., StNt+red_no-1
       Le type des reduce va de 0 a red_no-1.
       Retourne 0 si l'ARC est impur
       -1 si l'ARC a une boucle
       n>0 si l'ARC est fini et n est la longueur du plus long chemin */

    /* Codage du type Reduce
           Dans as_hd.list [.+1]:	-grand - No de la reduction
	   Dans forward_trans[].next:	-grand - No de la reduction
       Codage du type Shift
           Dans as_hd.list [.+1]:
	       Etat initial:		+germe
	       Autres:			-StT (de la 1ere transition)
	   Dans forward_trans[].next:	-StT
    */
    /* Conflict_t_set contiendra les terminaux pour lesquels la construction RLR
       n'a pas reussie a supprimer les conflits */

    /* Sur l'etat initial, il faut construire toutes les transitions possibles,
       memes celles lesquelles il n'y a pas de conflit LALR(1), car ces transitions
       peuvent etre celles permettant a un ARC d'etre pur. Ex :
          germe ->a Red p
          germe ->b Red q
	  germe ->c germe
       Si les transitions sur a et b ne sont pas construites, l'ARC pur n'a pas
       de sortie! */
    int			arc_no, conflict_kind, i, ARC_kind;
    bool		 must_loop;
    struct ARC_struct	*ARC;

/* Initialisations */
    ORIG_K_VALUE = k_value;
    ORIG_H_VALUE = h_value;
    ORIG_PSS_KIND = pss_kind;
    germe = xac1;
    StNt_orig = StNt;
    RED_NO = red_no;
    IS_SHIFT_REDUCE = is_shift_reduce;
    CONFLICT_T_SET = conflict_t_set;
    THERE_IS_AN_XPROD = false;

    if (has_xprod) {
	register int lim, x = 0;

	lim = bnf_ag.WS_NBPRO [bnf_ag.WS_TBL_SIZE.actpro + 1].prolon;

	do {
	   if (-Q0xV_to_Q0 [StNt + x] >= lim) {
	       THERE_IS_AN_XPROD = true;
	       break;
	   }
	} while (++x < red_no);
    }

    do {
	must_loop = false;

	/* On doit reconstruire l'etat initial, meme si on reboucle, le clonage ayant pu le
	   changer. */
	ARC_kind = pss_kind == UNBOUNDED_ ? ARC_UNBOUNDED : ARC_BOUNDED_or_FSA;
	ARC = &(ARCs [ARC_kind]);
	
	if (!ARC->is_allocated) {
	    ARC_allocate (ARC, ARC_kind, pss_kind == UNBOUNDED_ ? ARC_UNBOUNDED_Q_oflw :
			  ARC_BOUNDED_or_FSA_Q_oflw);
	}
	
	if (!ARC->is_initiated) {
	    ARC_init_state_building (ARC);
	    ARC->conflict_t_set = conflict_t_set;
	    ARC->is_clonable = true;
	    ARC->pss_kind = pss_kind;
	    ARC->h_value = h_value;
	    ARC->k_value = k_value;
	}
	
	ARC_building (ARC, ARC_trans,
		      pss_kind == UNBOUNDED_ ? ARC_UNBOUNDED_get_conflict_kind :
		      ARC_BOUNDED_or_FSA_get_conflict_kind);
	
	if (is_automaton && is_listing_opened) {
	    for (i = 0; i < 4; i++)
		if (ARCs [i].is_initiated)
		    ARC_print (&(ARCs [i]));
	}

	if (ARC->conflict_kind == NO_CONFLICT_) {
	    solved_by_ARC (xac1, solve_something (ARC), conflict_t_set);
	    sxba_empty (conflict_t_set);
	}
	else {
	    if (is_lr_constructor) {
		if (ARC->is_clonable) {
		    int old_xac2 = xac2;
		    
		    cloning ();
		    Q0xQ0_top = XxY_top (Q0xQ0_hd);
		    undo (old_xac2);
		    must_loop = true;
		}
	    }

	    if (!must_loop) {
		if ((arc_no = solve_something (ARC)) != 0) {
		    solved_by_ARC (xac1, arc_no, ARC->t_set);
		    sxba_minus (conflict_t_set, ARC->t_set);
		}

		if (ARC->conflict_kind != NO_CONFLICT_ && conflict_message &&
		    pss_kind != UNBOUNDED_ && !is_lr_constructor && get_quad_nb () == 0 &&
		    check_lc (max_min_lgth (ARC))) {
		    /* Aucun message de pre'pare'. Aucun conflit n'a donc ete
		       detecte sur les LA (soit ce n'etait pas des prefixes
		       de contextes corrects en LALR, soit il n'etait pas
		       conflictuel). On va donc tester si c'est LALR (lgth).
		       On prend pour lgth le max des longueurs min des etats conflictuels.
		       On relance le zinzin dans tous les cas pour avoir les
		       messages. */
		    pss_kind = UNBOUNDED_;
		    k_value = NEW_K_VALUE;
		    k_value_max = max (k_value_max, k_value);
		    h_value = -1;
		    must_loop = true;
		}
	    }
	}

	if (!must_loop && ARC->conflict_kind != NO_CONFLICT_ && conflict_message &&
	    open_listing (1, language_name, constructor_name, ".la.l")) {
	    conflict_messages_output (conflict_t_set);
	}
	
	conflict_kind = NO_CONFLICT_;

	for (i = 0; i < 4; i++)
	    if (ARCs [i].is_initiated) {
		conflict_kind |= ARCs [i].conflict_kind;
		ARC_clear (&(ARCs [i]));
	    }
	
	pss_clear ();

	if (must_loop) {
	    ARC_reinit ();
	}
    } while (must_loop);

    *failed_kind = conflict_kind;
    k_value = ORIG_K_VALUE;
    h_value = ORIG_H_VALUE;
    pss_kind = ORIG_PSS_KIND;

    /* Retourne vrai ssi tous les conflits ont ete resolus. */
    return conflict_kind == NO_CONFLICT_;
}



int	ARC_get_red_no (arc_no, t)
    int arc_no, t;
{
    /* Retourne le red_no, s'il existe, de la transition depuis le germe sur t
       (resolution par un look-ahead de longueur 1), sinon 0 */

    register int	bot, next, top;

    for (top = arc [arc_no + 1], bot = arc [arc_no];bot < top;bot++) {
	if (-arc_trans [bot++] == t) {
	    if ((next = arc_trans [bot]) > 0 || -next < grand)
		return 0;

	    return -next - grand;
	}
    }

    return 0;
}


void	RLR_code_generation (xac1, arc_no, t_set, plulong)
    int		xac1, arc_no, plulong;
    SXBA	t_set;
{
    /* A chaque etat d'un sous_ARC pur pour lequel on genere une action (par
       defaut) "Error" on fait correspondre par l'intermediaire de
       germe_to_gr_act l'adresse d'une action reduce. Il s'agit de l'une
       des actions reduce non encore resolue dans le sous-arbre enracine
       a cet etat. */
    /* Ce reduce par defaut sera la "recuperation globale" du traitement
       d'erreur en look-ahead appliquee si la phase de correction a echouee. */

    /* Principe de la correction d'erreur en LookAhead :
       - Pas de message
       - Ne pas aller au-dela de la limite d'un ARC
       - Ne pas modifier la token_string
       - Ne pas tenter de correction sur le token precedent le token en erreur. */

    /* On commence par etendre le langage reconnu par arc_no afin de diminuer
       eventuellement le nombre d'etats. */
    /* On ne peut pas reutiliser l'etat initial de l'ARC dans le sur-langage
       car il serait ainsi possible de lui ajouter des transitions qui seraient
       incompatibles avec des transitions produites ulterieurement (generation
       de code sur un autre ARC par exemple.) */
    /* memorisation de l'automate avec renommage des etats. */
    int new_state_nb;

    new_state_nb = ARC_store (arc_no, t_set, is_super_arc ? &QxTxQ_hd : &AxOxV_hd);

    /* Calcul de Super_ARC et code generation */
    if (is_super_arc)
	new_state_nb = super_FSA (&Q_hd, &QxTxQ_hd, &AxOxV_hd, XTMAX, true);

    ARC_cg (xac1, new_state_nb, plulong);

    if (MAX_LA != -1) {
	max_la = 1;
	MAX_LA = ARC_max_la (1, 0) ? -1 : max (MAX_LA, max_la);
    }
}

