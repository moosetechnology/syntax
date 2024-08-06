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

static char	*ME;

#include "sxversion.h"
#include "P_tables.h"
#include "csynt_optim.h"
#include "D_tables.h"
#include "bstr.h"
#include "SS.h"

char WHAT_XCSYNTLALR1[] = "@(#)SYNTAX - $Id: LALR1.c 4146 2024-08-02 10:21:59Z garavel $" WHAT_DEBUG;

extern bool	prio_read (SXINT t_priorities_size, 
			   SXINT r_priorities_size,
			   struct priority **t_priorities,
			   struct priority **r_priorities,
			   char *langname), 
                prio_free (struct priority **t_priorities, struct priority **r_priorities);
extern bool	ambiguity (SXBA conflict_set, SXBA ambig_root_set), 
                is_not_nullable (SXINT xnt);
extern SXINT	is_cycle_in_reads (void);
extern SXBA	is_not_LRpi_on_StNt (SXINT StNt);
extern VARSTR	cat_t_string (VARSTR cat_t_string_vstr, SXINT code);
extern char	*laset (VARSTR varstring, SXINT maxi, SXBA look_ahead_set);
extern bool	ante (SXINT state, SXINT item, bool (*f)(SXINT, SXINT));
extern bool	ARC_construction (SXINT *failed_kind, 
                                  SXINT xac1, 
                                  SXBA conflict_t_set, 
                                  bool is_shift_reduce, 
                                  SXINT StNt, 
                                  SXINT red_no);
/* extern void	sxtime (); */


static SXINT	**Titems_set;

static struct priority	*t_priorities /* 1:xtmax */ ;
static struct priority	*r_priorities /* 1:xnbpro */ ;
static char	star_71 [] = "***********************************************************************";
static bool	is_lalr1, is_ARCs, is_conflict, is_ARC_init_called,
                is_t_and_r_priorities;
static SXINT	processable_state_nb;
static SXINT	limt, limnt, limpro;
static SXINT	psr, dsr, rlrsr, prr, drr, rlrrr;
static SXINT	Mt_items, Mnt_items;
static SXINT	plulong;
static SXINT	*ter /* -xtmax:-1 */ ;
static SXINT	*nbt /* 0:max_red_per_state-1 */ ;
static SXBA	/* xtmax */ *look_ahead_sets /* 1:reduce_item_no */ ;
static SXBA	/* xtmax */ t_set, t2_set, t1_set;
static SXBA	ambig_root_set, conflict_set, shift_reduce_conflict_set;
static SXINT	grammar_kind;
static bool	is_users_conflict, is_systems_conflict;
static SXBA	users_conflict_set, systems_conflict_set;
struct chosen {
	   SXINT	conflict_type; /* SHIFT_REDUCE, REDUCE_REDUCE */
	   SXINT	conflict_kind; /* NO_CONFLICT_, NOT_LALRk_... */
	   SXINT	desambig_rule_kind; /* PRIO_drk, DEFAULT_drk, RLR_drk, ND_drk */
	   SXINT	action_taken; /* SHIFT, REDUCE, ERROR_ACTION, SYSTEM_ACTION, ND_ACTION,  RLR_ACTION */
	   SXINT	reduce_no;
	   struct priority	prio;
       };
static struct chosen	chosen;
static SXINT	cur_red_no, old_reduce_item_no, old_xac2;
static SXINT	*items_set;
static SXINT	*nt_trans_set, *nt_trans_stack;
static SXINT	nt_trans_set_size;
static SXINT	arc_state_nb, arc_trans_nb;
static SXINT	StNt, Nt1;
static bool print_current_conflict, print_next_conflict;
static SXINT	 next_processed_kind;

static XxY_header	Q0xT_to_conflict_action_hd;
static struct chosen	*Q0xT_to_chosen;

static struct bstr	*pre;

#define UNDEF_CONFLICT		((SXINT)0)
#define SHIFT_REDUCE		((SXINT)1)
#define REDUCE_REDUCE		((SXINT)2)

#define UNDEF_ACTION		((SXINT)0)
#define SHIFT			((SXINT)1)
#define REDUCE			((SXINT)2)
#define ERROR_ACTION		((SXINT)3)
#define SYSTEM_ACTION		((SXINT)4)
#define ND_ACTION		((SXINT)5)
#define RLR_ACTION		((SXINT)6)

#define UNDEF_drk		((SXINT)0)
#define PRIO_drk		((SXINT)1)
#define DEFAULT_drk		((SXINT)2)
#define RLR_drk			((SXINT)3)
#define ND_drk			((SXINT)4)

#define UNDEF_ASSOC		((SXINT)0)
#define LEFT_ASSOC		((SXINT)1)
#define RIGHT_ASSOC		((SXINT)2)
#define NON_ASSOC		((SXINT)3)


static struct priority	UNDEF_PRIO = {0, 0};
static struct priority	current_prio;
static bool	is_error_code_needed;
static char	header [] = "\
 LALR(1)  |                            |\n\
Conflicts |        Shift-Reduce        |        Reduce-Reduce\n\
----------|----------------------------|----------------------------\n\
          |  Prio  | %s |   Rlr  |  Prio  | %s |   Rlr\n\
          |--------|----------|--------|--------|----------|--------\n";


static void	oflw_Q0xT_to_conflict_action (SXINT old_size, SXINT new_size)
{
    sxuse(old_size); /* pour faire taire gcc -Wunused */
    Q0xT_to_chosen = (struct chosen*) sxrealloc (Q0xT_to_chosen, new_size +
	 1, sizeof (struct chosen));
}


void	CLEAR (SXINT *T)
{
    SXINT	x, y;

    for (x = T [y = 0]; x > 0; x = T [y = x])
	T [y] = 0;

    T [y] = 0;
    T [0] = -1;
}



/* A METTRE DS LR(0) */
static bool is_item_in_state (SXINT item, SXINT state)
{
    /* Retourne true ssi item est un element de state. */
    SXINT	tnt, n, item_n, is_item_in_state_StNt;
    SXINT			lim;
    struct Q0	*aQ0;

    if ((tnt = bnf_ag.WS_INDPRO [item].lispro) == 0) {
	/* reduce */
	return XxY_is_set (&Q0xV_hd,
			   state,
			   bnf_ag.WS_TBL_SIZE.xntmax + bnf_ag.WS_INDPRO [item].prolis);
    }

    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0) {
	/* item appartient a un nucleus et n'est pas complet. */
	n = 0;

	while ((item_n = nucl_i (state, ++n)) > 0 && item != item_n + 1);

	return item_n > 0;
    }


    /* item non vide de la fermeture */

    aQ0 = Q0 + state;
    lim = (is_item_in_state_StNt = aQ0->bot) + aQ0->t_trans_nb;
    item++;

    if (tnt > 0)
	is_item_in_state_StNt = lim, lim += aQ0->nt_trans_nb;
	    
    do {
	if (XxY_Y (Q0xV_hd, is_item_in_state_StNt) == tnt &&
	    is_item_in_state (item, Q0xV_to_Q0 [is_item_in_state_StNt]))
	    return true;
    } while (++is_item_in_state_StNt < lim);

    return false;
}


/* A METTRE DS AMBIG */
static SXINT *check_ambig1 (SXBA state_set)
{
    /* De'place' depuis BNF.
       Regarde s'il n'existe pas 2 regles A->alpha et A->beta telles que
       A=>alpha=>*epsilon et
       B=>beta=>*epsilon */
    /* On suppose que la grammaire n'est pas epsilon-free. */
    SXINT	nt, i, nb, xac1;
    SXINT	*epsilons;
    SXINT			item, xprod, lim;

    epsilons = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1, sizeof (SXINT));
    epsilons [0] = 0;

    nt = 0;

    while ((nt = sxba_scan (bnf_ag.BVIDE, nt)) > 0) {
	if ((lim = bnf_ag.WS_NTMAX [nt + 1].npg - 1) > (i = bnf_ag.WS_NTMAX [nt].npg)) {
	    /* Au moins deux productions ont meme LHS. */
	    nb = 0;
	    
	    for (; i <= lim; i++) {
		item = bnf_ag.WS_NBPRO [xprod = bnf_ag.WS_NBPRO [i].numpg].prolon;

		if (SXBA_bit_is_set (bnf_ag.NULLABLE, item)) {
		    /* alpha =>* epsilon */
		    ++nb;
		    epsilons [++*epsilons] = xprod;
		}
	    }
	    
	    if (nb == 1)
		--*epsilons;
	}
    }

    if ((nb = *epsilons) > 0) {
	/* Recherche des etats conflictuels impliques par une ambiguitee due
	   a ce trait de la grammaire. */
	/* Soit alpha = u v1 et beta = u v2 ou u est le plus grand prefixe commun
	   a alpha et beta.
	   Tout etat q contenant l'item A->u . v1 (ou A->u . v2) est conflictuel. */
	SXINT nt1, j, item1, xprod1, tnt;

	nt1 = bnf_ag.WS_NBPRO [xprod = epsilons [i = 1]].reduc;
	
	do {
	    nt = nt1;

	    do {
		for (j = i+1;
		     j <= nb && nt == bnf_ag.WS_NBPRO [xprod1 = epsilons [j]].reduc;
		     j++) {
		    /* on recherche le plus grand prefixe commun entre xprod et xprod1. */
		    item = bnf_ag.WS_NBPRO [xprod].prolon;
		    item1 = bnf_ag.WS_NBPRO [xprod1].prolon;

		    while ((tnt = bnf_ag.WS_INDPRO [item].lispro) ==
			   bnf_ag.WS_INDPRO [item1].lispro &&
			   tnt != 0)
			item++, item1++;

		    xac1 = 0;

		    while ((xac1 = sxba_scan (conflict_set, xac1)) > 0)
			if (is_item_in_state (item, xac1))
			    SXBA_1_bit (state_set, xac1);
		}
	    } while (++i <= nb && (nt == (nt1 = bnf_ag.WS_NBPRO [xprod = epsilons [i]].reduc)));
	} while (i <= nb);
    }

    if (nb == 0)
	sxfree (epsilons), epsilons = NULL;

    return epsilons;
}


static void ambig1_print (SXINT *epsilons, SXBA state_set)
{
    SXINT i, nt, xprod, nt1;

    put_edit_nl (1);
    put_edit (1, "This grammar is ambiguous.\n\
\tthe following productions with identical left-hand-side:");
    nt = bnf_ag.WS_NBPRO [xprod = epsilons [i = 1]].reduc;
    
    do {
	put_edit_nnl (1, varstr_tostr (prod_to_str (vstr, xprod)));
	
	if (++i <= *epsilons && nt != (nt1 = bnf_ag.WS_NBPRO [xprod = epsilons [i]].reduc)) {
	    nt = nt1;
	    put_edit_nl (1);
	}
    } while (i <= *epsilons);
    
    put_edit (1, "\tderive the empty string and hence imply conflicts in states:");

    i = sxba_scan (state_set, 0);

    do {
	put_edit_apnb (i);

	if ((i = sxba_scan (state_set, i)) > 0)
	    put_edit_apnnl (", ");
    } while (i > 0);

    put_edit_nl (2);
}




static SXBA *is_cycle_in_grammar (SXBA state_set)
{
    /* CYCLE_FREE  TEST 				*/
    /* we first compute the relation R :	 	*/
    /* <A>R<B> iff <A>-> alpha <B> beta is in P		*/
    /* with alpha beta *=> epsilon		 	*/
    /* and then <A> R+ <B>			 	*/
    /* and check wether it exists <A> such that  	*/
    /* <A> R+ <A>				 	*/

    SXINT	i;
    bool		is_cycle;
    SXINT	nt, x_lispro, y_lispro;
    SXINT			lim, head, tail, nt1, x_rule, xac1, n, item, next;
    bool		is_non_terminal;
    SXBA	*R, *Rplus, rule_set, cycle_prods;
    struct Q0		*aQ0;

    R = sxbm_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, bnf_ag.WS_TBL_SIZE.ntmax + 1);
    Rplus = sxbm_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1, bnf_ag.WS_TBL_SIZE.ntmax + 1);
    rule_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1);

    for (nt = 1; nt <= bnf_ag.WS_TBL_SIZE.ntmax; nt++) {
	for (lim = bnf_ag.WS_NTMAX [nt + 1].npg, i = bnf_ag.WS_NTMAX [nt].npg; i < lim; i++) {
	    x_rule = bnf_ag.WS_NBPRO [i].numpg;
	    
	    if ((tail = bnf_ag.WS_NBPRO [x_rule + 1].prolon - 2) - (head = bnf_ag.WS_NBPRO [x_rule].prolon) >= 0) {
		is_non_terminal = true;
		
		for (x_lispro = head; x_lispro <= tail && (is_non_terminal = (bnf_ag.WS_INDPRO [x_lispro].lispro > 0)); x_lispro
		     ++)
		    ;
		
		if (is_non_terminal) {
		    /* only non terminals */
		    for (x_lispro = head; x_lispro <= tail; x_lispro++) {
			if (SXBA_bit_is_set (bnf_ag.NULLABLE, x_lispro + 1)) {
			    for (y_lispro = head; y_lispro < x_lispro; y_lispro++) {
				nt1 = bnf_ag.WS_INDPRO [y_lispro].lispro;

				if (!SXBA_bit_is_set (bnf_ag.BVIDE, nt1))
				    break;
			    }

			    if (y_lispro == x_lispro) {
				nt1 = XNT_TO_NT_CODE (bnf_ag.WS_INDPRO [x_lispro].lispro);
				SXBA_1_bit (R [nt], nt1); /* R[nt][nt1]=true */
				SXBA_1_bit (Rplus [nt], nt1);
				SXBA_1_bit (rule_set, x_rule); /* production interessante. */
			    }
			}
		    }
		}
	    }
	}
    }

    fermer (Rplus, bnf_ag.WS_TBL_SIZE.ntmax + 1);
    is_cycle = false;

    for (i = 1; i <= bnf_ag.WS_TBL_SIZE.ntmax; i++) {
	if (SXBA_bit_is_set (Rplus [i], i)) {
	    SXBA_1_bit (Rplus [0], i);
	    is_cycle = true;
	}
    }
    
    if (is_cycle > 0) {
	/* Recherche des etats conflictuels (LALR(1)) impliques par une ambiguitee due
	   a un cycle de la grammaire. */
	/* Le calcul exact semble assez complique'!. */
	/* On retourne ici un sur-ensemble : on suppose que si un etat conflictuel
	   contient un "item du cycle", ce conflit est du au cycle. */
	SXINT A, B;

	/* On recherche les "productions des cycles." */
	cycle_prods = sxba_calloc (bnf_ag.WS_TBL_SIZE.xnbpro + 1);
	A = 0;

	while ((A = sxba_scan (Rplus [0], A)) > 0) {
	    /* cycle sur A */
	    /* On cherche les productions p: A -> alpha B beta t.q.
	         - p est dans rule_set
		 - on a A R B et B Rplus A */
	    for (lim = bnf_ag.WS_NTMAX [A + 1].npg, i = bnf_ag.WS_NTMAX [A].npg; i < lim; i++) {
		x_rule = bnf_ag.WS_NBPRO [i].numpg;

		if (SXBA_bit_is_set (rule_set, x_rule)) {
		    /* x_rule est une candidate */
		    tail = bnf_ag.WS_NBPRO [x_rule + 1].prolon - 2;
		    head = bnf_ag.WS_NBPRO [x_rule].prolon;
		
		    for (x_lispro = head; x_lispro <= tail; x_lispro++) {
			B = bnf_ag.WS_INDPRO [x_lispro].lispro;

			if (SXBA_bit_is_set (R [A], B) &&
			    SXBA_bit_is_set (Rplus [B], A)) {
			    SXBA_1_bit (cycle_prods, x_rule);
			    break;
			}
		    }
		}
	    }
	}

	xac1 = 0;

	while ((xac1 = sxba_scan (conflict_set, xac1)) > 0) {
	    /* On regarde si un item de xac1 est une production de cycle_prods. */
	    n = 0;

	    while ((item = nucl_i (xac1, ++n)) > 0) {
		x_rule = bnf_ag.WS_INDPRO [item].prolis;

		if (SXBA_bit_is_set (cycle_prods, x_rule))
		    break;
	    }

	    if (item == 0) {
		/* Il faut egalement regarder les items de la fermeture. */
		/* On regarde les transitions non_terminales seulement. */
		aQ0 = Q0 + xac1;
		lim = (StNt = aQ0->bot + aQ0->t_trans_nb) + aQ0->nt_trans_nb;

		do {
		    if ((next = Q0xV_to_Q0 [StNt]) > 0) {
			/* vers un etat non vide. */
			n = 0;

			while ((item = nucl_i (next, ++n)) > 0) {
			    x_rule = bnf_ag.WS_INDPRO [item].prolis;

			    if (SXBA_bit_is_set (cycle_prods, x_rule))
				break;
			}

			if (item != 0)
			    break;
		    }
		} while (++StNt < lim);
	    }

	    if (item != 0)
		SXBA_1_bit (state_set, xac1);
	}

	sxfree (cycle_prods);
   }

    sxbm_free (R);

    if (!is_cycle)
	sxbm_free (Rplus), Rplus = NULL;

    sxfree (rule_set);
    return Rplus;
}


