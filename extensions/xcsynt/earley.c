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
static char	ME [] = "earley";
#endif

#include "P_tables.h"
#include "csynt_optim.h"

char WHAT_XCSYNTEARLEY[] = "@(#)SYNTAX - $Id: earley.c 3652 2023-12-24 09:43:15Z garavel $" WHAT_DEBUG;

#include "SS.h"
#include "RLR.h"


extern bool	is_t_in_first (SXINT t, SXINT item);
extern bool	check_real_conformity (SXINT *parse_stack);
extern bool	check_level_conformity (SXINT item, SXINT level);
extern bool	make_a_quad (SXINT etat_charniere, SXINT item_charniere, SXINT *items_stack);

/* Appeler earley en // avec le calcul de LA. Ca permet de reutiliser les ensembles d'items
   calcules sur les prefixes communs des chaines de pre-vision. */

/* Structure contenant la liste de longueur LA_length+1 des ensembles d'items a la earley */
/* Earley's Item Set List */
static struct EISL_stack {
    SXINT	id, nb;
} *EISL_stack;
static SXINT		*EISL_disp, EISL_stack_top;
/* ensemble d'items a la earley */

static SXBA		EI_set, xnt_set, item_set, not_item_set;

/* Un item a la earley est un doublet EI = (i, d) ou
      - i est un item LR(0)
      - si d est >0 c'est l'identifiant d'un ensemble d'items a la earley
        si d est <=0 -d est l'identifiant d'un (i1, q1) ou
	      - i1 est un item LR(0)
	      - q1 est un etat de Q0
	      i1 : A -> alpha . X1  Xn beta
	      i  : A -> alpha X1  Xn . beta */
/* Les (i, d) et (i1, q1) sont implantes dans la meme XxY soit EI_hd */

static XxY_header	EI_hd;

/* Relation from :
   XEI from XEI ou
   XEI = (EI, EIS) est un couple permettant de reperer l'item a la earley EI dans l'ensemble EIS */
/* Regarder ds Earley l'algo de construction des arbres de derivation a partir de la liste des
   ensembles. */
static XxY_header	right_son_hd, left_brother_hd;

static SXINT		next_t, zi, unused, list_elem_nb, LA_length,
    ETAT_CHARNIERE, ITEM_CHARNIERE, BS2_TOP;

static SXINT		*items_stack, *up_stack, *left_stack;
static SXBA		right_set;

static XH_header	*stack_hd;
static SXINT		LA_init, LA_cur, LA_final;

static SXINT		*parse_stack;

static SXINT		*EISL_check_stack;
static bool		make_a_quad_has_been_called;


static void	oflw_EI (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    EI_set = sxba_resize (EI_set, new_size + 1);
}

static void EISL_stack_sature (void)
{
    SXINT new_size, old_size;

    new_size = 2 * (old_size = EISL_stack [0].id);
    EISL_stack = (struct EISL_stack*) sxrealloc (EISL_stack, new_size + 1, sizeof (struct EISL_stack));
    EISL_stack [0].id = new_size;
    right_set = sxba_resize (right_set, new_size + 1);
    EISL_check_stack = (SXINT*) sxrealloc (EISL_check_stack, new_size + 1, sizeof (SXINT));

    while (new_size > old_size)
	EISL_check_stack [new_size--] = 0;
}

bool ttrue (SXINT item, SXINT LA_ref, bool is_prefixe)
{
    /* Si is_prefixe alors item : A -> alpha .B beta et
       la reconnaissance s'est arretee sur un prefixe strict de B et
       LA_ref designe le prefixe du look-ahead reconnu avant B. */
    sxuse(item); /* pour faire taire gcc -Wunused */
    sxuse(LA_ref); /* pour faire taire gcc -Wunused */
    sxuse(is_prefixe); /* pour faire taire gcc -Wunused */
    return true;
}

