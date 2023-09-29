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
/* 23-10-87 14:00 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#define WHAT	"@(#)algoV.c - SYNTAX [unix] - 6 Novembre 1990"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

static char	ME [] = "algoV";

#include "sxunix.h"
#include "sxba.h"

struct state {
    int		lgth, start;
};
struct item {
    int		check;
    int		action;
};
struct attribute {
    int		poids, mint, maxt;
};
struct base {
    int		defaut, commun, propre;
};
static struct Ei {
	   int	Vj, gain;
       }	*Ei;
static struct group {
	   int	lgth, adr, etat_nb, xetat;
	   SXBOOLEAN	is_propre;
       }	*Ecp;
static struct attribute		*attributes;
static struct base	*bases;
static struct item	*cpitems, *vector;
static int	*new_to_old, *Vic, *sorted, *etats;
static int	MIN, MAX, action_no, Ei_set_card, xEcp, xcpitems, xetats, delta;
static SXBOOLEAN	up, first_time;
static SXBA	bases_set, not_yet_processed, Vic_set, Ei_set, working_set;



static SXVOID	overflow (lower, upper)
    int		*lower, *upper;
{
    register struct item	*old_vector = vector, *xnew, *xold;
    SXBA	old_bases_set = bases_set;
    int		old_size = vector [0].action, new_size;
    register int i;

    delta += action_no;
    new_size = old_size + 2 * action_no;
    vector = (struct item*) sxcalloc (new_size + 1, sizeof (struct item));
    vector [0].action = new_size;
    bases_set = sxba_calloc (new_size + 1);

    for (xnew = vector + old_size + action_no, xold = old_vector + old_size; old_vector < xold; xnew--, xold--)
	*xnew = *xold;

    i = 0;

    while ((i = sxba_scan (old_bases_set, i)) > 0) {
	/* bitassign (bases_set, action_no + 1, old_size, old_bases_set); */
	SXBA_1_bit (bases_set, action_no + i);
    }

    sxfree (old_bases_set);
    sxfree (old_vector);
    *upper += action_no;
    *lower += action_no;
    MIN += action_no;
    MAX += action_no;
}



static SXBOOLEAN	is_compatible (avector, aitem, lim)
    register struct item	*avector, *aitem, *lim;
{
    register int	act, actpi, t;
    register struct item	*pi;

    for (; aitem < lim; aitem++) {
	if ((act = aitem->action) != 0 /* pas defaut */ ) {
	    pi = avector + (t = aitem->check);

	    if ((actpi = pi->action) != 0 && (actpi != act || pi->check != t))
		return SXFALSE;
	}
    }

    return SXTRUE;
}



static int	get_next_origine (state_no, aitem, lim)
    int		state_no;
    register struct item	*aitem, *lim;
{
    static int	prev_origine, prev_state_no, lower, upper;
    static SXBOOLEAN	in_lower_upper;
    register int	mt, Mt, x;


#define min(x,y) ((x)<=(y))?(x):(y)
#define max(x,y) ((x)>=(y))?(x):(y)

    mt = attributes [state_no].mint;
    Mt = attributes [state_no].maxt;

    do {
	if (first_time) {
	    first_time = SXFALSE;
	    up = SXTRUE;
	    upper = lower = delta = action_no;
	    MIN = lower + mt;
	    MAX = lower + Mt;
	    prev_state_no = state_no;
	    prev_origine = lower;
	}
	else if (prev_state_no != state_no) {
	    prev_state_no = state_no;

	    if (MAX - MIN >= Mt - mt) {
		lower = MIN - mt;
		upper = MAX - Mt;
	    }
	    else {
		lower = MAX - Mt;
		upper = MIN - mt;
	    }

	    while (lower <= 0)
		overflow (&lower, &upper);

	    in_lower_upper = SXTRUE;
	    prev_origine = lower;
	}
	else if (in_lower_upper && prev_origine < upper)
	    ++prev_origine;
	else {
	    in_lower_upper = SXFALSE;

	    if (up) {
		upper++;
		up = SXFALSE;

		while (upper + Mt > vector [0].action)
		    overflow (&lower, &upper);

		prev_origine = upper;
	    }
	    else {
		lower--;
		up = SXTRUE;

		while (lower <= 0)
		    overflow (&lower, &upper);

		prev_origine = lower;
	    }
	}
    } while (SXBA_bit_is_set (bases_set, prev_origine) || !is_compatible (vector + prev_origine, aitem, lim));

    x = prev_origine + mt;
    MIN = min (MIN, x);
    x = prev_origine + Mt;
    MAX = max (MAX, x);
    return prev_origine;
}