static void cycle_print (SXBA *Rplus, SXBA state_set)
{
    SXINT i, nt;

    /* non_terminals involve in cycles */
    /* On partitionne cet ensemble en cycles disjoints. */
    put_edit_nl (1);
    put_edit (1, "This grammar is ambiguous.\n\
\tcycles involving the following non terminal symbols:");
    i = sxba_scan (Rplus [0], 0);
    
    while (i > 0) {
	sxba_and (Rplus [i], Rplus [0]);
	nt = i;
	
	do {
	    put_edit (1, get_nt_string (nt));
	} while ((nt = sxba_scan (Rplus [i], nt)) > 0);
	
	if ((i = sxba_scan (sxba_minus (Rplus [0], Rplus [i]), i)) > 0)
	    put_edit_nl (1);
    }
    
    put_edit (1, "\tcould imply conflicts in states:");

    i = sxba_scan (state_set, 0);

    do {
	put_edit_apnb (i);

	if ((i = sxba_scan (state_set, i)) > 0)
	    put_edit_apnnl (", ");
    } while (i > 0);

    put_edit_nl (2);
}



static void	lookback (SXINT state, SXINT item, SXINT lookback_StNt)
{
    /* Cette fonction recursive calcule la relation binaire includes entre des couples
       (lookback_StNt, StNt1) ou chaque lookback_StNt est un couple (etat, nt)
       lookback_StNt = (s, nt) includes StNt1 = (s1, nt1) ou
       "nt -> . alpha" et "nt1 -> delta . nt gamma" sont dans s
       s = goto*(s1, delta), et gamma =>* epsilon */

    if (bnf_ag.WS_INDPRO [item - 1].lispro != 0 /* item du kernel A -> alpha X .beta */ ) {
	/* Calcul des etats predecesseurs de state (sur X) */
	SXINT	x;

	XxY_Yforeach (Q0xQ0_hd, state, x) {
	    lookback (XxY_X (Q0xQ0_hd, x), item - 1, lookback_StNt);
	}
    }
    else /* item de la fermeture: A -> . beta */  {
	/* On ajoute (lookback_StNt, (state, A)) a la relation includes */
	SXINT	nt;
	SXINT	StNt1, unused, StNt_orig;

	nt /* A */  = bnf_ag.WS_NBPRO [bnf_ag.WS_INDPRO [item].prolis].reduc;
	StNt_orig = StNt1 = XxY_is_set (&Q0xV_hd, state, limnttrans + nt /* (state, A) */ );

	while ((StNt1 = Q0xV_to_Q0xXNT [StNt1]) > 0)
	    XxY_set (&includes_hd, lookback_StNt, StNt1, &unused);

	if (nt_trans_set [StNt_orig] == 0) {
	    /* StNt_orig est nouveau, on calcule donc ses predecesseurs. */
	    SXINT	xnucl;

	    PUSH (nt_trans_set, StNt_orig);
	    
	    StNt1 = StNt_orig ;

	    while ((StNt1 = Q0xV_to_Q0xXNT [StNt1]) > 0) {
		if ((xnucl = Q0xV_to_Q0 [StNt1]) < 0) {
		    /* -xnucl : B -> alpha . A => nullable */
		    lookback (state, -xnucl, StNt1);
		}
		else {
		    SXINT	*bot, *top;

		    xnucl = XQ0_TO_Q0 (xnucl);
		    bot = XH_BOT (nucleus_hd, xnucl);
		    top = XH_TOP (nucleus_hd, xnucl);

		    do {
			item = *bot;

			if (SXBA_bit_is_set (bnf_ag.NULLABLE, item + 1))
			    lookback (state, item, StNt1);
		    } while (++bot < top);
		}
	    }
	}
    }
}



void compute_Next_states_set (void)
{
    SXINT	xac1, nt, compute_Next_states_set_StNt;
    struct Q0	*aQ0;
    SXINT			lim;

    if (Next_states_set != NULL)
	return;

    Next_states_set = LR0_sets [4];

    for (xac1 = 1; xac1 < xac2;xac1++) {
	if ((lim = (aQ0 = Q0 + xac1)->nt_trans_nb) > 0) {
	    /* Il y a des transitions non terminales */
	    lim += (compute_Next_states_set_StNt = aQ0->bot + aQ0->t_trans_nb);
	    
	    do {
		if (Q0xV_to_Q0 [compute_Next_states_set_StNt] > 0) {
		    /* vers un etat non vide. */
		    nt = XxY_Y (Q0xV_hd, compute_Next_states_set_StNt);
		    
		    if (SXBA_bit_is_set (bnf_ag.BVIDE, nt)) {
			SXBA_1_bit (Next_states_set, xac1);
			break;
		    }
		}
	    } while (++compute_Next_states_set_StNt < lim);
	}

    }
}



static void	compute_includes (SXINT xac1)
{
    SXINT	compute_includes_StNt, lim;
    struct Q0	*aQ0 = Q0 + xac1;

    if ((lim = aQ0->red_trans_nb) != 0) {
	lim += (compute_includes_StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);

	do {
	    lookback (xac1, -Q0xV_to_Q0 [compute_includes_StNt] /* item */, compute_includes_StNt);
	} while (++compute_includes_StNt < lim);
    }
}



static void	set_chosen (SXINT drk, SXINT at, SXINT rn, struct priority p)
{
    chosen.desambig_rule_kind = drk;
    chosen.action_taken = at;
    chosen.reduce_no = rn;
    chosen.prio = p; /* end set_chosen */
}



static bool erase_StNt_StNt1 (SXINT St, SXINT item)
{
    /* Appelee depuis ante */
    /* Nt1 et StNt sont statiques */
    /* Nt1 est une LHS */
    SXINT	StNt1, elem;
 
    sxuse(item); /* pour faire taire gcc -Wunused */
    StNt1 = XxY_is_set (&Q0xV_hd, St, limnttrans + Nt1);

    while ((StNt1 = Q0xV_to_Q0xXNT [StNt1]) > 0) {
      if ((elem = XxY_is_set (&includes_hd, StNt, StNt1)))
	    XxY_erase (includes_hd, elem);
    }

    return false;
}

void	cut_includes (SXINT q, SXINT q1)
{
    /* Supprime tous les elements de la relation includes "passant" par q1 et q tels que
           (St, Nt) includes (St1, Nt1)
       ou St, Nt, St1 et Nt1 sont tels que:
	   Nt1 -> delta1 X delta2 . Nt gamma  est dans St
	   Nt1 -> . delta1 X delta2 Nt gamma  est dans St1
	   gamma =>* epsilon
	   St = goto* (St1, delta1 X delta2)
       On a
           goto*(St1, delta1) = q
           goto (q, X) = q1
           goto*(q1, delta2) = St */
    /* Les items du noyau de q1 sont donc de la forme:
	   Nt1 -> delta1 X . delta2 Nt gamma
       Les candidats a la suppression sont donc de la forme:
       St1 dans pred (q, delta1) et St = goto*(q1, delta2) */
    /* Attention, (St, Nt) peut etre une "origine" de la relation includes
       et donc nt peut etre superieur a XNTMAX et designer une reduction. */
       
    SXINT	*bot, *top;
    SXINT	tail, item, Nt, elem;
    SXINT			reduce, orig;


/* pour chaque item du noyau de q1 */
    orig = XQ0_TO_Q0 (q1);
    bot = XH_BOT (nucleus_hd, orig);
    top = XH_TOP (nucleus_hd, orig);

    do {
	Nt1 = bnf_ag.WS_NBPRO [reduce = bnf_ag.WS_INDPRO [item = *bot + 1].prolis].reduc;

	/* Recherche, dans la regle courante d'une position reduce ou " . Nt gamma" */
	for (tail = bnf_ag.WS_NBPRO [reduce + 1].prolon - 1;tail >= item;tail--) {
	    if ((Nt = bnf_ag.WS_INDPRO [tail].lispro) == 0 || (Nt > 0 &&
		SXBA_bit_is_set (bnf_ag.NULLABLE, tail + 1))) {
		if (Nt == 0)
		    Nt = bnf_ag.WS_TBL_SIZE.xntmax + reduce;

		/* . Nt gamma  */
		StNt = XxY_is_set (&Q0xV_hd, gotostar (q1, item, tail) /* St > 0 */, Nt);

		XxY_Xforeach (includes_hd, StNt, elem) {
		    /* Il y a des (St, Nt) includes ? */
		    ante (q, item - 1, erase_StNt_StNt1);
		    break;
		}
	    }
	    else
		break;
	}
    } while (++bot < top);
}




static void	fill_items_set (SXINT *fill_items_set_nt_trans_set, 
				SXINT *fill_items_set_nt_trans_stack, 
				SXINT nt_trans)
{
    /* Rappelons qu'un noeud est une transition non terminale cad un couple (etat, liste d'items)
       ou chaque item de la liste est un element de "etat" et repere le meme non-terminal */
    /* Cette procedure non recursive traverse une fois et une seule chaque noeud du graphe
       induit par la relation includes.
       Sur chaque noeud range chaque item de la liste dans items_set s'il ne s'y trouve pas
       deja */
    /* De plus regarde si le graphe enracine en nt_trans est un arbre (si non, possibilite
       de cycle). */

    SXINT	xpnt1, x, xnucl, X2;
    SXINT	*bot, *top;
    SXINT			item, xpnt2, fill_items_set_StNt;
    bool		is_a_tree = false;

    CLEAR (fill_items_set_nt_trans_set);
    PUSH (fill_items_set_nt_trans_set, nt_trans);
    fill_items_set_nt_trans_stack [xpnt2 = 1] = nt_trans, xpnt1 = 0;

    while (++xpnt1 <= xpnt2) {
	fill_items_set_StNt = fill_items_set_nt_trans_stack [xpnt1];

	XxY_Xforeach (includes_hd, fill_items_set_StNt, x) {
	    if (fill_items_set_nt_trans_set [X2 = XxY_Y (includes_hd, x)] == 0 /* nouvelle transition */ ) {
		PUSH (fill_items_set_nt_trans_set, X2);
		fill_items_set_nt_trans_stack [++xpnt2] = X2;

		/* item est de la forme A -> alpha . nt Y1 Y2 ... Yn
		   Les terminaux visibles sont ceux de FIRST (Y1 Y2 ... Yn) */
		/* On remplit l'ensemble items_set */
		if ((xnucl = Q0xV_to_Q0 [X2]) < 0) {
		    /* -xnucl : B -> alpha . A => nullable */
		    if (items_set [item = -xnucl + 1] == 0)
			PUSH (items_set, item);
		}
		else {
		    xnucl = XQ0_TO_Q0 (xnucl);
		    bot = XH_BOT (nucleus_hd, xnucl);
		    top = XH_TOP (nucleus_hd, xnucl);

		    do {
			if (items_set [item = *bot + 1] == 0)
			    PUSH (items_set, item);
		    } while (++bot < top);
		}
	    }
	    else
		is_a_tree = false;
	}
    }

    if (!is_a_tree) {
	if (ambig_root_set == NULL) {
	    ambig_root_set = sxba_calloc (nt_trans_set_size + 1);
	}

	SXBA_1_bit (ambig_root_set, nt_trans);
    }
}