static void earley_alloc (SXINT l)
{
    if (EI_set == NULL) {
	EISL_stack = (struct EISL_stack*) sxalloc (xac2 + 1, sizeof (struct EISL_stack));
	EISL_stack [0].id = xac2;
	right_set = sxba_calloc (xac2 + 1);
	EISL_check_stack = (SXINT*) sxcalloc (xac2 + 1, sizeof (SXINT));
	EISL_check_stack [0] = -1;
	EISL_disp = (SXINT*) sxalloc (l + 3, sizeof (SXINT));
	EISL_disp [0] = l;
	XxY_alloc (&EI_hd, "EI", xac2, 2, 0, 0, oflw_EI, statistics_file);
	EI_set = sxba_calloc (XxY_size (EI_hd) + 1);
	XxY_alloc (&left_brother_hd, "XEI", xac2, 2, 1, 0, NULL, statistics_file);
	XxY_alloc (&right_son_hd, "from", xac2, 2, 1, 0, NULL, statistics_file);
	items_stack = SS_alloc (xac2);
	up_stack = SS_alloc (xac2);
	left_stack = SS_alloc (xac2);
	parse_stack = SS_alloc (xac2);
	xnt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xntmax + 1);
	item_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
	not_item_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.indpro + 1);
    }
    else {
	if (EISL_disp [0] < l) {
	    EISL_disp = (SXINT*) sxrealloc (EISL_disp, l + 3, sizeof (SXINT));
	    EISL_disp [0] = l;
	}

	XxY_clear (&EI_hd);
	sxba_empty (EI_set);
	XxY_clear (&left_brother_hd);
	XxY_clear (&right_son_hd);
    }
}

void earley_free (void)
{
    if (EI_set != NULL) {
	sxfree (EISL_stack);
	sxfree (right_set);
	sxfree (EISL_check_stack);
	sxfree (EISL_disp);
	XxY_free (&EI_hd);
	sxfree (EI_set), EI_set = NULL;
	XxY_free (&left_brother_hd);
	XxY_free (&right_son_hd);
	SS_free (items_stack);
	SS_free (up_stack);
	SS_free (left_stack);
	SS_free (parse_stack);
	sxfree (xnt_set);
	sxfree (item_set);
	sxfree (not_item_set);
    }
}


	
static bool check_item (SXINT item)
{
    /* On s'apprete a mettre un item a la earley qui contient l'item LR(0) "item"
       item : A -> alpha . beta
       Si cet item a deja ete mis => OK
       sinon si next_t est dans FIRST1 (beta) et un prefixe de LA est un prefixe
       d'une phrase de beta => OK. */
    bool	result;
    SXBA	set;

    if (SXBA_bit_is_set (item_set, item))
	return true;

    if (SXBA_bit_is_set (not_item_set, item))
	return false;

    if ((result = ((bnf_ag.WS_INDPRO [item].lispro == next_t && check_level_conformity (item, LA_cur - LA_init + 1)) ||
		  SXBA_bit_is_set (bnf_ag.NULLABLE, item) ||
		  (is_t_in_first (next_t, item) &&
		  ((LA_final - LA_cur) == 1 ||
		   earley_parse (LA_cur, LA_final, stack_hd, item, ttrue))))))
	set = item_set;
    else
	set = not_item_set;

    SXBA_1_bit (set, item);
    return result;
}



static SXINT ei_put (SXINT item, 
		     SXINT d, 
		     bool return_value_needed)
{
    SXINT id, x;

    sxinitialise (x); /* pour faire taire gcc -Wuninitialized */

    /* item : A -> alpha .xnt beta */
    XxY_set (&EI_hd, item, d, &id);

    if (SXBA_bit_is_reset_set (EI_set, id)) {
	EISL_stack [x = EISL_stack_top].nb = list_elem_nb;
	EISL_stack [x].id = id;

	if (++EISL_stack_top > EISL_stack [0].id)
	    EISL_stack_sature ();
    }
    else if (return_value_needed)
	for (x = EISL_disp [list_elem_nb];x < EISL_stack_top;x++) {
	    if (EISL_stack [x].id == id)
		break;
	}

    return x;
}