static SXBOOLEAN	par_poids (i, j)
    int		i, j;
{
    register struct attribute	*ai = attributes + i, *aj = attributes + j;

    return ai->poids > aj->poids || ai->poids == aj->poids && ai->maxt - ai->mint > aj->maxt - aj->mint;
}



static SXBOOLEAN	par_gain (i, j)
    int		i, j;
{
    return Ei [i].gain > Ei [j].gain;
}



static SXVOID	put_in_Vic (aitem, lim)
    register struct item	*aitem, *lim;
{
    register int	t, act;

    for (; aitem < lim; aitem++) {
	if ((act = aitem->action) != 0) {
	    Vic [t = aitem->check] = act;
	    SXBA_1_bit (Vic_set, t);
	}
    }
}



static SXVOID	put_in_Vic_with_default (aitem, lim)
    register struct item	*aitem, *lim;
{
    register int	t, act, actVic;

    sxba_copy (working_set, Vic_set);

    for (; aitem < lim; aitem++) {
	if ((act = aitem->action) != 0 && (actVic = Vic [t = aitem->check]) != 0) {
	    if (act != actVic) {
		Vic [t] = 0;
		SXBA_0_bit (Vic_set, t);
	    }

	    SXBA_0_bit (working_set, t);
	}
    }

    t = 0;

    while ((t = sxba_scan (working_set, t)) > 0) {
	Vic [t] = 0;
	SXBA_0_bit (Vic_set, t);
    }
}



static SXVOID	get_compatibility_with_Vic (aitem, lim, size, def)
    register struct item	*aitem, *lim;
    int		*size, *def;
{
    register int	t, act, actVic;

    *def = *size = 0;
    sxba_copy (working_set, Vic_set);

    for (; aitem < lim; aitem++) {
	if ((act = aitem->action) != 0 && (actVic = Vic [t = aitem->check]) != 0) {
	    if (act != actVic)
		*def += Ei_set_card - 1 /* On suppose que la place liberee pourra
					   etre reutilisee par un autre vecteur */;
	    else
		(*size)++;

	    SXBA_0_bit (working_set, t);
	}
    }

    *def += sxba_cardinal (working_set) * (Ei_set_card - 1);
}



static SXVOID	Ecp_construction (states, items)
    struct state	*states;
    struct item		*items;

/* On remplit les structures Ecp, cpitems, etats et attributes [].poids a partir
       de Vic et Ei_set afin d'utiliser l'algotithme IV */

{
    register struct state	*astate;
    register struct item	*aitem, *lim;
    register int	Vi, Vj, t;
    register struct group	*aEcp;


/* On commence par remplir les parties propres a chaque vecteur de Ei */

    for (Vi = Vj = sxba_scan (Ei_set, 0); Vj != -1; Vj = sxba_scan (Ei_set, Vj)) {
	aEcp = Ecp + ++xEcp;
	aEcp->adr = xcpitems + 1;
	aEcp->lgth = 0;
	aEcp->is_propre = SXTRUE;
	aEcp->xetat = Vj;
	/* etat unique code directement dans xetat */
	aEcp->etat_nb = 1;
	attributes [xEcp].poids = 0;
	astate = states + new_to_old [Vj];


/* On recopie les defauts si la partie propre est non vide */

	for (lim = (aitem = items + astate->start) + astate->lgth; aitem < lim; aitem++) {
	    if ((aitem->action != 0) && !SXBA_bit_is_set (Vic_set, aitem->check))
		break /* partie propre non vide */ ;
	}

	if (aitem < lim /* partie propre non vide */ )
	    for (aitem = items + astate->start; aitem < lim; aitem++) {
		if ((t = aitem->check) != 0 && (aitem->action == 0 /* defaut */	 || !SXBA_bit_is_set (Vic_set, t))) {
		    cpitems [++xcpitems] = *aitem;
		    (aEcp->lgth)++;

		    if (aitem->action != 0 /* pas defaut */ )
			(attributes [xEcp].poids)++;
		}
	    }
    }


/* On remplit maintenant la partie commune */

    aEcp = Ecp + ++xEcp;
    aEcp->adr = xcpitems + 1;
    aEcp->lgth = 0;
    aEcp->is_propre = SXFALSE;
    aEcp->xetat = xetats + 1;
    aEcp->etat_nb = 0;
    astate = states + new_to_old [Vi];

    for (lim = (aitem = items + astate->start) + astate->lgth; aitem < lim; aitem++) {
	if ((t = aitem->check) != 0 && (aitem->action == 0 /* defaut */	 || SXBA_bit_is_set (Vic_set, t))) {
	    cpitems [++xcpitems] = *aitem;
	    (aEcp->lgth)++;
	    Vic [t] = 0;
	    SXBA_0_bit (Vic_set, t);
	}
    }

    Vj = 0;

    while ((Vj = sxba_scan (Ei_set, Vj)) > 0) {
	etats [++xetats] = Vj;
	(aEcp->etat_nb)++;
    }
}