static void	fill_look_ahead_set (SXINT *fill_look_ahead_set_items_set, SXBA la_set)
{
    /* Cette procedure remplit l'ensemble la_set avec les terminaux visibles depuis les items
       de fill_look_ahead_set_items_set. Si item est un element de fill_look_ahead_set_items_set: A -> alpha . Y1 Y2 ... Yn 
       Les terminaux visibles sont ceux de FIRST (Y1 Y2 ... Yn) */

    SXINT	x, tnt, item;

    for (x = fill_look_ahead_set_items_set [0]; x > 0; x = fill_look_ahead_set_items_set [x]) {
	item = x;

	while ((tnt = bnf_ag.WS_INDPRO [item++].lispro) != 0) {
	    if (tnt > 0) {
		sxba_or (la_set, bnf_ag.FIRST [XNT_TO_NT_CODE (tnt)]);

		if (!SXBA_bit_is_set (bnf_ag.BVIDE, tnt))
		    break;
	    }
	    else {
		SXBA_1_bit (la_set, -tnt);
		break;
	    }
	}
    }
}




/* T R A I T E M E N T   D E S   C O N F L I T S   */

/*
   R E G L E S    D E    D E S A M B I G U A T I O N
  
   The precedences and associativities are used by SYNTAX to resolve parsing
conflicts; they give rise to disambiguating rules. Formally the rules work as
follows.
  
User Disambiguating Rules (UDR):
     1.  The precedences and associativities are recorded for those tokens and
         literals that have them (specified in L.prio). The language prio is such
	 that if two tokens have the same precedence, they also have identical
	 associativity.
  
     2.  A precedence and associativity is associated with each grammar rule;
         it is the precedence and associativity of the last token or literal
         in the body of the rule. If the %prec construction is used, it
	 overrides this default. Some grammar rules may have no precedence and
	 associativity associated with them.
  
     3.  When there is a Reduce/Reduce conflict and one grammar rule has no
         precedence and associativity, or there is a Shift/Reduce conflict
         and either the input symbol or one Reduce grammar rule has no
	 precedence and associativity, then the System Disambiguating Rules
	 (SDR) are used and the conflicts are reported.
  
     4.  If there is a Shift/Reduce conflict, and both the grammar rule(s)
	 involved in the Reduce conflict(s) and the input token have
	 precedence and associativity associated with them, then the conflict
	 is resolved in favor of the action (Shift or Reduce) associated with
	 the higher precedence. If the precedences are the same, then the
	 identical associativity is used; left associative implies Reduce, right
	 associative implies Shift and nonassociating implies Error (at analysis time).
  
     5.  If there is a Reduce/Reduce conflict, and all the grammar rules have
	 precedence and associativity associated with them, then the conflict
	 is resolved in favor of the Reduce action associated with the higher
	 precedence. If the precedences are the same, then the SDR are used.
	 Associativity is meaningless in this case.
  
System Disambiguating Rules (SDR)
  
     6.  If there is a Shift/Reduce conflict and the UDR do not apply then the
         conflict is resolved in favor of the Shift action.
  
     7.  If there is a Reduce/Reduce conflict and the UDR do not apply then
	 the conflict is resolved in favor of the Reduce action involving the
	 rule which appears first in the grammar text.
*/