static bool trans (SXINT t)
{
    SXINT	id, x, item;
    SXINT			z, lim;
    bool		not_empty = false;

    lim = EISL_disp [list_elem_nb++];
    x = EISL_stack_top;
    sxba_empty (EI_set);

    while (--x >= lim) {
	id = EISL_stack [x].id;
	item = XxY_X (EI_hd, id);

	if (bnf_ag.WS_INDPRO [item].lispro == t /* && check_item (item + 1) semble inutile */) {
	    z = ei_put (item + 1, XxY_Y (EI_hd, id), true);
	    not_empty = true;
	    /* Conservation de "d" ds les transitions terminales. */
	    XxY_set (&left_brother_hd, z, x, &unused);
	}
    }

    return not_empty;
}




static bool new_EI (SXINT state, SXINT item)
{
    /* item : lhs -> . gamma dans q */
    /* next_t et zi sont positionnes par l'appelant */
    SXINT	q, n, q1, z;
    SXINT			d, lhs, StNt;
    bool		checked;

    q = state > 0 ? state : XH_list_elem (*stack_hd, -state);

    /* q ->lhs = q1 */
    
    lhs = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;
    StNt = XxY_is_set (&Q0xV_hd, state, limnttrans + lhs);

    while ((StNt = Q0xV_to_Q0xXNT [StNt]) > 0) {
	q1 = Q0xV_to_Q0 [StNt];

	/* On parcourt les items du nucleus de q1 */
	checked = state < 0 || check_bot_state_conformity (q, LA_cur - LA_init + 1);
	n = 0;
    
	while ((item = nucl_i (q1, ++n)) > 0) {
	    /* item : A -> alpha . lhs beta */
	    if ((checked || SXBA_bit_is_set (bnf_ag.NULLABLE, item + 1)) &&
		check_item (item + 1)) {
		/* d = (item, state) */
		XxY_set (&EI_hd, item, state, &d);
		z = ei_put (item + 1, -d, true);
		XxY_set (&left_brother_hd, z, 0, &unused); /* bidon */
		XxY_set (&right_son_hd, z, zi, &unused);
	    }
	}
    }
	
    return false;
}
    
    
static bool prev_state (SXINT state, SXINT item)
{
    /* Si state est >0, c'est un etat du LR(0) sinon -state designe dans stack_hd
       l'indice d'un etat du LR(0) et il faut obligatoirement "passer" par ses
       predecesseurs. */
    /* item : A -> alpha . beta est dans state.
       Appelle la fonction f pour chaque etat q tel que goto*(q, alpha) = state.
       new_EI a la possibilite d'interrompre le parcourt en retournant true */
    SXINT	x, cur_state, pred_state, pred_BS2_state;
    SXINT			indice;

    cur_state = (state > 0) ? state : XH_list_elem (*stack_hd, -state);

    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0) {
	/* item du kernel: A -> alpha X . beta */
	if (state < 0) {
	    if ((indice = -state + 1) >= BS2_TOP)
		return false;

	    pred_BS2_state = XH_list_elem (*stack_hd, indice);
	}
	else {
	  sxinitialise (indice); /* pour faire taire gcc -Wuninitialized */
	  sxinitialise (pred_BS2_state); /* pour faire taire gcc -Wuninitialized */
	}

	XxY_Yforeach (Q0xQ0_hd, cur_state, x) {
	    /* Calcul des etats predecesseurs de state (sur X) */
	    pred_state = XxY_X (Q0xQ0_hd, x);

	    if (state > 0 || pred_BS2_state == pred_state) {
		if (prev_state (state > 0 ? pred_state : -indice, item - 1))
		    return true;

		if (state < 0)
		    /* BS2 represente un chemin unique => return dans tous les cas */
		    return false; /* valeur retournee par l'appel precedent de prev_state */
	    }
	}
    }
    else /* item de la fermeture: A -> . beta */
	return new_EI (state, item);

    return false;
}


static bool check_bottom (SXINT d)
{
    /* d est un couple (item, q) */
    SXINT q;

    if ((q = XxY_Y (EI_hd, d)) > 0)
	/* Remontee "hors des listes d'items a la Earley" dans l'automate LR(0)
	   on regarde si l'etat atteint "state" est compatible avec les pss stockes
	   pour le niveau courant de look_ahead. */
	return check_bot_state_conformity (q, LA_cur - LA_init + 1);

    return true;
}