static int	algoIV (aitem, lim, state)
    struct item		*aitem, *lim;
    int		state;
{
    register int	x;
    register struct item	*avector;

    avector = vector + (x = get_next_origine (state, aitem, lim));

    for (; aitem < lim; aitem++)
	if (aitem->action != 0 /* pas defaut */ )
	    avector [aitem->check] = *aitem;

    return x;
}



SXVOID	algoV (is_nt, statemax, tmax, xrupt1, states, items, BASES, VECTOR, BASES_SET, m, M, DELTA)
    SXBOOLEAN	is_nt;
    int		statemax, tmax, xrupt1;
    struct state	*states;
    struct item		*items, **VECTOR;
    struct base		*BASES;
    SXBA	*BASES_SET;
    int		*m, *M, *DELTA;
{
    int		state_no;

    action_no = xrupt1;
    bases = BASES;
    vector = *VECTOR;
    bases_set = *BASES_SET;
    MIN = *m;
    MAX = *M;
    delta = *DELTA;
    new_to_old = (int*) sxalloc (statemax + 1, sizeof (int));
    attributes = (struct attribute*) sxcalloc (2 * statemax + 1, sizeof (struct attribute));
    first_time = !is_nt;
    state_no = 0;

    {
	register struct item	*aitem, *lim;
	register struct attribute	*ai;
	register int	i, l;
	register SXBOOLEAN	first_nt;


/* Tri par poids decroissants des vecteurs non vides de la matrice de transition */

	for (i = 1; i < statemax; i++) {
	    if ((l = states [i].lgth) > 0) {
		ai = attributes + i;
		new_to_old [++state_no] = i;

		if (!is_nt)
		    ai->mint = 1, ai->maxt = tmax;

		first_nt = SXTRUE;

		for (lim = (aitem = items + states [i].start) + l; aitem < lim; aitem++) {
		    if (aitem->action != 0)
			(ai->poids)++;

		    if (is_nt && aitem->check != 0 /* on tient compte des defauts */ ) {
			if (first_nt) {
			    first_nt = SXFALSE;
			    ai->mint = aitem->check;
			}
			else
			    ai->maxt = aitem->check;
		    }
		}
	    }
	}
    }

    sort_by_tree (new_to_old, 1, state_no, par_poids);

/* Pour chaque vecteur non traite Vi, on cree l'ensemble Ei des vecteurs
   Vj dont l'intersection Vic avec Vi est non libre, les Ei sont stockes dans Ecp */

    not_yet_processed = sxba_calloc (state_no + 1);
    sxba_fill (not_yet_processed), SXBA_0_bit (not_yet_processed, 0);
    Vic = (int*) sxcalloc (tmax + 1, sizeof (int));
    Vic_set = sxba_calloc (tmax + 1);
    Ei_set = sxba_calloc (state_no + 1);
    Ei = (struct Ei*) sxalloc (state_no + 1, sizeof (struct Ei));
    sorted = (int*) sxalloc (2 * state_no + 1, sizeof (int));
    working_set = sxba_calloc (tmax + 1);
    cpitems = (struct item*) sxalloc (2 * action_no + 1, sizeof (struct item))
    /* ca peut deborder avec action_no, les defauts etant dupliques */
									      ;
    Ecp = (struct group*) sxalloc (2 * state_no + 1, sizeof (struct group));
    etats = (int*) sxalloc (2 * state_no + 1, sizeof (int));
    xEcp = xcpitems = xetats = 0;

    {
	register int	i, Vi, Vj, xEi;
	int	size, def;
	register struct state	*astate;
	register struct item	*aitem, *lim;

	while ((Vi = sxba_scan_reset (not_yet_processed, 0)) > 0) {
	    sxba_empty (Ei_set);
	    SXBA_1_bit (Ei_set, Vi);
	    xEi = 0;
	    Ei_set_card = 1;
	    astate = states + new_to_old [Vi];
	    lim = (aitem = items + astate->start) + astate->lgth;
	    put_in_Vic (aitem, lim);
	    Vj = Vi;

	    while ((Vj = sxba_scan (not_yet_processed, Vj)) > 0) {
		astate = states + new_to_old [Vj];
		lim = (aitem = items + astate->start) + astate->lgth;
		get_compatibility_with_Vic (aitem, lim, &size, &def);

		if (size > def) {
		    Ei [++xEi].Vj = Vj;
		    Ei [xEi].gain = size - def;
		}
	    }

	    if (xEi > 0) {
		for (i = 1; i <= xEi; i++) {
		    sorted [i] = i;
		}


/* On trie les Vj par gain */

		if (xEi > 1)
		    sort_by_tree (sorted, 1, xEi, par_gain);

		for (i = 1; i <= xEi; i++) {
		    Vj = Ei [sorted [i]].Vj;
		    astate = states + new_to_old [Vj];
		    lim = (aitem = items + astate->start) + astate->lgth;
		    get_compatibility_with_Vic (aitem, lim, &size, &def);

		    if (size > def) {
			put_in_Vic_with_default (aitem, lim);
			SXBA_0_bit (not_yet_processed, Vj);
			SXBA_1_bit (Ei_set, Vj);
			Ei_set_card++;
		    }
		}
	    }

	    Ecp_construction (states, items);
	}
    }


/* Tri par poids decroissants des vecteurs propres et communs */

    state_no = 0;

    {
	register struct item	*aitem;
	register struct attribute	*ai;
	register int	i, l;
	register SXBOOLEAN	first_nt;


/* Tri par poids decroissants des vecteurs non vides de la matrice de transition */



	for (i = 1; i <= xEcp; i++) {
	    ai = attributes + i;


/* ai->poids a deja ete (correctement) initialise dans Ecp_construction */

	    if ((l = Ecp [i].lgth) > 0) {
		sorted [++state_no] = i;

		if (is_nt) {
		    ai->mint = (aitem = cpitems + Ecp [i].adr)->check;
		    ai->maxt = (aitem + l - 1)->check;
		}
		else
		    ai->mint = 1, ai->maxt = tmax;
	    }
	}
    }

    sort_by_tree (sorted, 1, state_no, par_poids);

    {
	register int	i, x, j;
	register int	state;
	register struct group	*aEcp;
	register struct item	*aitem, *lim;

	for (i = 1; i <= state_no; i++) {
	    state = sorted [i];
	    aEcp = Ecp + state;
	    lim = (aitem = cpitems + aEcp->adr) + aEcp->lgth;
	    x = algoIV (aitem, lim, state);
	    SXBA_1_bit (bases_set, x);
	    x -= delta;

	    if (aEcp->is_propre) {
		bases [new_to_old [aEcp->xetat]].propre = x;
	    }
	    else {
		for (j = 0; j < aEcp->etat_nb; j++) {
		    bases [new_to_old [etats [aEcp->xetat + j]]].commun = x;
		}
	    }
	}
    }

    *VECTOR = vector;
    *BASES_SET = bases_set;
    *m = MIN;
    *M = MAX;
    *DELTA = delta;
    sxfree (etats);
    sxfree (Ecp);
    sxfree (cpitems);
    sxfree (working_set);
    sxfree (sorted);
    sxfree (Ei);
    sxfree (Ei_set);
    sxfree (Vic_set);
    sxfree (Vic);
    sxfree (not_yet_processed);
    sxfree (attributes);
    sxfree (new_to_old);
}