static void	set_conflict_action (SXINT xac1)
{
    /* L'etat xac1 est conflictuel au sens LALR(1) du terme.
       Cette procedure calcule et stocke les actions par defaut
       correspondantes (en utilisant eventuellement prio) qui
       seront generees si le conflit n'est pas resolu par une
       methode plus puissante. */
    /* Les resolutions specifiees par "prio" sont + prioritaires
       que le non-determinisme. */

    /* t_set contient les t incrimines */
    /* t2_set contient le sous-ensemble de t_set incrimine dans
       un conflit shift/reduce */
    SXINT	t, set_conflict_action_StNt, xla, red_no;
    struct Q0	*aQ0;
    SXINT			lim, min_red_no, indice;
    bool		to_be_processed;

    /* Attention, certains reduce peuvent ne pas etre conflictuels. */
    /* On commence par regarder le statuts des reduces de xac1 vis-a-vis de prio. */
    /* On calcule min_red_no. */
    t = 0;

    while ((t = sxba_scan (t_set, t)) > 0) {
	chosen.conflict_type = SXBA_bit_is_set (t2_set, t) ? SHIFT_REDUCE : REDUCE_REDUCE;
	chosen.conflict_kind = NOT_LALRk_;

	to_be_processed = false;

	if (chosen.conflict_type == SHIFT_REDUCE) {
	    /* conflit Shift/Reduce */
	    current_prio = t_priorities [t];

	    if (current_prio.assoc == UNDEF_ASSOC) {
		/* Pas de specif prio sur t ou sur l'un des reduce */
		if (is_non_deterministic_automaton)
		    set_chosen (ND_drk, ND_ACTION , (SXINT)0, UNDEF_PRIO);
		else
		    set_chosen (DEFAULT_drk, SHIFT, (SXINT)0, UNDEF_PRIO);
	    }
	    else {
		/* A completer. */
		to_be_processed = true;
		set_chosen (PRIO_drk, SHIFT, (SXINT)0, current_prio);
	    }
	}
	else {
	    /* A completer. */
	    to_be_processed = true;
	    set_chosen (PRIO_drk, REDUCE, (SXINT)0, UNDEF_PRIO);
	}
	
	if (to_be_processed) {
	    /* application des regles de priorites de "prio". */
	    /* On doit regarder les reduces */
	    lim = (aQ0 = Q0 + xac1)->red_trans_nb;
	    min_red_no = bnf_ag.WS_TBL_SIZE.actpro + 1;
	    lim += (set_conflict_action_StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
	    xla = aQ0->xla;
	    
	    do {
		if (SXBA_bit_is_set (look_ahead_sets [xla], t)) {
		    red_no = XxY_Y (Q0xV_hd, set_conflict_action_StNt) - bnf_ag.WS_TBL_SIZE.xntmax;
		    red_no = PROD_TO_RED (red_no);
		    min_red_no = min (min_red_no, red_no);
		    current_prio = r_priorities [red_no];
		    
		    if (current_prio.assoc == UNDEF_ASSOC ||
			chosen.desambig_rule_kind == DEFAULT_drk) {
			if (is_non_deterministic_automaton) {
			    set_chosen (ND_drk, ND_ACTION , (SXINT)0, UNDEF_PRIO);
			    break;
			}
			else
			    if (chosen.conflict_type == REDUCE_REDUCE)
				/* On boucle pour mettre a jour min_red_no */
				set_chosen (DEFAULT_drk, REDUCE, min_red_no, UNDEF_PRIO);
			    else {
				set_chosen (DEFAULT_drk, SHIFT, (SXINT)0, UNDEF_PRIO);
				break;
			    }
		    }
		    else if (chosen.prio.value < current_prio.value)
			set_chosen (PRIO_drk, REDUCE, red_no, current_prio);
		    else
			if (chosen.prio.value == current_prio.value &&
			    chosen.action_taken != SYSTEM_ACTION) {
			    if (chosen.action_taken == SHIFT /* &&
				chosen.prio.assoc == current_prio.assoc */) {
/* chosen.prio.assoc == current_prio.assoc est toujours vrai car s'ils ont meme
   priorite, il proviennent de la meme ligne de L.prio. */
				if (current_prio.assoc == LEFT_ASSOC)
				    set_chosen (PRIO_drk, REDUCE, red_no, current_prio);
				else if (current_prio.assoc == NON_ASSOC)
				    set_chosen (PRIO_drk, ERROR_ACTION, (SXINT)0, current_prio);
				/* else RIGHT_ASSOC on conserve Shift */
			    }
			    else set_chosen (PRIO_drk, SYSTEM_ACTION, (SXINT)0, current_prio);
			}
		}
		
		xla++;
	    } while (++set_conflict_action_StNt < lim);
	    
	    if (chosen.action_taken == SYSTEM_ACTION) {
		if (chosen.conflict_type == SHIFT_REDUCE)
		    if (is_non_deterministic_automaton)
			set_chosen (ND_drk, ND_ACTION , (SXINT)0, UNDEF_PRIO);
		    else
			set_chosen (DEFAULT_drk, SHIFT, (SXINT)0, UNDEF_PRIO);
		else
		    if (is_non_deterministic_automaton)
			set_chosen (ND_drk, ND_ACTION , (SXINT)0, UNDEF_PRIO);
		    else
			set_chosen (DEFAULT_drk, REDUCE, min_red_no, UNDEF_PRIO);
	    }
	}
	
	if (chosen.desambig_rule_kind == PRIO_drk) {
	    is_users_conflict = true;
	    SXBA_1_bit (users_conflict_set, xac1);
	    
	    if (chosen.conflict_type == SHIFT_REDUCE)
		psr++;
	    else
		prr++;
	}
	else {
	    is_systems_conflict = true;
	    SXBA_1_bit (systems_conflict_set, xac1);
	    
	    if (chosen.conflict_type == SHIFT_REDUCE)
		dsr++;
	    else
		drr++;
	}
	
	/* On range chosen */
	XxY_set (&Q0xT_to_conflict_action_hd, xac1, t, &indice);
	Q0xT_to_chosen [indice] = chosen;
    }
}


static SXINT equiv_rule_processing (SXINT equiv_rule_processing_StNt, SXINT lim, SXINT base, SXBA set)
{
    /* On est dans le cas has_xprod */
    /* Recherche des productions ayant meme representant que equiv_rule_processing_StNt
       On cumule les look-ahead dans set. */
    /* Retourne le nombre de productions equivalentes trouvees. */
    SXINT	x, nb, red_no, xred_no;

    nb = 0;

    if (equiv_rule_processing_StNt + 1 < lim) {
	xred_no = XxY_Y (Q0xV_hd, equiv_rule_processing_StNt) - bnf_ag.WS_TBL_SIZE.xntmax;
	red_no = PROD_TO_RED (xred_no);
	x = 0;

	 while (++equiv_rule_processing_StNt < lim) {
	    xred_no = XxY_Y (Q0xV_hd, equiv_rule_processing_StNt) - bnf_ag.WS_TBL_SIZE.xntmax;

	    if (PROD_TO_RED (xred_no) != red_no)
		/* Elles sont consecutives. */
		break;

	    nb++;
	    sxba_or (set, look_ahead_sets [base + ++x]);
	}
    }

    return nb;
}


static bool	compute_lalr1_conflicts (SXINT xac1)
{
    /* Recherche des conflits (eventuels) en LALR(1) */
    SXINT	r1, compute_lalr1_conflicts_StNt, xla, t;
    SXINT			lim, base_xla, base_StNt, cur_rule_no;
    SXBA	la_set_r1;
    struct Q0	*aQ0;
    bool	compute_lalr1_conflicts_is_conflict = false;

    if ((lim = (aQ0 = Q0 + xac1)->red_trans_nb) > 0) {
	/* Il y a au moins un reduce, on calcule le[s] look-ahead-set[s]
	   meme si a priori on sait (1 seul red et pas de shift) qu'il n'y
	   a pas de conflit. Cet ensemble sera utilise pour la generation
	   du Floyd-Evans. */
	lim += (base_StNt = compute_lalr1_conflicts_StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
	cur_red_no = 0;
	base_xla = xla = aQ0->xla;

	do {
	    items_set = Titems_set [cur_red_no];
	    CLEAR (items_set);
	    fill_items_set (nt_trans_set, nt_trans_stack, compute_lalr1_conflicts_StNt);
	    fill_look_ahead_set (items_set, look_ahead_sets [xla]);
	    xla++;
	    cur_red_no++;
	} while (++compute_lalr1_conflicts_StNt < lim);

	if (cur_red_no > 1 || aQ0->t_trans_nb > 0) {
	    sxba_copy (t1_set, look_ahead_sets [base_xla]);
	    cur_rule_no = cur_red_no;

	    if (has_xprod)
		cur_red_no -= (r1 = equiv_rule_processing (base_StNt, lim, base_xla, t1_set));
	    else
		r1 = 0;

	    sxba_empty (t_set);
	    
	    /* On regarde s'il y a un conflit Reduce/Reduce */
	    /* Attention: il ne peut y avoir de conflit Reduce/Reduce entre deux
	       productions etendues (leur LHS est un XNT) ayant meme representant. */
	    if (cur_red_no > 1) {
		while (++r1 < cur_rule_no) {
		    la_set_r1 = look_ahead_sets [base_xla + r1];

		    if (has_xprod)
			r1 += equiv_rule_processing (base_StNt + r1, lim, base_xla + r1, la_set_r1);

		    if (!sxba_is_empty (sxba_and (sxba_copy (t2_set, t1_set), la_set_r1))) {
			/* Conflit Reduce/Reduce => non LALR(1) */
			sxba_or (t_set, t2_set);
			
			if (!compute_lalr1_conflicts_is_conflict) {
			    SXBA_1_bit (conflict_set, xac1);
			    is_lalr1 = false;
			    compute_lalr1_conflicts_is_conflict = true;
			}
		    }
		    
		    sxba_or (t1_set, la_set_r1);
		}
	    }
	    
	    /* t1_set contient l'union des T du look_ahead */
	    /* t_set contient les T impliques dans un conflit reduce/reduce */
	    /* On regarde s'il y a un conflit Shift/Reduce, si oui => non LR(1) */
	    
	    sxba_empty (t2_set);

	    if ((lim = aQ0->t_trans_nb) > 0) {
		/* Il y a du shift. */
		lim += (compute_lalr1_conflicts_StNt = aQ0->bot);
		
		do {
		    t = -XxY_Y (Q0xV_hd, compute_lalr1_conflicts_StNt);
		    
		    if (SXBA_bit_is_set (t1_set, t)) {
			/* Conflit Shift/Reduce */
			/* La grammaire n'est pas LR(1) */
			SXBA_1_bit (t_set, t);
			SXBA_1_bit (t2_set, t);
			SXBA_1_bit (shift_reduce_conflict_set, xac1);
			
			if (!compute_lalr1_conflicts_is_conflict) {
			    compute_lalr1_conflicts_is_conflict = true;
			    SXBA_1_bit (conflict_set, xac1);
			    is_lalr1 = false;
			}
		    }	
		} while (++compute_lalr1_conflicts_StNt < lim);
	    }
	    
	    if (compute_lalr1_conflicts_is_conflict)
		/* t_set contient les t incrimines dans le conflit */
		/* t2_set contient le sous-ensemble de t_set incrimine dans
		   un conflit shift/reduce */
		set_conflict_action (xac1);
	}
    }

    return compute_lalr1_conflicts_is_conflict;
}



static struct bstr	*bstr_cat_T_set (struct bstr *bstr,
					 SXBA bstr_cat_T_set_t_set)
{
    SXINT nb, t;
    bool is_af;

    nb = sxba_cardinal (bstr_cat_T_set_t_set);

    if ((is_af = (nb > 1)))
	bstr_cat_str (bstr, "{");

    t = 0;

    while ((t = sxba_scan (bstr_cat_T_set_t_set, t)) > 0) {
	bstr_cat_str (bstr, xt_to_str (-t));
	
	if (--nb > 0)
	    bstr_cat_str (bstr, ", ");
    }

    if (is_af)
	bstr_cat_str (bstr, "}");

    return bstr;
}



static void	resize (void)
{
    SXINT	xac1, l;
    SXINT	*p, *lim;
    bool		is_new_conflict;

    LR0_sets = sxbm_resize (LR0_sets, 8, 8, xac2);
    conflict_set = LR0_sets [0];
    users_conflict_set = LR0_sets [1];
    systems_conflict_set = LR0_sets [2];
    shift_reduce_conflict_set = LR0_sets [3];
    Next_states_set = NULL;
    look_ahead_sets = sxbm_resize (look_ahead_sets, old_reduce_item_no + 1, reduce_item_no + 1, bnf_ag.WS_TBL_SIZE.xtmax +
	 1);

/* Mise a jour de la relation includes */

    if (nt_trans_set_size < (l = XxY_top (Q0xV_hd))) {
	CLEAR (nt_trans_set);
	nt_trans_set = (SXINT*) sxrealloc (nt_trans_set, l + 1, sizeof (SXINT));
	lim = nt_trans_set + l;

	for (p = nt_trans_set + nt_trans_set_size + 1; p <= lim; p++)
	    *p = 0;

	nt_trans_stack = (SXINT*) sxrealloc (nt_trans_stack, l + 1, sizeof (SXINT));
    }

    for (xac1 = old_xac2; xac1 < xac2; xac1++)
	compute_includes (xac1);

    compute_Next_states_set ();

    is_new_conflict = false;

    for (xac1 = old_xac2; xac1 < xac2; xac1++)
	is_new_conflict = is_new_conflict || compute_lalr1_conflicts (xac1);

    nt_trans_set_size = l;
}



static bool state_to_conflict_set (SXINT xac1, 
				      bool is_u, 
				      bool is_s, 
				      SXINT mask, 
				      SXBA state_to_conflict_set_t_set)
{
    /* Remplit state_to_conflict_set_t_set avec un sous_ensemble des terminaux impliques
       dans les conflits de l'etat xac1. */
    /* mask permet de ne selectionner que les conflits strictement
       "inferieurs" a lui_meme:
          mask == NO_CONFLICT_		=>	aucun conflits
          mask == ALL_CONFLICT_		=>	tous les conflits */
    SXINT	x, t, kind;
    bool	not_empty = false;

    sxba_empty (state_to_conflict_set_t_set);

    XxY_Xforeach (Q0xT_to_conflict_action_hd, xac1, x) {
	kind = Q0xT_to_chosen [x].desambig_rule_kind;

	if ((((kind != PRIO_drk) && is_s) || ((kind == PRIO_drk) && is_u)) &&
	    (mask & Q0xT_to_chosen [x].conflict_kind) != mask) {
	    t = XxY_Y (Q0xT_to_conflict_action_hd, x);
	    SXBA_1_bit (state_to_conflict_set_t_set, t);
	    not_empty = true;
	}
    }

    return not_empty;
}



void print_conclusions (SXINT xac1, SXBA print_conclusions_t_set)
{
    /* print_conclusions_t_set contient l'ensemble t des transitions terminales (initiales)
       qui n'ont pas pues etre resolues (il existe au moins une chaine
       de look_ahead commencant par t et menant a un etat impur).
       On recapitule dans le listing les action choisies. */
    SXINT t = 0, l, i;
    SXINT	indice;
    char	*tstr;
    struct chosen	*pchosen;
    static char line [] = "\
|--------------------------------------------------|--------|---------------|----------|\n";
    static char head [] = "\
|                   1-look-ahead                   |   By   |     Kind      |  Action  |\n";


    fprintf (listing, "\n\n\tConflicts resolution in state #%ld\n", (SXINT) xac1);
    fputs (line, listing);
    fputs (head, listing);
    fputs (line, listing);

    while ((t = sxba_scan (print_conclusions_t_set, t)) > 0) {
	indice = XxY_is_set (&Q0xT_to_conflict_action_hd, xac1, t);
	pchosen = &(Q0xT_to_chosen [indice]);
	put_edit_nnl (1, "|");
	tstr = xt_to_str (-t);
	
	if ((l = strlen (tstr)) > 41)
	    i = 3;
	else
	    i = (SXINT) ((50 - l) / 2) + 1;
	
	put_edit_nnl (i, tstr);
	put_edit_nnl (52, "|");

	if (pchosen->desambig_rule_kind == PRIO_drk)
	    put_edit_nnl (55, "Prio");
	else if (pchosen->desambig_rule_kind == DEFAULT_drk)
	    put_edit_nnl (54, "Default");
	else if (pchosen->desambig_rule_kind == RLR_drk)
	    put_edit_nnl (55, "Rlr ");
	else
	    put_edit_nnl (53, "NonDeter");


	put_edit_nnl (61, "|");
	put_edit_nnl (63, pchosen->conflict_type == SHIFT_REDUCE ? "Shift-Reduce" : "Reduce-Reduce");
	put_edit_nnl (77, "|");
	
	if (pchosen->action_taken == ERROR_ACTION)
	    put_edit_nnl (80, "Error");
	else
	    if (pchosen->action_taken == ND_ACTION)
		put_edit_nnl (80, "NDPDA");
	    else
		if (pchosen->action_taken == SHIFT)
		    put_edit_nnl (80, "Shift");
		else {
		    put_edit_nnl (79, "Red #");
		    put_edit_apnb (pchosen->reduce_no);
		}

	put_edit (88, "|");
    }

    fputs (line, listing);
}


static void print_minimal_message (SXINT xac1, SXBA print_minimal_message_t_set, char *gk)
{
    SXINT	print_minimal_message_StNt, lim;
    SXBA	la_set;
    struct Q0	*aQ0 = Q0 + xac1;
    bool		is_shift_reduce;
    char		str [8];
    
    if (pre == NULL)
	pre = bstr_alloc (64, 16, 100);

    put_edit_nl (2);

    if (gk != NULL)
	put_edit (1, gk);

    bstr_cat_str (bstr_raz (pre), (is_shift_reduce = SXBA_bit_is_set (shift_reduce_conflict_set, xac1)) ? "Shift" : "Reduce");
    bstr_cat_str (pre, "-Reduce conflict in state #");
    sprintf (str, "%ld ", (SXINT) xac1);
    bstr_cat_str (pre, str);
    bstr_cat_str (pre, "on ");
    bstr_cat_T_set (pre, print_minimal_message_t_set);
    bstr_cat_str (pre, " between:");
    put_edit (1, bstr_tostr (pre));

    if ((lim = aQ0->red_trans_nb) > 0) {
	SXINT	xla;

	lim += (print_minimal_message_StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
	xla = aQ0->xla;

	do {
	    if (!sxba_is_empty (sxba_and (sxba_copy (t2_set, print_minimal_message_t_set), la_set = look_ahead_sets [xla++])))
		soritem (-Q0xV_to_Q0 [print_minimal_message_StNt], 1, 71, "", laset (vstr, 60, la_set));

	} while (++print_minimal_message_StNt < lim);
    }

    if (is_shift_reduce) {
	SXINT t, n, item, next;
	
	lim = (print_minimal_message_StNt = aQ0->bot) + aQ0->t_trans_nb;

	do {
	    t = -XxY_Y (Q0xV_hd, print_minimal_message_StNt);

	    if (SXBA_bit_is_set (print_minimal_message_t_set, t)) {
		next = Q0xV_to_Q0 [print_minimal_message_StNt];
		n = 0;

		while ((item = nucl_i (next, ++n)) > 0) {
		    soritem (item, 1, 71, "", "");
		}
	    }
	} while (++print_minimal_message_StNt < lim);
    }
}


static void print_minimal_messages (void)
{
    SXINT	xac1 = 0;
    
    while ((xac1 = sxba_scan (conflict_set, xac1)) > 0) {
	if (((is_list_system_conflicts && SXBA_bit_is_set (systems_conflict_set, xac1)) ||
	     (is_list_user_conflicts && SXBA_bit_is_set (users_conflict_set, xac1))) &&
	    state_to_conflict_set (xac1, is_list_user_conflicts, is_list_system_conflicts, NOT_LRPI_, t_set)) {
	    print_minimal_message (xac1, t_set, "NOT LALR (1)"); 
	    print_conclusions (xac1, t_set);
	}
    }
}



static void set_conflict_kind (SXINT germe_local, SXBA set_conflict_kind_t_set, SXINT failed_kind, bool is_ALL)
{
    /* Positionne le conflict_kind des conflits de germe_local a failed_kind.
       Si is_ALL tous les conflits, sinon ceux de set_conflict_kind_t_set. */
    SXINT x, t;
    
    XxY_Xforeach (Q0xT_to_conflict_action_hd, germe_local, x) {
	t = XxY_Y (Q0xT_to_conflict_action_hd, x);

	if (is_ALL || SXBA_bit_is_set (set_conflict_kind_t_set, t))
	    Q0xT_to_chosen [x].conflict_kind = failed_kind;
    }
}



void solved_by_ARC (SXINT germe_local, SXINT arc_no, SXBA solved_by_ARC_t_set)
{
    /* Les conflits de solved_by_ARC_t_set sur l'etat germe_local ont ete resolus par l'ARC arc_no. */
    SXINT t = 0;
    struct chosen	*pchosen;
    
    while ((t = sxba_scan (solved_by_ARC_t_set, t)) > 0) {
	pchosen = &(Q0xT_to_chosen [XxY_is_set (&Q0xT_to_conflict_action_hd, germe_local, t)]);
	
	if (pchosen->desambig_rule_kind == PRIO_drk) {
	    if (pchosen->conflict_type == SHIFT_REDUCE)
		psr--, rlrsr++;
	    else
		prr--, rlrrr++;
	}
	else {
	    if (pchosen->conflict_type == SHIFT_REDUCE)
		dsr--, rlrsr++;
	    else
		drr--, rlrsr++;
	}
	
	pchosen->conflict_kind = NO_CONFLICT_;
	pchosen->desambig_rule_kind = RLR_drk;
	pchosen->action_taken = RLR_ACTION;
	/* On code en negatif dans reduce_no le numero de l'ARC ayant permis la resolution
	   du conflit sur t ... */
	pchosen->reduce_no = -arc_no;
	is_ARCs = true;
    }
}



bool	should_print_conflict (SXINT conflict_kind)
{
    /* Cette fonction determine a partir du conflit courant conflict_kind
       et des variables statiques print_current_conflict, print_next_conflict
       et next_processed_kind s'il faut imprimer le conflit courant. */

    if (print_current_conflict)
	return true;

    if (!print_next_conflict)
	return false;

    return (conflict_kind & next_processed_kind) == next_processed_kind;
}


static bool	call_ARC (bool is_u, bool is_s, SXINT processed_kind)
{
    SXINT	xac1 = 0;
    bool	is_RLR = true;

    do {
	old_xac2 = xac2;
	old_reduce_item_no = reduce_item_no;

	while ((xac1 = sxba_scan (conflict_set, xac1)) > 0) {
	    if (((SXBA_bit_is_set (systems_conflict_set, xac1) && is_s) ||
		 (SXBA_bit_is_set (users_conflict_set, xac1) && is_u)) &&
		state_to_conflict_set (xac1, is_u, is_s, processed_kind, t_set)) {
		struct Q0	*aQ0 = Q0 + xac1;
		SXINT	oxac2 = xac2;
		SXINT	failed_kind = 0;
		bool	is_pure;


/* Si le type de conflit detecte par l'appel courant de ARC_construction
   est trop severe pour l'appel suivant eventuel, il faut :
      - inhiber cet appel ulterieur
      - reporter la sortie eventuelle des messages sur l'appel courant. */

/* t_set est non vide, on construit l'ARC */
		is_pure = ARC_construction (&failed_kind, xac1, t_set,
					     SXBA_bit_is_set (shift_reduce_conflict_set, xac1),
					     aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb,
					     aQ0->red_trans_nb);
		/* Attention, ARC a pu reallouer Q0! */

		if (!is_pure) {
		    /* non RLR ou non LR(pi) ou non [LA]LR (k_value) ou non [LA]LR ou ambigu */
		    /* L'appel courant de ARC_construction n'a pas su resoudre les conflits
		       de t_set. On suppose (grossierement, ca pourrait peut etre etre fait
		       pour chaque terminal individuellement) que le conflit detecte pour
		       chaque t est failed_kind. On pourra l'utiliser ulterieurement pour
		       eviter par exemple de lancer du RoxoLALR si le LALR(3) a deja detecte
		       la non appartenance. */
		    set_conflict_kind (xac1, t_set, failed_kind, false);
		    grammar_kind |= failed_kind;
		    is_RLR = false;

		    if ((next_processed_kind & failed_kind) == next_processed_kind) {
			/* Le prochain traitement est moins puissant que l'origine
			de l'echec courant => inutile. */
			processable_state_nb--;
			SXBA_0_bit (conflict_set, xac1);
		    }
		}
		else {
		    /* Ici, tous les conflits ont ete resolus par l'ARC */
		    processable_state_nb--;
		    SXBA_0_bit (conflict_set, xac1);
		}

		if (xac2 > oxac2) {
		    /* Il y a eu du clonage, on recalcule. */
		    /* On suppose que le clonage a appele cut_includes () et que
		       la relation includes est correcte. */
		    SXINT	call_ARC_StNt, call_ARC_cur_red_no, xla;
		    SXINT			lim;

		    (aQ0 = Q0 + xac1);
		    lim = (call_ARC_StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb) + aQ0->red_trans_nb;
		    call_ARC_cur_red_no = 0;
		    xla = aQ0->xla;

		    do {
			items_set = Titems_set [call_ARC_cur_red_no];
			CLEAR (items_set);
			fill_items_set (nt_trans_set, nt_trans_stack, call_ARC_StNt);
			fill_look_ahead_set (items_set, sxba_empty (look_ahead_sets [xla]));
			xla++;
			call_ARC_cur_red_no++;
		    } while (++call_ARC_StNt < lim);
		}
	    }
	}

	if (xac2 > old_xac2) {
	    SXINT	opsn = processable_state_nb;

	    resize ();
	    processable_state_nb = sxba_cardinal (conflict_set);
	    XARC_resize (old_xac2, processable_state_nb - opsn /* nombre d'etats nouveaux non LALR(1) */);
	    xac1 = old_xac2 - 1;
	}
    } while (old_xac2 < xac2);

    return is_RLR;
}


char *conflict_kind_to_string (VARSTR result, SXINT conflict_kind, SXINT h_val, SXINT k_val)
{
    SXINT	ck, k, already_processed = 0;
    char		string [32];
    static SXINT		left_to_ck [] = {NO_CONFLICT_, NOT_LALRk_, NOT_LALR_, NOT_LRk_, NOT_LR_,
					     NOT_RhLALRk_, NOT_RoxoLALRk_,
					     NOT_RhLALR0_, NOT_RoxoLALR0_, NOT_RLALR_,
					     NOT_RoxoLRk_, NOT_RoxoLR0_, NOT_LRPI_, AMBIGUOUS_};

    /* Si la grammaire n'est pas RhLALR ni RoxoLALR, ca couvre le cas non LALR (1). */

    if (k_val == 1 && (conflict_kind & NOT_LALRk_) == NOT_LALRk_ &&
	((conflict_kind & NOT_RhLALR0_) == NOT_RhLALR0_ ||
	 (conflict_kind & NOT_RoxoLALR0_) == NOT_RoxoLALR0_))
	conflict_kind &= ~NOT_LALRk_;

    varstr_catenate (varstr_raz (result), "**** ");

    while (conflict_kind != 0) {
	ck = left_to_ck [k = sxlast_bit (conflict_kind)];

	if ((ck & already_processed) != ck) {
	    /* Conflit non encore traite par qqc de + puissant. */

	    if (already_processed)
		varstr_catenate (result, ", ");

	    switch (ck) {
	    case AMBIGUOUS_:
		varstr_catenate (result, "A M B I G U O U S");
		break;
		
	    case NOT_LRPI_:
		varstr_catenate (result, "N O T   L R (pi)");
		break;
		
	    case NOT_RoxoLR0_:
		varstr_catenate (result, "N O T   R (oxo) L R (0)");
		break;
		
	    case NOT_RoxoLRk_:
		sprintf (string, "N O T   R (oxo) L R (%ld)", (SXINT) k_val);
		varstr_catenate (result, string);
		break;
		
	    case NOT_RLALR_:
		varstr_catenate (result, "N O T   R - L A L R");
		break;
		
	    case NOT_RoxoLALR0_:
		varstr_catenate (result, "N O T   R (oxo) L A L R (0)");
		break;
		
	    case NOT_RhLALR0_:
		sprintf (string, "N O T   R (%ld) L A L R (0)", (SXINT) h_val);
		varstr_catenate (result, string);
		break;
		
	    case NOT_RoxoLALRk_:
		sprintf (string, "N O T   R (oxo) L A L R (%ld)", (SXINT) k_val);
		varstr_catenate (result, string);
		break;
		
	    case NOT_RhLALRk_:
		sprintf (string, "N O T   R (%ld) L A L R (%ld)", (SXINT) h_val, (SXINT) k_val);
		varstr_catenate (result, string);
		break;
		
	    case NOT_LR_:
		varstr_catenate (result, "N O T   L R");
		break;
		
	    case NOT_LRk_:
		sprintf (string, "N O T   L R (%ld)", (SXINT) k_val);
		varstr_catenate (result, string);
		break;
		
	    case NOT_LALR_:
		varstr_catenate (result, "N O T   L A L R");
		break;
		
	    case NOT_LALRk_:
		sprintf (string, "N O T   L A L R (%ld)", (SXINT) k_val);
		varstr_catenate (result, string);
		break;
		
	    case NO_CONFLICT_:
	    default:
		break;
	    }
	    
	    already_processed |= ck;
	}

	conflict_kind &= ~(1 << (k - 1));
    }
    
    return varstr_tostr (varstr_catenate (result, " ****\n"));
}    
    

static bool is_ambig_1 (SXBA StNt_set)
{
    /* Si l'un des Nt des is_ambig_1_StNt de StNt_set est non nullable, alors il y a
       ambiguite. */
    SXINT is_ambig_1_StNt = 0;

    while ((is_ambig_1_StNt = sxba_scan (StNt_set, is_ambig_1_StNt)) > 0) {
	if (is_not_nullable (XxY_Y (Q0xV_hd, is_ambig_1_StNt)))
	    return true;
    }

    return false;
}

    
#define ND_LIST_PUT	((SXINT)1)
#define ND_LIST_GET	((SXINT)2)
#define ND_LIST_CLEAR	((SXINT)4)
#define ND_LIST_FREE	((SXINT)3)

static void nd_list (SXINT kind, SXINT t, SXINT *act)
{
    /* gere, pour chaque etat les actions du non-determinisme */
    static SXINT *nd_hd, *nd_stack, *nd_lnk;

    SXINT		hd;

    switch (kind) {
    case ND_LIST_PUT:
	if (nd_hd == NULL) {
	    nd_hd = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.xtmax + 1, sizeof (SXINT));
	    nd_stack = SS_alloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
	    nd_lnk = SS_alloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
	}

	SS_push (nd_stack, *act);
	SS_push (nd_lnk, nd_hd [t]);
	nd_hd [t] = SS_top (nd_stack) - 1;
	break;

    case ND_LIST_GET:
	if ((hd = nd_hd [t]) != 0) {
	    *act = SS_get (nd_stack, hd);
	    nd_hd [t] = SS_get (nd_lnk, hd);
	}
	else
	    *act = 0;

	break;

    case ND_LIST_CLEAR:
	if (nd_hd != NULL) {
	    SS_clear (nd_stack);
	    SS_clear (nd_lnk);
	}

	break;

    case ND_LIST_FREE:
	if (nd_hd != NULL) {
	    sxfree (nd_hd), nd_hd = NULL;
	    SS_free (nd_stack);
	    SS_free (nd_lnk);
	}
	break;
    default:
#if EBUG
      sxtrap("xcsynt_LALR1","unknown switch case #1");
#endif
      break;
    }
}


    
SXINT	LALR1 (void)
{
    ME = constructor_name;

    if (sxverbosep) {
	fprintf (sxtty, "%s\n", ME);

	if (print_time)
	    sxtime (SXINIT, NULL);
    }

    if (is_statistics) {
	char	statistics_file_name [32];

	if ((statistics_file = fopen (strcat (strcpy (statistics_file_name, language_name), ".la.s"), "w")) == NULL) {
	    fprintf (sxstderr, "%s: cannot open (create) ", ME);
	    sxperror (statistics_file_name);
	}
    }
    else
	statistics_file = NULL;

    psr = prr = drr = dsr = rlrsr = rlrrr = 0;
    MAX_LA = 1; /* nombre max de tokens en avance. */
    grammar_kind = NO_CONFLICT_;
    k_value_max = k_value;
    is_lalr1 = true;
    is_not_LRpi = false;
    is_ambiguous = false;
    is_listing_opened = false;
    limt = -bnf_ag.WS_TBL_SIZE.xtmax - 1;
    limnt = bnf_ag.WS_TBL_SIZE.xntmax + 1;
    limpro = bnf_ag.WS_TBL_SIZE.xnbpro + 1;
    has_xprod = limpro > bnf_ag.WS_TBL_SIZE.actpro + 1;
    not_used_red_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.actpro + 1);
    sxba_fill (not_used_red_set);
    SXBA_0_bit (not_used_red_set, 0);

    if (is_tspe) {

/* Construction de s_red_set ensemble des reductions qui ont une lhs qui
   est la rhs d'une production simple */

	SXBA	nt_set;
	SXINT	i, j, k, r;

	s_red_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.actpro + 1);
	nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.ntmax + 1);

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.actpro; i++) {
	    if (bnf_ag.WS_NBPRO [i].bprosimpl) {
		k = bnf_ag.WS_INDPRO [bnf_ag.WS_NBPRO [i].prolon].lispro;
		k = XNT_TO_NT_CODE (k);
		SXBA_1_bit (nt_set, k);
	    }
	}

	i = 0;

	while ((i = sxba_scan (nt_set, i)) > 0) {
	    k = bnf_ag.WS_NTMAX [i + 1].npg;

	    for (j = bnf_ag.WS_NTMAX [i].npg; j < k; j++) {
		r = bnf_ag.WS_NBPRO [j].numpg;
		SXBA_1_bit (s_red_set, r);
	    }
	}

	sxfree (nt_set);
    }
    else
	s_red_set = NULL;