static void closure (void)
{
    SXINT	id, x, item, y;
    SXINT			xnt, lim, d, z;
    bool		hard_case;
    
    sxba_empty (xnt_set);
    hard_case = false;
    
    for (x = EISL_disp [list_elem_nb]; x < EISL_stack_top; x++) {
	id = EISL_stack [x].id;
	item = XxY_X (EI_hd, id);
	d = XxY_Y (EI_hd, id);
	
	if ((xnt = bnf_ag.WS_INDPRO [item].lispro) > 0) {
	    /* item : lhs -> alpha . xnt beta */
	    if (SXBA_bit_is_set (bnf_ag.BVIDE, xnt) && check_item (item + 1)) {
		z = ei_put (item + 1, d, true);
		XxY_set (&left_brother_hd, z, x, &unused);
		/* impossible de remplir tout de suite "right_son_hd" car on
		   n'a pas traite les "xnt -> gamma." (gamma =>* epsilon). */
		hard_case = true;
		SS_push (left_stack, z);
	    }
	    
	    if ((d > 0 || check_bottom (-d)) &&
		SXBA_bit_is_reset_set (xnt_set, xnt)) {
		for (lim = bnf_ag.WS_NTMAX [xnt + 1].npg, y = bnf_ag.WS_NTMAX [xnt].npg; y < lim; y++) {
		    item = bnf_ag.WS_NBPRO [bnf_ag.WS_NBPRO [y].numpg].prolon;
		    /* item : xnt -> . gamma */
		    /* next_t est dans First1 (xnt beta) on regarde s'il est dans
		       First1 (gamma). */
		    
		    if (check_item (item))
			ei_put (item, list_elem_nb, false);
		}
	    }
	}
	else if (xnt == 0) {
	    /* item : xnt -> gamma . */
	    if (d > 0) {
		/* On regarde les (A -> alpha . xnt beta, d) de Id */
		/* Attention, d peut etre egal a list_elem_nb. */
		/* Dans ce cas la recherche d'item de la forme ". xnt" peut ne pas
		   etre complete au moment ou elle est faite (un ". xnt" pouvant
		   apparaitre plus tard).
		   Pour traiter ce cas, lors de l'examen d'un item
		   [A -> gamma .B delta, d], si B =>+ epsilon, on ajoute
		   [A -> gamma B .delta, d] (avant de fermer B) */
		
		if (d == list_elem_nb) {
		    SS_push (up_stack, x);
		}
		else {
		    lim = EISL_disp [d + 1];
		    xnt = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;
		    
		    for (y = EISL_disp [d]; y < lim; y++) {
			id = EISL_stack [y].id;
			item = XxY_X (EI_hd, id);
			
			if (xnt == bnf_ag.WS_INDPRO [item].lispro && check_item (item + 1)) {
			    z = ei_put (item + 1, XxY_Y (EI_hd, id), true);
			    XxY_set (&left_brother_hd, z, y, &unused);
			    XxY_set (&right_son_hd, z, x, &unused);
			}
		    }
		}
	    }
	    else {
		/* d <= 0 donc -d = (item, q1) */
		/* gamma = X1 ... Xl Xl+1 ... Xn */
		/* item : xnt -> X1 ... Xl . Xl+1 ... Xn est dans q1 */
		zi = x; /* utilise dans new_EI */
		prev_state (XxY_Y (EI_hd, -d), XxY_X (EI_hd, -d));
	    }
	}
    }
    
    /* Dans le cas ou des reductions (RHS derivant dans le vide) nous ont
       laissees dans l'ensemble d'items courant i, on n'est pas sur d'avoir examine
       toutes les transitions possibles sur la LHS. Appel de closure tant
       que ce n'est pas stable */

    EISL_disp [list_elem_nb + 1] = EISL_stack_top;

    if (hard_case) {
	/* mise a jour des "right_son_hd" qui n'ont pu l'etre "on the fly". */
	while (!SS_is_empty (up_stack)) {
	    x = SS_pop (up_stack);
	    xnt = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [XxY_X (EI_hd, EISL_stack [x].id)].prolis].reduc;

	    for (y = SS_bot (left_stack); y < SS_top (left_stack); y++) {
		z = SS_get (left_stack, y);

		if (xnt == bnf_ag.WS_INDPRO [XxY_X (EI_hd, EISL_stack [z].id) - 1].lispro)
		    XxY_set (&right_son_hd, z, x, &unused);
	    }
	}

	SS_clear (left_stack);
    }
}