/* Construction de l'automate LR (0) */
    LR0 ();
    XH_lock (&nucleus_hd);
    final_state = Q0xV_to_Q0 [XxY_is_set (&Q0xV_hd, 1 /* etat initial */, 1 /* axiome */)];

    lr0_automaton_state_nb = xac2 - 1;
    LR0_sets = sxbm_calloc (8, xac2);
    conflict_set = LR0_sets [0];
    users_conflict_set = LR0_sets [1];
    systems_conflict_set = LR0_sets [2];
    shift_reduce_conflict_set = LR0_sets [3];
    Next_states_set = NULL;
    t_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
    t2_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
    t1_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xtmax + 1);
    look_ahead_sets = sxbm_calloc (reduce_item_no + 1, bnf_ag.WS_TBL_SIZE.xtmax + 1);

    if (sxverbosep)
	fputs ("\tLALR (1) construction ... ", sxtty);

/* Calcul de la relation includes */
/* On utilise nt_trans_set */

    nt_trans_set_size  = XxY_top (Q0xV_hd);
    nt_trans_set = (SXINT*) sxcalloc (nt_trans_set_size + 1, sizeof (SXINT));
    nt_trans_set [0] = -1;
    nt_trans_stack = (SXINT*) sxalloc (nt_trans_set_size + 1, sizeof (SXINT));

    {
	SXINT	xac1;

	XxY_alloc (&includes_hd, "includes", 2 * xac2, 4, 1, 0, NULL, statistics_file);

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    compute_includes (xac1);
	}
    }

    Titems_set = (SXINT**) sxalloc (max_red_per_state, sizeof (SXINT*));

    {
	SXINT	i;

	for (i = 0; i < max_red_per_state; i++) {
	    Titems_set [i] = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.indpro + 1, sizeof (SXINT));
	    Titems_set [i] [0] = -1;
	}
    }

    /* Allocation et initialisation de t_priorities et r_priorities par lecture de
       L.dt (desambig_tables). */
    is_t_and_r_priorities = prio_read (-limt, limpro, &t_priorities, &r_priorities, language_name);
    XxY_alloc (&Q0xT_to_conflict_action_hd, "Q0xT_to_conflict_action", xac2, 2, 1, 0, oflw_Q0xT_to_conflict_action, statistics_file);
    Q0xT_to_chosen = (struct chosen*) sxalloc (XxY_size (Q0xT_to_conflict_action_hd) + 1
	     , sizeof (struct chosen));
    is_users_conflict = is_systems_conflict = false;

    {
	/* On calcule les look-ahead sets de tous les items reduce, on en profite pour
	   noter dans conflict_set les etats ou la grammaire n'est pas LALR(1) */

	SXINT	xac1;

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    compute_lalr1_conflicts (xac1);
	}

	if (is_lalr1 || !is_lr_constructor) {
	    XxY_lock (&includes_hd);
	    XxY_lock (&Q0xT_to_conflict_action_hd);
	    XxY_lock (&Q0xV_hd);
	    XxY_lock (&Q0xQ0_hd);
	}

	Q0xQ0_top = XxY_top (Q0xQ0_hd);
    }

    if (!is_lalr1) {
	grammar_kind = NOT_LALRk_;
	processable_state_nb = sxba_cardinal (conflict_set);
    }
    else
	processable_state_nb = 0;

    if (sxverbosep)
	sxtime (SXACTION, processable_state_nb > 0 ? "failed" : "succeeded");

    if (is_automaton)
	open_listing (1, language_name, ME, ".la.l");

    is_ARC_init_called = false;

    if (processable_state_nb > 0 &&
	(k_value > 1 ||
	 conflict_derivation_kind != 0 ||
	 is_lr_constructor ||
	 is_rlr_constructor ||
	 is_ambiguity_check)) {
	SXINT kind;
	bool	is_epsilon_free;
	SXBA	ambig_states_by_includes = NULL,
	        nonLRpi_states_by_reads = NULL,
	        ambig_states_by_grammar_cycle = NULL,
	        ambig_states_by_identical_epsilon_derivation = NULL;
	SXBA	*Rplus;
	SXINT	*epsilons;

	if (sxverbosep)
	    fputs ("\tPartial ambiguity check ... ", sxtty);

	if (!(is_epsilon_free = sxba_scan (bnf_ag.BVIDE, 0) < 0)) {
	    ambig_states_by_identical_epsilon_derivation = sxba_calloc (xac2 + 1);

	    if ((epsilons = check_ambig1 (ambig_states_by_identical_epsilon_derivation)) != NULL) {
		SXINT i;
		
		is_ambiguous = true;
		
		if (!is_listing_opened)
		    open_listing (1, language_name, ME, ".la.l");
		
		if (is_listing_opened)
		    ambig1_print (epsilons, ambig_states_by_identical_epsilon_derivation);
		
		i = 0;
		
		while ((i = sxba_scan (ambig_states_by_identical_epsilon_derivation, i)) > 0)
		    set_conflict_kind (i, (SXBA)NULL, AMBIGUOUS_, true);
			
		sxfree (epsilons);
	    }
	    else
		sxfree (ambig_states_by_identical_epsilon_derivation),
		ambig_states_by_identical_epsilon_derivation = NULL;
	}
	    
	ambig_states_by_grammar_cycle = sxba_calloc (xac2 + 1);
	    
	if ((Rplus = is_cycle_in_grammar (ambig_states_by_grammar_cycle)) != NULL) {
	    SXINT i;

	    is_ambiguous = true;

	    if (!is_listing_opened)
		open_listing (1, language_name, ME, ".la.l");

	    if (is_listing_opened)
		cycle_print (Rplus, ambig_states_by_grammar_cycle);

	    i = 0;

	    while ((i = sxba_scan (ambig_states_by_grammar_cycle, i)) > 0)
		set_conflict_kind (i, (SXBA)NULL, AMBIGUOUS_, true);

	    sxfree (Rplus);
	}
	else
	    sxfree (ambig_states_by_grammar_cycle), ambig_states_by_grammar_cycle = NULL;

	if (ambig_root_set != NULL) {
	    if (ambiguity (conflict_set, ambig_root_set)) {
		/* ambig_root_set est modifie par ambiguity (), il ne contient plus
		   que les racines produisant une ambiguite. */
		SXINT	xac1, LALR1_StNt, lim, x;
		struct Q0	*aQ0;
		bool		LALR1_should_print_conflict;

		ambig_states_by_includes = sxba_calloc (xac2 + 1);
		LALR1_StNt = 0;
		
		while ((LALR1_StNt = sxba_scan (ambig_root_set, LALR1_StNt)) > 0) {
		    xac1 = XxY_X (Q0xV_hd, LALR1_StNt);
		    
		    if (SXBA_bit_is_reset_set (ambig_states_by_includes, xac1)) {
			set_conflict_kind (xac1, (SXBA)NULL, AMBIGUOUS_, true);
			
			if (!is_listing_opened)
			    open_listing (1, language_name, ME, ".la.l");
			
			LALR1_should_print_conflict = is_listing_opened &&
			    ((is_list_system_conflicts &&
			     SXBA_bit_is_set (systems_conflict_set, xac1)) ||
			     (is_list_user_conflicts &&
			     SXBA_bit_is_set (users_conflict_set, xac1)));
			
			if (LALR1_should_print_conflict) {
			    state_to_conflict_set (xac1,
						   is_list_user_conflicts,
						   is_list_system_conflicts,
						   ALL_CONFLICT_,
						   t_set); /* Should be true */
			    print_minimal_message (xac1,
						   t_set,
						   (char*)NULL);
			    aQ0 = Q0 + xac1;
			    lim = (x = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb) + aQ0->red_trans_nb;
			    
			    do {
				if (SXBA_bit_is_set_reset (ambig_root_set, x))
				  print_ambiguities (/* xac1, */t_set, x);
			    } while (++x < lim);
			    
			    print_conclusions (xac1, t_set);
			}
		    }
		    
		    is_ambiguous = true;
		}
	    }

	    sxfree (ambig_root_set);
	    free_ambiguities ();
	}

	/* Si un etat est ambigu, on ne regarde pas s'il est LR (pi)... */
	if (ambig_states_by_includes != NULL) {
	    sxba_minus (conflict_set, ambig_states_by_includes);
	    sxfree (ambig_states_by_includes);
	}

	if (ambig_states_by_grammar_cycle != NULL) {
	    sxba_minus (conflict_set, ambig_states_by_grammar_cycle);
	    sxfree (ambig_states_by_grammar_cycle);
	}

	if (ambig_states_by_identical_epsilon_derivation != NULL) {
	    sxba_minus (conflict_set, ambig_states_by_identical_epsilon_derivation);
	    sxfree (ambig_states_by_identical_epsilon_derivation);
	}

	if (is_ambiguous) {
	    grammar_kind |= AMBIGUOUS_;
	    processable_state_nb = sxba_cardinal (conflict_set);
	}

	if (sxverbosep)
	    sxtime (SXACTION, is_ambiguous ? "failed" : "succeeded");

	if (!is_epsilon_free) {
	    /* On calcule les cycles de reads. */
	    if (sxverbosep)
		fputs ("\tPartial LR (pi) check ... ", sxtty);

	    if ((kind = is_cycle_in_reads ()) != NO_CONFLICT_ &&
		processable_state_nb > 0) {
		SXINT	xac1;
		SXINT		lim, LALR1_StNt_2;
		struct Q0	*aQ0;
		SXBA			StNt_set2;
		bool			LALR1_should_print_conflict_2;
		
		is_not_LRpi = true;
		nonLRpi_states_by_reads = sxba_calloc (xac2 + 1);
		xac1 = 0;

		while ((xac1 = sxba_scan (conflict_set, xac1)) > 0) {
		    aQ0 = Q0 + xac1;
		    lim = (LALR1_StNt_2 = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb) +
			aQ0->red_trans_nb;
		    
		     do {
			if ((StNt_set2 = is_not_LRpi_on_StNt (LALR1_StNt_2)) != NULL) {
			    /* StNt_set2 contient des StNt impliques dans
			       des cycles de reads. */
			    if (SXBA_bit_is_reset_set (nonLRpi_states_by_reads, xac1)) {
				kind = is_ambig_1 (StNt_set2) ? AMBIGUOUS_ : NOT_LRPI_;
				grammar_kind |= kind;
				set_conflict_kind (xac1, (SXBA)NULL, kind, true);
				
				if (!is_listing_opened)
				    open_listing (1, language_name, ME, ".la.l");
				
				LALR1_should_print_conflict_2 = is_listing_opened &&
				    ((is_list_system_conflicts &&
				     SXBA_bit_is_set (systems_conflict_set, xac1)) ||
				     (is_list_user_conflicts &&
				     SXBA_bit_is_set (users_conflict_set, xac1)));
				
				if (LALR1_should_print_conflict_2) {
				    print_not_LRpi_conflicts (kind); /* utilise StNt_set2. */
				    state_to_conflict_set (xac1,
							   is_list_user_conflicts,
							   is_list_system_conflicts,
							   ALL_CONFLICT_,
							   t_set); /* Should be true */
				    print_minimal_message (xac1,
							   t_set,
							   (char*)NULL);
				    print_conclusions (xac1, t_set);
				}
			    }
			}
		    } while (++LALR1_StNt_2 < lim);
		}

		sxba_minus (conflict_set, nonLRpi_states_by_reads);
		sxfree (nonLRpi_states_by_reads);
		processable_state_nb = sxba_cardinal (conflict_set);
	    }
	    
	    free_not_LRpi ();

	    if (sxverbosep)
		sxtime (SXACTION, is_not_LRpi ? (kind == NOT_LRPI_ ? "failed" : "failed (ambiguous)") : "succeeded");
	}
    }
    
    arc_state_nb = arc_trans_nb = 0;
    is_ARCs = false;
    germe_to_gr_act = NULL;
    xe1 = (init_state = xac2) + 1;

    if (processable_state_nb > 0) {
	bool is_u, is_s, old_is_lr_constructor;
	SXINT	old_h_value, old_k_value;

	is_u = is_list_user_conflicts && is_users_conflict;
	is_s = is_list_system_conflicts && is_systems_conflict;
	
	if (!is_listing_opened)
	    open_listing (1, language_name, ME, ".la.l");

	if (is_listing_opened &&
	    (is_u || (((is_s && k_value == 1) && !is_rlr_constructor) && !is_lr_constructor))) {
	    /* On simule le cas LALR(1) pour sortir les messages des conflits
	       en LALR(1) si on ne demande pas de methode plus
	       puissante ou, dans tous les cas, si l'utilisateur a demande
	       de sortir les conflits resolus par prio. */

	    if (sxverbosep)
		fprintf (sxtty, "\t%s%s%s LALR (1) conflict messages ... ",
			 is_u ? "Prio" : "",
			 is_u && is_s ? " & " : "",
			 is_s ? "System" : "");

	    if (conflict_derivation_kind == 0)
		print_minimal_messages ();
	    else {
		old_h_value = h_value;
		old_k_value = k_value;
		old_is_lr_constructor = is_lr_constructor;
		h_value = -1; /* UNBOUNDED_ */
		k_value = 1, is_lr_constructor = false; /* LALR (1) */
		/* L'initialisation de print_next_conflict est inutile. */
		ARC_init (print_current_conflict = true, UNBOUNDED_, max_red_per_state, processable_state_nb);
		is_ARC_init_called = true;
		call_ARC (is_u, is_s, next_processed_kind = NOT_LRPI_);
		h_value = old_h_value;
		k_value = old_k_value;
		is_lr_constructor = old_is_lr_constructor;
	    }

	    if (sxverbosep)
		sxtime (SXACTION, "done");
	}
	    
	if (is_systems_conflict) {
	    if (!is_rlr_constructor && (k_value > 1 || (k_value > 0 && is_lr_constructor))) {
		/* Cas [LA]LR. */
		/* Certaines ambiguites et conflits LR (k) ont deja ete testes
		   Le [LA]LR (k_value) n'est lance que sur les etats conflictuels non detectes
		   comme etant ambigus ou non LR (pi). */
		if (sxverbosep)
		    fprintf (sxtty, "\t%sLR (%ld) construction%s ... ", is_lr_constructor ? "" : "LA", (SXINT) k_value, is_automaton &&
			     is_listing_opened ? " & ARC output" : "");
		
		next_processed_kind = is_lr_constructor ? NOT_RoxoLR0_ :
					 (h_value == 0 ? NOT_RoxoLALR0_ : NOT_RLALR_);
		print_current_conflict = (bool) (!is_rlr_constructor && is_list_system_conflicts);
		print_next_conflict = is_list_system_conflicts;
		old_h_value = h_value;
		h_value = -1; /* UNBOUNDED_ */
		/* Sortie eventuelle des conflits. */
		ARC_init (is_list_system_conflicts, UNBOUNDED_, max_red_per_state, processable_state_nb);
		is_ARC_init_called = true;
		is_conflict = !call_ARC (false, true, NOT_LRPI_);
		h_value = old_h_value;
		
		if (sxverbosep)
		    sxtime (SXACTION, is_conflict > 0 ? "failed" : "succeeded");
	    }
	    else if (is_rlr_constructor) {
		/* Cas R[LA]LR */
		/* certaines ambiguites ont deja ete testees
		   Le RLR n'est lance que sur les etats conflictuels non detectes
		   comme etant ambigus ou non LR (pi). */
		if (is_lr_constructor)
		    h_value = 0;
		
		if (sxverbosep) {
		    char	name [24];

		    fprintf (sxtty, "\t%s construction",
			     get_constructor_name (name, (bool) (!is_lr_constructor), true, h_value, k_value));
		    
		    if (is_automaton && is_listing_opened)
			fputs (" & ARC output ... ", sxtty);
		    else
			fputs (" ... ", sxtty);
		}
		
		print_current_conflict = is_list_system_conflicts;
		print_next_conflict = false;
		next_processed_kind = NO_CONFLICT_;
		ARC_init (is_list_system_conflicts, h_value == 0 ? FSA_ : BOUNDED_, max_red_per_state, processable_state_nb);
		is_ARC_init_called = true;
		is_conflict = !call_ARC (false, true, is_lr_constructor ? NOT_RoxoLR0_ :
					 (h_value == 0 ? NOT_RoxoLALR0_ : NOT_RLALR_));
		
		if (sxverbosep)
		    sxtime (SXACTION, is_conflict > 0 ? "failed" : "succeeded");
	    }
	    
	    if (sxverbosep && lr0_automaton_state_nb < xac2 - 1) {
		/* On sort les clones au terminal. */
		for (xe1 = lr0_automaton_state_nb + 1; xe1 < xac2; xe1++) {
		    fprintf (sxtty, "\t\tState #%ld is a clone of #%ld.\n", (SXINT) xe1, (SXINT) clone_to_lr0_state [xe1 - lr0_automaton_state_nb]);
		}
	    }

	    xe1 = (init_state = xac2) + 1;

	    if (is_ARCs) {
		ARC_get_sizes (&arc_state_nb, &arc_trans_nb);
		xe1 += arc_state_nb /* - arc_nb On ne decompte plus les germes car l'etat initial
				     peut etre duplique (non reutilise) */ ;
		germe_to_gr_act = (SXINT*) sxcalloc (xe1, sizeof (SXINT));
	    }
	}

	if (is_ARC_init_called)
	    ARC_free ();

	if (is_lr_constructor) {
	    XxY_lock (&Q0xT_to_conflict_action_hd);
	    XxY_lock (&Q0xV_hd);
	    XxY_lock (&Q0xQ0_hd);
	}
    }

    if (pre != NULL)
	bstr_free (pre), pre = NULL;

    free_cycles ();
    XxY_free (&includes_hd);

    if (is_automaton && is_listing_opened) {
      SXINT		xac1, x, z;
	struct Q0	*aQ0;
	SXINT			LALR1_StNt_3, lim;

	if (sxverbosep)
	    fputs ("\tAutomaton Listing Output ... ", sxtty);

	put_edit (1, "*****        A U T O M A T O N       *****");

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    put_edit_nl (6);
	    put_edit_nnl (1, "STATE ");
	    put_edit_apnb (xac1);

	    if (xac1 > lr0_automaton_state_nb && clone_to_lr0_state != NULL) {
		put_edit_apnnl (" (clone of ");
		put_edit_apnb (clone_to_lr0_state [xac1 - lr0_automaton_state_nb]);
		put_edit_apnnl (")");
	    }

	    if (xac1 != 1) {
		SXINT	n, state;

		put_edit_nnl (31, "Transition on ");
		z =  Q0_to_tnt_trans (xac1);
		put_edit_apnnl ((z > 0) ? get_nt_string (z) : xt_to_str (z));
		put_edit_apnnl (" from ");
		n = 0;
		sxinitialise (state); /* pour faire taire gcc -Wuninitialized */

		XxY_Yforeach (Q0xQ0_hd, xac1, x) {
		    if (++n > 1)
			x = 0 /* ont sort du foreach */;
		    else
			state = XxY_X (Q0xQ0_hd, x);
		}

		if (n == 1) {
		    put_edit_apnnl ("state ");
		    put_edit_apnb (state);
		    put_edit_nl (1);
		}
		else {
		    put_edit_ap ("states:");
		    put_edit_nb (11, state);
		    n = 0;

		    XxY_Yforeach (Q0xQ0_hd, xac1, x) {
			if (++n > 1) {
			    put_edit_apnnl (", ");
			    state = XxY_X (Q0xQ0_hd, x);

			    if (put_edit_get_col () >= 81)
				put_edit_nb (11, state);
			    else
				put_edit_apnb (state);
			}
		    }
		}
	    }

	    put_edit (1, star_71);
	    sornt (xac1);
	    put_edit (1, star_71);
	    sort (xac1);
	    put_edit (1, star_71);

	    if ((lim = (aQ0 = Q0 + xac1)->red_trans_nb) > 0) {
		SXINT	LALR1_xla;

		lim += (LALR1_StNt_3 = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
		LALR1_xla = aQ0->xla;

		do {
		    soritem (-Q0xV_to_Q0 [LALR1_StNt_3], 1, 71, "*", laset (vstr, 60, look_ahead_sets [LALR1_xla++]));
		} while (++LALR1_StNt_3 < lim);
	    }

	    put_edit (1, star_71);
	}

	if (sxverbosep)
	    sxtime (SXACTION, "done");
    }

    {
	/* On calcule une borne max de la taille du genere pour

	   les transitions terminales+reduces (Mt_items).
	   si xac1 n'a pas de reduce     => +tt + 1 (error)
	   si xac1 a 1 reduce	     => +tmax (prudence)
	   si xac1 a >1 reduce	     => +tmax (prudence)

	   les transitions non-terminales (Mnt_items) */

	SXINT		xac1, r;
	struct Q0	*aQ0;

	Mt_items = Mnt_items = 0;
	aQ0 = Q0;

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    aQ0++;
	    Mt_items += aQ0->t_trans_nb + 1;
	    Mnt_items += aQ0->nt_trans_nb;
	    r = aQ0->red_trans_nb;

	    if (r > 0 /* Il y a au moins 1 reduce */ )
		Mt_items += bnf_ag.WS_TBL_SIZE.xtmax * (is_non_deterministic_automaton ? r : 1);
	}
    }