static bool left (SXINT x)
{
    SXINT	y, z, i, xnt;
    SXINT			t;

    if (x == 1) {
	/* "Remontee" correcte, on vient d'atteindre l'item initial. */
	make_a_quad_has_been_called = true;
	return make_a_quad (ETAT_CHARNIERE, ITEM_CHARNIERE, items_stack);
    }
    else if ((xnt = bnf_ag.WS_INDPRO [(i = XxY_X (EI_hd, EISL_stack [x].id)) - 1].lispro) == 0) {
	/* i : A -> . alpha */
	x = SS_pop (left_stack);

	if (left (x))
	    return true;

	SS_push (left_stack, x);
    } else if (xnt < 0) {
	XxY_Xforeach (left_brother_hd, x, y) {
	    if (left (XxY_Y (left_brother_hd, y)))
		return true;
	}
    }
    else /* xnt > 0 */ {
	if (SXBA_bit_is_reset_set (right_set, x)) {
	    /* Permet, dans le cas ou xnt n'a pas ete traite entierement,
	       (il n'y a pas de frere gauche -- XxY_Y (left_brother_hd, t) == 0 --)
	       de ne pas boucler sur les recursivites droites. */

	    XxY_Xforeach (left_brother_hd, x, t) {
		z = XxY_Y (left_brother_hd, t); /* z peut etre nul (jamais utilise) */
		SS_push (left_stack, z);
		
		XxY_Xforeach (right_son_hd, x, y) {
		    i = XxY_X (EI_hd, EISL_stack [z = XxY_Y (right_son_hd, y)].id);
		    SS_push (items_stack, i);
		    
		    if (left (z))
			return true;
		    
		    SS_decr (items_stack);
		}

		SS_decr (left_stack);
	    }
	}
    }
    
    return false;
}




static void build_parse_stack (SXINT x)
{
    SXINT	item, z, q1;
    SXINT			id, i, d;
       
    SS_clear_ss (parse_stack);
    id = EISL_stack [x].id;
    i = XxY_X (EI_hd, id);
    d = XxY_Y (EI_hd, id);

    if ((q1 = XxY_Y (EI_hd, -d)) < 0)
	q1 = XH_list_elem (*stack_hd, -q1);

    ETAT_CHARNIERE = q1;
    SS_push (parse_stack, q1);

    for (ITEM_CHARNIERE = item = XxY_X (EI_hd, -d); item < i; item++) {
	q1 = Q0xV_to_Q0 [XxY_is_set (&Q0xV_hd, q1, bnf_ag.WS_INDPRO [item].lispro)];
	SS_push (parse_stack, q1);
    }

    z = SS_top (up_stack);
    
    while (--z >= SS_bot (up_stack)) {
	id = EISL_stack [SS_get (up_stack, z)].id;
	i = XxY_X (EI_hd, id);
	/* vieil i : A -> alpha .B sigma1
	   i       : B -> X1  Xm . sigma2
	   On ajoute goto (q, X1   Xm) a parse_stack */

	for (item = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [i].prolis].prolon; item < i; item++) {
	    q1 = Q0xV_to_Q0 [XxY_is_set (&Q0xV_hd, q1, bnf_ag.WS_INDPRO [item].lispro)];
	    SS_push (parse_stack, q1);
	}
    }
}