/* Floyd_Evans des transitions non-terminales */

    {
	SXINT	xac1, LALR1_StNt_4, x, lim;
	SXINT		xtant_fe, next, xnt;
	struct Q0	*aQ0;
	struct P_item	*aitem;
	struct state	*astate;
	SXINT	*tant_fe;
	bool		is_xnt;
	SXBA	nt_set;

	if (sxverbosep)
	    fputs ("\tNT_Tables ... ", sxtty);

	xnt_state_set = NULL;
	nt_set = sxba_calloc (bnf_ag.WS_TBL_SIZE.xntmax + 1);
	xpredicates = xprdct_items = 0;
	any = bnf_ag.WS_TBL_SIZE.xtmax + 1;
	t_states = (struct state*) sxcalloc (xe1 + xac2 /* tous les nt en ScanReduce */
							+ 1, sizeof (struct state));
	t_items = (struct P_item*) sxalloc (Mt_items + 2 * xac2 /* tous les nt en ScanReduce */
								+ 4 /* etats initial et final */
								    + arc_trans_nb + arc_state_nb /* ARC trans + error */
												  + 1, sizeof (struct
	     P_item));
	t_fe = (struct floyd_evans*) sxalloc (x = Mt_items + xac2 /* tous les nt en ScanReduce */
								  + 2 /* etats initial et final */
								      + 2 /* [la]error */
									  + arc_trans_nb /* ARCs */
											 + 1, sizeof (struct floyd_evans))
	     ;
	t_lnks = (SXINT*) sxalloc (x, sizeof (SXINT));
	nt_states = (struct state*) sxcalloc (xac2, sizeof (struct state));
	nt_items = (struct P_item*) sxalloc (Mnt_items + 1, sizeof (struct P_item));
	x = bnf_ag.WS_TBL_SIZE.xnbpro + xe1 + 1;
	nt_fe = (struct floyd_evans*) sxalloc (x + 1, sizeof (struct floyd_evans));
	nt_lnks = (SXINT*) sxalloc (x + 1, sizeof (SXINT));
	tant_fe = (SXINT*) sxcalloc (x + 1, sizeof (SXINT)) + bnf_ag.WS_TBL_SIZE.xnbpro;

	for (x = 0; x < HASH_SIZE; x++)
	    t_hash [x] = nt_hash [x] = 0;

	nt_lnks [0] = t_lnks [0] = 0;
	xnt_items = xt_items = 1;
	t_fe->codop = Error;
	t_fe->reduce = 0;
	t_fe->pspl = 0;
	t_fe->next = 0;
	t_error = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
	/* Correction du Mer 24 Nov 1999 11:23:24 : t_error est utilise ds OPTIM (sequence de predicats)
	   ds le fe des transitions non-terminales */
	put_in_fe (nt_fe, nt_hash, nt_lnks, t_fe); /* Retourne t_error */
	force_error = 0;

/* On genere l'etat initial */

	astate = t_states + init_state;
	astate->lgth = 2;
	astate->start = xt_items;
	t_fe->codop = ScanShift;
	t_fe->reduce = 0;
	t_fe->pspl = 1;
	t_fe->next = 1;
	(aitem = t_items + xt_items++)->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
	aitem->check = -bnf_ag.WS_TBL_SIZE.tmax /* Test de "End Of File" en debut de texte source */ ;
	(aitem = t_items + xt_items++)->action = t_error;
	aitem->check = any;

	for (xac1 = 1; xac1 < xac2; xac1++) {
	    if ((lim = (aQ0 = Q0 + xac1)->nt_trans_nb) > 0) {
		astate = nt_states + xac1;
		aitem = nt_items + (astate->start = xnt_items);
		lim += (LALR1_StNt_4 = aQ0->bot + aQ0->t_trans_nb);
		is_xnt = false;

		do {
		    xnt = aitem->check = XxY_Y (Q0xV_hd, LALR1_StNt_4);
		    next = Q0xV_to_Q0 [LALR1_StNt_4];

		    if (aitem->check > bnf_ag.WS_TBL_SIZE.ntmax) {
			is_xnt = true;
			xnt = XNT_TO_NT_CODE (xnt);
			SXBA_1_bit (nt_set, xnt);

			if (xnt_state_set == NULL)
			    xnt_state_set = sxba_calloc (xac2);

			SXBA_1_bit (xnt_state_set, xac1);
			xprdct_items++;
		    }

		    xtant_fe = next < 0 ? -bnf_ag.WS_INDPRO [-next].prolis : next;

		    if ((aitem->action = tant_fe [xtant_fe]) == 0) {
			if (next < 0) {
			    /* pas de successeur => un seul item en fin de partie droite */
			    nt_fe->codop = Reduce;
			    nt_fe->reduce = bnf_ag.WS_INDPRO [-next].prolis;
			    nt_fe->next = bnf_ag.WS_NBPRO [nt_fe->reduce].reduc;
			    nt_fe->next = XNT_TO_NT_CODE (nt_fe->next);
			    nt_fe->pspl = LGPROD (nt_fe->reduce) - 1;
			    nt_fe->reduce = PROD_TO_RED (nt_fe->reduce);
			    SXBA_0_bit (not_used_red_set, nt_fe->reduce);
			}
			else {
			    /* cas general */
			    nt_fe->codop = Shift;
			    nt_fe->reduce = 0;
			    nt_fe->next = next;
			    nt_fe->pspl = Q0 [next].nt_trans_nb == 0 ? 0 : next;
			}

			tant_fe [xtant_fe] = aitem->action = put_in_fe (nt_fe, nt_hash, nt_lnks, nt_fe);
		    }

		    aitem++;
		    xnt_items++;
		    astate->lgth++;
		} while (++LALR1_StNt_4 < lim);

		if (is_xnt) {
		    xpredicates += sxba_cardinal (nt_set) /* nb de sequences */ ;
		    sxba_empty (nt_set);
		}
	    }
	}

	sxfree (tant_fe - bnf_ag.WS_TBL_SIZE.xnbpro);
	sxfree (nt_set);

	if (sxverbosep)
	    sxtime (SXACTION, "done");
    }


/* Floyd_Evans des transitions terminales */

    {
	if (sxverbosep)
	    fputs ("\tT_Tables ... ", sxtty);

	ter = (SXINT*) sxcalloc (-limt, sizeof (SXINT));
	nbt = (SXINT*) sxalloc (max_red_per_state + 1, sizeof (SXINT));
	xt_state_set = NULL;

	{
	    /* Au boulot */
	    SXINT	xac1;
	    SXINT		lim, pop_fe, arc_no;
	    bool	is_xt, should_call_RLR_cg;
	    struct Q0	*aQ0;

	    for (xac1 = 1; xac1 < xac2; xac1++) {
		aQ0 = Q0 + xac1;
		is_error_code_needed = false;
		should_call_RLR_cg = false;
		sxba_empty (t1_set);

		if ((lim = aQ0->t_trans_nb) > 0) {
		    SXINT LALR1_StNt_5, t;

		    lim += (LALR1_StNt_5 = aQ0->bot);

		    do {
			t = -XxY_Y (Q0xV_hd, LALR1_StNt_5);
			SXBA_1_bit (t1_set, t);
			ter [t] = -LALR1_StNt_5; /* Shift */
		    } while (++LALR1_StNt_5 < lim);
		}

		if ((lim = aQ0->red_trans_nb) > 0) {
		    SXINT		LALR1_StNt_6, t, indice;
		    SXINT				red_no, dum;
		    SXINT		*cur_nbt;
		    struct chosen	*pchosen;
		    SXBA		la_set;
			
		    lim += (LALR1_StNt_6 = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
		    cur_red_no = 0;

		    do {
			*(cur_nbt = nbt + cur_red_no) = 0;
			red_no = XxY_Y (Q0xV_hd, LALR1_StNt_6 + cur_red_no) - bnf_ag.WS_TBL_SIZE.xntmax;
			la_set = look_ahead_sets [aQ0->xla + cur_red_no];

			if (has_xprod) {
			    red_no = PROD_TO_RED (red_no);
			    cur_red_no += equiv_rule_processing (LALR1_StNt_6 + cur_red_no,
							lim,
							aQ0->xla + cur_red_no,
							la_set = sxba_copy (t2_set, la_set));
			}

			sxba_or (t1_set, la_set);
			t = 0;
			
			while ((t = sxba_scan (la_set, t)) > 0) {
			    if ((indice = XxY_is_set (&Q0xT_to_conflict_action_hd, xac1, t)) == 0) {
				ter [t] = red_no;
				++*cur_nbt;
			    }
			    else {
				pchosen = &(Q0xT_to_chosen [indice]);

				if (pchosen->action_taken == ERROR_ACTION)
				    is_error_code_needed = true;
				else if (pchosen->action_taken == ND_ACTION) {
				    /* Non-deterministe */
				    SXINT	act;

				    if ((act = ter [t]) < 0) {
					/* Il y avait du Shift. */
					ter [t] = 0;
					nd_list (ND_LIST_PUT, t, &act);
				    }

				    nd_list (ND_LIST_PUT, t, &red_no);
				}
				else if (pchosen->action_taken == RLR_ACTION) {
				    /* RLR_code_generation */
				    /* On tient compte des resolutions par ARC de longueur 1
				       pour calculer la reduction la plus populaire. */
				    if (ARC_get_red_no (-pchosen->reduce_no, t) == red_no)
					++*cur_nbt;

				    ter [t] = dum = 0;
				    nd_list (ND_LIST_PUT, t, &dum);
				}
				else if (pchosen->action_taken == REDUCE &&
					 pchosen->reduce_no == red_no) {
				    ter [t] = red_no;
				    ++*cur_nbt;
				}
			    }
			}
		    } while (++cur_red_no < aQ0->red_trans_nb);
		}
	    
		/* Recherche de la reduction la plus populaire */
	    
		/* Du fait du traitement des conflits tous les reduce ont pu disparaitre, dans
	       ce cas on produit le code erreur */
	    
		/* S'il y a au moins un terminal etendu dans le coup, on ne fait rien; on laisse
	       l'optimiseur s'en occuper */
	    
		is_xt = (sxba_scan (t1_set, -bnf_ag.WS_TBL_SIZE.tmax) > 0);
	    
	         {
		    SXINT	n, LALR1_lim;
		    SXINT			red_no, cur_nbt;

		    n = plulong = 0;

		    if (!is_xt && !is_error_code_needed && (LALR1_lim = aQ0->red_trans_nb) > 0) {
			/* Il y a[vait] au moins un Reduce */
			LALR1_lim += (StNt = aQ0->bot + aQ0->t_trans_nb + aQ0->nt_trans_nb);
			cur_red_no = 0;

			do {
			    if ((cur_nbt = nbt [cur_red_no]) > n) {
				red_no = XxY_Y (Q0xV_hd, StNt) - bnf_ag.WS_TBL_SIZE.xntmax;
				red_no = PROD_TO_RED (red_no);

				if (is_tspe && (SXBA_bit_is_set (s_red_set, red_no)
						|| bnf_ag.WS_NBPRO [red_no].bprosimpl)) {
				    /* la plus populaire sera generee ulterieurement dans OPTIM */
				    plulong = 0;
				    break;
				}

				n = cur_nbt;
				plulong = red_no;
			    }

			    cur_red_no++;
			} while (++StNt < LALR1_lim);
		    }
		}

		if (plulong != 0) {
		    /* On genere la reduction la plus populaire */
		    /* plulong n'est pas une prod dont le LHS est un XNT. */
		    t_fe->codop = Reduce;
		    t_fe->pspl = LGPROD (plulong);
		    t_fe->next = bnf_ag.WS_NBPRO [plulong].reduc;
		    t_fe->reduce = plulong;
		    SXBA_0_bit (not_used_red_set, t_fe->reduce);
		    pop_fe = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
		}
		else
		    pop_fe = 0;


/* Generation du Floyd_Evans pour les t_tables */

/* On ne s'occupe pas [trop] des predicats c'est l'optimiseur qui produira le code correspondant */

		{
		    SXINT		xt, t;
		    SXINT				red_no;
		    struct P_item	*aitem;
		    struct state	*astate;

		    astate = t_states + xac1;
		    /* astate->lgth = 0; inutile (calloc) */
		    astate->start = xt_items;
		    sxba_empty (t_set);
		    sxba_empty (t2_set);
		    xt = 0;

		    while ((xt = sxba_scan (t1_set, xt)) > 0) {
			red_no = ter [xt];
			ter [xt] = 0;

			if (xt > -bnf_ag.WS_TBL_SIZE.tmax) {
			    xprdct_items++;
			    t = XT_TO_T_CODE (xt);
			    SXBA_1_bit (t_set, t);
			    
			    if (xt_state_set == NULL)
				xt_state_set = sxba_calloc (xe1 + 1);
			    
			    SXBA_1_bit (xt_state_set, xac1);
			}
			
			if (red_no != 0) {
			    if (red_no !=  plulong) {
				if (red_no < 0 /* Shift */ ) {
				    SXINT 	next;
				    
				    if ((next = Q0xV_to_Q0 [-red_no]) < 0) {
					if ((red_no = bnf_ag.WS_INDPRO [-next].prolis) == 0) {
					    /* Generation de l'instruction Halt */
					    t_fe->codop = Reduce;
					    t_fe->reduce = 0;
					    t_fe->pspl = 2;
					    t_fe->next = 1;
					}
					else {
					    t_fe->codop = ScanReduce;
					    t_fe->pspl = LGPROD (red_no) - 1;
					    t_fe->next = bnf_ag.WS_NBPRO [red_no].reduc;
					    t_fe->next = XNT_TO_NT_CODE (t_fe->next);
					    t_fe->reduce = PROD_TO_RED (red_no);
					    SXBA_0_bit (not_used_red_set, t_fe->reduce);
					}
				    }
				    else {
					t_fe->codop = ScanShift;
					t_fe->reduce = 0;
					t_fe->pspl = Q0 [next].nt_trans_nb == 0 ? 0 : next;
					t_fe->next = next;
				    }
				}
				else  /* (red_no > 0) => Reduce */ {
				    /* red_no est OK. */
				    t_fe->codop = Reduce;
				    t_fe->pspl = LGPROD (red_no);
				    t_fe->next = bnf_ag.WS_NBPRO [red_no].reduc;
				    t_fe->reduce = red_no;
				    SXBA_0_bit (not_used_red_set, red_no);
				}
				
				aitem = t_items + xt_items++;
				aitem->check = xt;
				astate->lgth++;
				aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
			    }
			}
			else {
			    nd_list (ND_LIST_GET, xt, &red_no);

			    if (red_no == 0) {
				should_call_RLR_cg = true;
				SXBA_1_bit (t2_set, xt);
			    }
			    else {
				/* Ici on est dans le cas non deterministe. */
				/* l'instruction Shift, si elle existe est generee en dernier. */
				if (nd_state_set == NULL)
				    nd_state_set = sxba_calloc (xe1 + 1);
				
				SXBA_1_bit (nd_state_set, xac1);
				xpredicates++;

				do {
				    if (red_no < 0 /* Shift */ ) {
					SXINT 	next;
					
					if ((next = Q0xV_to_Q0 [-red_no]) < 0) {
					    if ((red_no = bnf_ag.WS_INDPRO [-next].prolis) == 0) {
						/* Generation de l'instruction Halt */
						t_fe->codop = Reduce;
						t_fe->reduce = 0;
						t_fe->pspl = 2;
						t_fe->next = 1;
					    }
					    else {
						t_fe->codop = ScanReduce;
						t_fe->pspl = LGPROD (red_no) - 1;
						t_fe->next = bnf_ag.WS_NBPRO [red_no].reduc;
						t_fe->next = XNT_TO_NT_CODE (t_fe->next);
						t_fe->reduce = PROD_TO_RED (red_no);
						SXBA_0_bit (not_used_red_set, t_fe->reduce);
					    }
					}
					else {
					    t_fe->codop = ScanShift;
					    t_fe->reduce = 0;
					    t_fe->pspl = Q0 [next].nt_trans_nb == 0 ? 0 : next;
					    t_fe->next = next;
					}
				    }
				    else /* Reduce */ {
					/* red_no est OK. */
					t_fe->codop = Reduce;
					t_fe->pspl = LGPROD (red_no);
					t_fe->next = bnf_ag.WS_NBPRO [red_no].reduc;
					t_fe->reduce = red_no;
					SXBA_0_bit (not_used_red_set, red_no);
				    }
				    
				    xprdct_items++; /* mis avec les predicats. */
				    aitem = t_items + xt_items++;
				    aitem->check = xt;
				    astate->lgth++;
				    aitem->action = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
				    nd_list (ND_LIST_GET, xt, &red_no);
				} while (red_no != 0);
			    }
			}
		    }
		    
		    if (is_xt) {
			xpredicates += sxba_cardinal (t_set) /* nb de sequences */ ;
		    }
		    
		    
		    if (should_call_RLR_cg) {
			/* gestion de xt_items et astate->lgth est faite ds RLR_cg */
			/* On partitionne t2_set par les differents ARCs assurant sa resolution. */
			xt = 0;

			while ((xt = sxba_scan (t2_set, xt)) > 0) {
			    sxba_empty (t_set);
			    SXBA_1_bit (t_set, xt);
			    arc_no = -Q0xT_to_chosen [XxY_is_set (&Q0xT_to_conflict_action_hd, xac1, xt)].reduce_no;
			    t = xt;

			    while ((t = sxba_scan (t2_set, t)) > 0) {
				if (arc_no == -Q0xT_to_chosen [XxY_is_set (&Q0xT_to_conflict_action_hd, xac1, t)].reduce_no) {
				    SXBA_1_bit (t_set, t);
				    SXBA_0_bit (t2_set, t);
				}
			    }

			    RLR_code_generation (xac1, arc_no, t_set, plulong);
			}
		    }
		    else {
			/* maj pour le defaut */
			xt_items++;
			astate->lgth++;
		    }

/* Generation du defaut */

		    aitem = t_items + astate->start + astate->lgth - 1;
		    aitem->check = any;

		    if (is_error_code_needed) {
			if (force_error == 0) {
			    t_fe->codop = ForceError;
			    t_fe->reduce = t_fe->pspl = t_fe->next = 0;
			    force_error = put_in_fe (t_fe, t_hash, t_lnks, t_fe);
			}

			aitem->action = force_error;
		    }
		    else
			aitem->action = pop_fe == 0 ? t_error : pop_fe;

		    nd_list (ND_LIST_CLEAR, (SXINT)0, (SXINT*)NULL);
		}
	    }
	}

	nd_list (ND_LIST_FREE, (SXINT)0, (SXINT*)NULL);

	if (is_ARC_init_called)
	    xe1 = ARC_final ();

	xprdct_items += xpredicates /* Nombre de &Else */ ;

	if (sxverbosep)
	    sxtime (SXACTION, "done");

	if (sxverbosep || is_listing_opened) {
	    if (MAX_LA != 1 ) {
		char s [12];
		
		varstr_raz (vstr);
		
		if (MAX_LA != -1) {
		    sprintf (s, "%ld", (SXINT) MAX_LA);
		    varstr_catenate (vstr, s);
		}
		else
		    varstr_catenate (vstr, "An unbounded number of");
		
		varstr_catenate (vstr, " look-ahead tokens ");
		varstr_catenate (vstr, (MAX_LA == -1) ? "is" : "are");
		varstr_catenate (vstr, " needed in the parser.\n");
		
		if (sxverbosep)
		    fputs (varstr_tostr (vstr), sxtty);
		
		if (is_listing_opened) {
		    fputs (varstr_tostr (vstr), listing);
		    put_edit_nl (2);
		}
	    }

	    if (nd_state_set != NULL) {
		static char mess [] = "The generated push-down automaton is nondeterministic\n";

		if (sxverbosep)
		    fputs (mess, sxtty);
		if (is_listing_opened) {
		    fputs (mess, listing);
		    put_edit_nl (2);
		}
	    }
	}
	
	/* Recapitulatif des conflits */
	if (psr + dsr + rlrsr + prr + drr + rlrrr > 0) {
	    char	results [128], *str;

	    fputs (str = conflict_kind_to_string (vstr, grammar_kind, h_value, k_value_max), sxstderr);
	    sprintf (results, "   %-6ld | %-6ld |  %-6ld  | %-6ld | %-6ld |  %-6ld  | %-6ld\n",
		     (SXINT) (psr + dsr + rlrsr + prr + drr + rlrrr), (SXINT) psr, (SXINT) dsr, (SXINT) rlrsr, (SXINT) prr, (SXINT) drr, (SXINT) rlrrr);
	    fprintf (sxstderr,
		     header,
		     is_non_deterministic_automaton ? "NonDeter" : "Default ",
		     is_non_deterministic_automaton ? "NonDeter" : "Default ");

	    fputs (results, sxstderr);

	    if (is_listing_opened) {
		fputs (str, listing);
		fprintf (listing,
			 header,
			 is_non_deterministic_automaton ? "NonDeter" : "Default ",
			 is_non_deterministic_automaton ? "NonDeter" : "Default ");
		fputs (results, listing);
		put_edit_nl (2);
	    }
	}

	{
	    SXINT	i;

	    for (i = max_red_per_state - 1; i >= 0; i--)
		sxfree (Titems_set [i]);

	    sxfree (Titems_set);
	    sxfree (nt_trans_stack);
	    sxfree (nt_trans_set);
	}

	prio_free (&t_priorities, &r_priorities);
	XxY_free (&Q0xT_to_conflict_action_hd);
	sxfree (Q0xT_to_chosen);
	sxfree (nbt), nbt = NULL;
	sxfree (ter), ter = NULL;
    }

    sxbm_free (look_ahead_sets);
    sxfree (t_set), t_set = NULL;
    sxfree (t2_set), t2_set = NULL;
    sxfree (t1_set), t1_set = NULL;
    sxbm_free (LR0_sets);

    if (!is_tspe) {
	XxY_free (&Q0xV_hd);
	sxfree (Q0xV_to_Q0);
	sxfree (Q0xV_to_Q0xXNT);
	XxY_free (&Q0xQ0_hd);
	sxfree (Q0), Q0 = NULL;
	XH_free (&nucleus_hd);

	if (clone_to_lr0_state != NULL)
	    sxfree (clone_to_lr0_state);
    }

    {

/* On verifie que toutes les reductions sont generees.
   Le traitement des conflits ayant pu en supprimer. */

	SXINT	i;

	if ((i = sxba_scan (not_used_red_set, 0)) > 0) {
	    /* Il y a des reductions non generees */
	    static char		ws_local [] = "\nWarning :\tThe following grammar rules are not recognized:\n";

	    fputs (ws_local, sxstderr);

	    if (is_listing_opened) {
		put_edit_apnnl (ws_local);
	    }

	    do {
		prod_to_str (vstr, i);
		fputs (varstr_tostr (vstr), sxstderr);

		if (is_listing_opened)
		    put_edit_apnnl (varstr_tostr (vstr));
	    } while ((i = sxba_scan (not_used_red_set, i)) > 0);

	    sxttycol1p = true;
	}

	if (is_tspe)
	    sxfree (s_red_set);

	sxfree (not_used_red_set);
    }

    {
	/* On alloue et initialise certaines structures de OPTIM pour pouvoir sortir le
	   Floyd_Evans si long_listing a ete demande */

	SXINT	i;

	t_state_equiv_class = (SXINT*) sxalloc (xe1, sizeof (SXINT));
	nt_state_equiv_class = (SXINT*) sxalloc (xac2, sizeof (SXINT));
	nt_to_ad = (SXINT*) sxcalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
	nt_to_nt = (SXINT*) sxalloc (bnf_ag.WS_TBL_SIZE.xntmax + 1, sizeof (SXINT));
	NTMAX = bnf_ag.WS_TBL_SIZE.ntmax;

	for (i = 0; i < xe1; i++)
	    t_state_equiv_class [i] = (t_states [i].lgth == 0) ? 0 : i;

	for (i = 0; i < xac2; i++)
	    nt_state_equiv_class [i] = (nt_states [i].lgth == 0) ? 0 : i;

	for (i = 1; i <= bnf_ag.WS_TBL_SIZE.xntmax; i++) {
	    nt_to_nt [i] = i;
	}
    }

    if (is_long_listing && open_listing (1, language_name, ME, ".la.l")) {
	struct state	*astate;
	struct P_item	*aitem, *lim;
	SXINT	i;

	if (sxverbosep)
	    fprintf (sxtty, "\tNon Optimized Floyd-Evans Listing Output ... ");

	put_edit_ap ("************ T _ T A B L E S *************");
	put_edit_nl (2);
	put_edit_nnl (11, "Parsing starts in Etq ");
	put_edit_apnb (init_state);
	put_edit_nl (2);
	put_edit_nnl (11, "size = ");
	put_edit_apnb (t_lnks [0]);
	put_edit_nl (2);

	for (i = 1; i < xe1; i++) {
	    astate = t_states + i;

	    if (astate->lgth != 0 /* etat non vide */ ) {
		put_edit_nl (1);
		put_edit_nnl (1, "Etq ");
		put_edit_apnb (i);

		for (lim = (aitem = t_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
		    if (aitem->action != 0)
			output_fe (Terminal, aitem->action, aitem->check, t_fe + aitem->action);
		}
	    }
	}

	put_edit_ap ("************ N T _ T A B L E S *************");
	put_edit_nl (2);
	put_edit_nnl (11, "size = ");
	put_edit_apnb (nt_lnks [0]);
	put_edit_nl (2);

	for (i = 1; i < xac2; i++) {
	    astate = nt_states + i;

	    if (astate->lgth != 0 /* etat non vide */ ) {
		put_edit_nl (1);
		put_edit_nnl (1, "State ");
		put_edit_apnb (i);

		for (lim = (aitem = nt_items + astate->start) + astate->lgth; aitem < lim; aitem++) {
		    if (aitem->action != 0)
			output_fe (NonTerminal, aitem->action, aitem->check, nt_fe + aitem->action);
		}
	    }
	}

	if (sxverbosep)
	    sxtime (SXACTION, "done");
    }

    if (is_listing_opened) {
	put_edit_finalize ();
	is_listing_opened = false;
    }

    nd_degree = is_non_deterministic_automaton ? dsr + drr : -1;

    return (is_non_deterministic_automaton || (dsr + drr == 0)) ? 0 : 1;
}