static bool up (SXINT x)
{
    /* x est un index dans EISL_stack. */
    SXINT	id, item, y, nt;
    SXINT			d, bot, top, push_nb;
    bool		ret_val;

    id = EISL_stack [x].id;
    d = XxY_Y (EI_hd, id);

    if (d > 0) {
	SS_push (up_stack, x);
	nt = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item = XxY_X (EI_hd, id)].prolis].reduc;
	/* On remonte a peut pres au plus court, dans le meme etat */
	top = EISL_disp [d + 1];
	bot = EISL_disp [d];
	ret_val = false;

	if (bnf_ag.WS_INDPRO [item - 1].lispro == 0) {
	    /* item : nt -> . gamma */
	    push_nb = 0;

	    for (y = bot; y < top; y++) {
		item = XxY_X (EI_hd, id = EISL_stack [y].id);

		if (bnf_ag.WS_INDPRO [item - 1].lispro == 0 &&
		    bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc == nt &&
		    XxY_Y (EI_hd, id) == d) {
		    /* On inhibe les [nt -> . delta, d] */
		    push_nb++;
		    PUSH (EISL_check_stack, y);
		}
	    }
	}
	else {
	    push_nb = 1;
	    PUSH (EISL_check_stack, x);
	}
	
	for (y = bot; y < top;y++) {
	    if (bnf_ag.WS_INDPRO [XxY_X (EI_hd, EISL_stack [y].id)].lispro == nt &&
		EISL_check_stack [y] == 0) {
		if (up (y)) {
		    ret_val =  true;
		    break;
		}
	    }
	}
	
	SS_decr (up_stack);

	while (push_nb-- > 0)
	    POP (EISL_check_stack, y);

	return ret_val;
    }
    
    build_parse_stack (x);

    if (!check_real_conformity (parse_stack))
    /* Si ce n'est pas une "bonne" parse_stack => return false */
	return false;

    SS_clear_ss (items_stack);
    SS_clear (left_stack);
    sxba_empty (right_set);
    y = SS_top (up_stack);
    
    while (--y >= SS_bot (up_stack)) {
	SS_push (left_stack, x);
	SS_push (items_stack, XxY_X (EI_hd, EISL_stack [x].id));
	x = SS_get (up_stack, y);
    }
    
    SS_push (items_stack, XxY_X (EI_hd, EISL_stack [x].id));
    return left (x);
}


bool earley (SXINT item, 
		SXINT q, 
		SXINT LA, 
		SXINT LA_lgth, 
		XH_header *hd, 
		SXINT BS2)
{
    /* item : A -> alpha . beta est dans q
       LA, identifiant de stack_hd designe la chaine de prevision */
  SXINT	t, x;
  SXINT			d;

    make_a_quad_has_been_called = false;
    stack_hd = hd;
    LA_length = LA_lgth;
    earley_alloc (LA_length);
    EISL_stack_top = 1;
    EISL_disp [1] = list_elem_nb = 1;
    SS_clear_ss (up_stack);
    SS_clear_ss (left_stack);
    sxba_empty (item_set);
    sxba_empty (not_item_set);
    XxY_set (&EI_hd, item, BS2 == 0 ? q : -XH_X (*stack_hd, BS2), &d);
    BS2_TOP = BS2 == 0 ? 0 : XH_X (*stack_hd, BS2 + 1);
    ei_put (item, -d, false);
    next_t = XH_list_elem (*stack_hd, LA_init = XH_X (*stack_hd, LA));
    LA_cur = LA_init;
    LA_final = LA_init + LA_length;
    closure ();

    while (list_elem_nb < LA_length) {
	t = next_t;
	next_t = list_elem_nb == LA_length ? 0 : XH_list_elem (*stack_hd, LA_cur = LA_init + list_elem_nb);
	sxba_empty (item_set);
	sxba_empty (not_item_set);

	if (!trans (t))
	    return false;

	closure ();
    }

    for (x = EISL_disp [list_elem_nb]; x < EISL_stack_top;x++) {
	if (bnf_ag.WS_INDPRO [XxY_X (EI_hd, EISL_stack [x].id)].lispro < 0) {
	    SS_clear (up_stack);

	    if (up (x))
		break;
	}
    }

    return make_a_quad_has_been_called;
}
