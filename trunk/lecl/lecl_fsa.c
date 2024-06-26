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

/*  F I N I T E _ S T A T E _ A U T O M A T O N   C O N S T R U C T I O N   */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"
#include "lecl_ag.h"

char WHAT_LECLFSA[] = "@(#)SYNTAX - $Id: lecl_fsa.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern SXINT	lecl_diagnoses_one_la (struct lr_marker *current_state_ptr, 
				       SXBA sra_set, 
				       SXINT detection_state, 
				       char conflict_kind /* bit (5) */, 
				       SXINT choice, 
				       SXBA esc_set);
extern SXINT	lecl_diagnoses_unbounded (struct lr_marker *current_state_ptr, 
					  SXBA sra_set, 
					  SXINT seed_state, 
					  SXINT detection_state, 
					  char conflict_kind /* bit (5) */, 
					  SXINT choice, 
					  struct conflict_record **u_head_ptr);
extern SXINT	lecl_diagnoses_mixte (struct lr_marker *current_state_ptr, 
				      SXBA sra_set, 
				      SXINT seed_state, 
				      SXINT detection_state, 
				      char conflict_kind /* bit (5) */, 
				      SXINT choice, 
				      struct conflict_record **m_head_ptr);
extern SXINT	lecl_diagnoses_immediate (struct lr_marker *current_state_ptr, 
					  SXBA sra_set, 
					  SXINT detection_state, 
					  char conflict_kind /* bit (5) */,
					  SXINT choice, 
					  bool with_detection_point);

extern SXINT	lecl_diagnoses_unbounded_output (struct conflict_record *u_head_ptr, struct conflict_record *m_head_ptr);
extern SXINT	lecl_sature (SXINT nbt);
/* extern SXINT	sort_by_tree (); 1. c'est un void et 2. il est maintenant dans sxunix.h */
extern VARSTR	lecl_gen_cc (SXBA *SC_TO_CHAR_SET, 
			     SXBA esc_set, 
			     VARSTR varstr_ptr);
static SXINT	ctxt_mngr__put (SXBA ctxt_mngr__put_ctxt_set);
struct sc_prdct_to_next_item {
    SXINT		state_no, lnk, *bwrd_ptr;
    bool	is_del, is_scan;
};
static struct conflict_record	*u_head_ptr, *m_head_ptr;
static SXINT	FSA_HSH_TBL [127];
static SXINT	*fsa_hsh_tbl;

/* -126:0 */

static struct lr_marker		*lr_marker_ptr, lrm, *current_state, *store
				/* 1:items_no */
									   ;
static SXINT	action_no, hsh, i, j, k, ner, next_state, seed_state, shift_state,
                xfsa1, xfsa_t2, xitem, xitem2, xstate, xsubstate, xcc, items_no, origine,
                sommet, predicate_no, indx, max_items_no, esc1, esc, x, xcurrent_state,
                conflict_mode, sc, sc1, else_state, erase_inconsistency;
static bool	bdel, scan, is_first_reduce, is_the_same_trans, is_first,
                process_predicates, has_predicate;
static char	current_state_kind, next_state_kind /* bit (6) */ ;
static SXINT	*to_be_sorted, *indexes, *sorted_actions, *new_ctxt /* 1:items_no */ ;
static SXBA	/* max_re */ ctxt_set, and_set;
static SXBA	/* smax */ sc1_set, has_prdct, sc_set, has_true;
static SXBA	/* items_no */ xcurrent_state_set, not_yet_processed, is_promoted, is_new_ctxt;
static SXBA	/* last_esc */ esc_set, esc1_set;
static SXBA	/* xprdct_to_ate */ prdct_set;
static SXBA	/* items_no */ *esc_to_xcurrent_state /* 1:last_esc */ ;
static SXBA	/* xcurrent_state */ *sra_s /* 1:xcurrent_state */ ;
static SXBA	/* xprdct_to_ate */ *sc_to_prdct /* 1:smax */ ;
static SXINT	**sc_prdct_to_next; /* 1:smax, 1:xprdct_to_ate */
static struct sc_prdct_to_next_item	*next_state_no_list;
static SXINT	next_state_no_list_top, next_state_no_list_size;
static char	prio /* bit (3) */ ;
static SXINT	dum_int;
static char	dum_char;

static VARSTR	sc_name;

#define NO_CONFLICT ((SXINT)0)
#define ONELA_CONFLICT ((SXINT)1)
#define UNBOUNDED_CONFLICT ((SXINT)2)

#define UNDEF_KIND ((char)0)
#define FIRST_REDUCE ((char)1)
#define SHIFT_KIND ((char)2)
#define ACTION_KIND ((char)3)
#define NEXT_REDUCE ((char)0)

#define PURE_SHIFT ((char)1)
#define PURE_ACTION ((char)2)
#define DISGUISED_SHIFT ((char)3)
#define DISGUISED_ACTION ((char)4)
#define REDUCE_CONFLICT ((char)5)
#define UNKNOWN ((char)6)

static SXINT	T [6] [6] = { /* bidon */ {0, 0, 0, 0, 0, 0},
			     { /* PURE_SHIFT */ 0, 0, 0, 0, 1, 1},
			     { /* PURE_ACTION */ 0, 0, 0, 1, 2, 1},
			     { /* DISGUISED_SHIFT */ 0, 0, 1, 0, 1, 1},
			     { /* DISGUISED_ACTION */ 0, 1, 2, 1, 2, 1},
			     { /* REDUCE */ 0, 1, 1, 1, 1, 3}};


static void	maj (SXINT *maj_x, SXINT n)
{
    (*maj_x)++;

    while (*maj_x > lgt [n]) {
	lecl_sature (n);
    }
}



static void	put_item_in_substate (SXINT put_item_in_substate_index, 
				      SXINT put_item_in_substate_origine, 
				      SXINT context_no, 
				      char kind /* bit (2) */)
{
    struct lr_marker	*p;

    maj (&xsubstate, (SXINT)1);
    p = substate + xsubstate;
    p->index = put_item_in_substate_index;
    p->attributes.origine_index = put_item_in_substate_origine;
    p->attributes.ctxt_no = context_no;
    p->attributes.kind = kind;
}



static void	put (struct lr_marker *put_lrm)
{
    maj (&xstate, (SXINT)1);
    state [xstate] = *put_lrm;
}



static SXINT	get_red_no (SXINT get_red_no_x)
{
    switch (ers [get_red_no_x].lispro) {
    case -1:
    case -2:
	return ers [get_red_no_x - 1].prolis;

    default:
	return ers [get_red_no_x].prolis;
    }
}



static void	filsubstate (struct lr_marker *filsubstate_lrm)
{
    /* Le marqueur LR est en fin d'ER; cette procedure remplit substate avec les
       debutants des suivants valides de cette ER. */
    SXBA	/* max_re */ filsubstate_ctxt_set;
    SXINT	filsubstate_i, filsubstate_j;

    filsubstate_ctxt_set = ctxt [filsubstate_lrm->attributes.ctxt_no];
    filsubstate_i = 0;

    while ((filsubstate_i = sxba_scan (filsubstate_ctxt_set, filsubstate_i)) > 0) {
	filsubstate_j = ers_disp [filsubstate_i].master_token;

	if (filsubstate_j == comments_re_no) {
	    filsubstate_j = ers_disp [ers [filsubstate_lrm->index - 1].prolis].master_token;

	    if (filsubstate_j == comments_re_no)
		put_item_in_substate (ers_disp [filsubstate_i].prolon,
				      filsubstate_lrm->attributes.origine_index,
				      filsubstate_lrm->attributes.ctxt_no /* transmission du contexte */,
				      filsubstate_lrm->attributes.kind);
	    else {
		sxba_and (sxba_copy (and_set, ctxt [filsubstate_i]), ctxt [max_re + filsubstate_lrm->attributes.ctxt_no]);
		/* On utilise le contexte de la grammaire syntaxique */
		put_item_in_substate (ers_disp [filsubstate_i].prolon,
				      filsubstate_lrm->attributes.origine_index,
				      ctxt_mngr__put (and_set),
				      filsubstate_lrm->attributes.kind);
	    }
	}
	else
	    put_item_in_substate (ers_disp [filsubstate_i].prolon,
				  filsubstate_lrm->attributes.origine_index,
				  filsubstate_i,
				  filsubstate_lrm->attributes.kind);
    }
}



static void	subsuiv (bool is_kept, bool is_next, bool subsuiv_is_first_reduce)
{
    SXINT	l, t, subsuiv_origine, xs1, subsuiv_ner;
    struct lr_marker	subsuiv_lrm;
    SXBA	/* max_re_lgth */ subsuiv_follow;

    for (xs1 = 1; xs1 <= xsubstate; xs1++) {
	subsuiv_lrm = substate [xs1];
	subsuiv_origine = ((subsuiv_ner = ers [subsuiv_lrm.index].prolis) < 0 /* predicat */ ) ? prdct_to_ers [-subsuiv_ner] : ers_disp [subsuiv_ner].prolon;
	subsuiv_follow = suivant [subsuiv_lrm.index];
	t = 0;

	while ((t = sxba_scan (subsuiv_follow, t)) > 0) {
	    subsuiv_lrm.index = subsuiv_origine + t;
	    l = ers [subsuiv_lrm.index].lispro;

	    if (l > 0 || (l <= xactmin && is_kept) || ((l == -1 || l == -2) && !is_next))
		put (&subsuiv_lrm);
	    else if (l <= xactmin /* & ^is_kept */ ) {
		maj (&xsubstate, (SXINT)1);
		substate [xsubstate] = subsuiv_lrm;
	    }
	    else {
	      if (subsuiv_lrm.attributes.kind != FIRST_REDUCE) {
		    if (subsuiv_is_first_reduce
			/* Il y a deja des FIRST_REDUCE, on est en look_ahead */
				       )
			subsuiv_lrm.attributes.kind = NEXT_REDUCE;
		    else
			subsuiv_lrm.attributes.kind = FIRST_REDUCE;
	      }
		filsubstate (&subsuiv_lrm);
	    }
	} /* subsuiv_follow */
    }
    /* substate */
}



static bool	eq_lr_marker (struct lr_marker *si, struct lr_marker *sj)
{
    return si->index == sj->index && si->attributes.origine_index == sj->attributes.origine_index && si->attributes.
	 ctxt_no == sj->attributes.ctxt_no && si->attributes.kind == sj->attributes.kind;
}



static bool	is_state_already_defined (SXINT is_state_already_defined_seed_state, 
					  SXINT else_prdct, 
					  bool is_del, 
					  bool is_scan, 
					  SXINT *xfsa, 
					  SXINT is_state_already_defined_hsh)
{
    /* cette fonction regarde si dans l'automate l'etat dont les items sont
   compris entre xitem et xitem2-1 dans item est deja defini
   de plus ils doivent avoir meme seed_state, else_prdct et del afin 
   d'assurer que les etats en look_ahead ne sont reutilises que s'ils sont 
   entierement compatibles
   - oui 	xitem2=xitem
   xfsa=no etat deja defini

   - non	xfsa=xfsa2
*/
    SXINT	is_state_already_defined_i, m, is_state_already_defined_x, y, taille;
    struct lr_marker	*si, *sj;

    taille = xitem2 - xitem;
    is_state_already_defined_x = is_state_already_defined_hsh;

    for (y = fsa_hsh_tbl [is_state_already_defined_x]; y != 0; y = fsa [is_state_already_defined_x].lnk) {
	m = fsa [y].item_ref;

	if (fsa [y + 1].item_ref - m == taille) {
	    if (fsa [y].else_prdct != else_prdct
		|| fsa [y].seed_state != is_state_already_defined_seed_state
		|| fsa [y].del != is_del
		|| fsa [y].scan != is_scan)
		goto different;

	    for (si = item + m, sj = item + xitem, is_state_already_defined_i = 0; is_state_already_defined_i < taille; si++, sj++, is_state_already_defined_i++) {
		if (!eq_lr_marker (si, sj))
		    goto different;
	    }

	    xitem2 = xitem;
	    *xfsa = y;
	    return true;
	}

different:
	is_state_already_defined_x = y;
    }

    *xfsa = xfsa2;

    if (is_state_already_defined_x <= 0)
	fsa_hsh_tbl [is_state_already_defined_x] = xfsa2;
    else
	fsa [is_state_already_defined_x].lnk = xfsa2;

    fsa [xfsa2].lnk = 0;
    return false;
}



static bool	less (SXINT less_i, SXINT less_j)
{
    struct lr_marker	*si, *sj;

    si = state + less_i;
    sj = state + less_j;

    if (si->index < sj->index)
	return true;

    if (si->index > sj->index)
	return false;

    if (si->attributes.origine_index < sj->attributes.origine_index)
	return true;

    if (si->attributes.origine_index > sj->attributes.origine_index)
	return false;

    if (si->attributes.ctxt_no < sj->attributes.ctxt_no)
	return true;

    if (si->attributes.ctxt_no > sj->attributes.ctxt_no)
	return false;

    if (si->attributes.kind < sj->attributes.kind)
	return true;

    return false;
}



static	void build_state (SXINT *xt, 
			  SXINT *build_state_hsh, 
			  char *state_kind, 
			  SXINT *build_state_ner, 
			  bool is_la)
{
    /* Cette procedure prend les info ds state (1..xstate), les trie, supprime les
       doublons eventuels, calcule la valeur du hash_code et range le resultat
       dans le tableau item a partir de l'index xt. Elle calcule
       egalement le "state_kind". */
    SXINT		build_state_i, xl, class, bsup;
    SXINT				build_state_x, mner;
    struct lr_marker	*item1_ptr, *item2_ptr;

    sxinitialise(item2_ptr); /* pour faire taire gcc -Wuninitialized */
    for (build_state_i = 1; build_state_i <= xstate; build_state_i++) {
	to_be_sorted [build_state_i] = build_state_i;
    }

    if (xstate > 1)
	sort_by_tree (to_be_sorted, 1, xstate, less);

    *build_state_hsh = bsup = *build_state_ner = 0;

    *state_kind = (is_la) ? LA : 0;

    for (build_state_i = 1; build_state_i <= xstate; build_state_i++) {
	item1_ptr = state + to_be_sorted [build_state_i];

	if (build_state_i == 1 || !eq_lr_marker (item2_ptr, item1_ptr)) {
	    item [build_state_x = *xt] = *item1_ptr;
	    maj (xt, (SXINT)3);
	    *build_state_hsh += (xl = (item2_ptr = item + build_state_x)->index);

	    if (is_la)
		xl = item2_ptr->attributes.origine_index;

	    indexes [++bsup] = xl;
	    class = ers [xl].lispro;

	    if (class >= 0)
		*state_kind |= SHIFT;
	    else if (class <= xactmin)
		/* action */
		*state_kind |= ACT;
	    else
		/* reduce */
		*state_kind |= REDUCE;
	}
    }

    *build_state_hsh = -(*build_state_hsh % 127);
    build_state_i = 0;

    if (*state_kind & SHIFT)
	build_state_i++;

    if (*state_kind & ACT)
	build_state_i++;

    if (*state_kind & REDUCE)
	build_state_i++;

    if (build_state_i > 1)
	*state_kind |= MIXTE;

    if (!(*state_kind & (MIXTE + SHIFT))) {
        sxinitialise (mner);

	for (build_state_i = 1; build_state_i <= bsup; build_state_i++) {
	    xl = indexes [build_state_i];
	    class = ers [xl].lispro;

	    if (build_state_i == 1) {
		if (class <= xactmin)
		    *build_state_ner = class;
		else {
		    /* modif le 29/3/02 */
		    /* mner = ers_disp [*build_state_ner = ers [xl - 1].prolis].master_token; */
		    *build_state_ner = ers [xl - 1].prolis;

		    if (*build_state_ner >= 0)
			mner = ers_disp [*build_state_ner].master_token;
		    else
			/* predicat */
			mner = *build_state_ner;
		}
	    }
	    else if (class <= xactmin) {
		if (class != *build_state_ner) {
		    *build_state_ner = 0;
		    *state_kind |= MIXTE;
		    break;
		}
	    }
	    else {
		build_state_x = ers [xl - 1].prolis;

		if (build_state_x >= 0) {
		    if (ers_disp [build_state_x].master_token != mner) {
			*build_state_ner = 0;
			*state_kind |= MIXTE;
			break;
		    }
		    else if (ers [ers_disp [build_state_x].prolon].lispro == -2)
			*build_state_ner = build_state_x;
		}
		else {
		    *build_state_ner = 0;
		    *state_kind |= MIXTE;
		    break;
		}
	    }
	} /* end for(build_state_i ... ) */
    } /* end if(!(*state_kind & (MIXTE + SHIFT))) */
}



static	void put_in_substate (SXBA put_in_substate_not_yet_processed, SXINT d)
{
    maj (&xsubstate, (SXINT)1);
    substate [xsubstate] = item [xitem];
    SXBA_0_bit (put_in_substate_not_yet_processed, d);
}



static	void put_in_state (SXBA put_in_state_not_yet_processed, SXINT d)
{
    put (item + xitem);
    SXBA_0_bit (put_in_state_not_yet_processed, d);
}



static	void put_new_trans (SXINT ref, SXINT state_no, bool del, bool put_new_trans_scan)
{
    struct fsa_trans_item	*fsa_trans_item_ptr;

    fsa_trans_item_ptr = fsa_trans + xfsa_t;
    fsa_trans_item_ptr->cc_ref = ref;
    fsa_trans_item_ptr->next_state_no = state_no;
    fsa_trans_item_ptr->is_del = del;
    fsa_trans_item_ptr->is_scan = put_new_trans_scan;
    maj (&xfsa_t, (SXINT)4);
}



static void	set_new_state (SXINT seed_state_no, 
			       SXINT shift_state_no, 
			       SXINT else_prdct_no, 
			       bool set_new_state_bdel, 
			       bool set_new_state_scan, 
			       char state_kind)
{
    struct fsa_item	*fsa_item_ptr;

    fsa_item_ptr = fsa + xfsa2;
    fsa_item_ptr->seed_state = seed_state_no;
    fsa_item_ptr->shift_state = shift_state_no;
    fsa_item_ptr->else_prdct = else_prdct_no;
    fsa_item_ptr->del = set_new_state_bdel;
    fsa_item_ptr->scan = set_new_state_scan;
    fsa_item_ptr->state_kind = state_kind;
    maj (&xfsa2, (SXINT)2);
    fsa [xfsa2].item_ref = xitem2;
}



static	void predicates_processing (SXINT predicates_processing_origine, 
				    SXINT predicates_processing_items_no, 
				    bool is_in_la)
{
    bool	predicates_processing_is_first, is_prdct_true_used, is_first_trans, is_the_same, is_first_pass, predicates_processing_bdel, else_del, predicates_processing_scan, else_scan
	 , is_trans_to_prdct, is_conflict;
    SXINT	d, predicates_processing_x, prd, prdct;
    SXINT		predicates_processing_predicate_no, predicates_processing_k, predicates_processing_hsh, predicates_processing_next_state, predicates_processing_ner, predicates_processing_else_state;
    char	predicates_processing_next_state_kind;


/* On regarde d'abord si un des etats suivant est egalement un etat prdct */

    sxinitialise (prdct); /* pour faire taire gcc -Wuninitialized */
    sxinitialise(predicates_processing_predicate_no);
    sxinitialise(predicates_processing_next_state);
    sxba_empty (not_yet_processed);

    for (d = 1; d <= predicates_processing_items_no; d++)
	SXBA_1_bit (not_yet_processed, d);

    is_trans_to_prdct = false;

    while (!sxba_is_empty (not_yet_processed) && !is_trans_to_prdct) {
	xstate = xsubstate = 0;
	predicates_processing_is_first = true;
	d = 0;

	while ((d = sxba_scan (not_yet_processed, d)) > 0) {
	    xitem = predicates_processing_origine + d - 1;
	    predicates_processing_k = item [xitem].index;
	    prd = ers [predicates_processing_k].lispro;

	    if (prd > 0) {
		if (predicates_processing_is_first) {
		    predicates_processing_is_first = false;
		    prdct = prd;
		    put_in_substate (not_yet_processed, d);
		}
		else if (prd == prdct)
		    put_in_substate (not_yet_processed, d);
	    }
	    else
		SXBA_0_bit (not_yet_processed, d);
	}


/* d ^= 0 */

	subsuiv (true, false, false);

	for (predicates_processing_x = 1; predicates_processing_x <= xstate && !is_trans_to_prdct; predicates_processing_x++) {
	    if (ers [state [predicates_processing_x].index].lispro > 0)
		is_trans_to_prdct = true;
	}
    }

    for (d = 1; d <= predicates_processing_items_no; d++)
	SXBA_1_bit (not_yet_processed, d);

    sxinitialise (predicates_processing_scan); /* pour faire taire gcc -Wuninitialized */
    sxinitialise (predicates_processing_bdel); /* pour faire taire gcc -Wuninitialized */
    is_prdct_true_used = false;
    is_first_trans = true;
    is_first_pass = true;
    is_the_same = true;

    while (!sxba_is_empty (not_yet_processed)) {
	xstate = xsubstate = 0;
	predicates_processing_is_first = true;
	d = 0;

	while ((d = sxba_scan (not_yet_processed, d)) > 0) {
	    xitem = predicates_processing_origine + d - 1;
	    predicates_processing_k = item [xitem].index;
	    prd = ers [predicates_processing_k].lispro;

	    if (is_first_pass /* premier passage on traite &True en premier */
			     ) {
		if (is_trans_to_prdct) {
		    /* on collecte tout ce qui n'est pas prdct */
		    if (prd != prdct)
			put_in_state (not_yet_processed, d);
		}
		else
		     /* on collecte tout ce qui n'est pas un predicat (reduce) */
		     if (prd < 0)
			 put_in_state (not_yet_processed, d);
	    }
	    else {
		if (is_trans_to_prdct) {
		    if (prd == prdct)
			put_in_substate (not_yet_processed, d);
		}
		else if (predicates_processing_is_first) {
		    predicates_processing_is_first = false;

		    if (prd > 0) {
			predicates_processing_predicate_no = prd;
			put_in_substate (not_yet_processed, d);
		    }
		    else {
			predicates_processing_predicate_no = prdct_true_code;
			put_in_state (not_yet_processed, d);
		    }
		}
		else if (prd > 0) {
		    if (prd == predicates_processing_predicate_no)
			put_in_substate (not_yet_processed, d);
		}
		else if (predicates_processing_predicate_no == prdct_true_code)
		    put_in_state (not_yet_processed, d);
	    }
	}


/* d ^= 0 */

	predicates_processing_else_state = 0;
	else_del = false;
	else_scan = false;

	if (is_first_pass) {
	    predicates_processing_predicate_no = prdct_true_code;

	    if (is_trans_to_prdct) {
		predicates_processing_else_state = fsa [xfsa1].else_prdct;
		else_del = fsa [xfsa1].del;
		else_scan = fsa [xfsa1].scan;
	    }
	}
	else if (is_trans_to_prdct) {
	    predicates_processing_predicate_no = prdct;
	    predicates_processing_else_state = predicates_processing_next_state;
	    /* positionne a la passe precedente */
	    else_del = predicates_processing_bdel;
	    else_scan = predicates_processing_scan;
	}

	if (predicates_processing_predicate_no != prdct_true_code)
	    subsuiv (true, false, false);

	if (xstate == 0 /* vide & &True */ ) {
	    predicates_processing_next_state = fsa [xfsa1].else_prdct;
	    predicates_processing_bdel = fsa [xfsa1].del;
	    predicates_processing_scan = fsa [xfsa1].scan;
	}
	else {
	    xitem = xitem2;
	    build_state (&xitem2, &predicates_processing_hsh, &predicates_processing_next_state_kind, &predicates_processing_ner, false);

	    if (!(predicates_processing_next_state_kind & SHIFT)) {
		/* On a forcement predicates_processing_next_state_kind & REDUCE */
		predicates_processing_next_state = item [xitem].attributes.origine_index;
		predicates_processing_bdel = item [xitem].attributes.kind == 2;
		predicates_processing_scan = item [xitem].attributes.ctxt_no == 1;

		if (predicates_processing_next_state_kind & MIXTE) {
		    VARSTR	names;


/* on a des predicats differents non disjoints, il y a conflit si leur 
   next_state sont differents */

		    is_conflict = false;
		    names = varstr_alloc (128);

		    for (predicates_processing_x = xitem + 1; predicates_processing_x < xitem2; predicates_processing_x++) {
			if (predicates_processing_next_state != item [predicates_processing_x].attributes.origine_index) {
			    is_conflict = true;
			    varstr_catenate (varstr_catenate (names, sxstrget (action_or_prdct_to_ate [-ers [item [predicates_processing_x].
				 index - 1].prolis])), " ");
			}
		    }

		    if (is_conflict) {
			sxtmsg (sxsrcmngr.source_coord.file_name, "%sThe following predicates are not disjoint :\n\r%s%s",
			     err_titles [1]+1 /* warning */ , varstr_tostr (names), 
			     sxstrget (action_or_prdct_to_ate [- ers [item [xitem].index - 1].prolis]));
		    }

		    varstr_free (names);
		}

		xitem2 = xitem; /* l'etat est supprime */
	    }
	    else {
		if (predicates_processing_next_state_kind & SHIFT)
		    predicates_processing_next_state_kind -= SHIFT;

		predicates_processing_next_state_kind |= is_in_la ? PRDCT + LA : PRDCT;

		if (!is_state_already_defined (fsa [xfsa1].seed_state, predicates_processing_else_state, else_del, else_scan, &predicates_processing_next_state, predicates_processing_hsh)
		     )
		    set_new_state (fsa [xfsa1].seed_state, fsa [xfsa1].shift_state, predicates_processing_else_state, else_del, else_scan,
			 predicates_processing_next_state_kind);
	    }
	}

	if (predicates_processing_next_state) {
	    /* On ne genere pas les transitions vers erreur */
	    if (is_first_trans)
		is_first_trans = false;
	    else {
		if (fsa_trans [xfsa_t - 1].next_state_no != predicates_processing_next_state)
		    is_the_same = false;
	    }

	    if (predicates_processing_predicate_no == prdct_true_code)
		is_prdct_true_used = true;

	    put_new_trans (predicates_processing_predicate_no, predicates_processing_next_state, predicates_processing_bdel, predicates_processing_scan);
	}

	is_first_pass = false;
    }


/* not_yet_processed ^= "0"b */




/* Si toutes les transitions sont identiques, on compresse cet etat, il sera
   saute a la generation de code */

    if (is_prdct_true_used && is_the_same) {
	xfsa_t = fsa [xfsa1].transition_ref;
	put_new_trans (prdct_true_code, predicates_processing_next_state, predicates_processing_bdel, predicates_processing_scan);
    }

    fsa [xfsa1 + 1].transition_ref = xfsa_t;
}



static SXINT	ctxt_mngr__put (SXBA ctxt_mngr__put_ctxt_set)
{
    SXINT	ctxt_mngr__put_i, ctxt_mngr__put_old_size;
    SXBA	*old_ptr;

    for (ctxt_mngr__put_i = 1; ctxt_mngr__put_i <= x_ctxt; ctxt_mngr__put_i++) {
	if (sxba_first_difference (ctxt [ctxt_mngr__put_i], ctxt_mngr__put_ctxt_set) == -1)
	    return ctxt_mngr__put_i;
    }

    if (++x_ctxt > ctxt_size) {
	/* sature inline */
	ctxt_mngr__put_old_size = ctxt_size;
	ctxt_size <<= 1;
	old_ptr = ctxt;
	ctxt = sxbm_calloc (ctxt_size + 1, max_re + 1);

	for (ctxt_mngr__put_i = 1; ctxt_mngr__put_i <= ctxt_mngr__put_old_size; ctxt_mngr__put_i++)
	    sxba_copy (ctxt [ctxt_mngr__put_i], old_ptr [ctxt_mngr__put_i]);

	sxbm_free (old_ptr);
    }

    sxba_copy (ctxt [x_ctxt], ctxt_mngr__put_ctxt_set);
    return x_ctxt;
}



static bool	less_action (SXINT less_action_i, SXINT less_action_j)
{
    return ers [state [less_action_i].index].lispro < ers [state [less_action_j].index].lispro;
}

static void clear_sc_prdct_to_next (void)
{
    while (next_state_no_list_top > 0) {
	*next_state_no_list [next_state_no_list_top].bwrd_ptr = 0;
	next_state_no_list_top--;
    }
}

static void set_sc_prdct_to_next (SXINT set_sc_prdct_to_next_k, SXINT set_sc_prdct_to_next_x)
{
    SXINT					set_sc_prdct_to_next_i;
    struct sc_prdct_to_next_item	*p;
    SXINT					*bwrd_ptr = &(sc_prdct_to_next [set_sc_prdct_to_next_k] [set_sc_prdct_to_next_x]);

    for (set_sc_prdct_to_next_i = *bwrd_ptr; set_sc_prdct_to_next_i > 0; set_sc_prdct_to_next_i = p->lnk) {
	p = &(next_state_no_list [set_sc_prdct_to_next_i]);

	if (p->state_no == next_state
	    && p->is_del == bdel
	    && p->is_scan == scan)
	    return;
    }

    if (++next_state_no_list_top > next_state_no_list_size) {
	next_state_no_list = (struct sc_prdct_to_next_item *)
	    sxrealloc (next_state_no_list,
		       (next_state_no_list_size *= 2) + 1,
		       sizeof (struct sc_prdct_to_next_item));
    }

    p = &(next_state_no_list [next_state_no_list_top]);
    p->lnk = *bwrd_ptr;
    p->state_no = next_state;
    p->is_scan = scan;
    p->is_del = bdel;
    p->bwrd_ptr = bwrd_ptr;
    sc_prdct_to_next [set_sc_prdct_to_next_k] [set_sc_prdct_to_next_x] = next_state_no_list_top;
}


static	void build_action_states (SXINT build_action_states_items_no)
{
    SXINT				build_action_states_x, y, build_action_states_action_no, build_action_states_hsh;
    struct lr_marker	*build_action_states_lrm;

    for (build_action_states_x = 1; build_action_states_x <= build_action_states_items_no; build_action_states_x++) {
	sorted_actions [build_action_states_x] = build_action_states_x;
	store [build_action_states_x] = state [build_action_states_x];
    }

    if (build_action_states_items_no > 1)
	sort_by_tree (sorted_actions, 1, build_action_states_items_no, less_action);

    build_action_states_lrm = store + sorted_actions [1];
    bdel = scan = false;

    for (build_action_states_x = 1; build_action_states_x <= build_action_states_items_no; build_action_states_x = y) {
	xstate = 0;
	build_action_states_action_no = ers [build_action_states_lrm->index].lispro;
	put (build_action_states_lrm);

	for (y = build_action_states_x + 1; y <= build_action_states_items_no; y++) {
	    build_action_states_lrm = store + sorted_actions [y];

	    if (ers [build_action_states_lrm->index].lispro == build_action_states_action_no)
		put (build_action_states_lrm);
	    else
		break;
	}

	xitem = xitem2;
	build_state (&xitem2, &build_action_states_hsh, &next_state_kind, &dum_int /* unused */ , false);

	if (!is_state_already_defined ((SXINT)0, (SXINT)0, false, false, &next_state, build_action_states_hsh))
	    set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, false, false, next_state_kind);

	if (is_non_deterministic_automaton)
	    set_sc_prdct_to_next ((SXINT)1, prdct_true_code) /* Any */;
	else
	    put_new_trans (build_action_states_action_no, next_state, false, false);
    }
}



static SXINT	get_action_state (SXINT state_no, SXINT act_no)
{
    SXINT	get_action_state_i;
    SXINT	lim = fsa [state_no + 1].transition_ref;

    for (get_action_state_i = fsa [state_no].transition_ref; get_action_state_i < lim; get_action_state_i++) {
	if (fsa_trans [get_action_state_i].cc_ref == act_no)
	    return fsa_trans [get_action_state_i].next_state_no;
    }

    return 0;
    /* garde_fou */
}



static bool	get_del (SXBA get_del_sc_set)
{
    /* cette fonction verifie si la suppression des caracteres des items de 
   substate est consistante pour les terminaux de get_del_sc_set.
   On suppose que les items de substate sont coherents vis-a-vis de &True et &False
   c'est-a-dire qu'aucun n'est associe a &False et que si l'un est associe a &True,
   ils y sont tous.
   Si une E.R. comporte le predicat &True la suppression eventuelle des
   caracteres de cette E.R. non suivis de &true n'est pas prise en compte
   ex:  ANY | -quote &True
   est correct,le quote de ANY n'etant pas pris en compte.
   De plus elle retourne un booleen indiquant si les caracteres sur
   lesquels s'effectuera la transition doivent etre gardes ou non pendant
   l'analyse lexico. */
    struct ers_item	*ers_item_ptr;
    SXINT	get_del_i, get_del_x;
    bool	all_0 = true, all_1 = true;
    SXINT	n;
    bool is_sysRE = false;
    char er_name [66], suffixe [40];

    for (get_del_i = 1; get_del_i <= xsubstate; get_del_i++) {
	if ((ers_item_ptr = ers + substate [get_del_i].index)->is_erased) {
	    if (ers_item_ptr->liserindx.line > 0) {
		/* Occurrence dans une ER utilisateur */
		all_0 = false;
	    }
	    else {
		/* Il s'agit d'une ER systeme */
		/* Vu la "linearite" des ER systeme, une classe (caractere) ne peut intervenir
		   qu'une fois et une seule dans un etat, le defaut "is_erased == true"
		   est donc sans influence, ni sur le passe ni sur le futur, il s'adapte
		   a son environnement */
		is_sysRE = true;
	    }
	}
	else
	    all_1 = false;
    }

    if (all_1)
	return true;

    if (is_sysRE)
	for (get_del_i = 1; get_del_i <= xsubstate; get_del_i++) {
	    if ((ers_item_ptr = ers + substate [get_del_i].index)->liserindx.line == 0) {
		/* Pour le listing de la table des symboles */
		ers_item_ptr->is_erased = false;
	    }
	}

    if (all_0)
	return false;


    /* Il y a incohérence vis-a-vis des "delete", on sort les messages d'erreur */

    ++erase_inconsistency;
    lecl_gen_cc (sc_to_char_set, get_del_sc_set, varstr_raz (sc_name));
    n = sxba_cardinal (get_del_sc_set);

    for (get_del_i = 1; get_del_i <= xsubstate; get_del_i++) {
	ers_item_ptr = ers + (get_del_x = substate [get_del_i].index);

	if (ers_item_ptr->liserindx.line > 0) {
	    SXINT		get_del_ner, mner, tok, mtok;

	    get_del_ner = get_red_no (get_del_x);
	    mner = ers_disp [get_del_ner].master_token;

	    if ((tok = ers_disp [get_del_ner].reduc) < 0) {
		tok = -tok;
	    }

	    if ((mtok = ers_disp [mner].reduc) < 0) {
		mtok = -mtok;
	    }

	    sprintf (er_name, "%s%s%s", (mner == get_del_ner) ? "" : sxstrget (mtok), (mner == get_del_ner) ? "" : ".", sxstrget (tok));

	    if (ers_item_ptr->is_erased)
		sprintf (suffixe,"; %s %s", n>1 ? "they" : "it", "will be kept during scanning");
	    else
		suffixe [0] = SXNUL;

	    sxerror (ers_item_ptr->liserindx,
		     err_titles [1][0],
		     "%sErase inconsistency #%ld upon %s in this class which occurs in %s%s.", 
		     err_titles [1]+1,
		     (SXINT) erase_inconsistency, 
		     sc_name->first, 
		     er_name, 
		     suffixe);
	}
    }

    return false;
}



static bool	is_first_reduction (SXINT state_no)
{
    SXINT	is_first_reduction_x;
    SXINT	lim = fsa [state_no + 1].item_ref;

    for (is_first_reduction_x = fsa [state_no].item_ref; is_first_reduction_x < lim; is_first_reduction_x++) {
	if (item [is_first_reduction_x].attributes.kind == FIRST_REDUCE)
	    return true;
    }

    return false;
}



static void	maj_ctxt (SXINT maj_ctxt_x, SXBA set, SXBA sra_set)
{
    if (!sxba_is_empty (set)) {
	SXBA_1_bit (sra_set, maj_ctxt_x);
	SXBA_1_bit (is_new_ctxt, maj_ctxt_x);
	new_ctxt [maj_ctxt_x] = ctxt_mngr__put (set);
    }
}



static SXINT	get_type (SXINT get_type_x, char kind /* bit (2) */)
{
    SXINT	class;

    if (kind == SHIFT_KIND)
	return PURE_SHIFT;

    if (kind == ACTION_KIND)
	return PURE_ACTION;

    if ((class = ers [get_type_x].lispro) >= 0)
	return DISGUISED_SHIFT;

    if (class <= xactmin)
	return DISGUISED_ACTION;

    return REDUCE_CONFLICT;
}

static void set_conflict_mode_prio (SXBA sra_set, 
				    SXINT *set_conflict_mode_prio_conflict_mode, 
				    char *user_priority /* bit (3) */)
{
    SXINT		x1;
    char	item_kind /* bit (2) */ ;

    *set_conflict_mode_prio_conflict_mode = NO_CONFLICT;
    *user_priority = 0;

    if ((x1 = sxba_scan (sra_set, 0)) > 0) {
	do {
	    item_kind = current_state [x1].attributes.kind;
	    
	    if (item_kind == FIRST_REDUCE) {
		ner = get_red_no (current_state [x1].attributes.origine_index);
		*user_priority |= ers_disp [ner].priority_kind;
		*set_conflict_mode_prio_conflict_mode |= ers_disp [ner].is_unbounded_context
		    ? UNBOUNDED_CONFLICT
			: ONELA_CONFLICT;
	    }
	} while ((x1 = sxba_scan (sra_set, x1)) > 0) ;

	if (*set_conflict_mode_prio_conflict_mode & ONELA_CONFLICT)
	    *set_conflict_mode_prio_conflict_mode = ONELA_CONFLICT;
    }
}
    
static bool	is_conflict (SXBA sra_set, 
			     SXINT *is_conflict_conflict_mode, 
			     char *user_priority /* bit (3) */)
{
    struct lr_marker	*lrm1_ptr, *lrm2_ptr;
    SXINT			x1, typ1, typ2;

    set_conflict_mode_prio (sra_set, is_conflict_conflict_mode, user_priority);

    if ((x1 = sxba_scan (sra_set, 0)) > 0) {
	lrm1_ptr = current_state + x1;
	typ1 = get_type (lrm1_ptr->attributes.origine_index, lrm1_ptr->attributes.kind);
	
	while ((x1 = sxba_scan (sra_set, x1)) > 0) {
	    lrm2_ptr = current_state + x1;
	    typ2 = get_type (lrm2_ptr->attributes.origine_index, lrm2_ptr->attributes.kind);
	    
	    switch (T [typ1] [typ2]) {
	    case 1:
		/* conflict */
		if (*is_conflict_conflict_mode == ONELA_CONFLICT || lrm1_ptr->index == lrm2_ptr->index)
		    return true;
		
		break;
		
	    case 2:
		/* action/action */
		if (ers [lrm1_ptr->attributes.origine_index].lispro !=
		    ers [lrm2_ptr->attributes.origine_index].lispro)
		    if (*is_conflict_conflict_mode == ONELA_CONFLICT || lrm1_ptr->index == lrm2_ptr->index)
			return true;
		
		break;
		
	    case 3:
		/* reduce/reduce */
		if (ers_disp [ers [lrm1_ptr->attributes.origine_index - 1].prolis].master_token !=
		    ers_disp [ers [lrm2_ptr->attributes.origine_index - 1].prolis].master_token)
		    if (*is_conflict_conflict_mode == ONELA_CONFLICT || lrm1_ptr->index == lrm2_ptr->index)
			return true;
		
		break;
		
	    case 0:
		/* no_conflict */
		break;
            default:
#if EBUG
                sxtrap("lecl_fsa","unknown switch case #1");
#endif
                break;
	    }
	    
	    lrm1_ptr = lrm2_ptr;
	    typ1 = typ2;
	}
    }
    
    return false;
}



static void	promote (SXBA sra_set)
{

/* Si apres resolution des conflits unbounded FIRST_REDUCE est vide nous promouvons
   NEXT_REDUCE en FIRST_REDUCE afin de simuler (approximativement) la 
   poursuite eventuelle du mecanisme de resolution des conflits; ce n'est pas
   tres satisfaisant. Il faudrait associer a chaque Reduce un niveau et
   FIRST_REDUCE serait donne par les items de type reduce de niveau le plus
   bas. */

    SXINT	promote_x;

    promote_x = 0;

    while ((promote_x = sxba_scan (sra_set, promote_x)) > 0)
	if (current_state [promote_x].attributes.kind == FIRST_REDUCE)
	    return;

    promote_x = 0;

    while ((promote_x = sxba_scan (sra_set, promote_x)) > 0) {
	if (current_state [promote_x].attributes.kind == NEXT_REDUCE)
	    SXBA_1_bit (is_promoted, promote_x);
    }
}


static void apply_user_prio (SXBA sra_set, char apply_user_prio_prio /* bit (3) */)
{
    SXINT		apply_user_prio_x, apply_user_prio_ner;
    char	apply_user_prio_prio_kind /* bit (3) */ ;
    
    if (apply_user_prio_prio & Reduce_Reduce) {
	/*  Reduce>Reduce  En cas de conflit (1la ou unbounded), on supprime
	    l'ensemble des items de type FIRST_REDUCE dont l'ER
	    d'origine n'a pas la clause Reduce>Reduce (il en restera forcement). */
	apply_user_prio_prio = 0;
	apply_user_prio_x = 0;
	
	while ((apply_user_prio_x = sxba_scan (sra_set, apply_user_prio_x)) > 0) {
	    if (current_state [apply_user_prio_x].attributes.kind == FIRST_REDUCE) {
		apply_user_prio_ner = get_red_no (current_state [apply_user_prio_x].attributes.origine_index);
		apply_user_prio_prio_kind = ers_disp [apply_user_prio_ner].priority_kind;
		
		if (!(apply_user_prio_prio_kind & Reduce_Reduce))
		    SXBA_0_bit (sra_set, apply_user_prio_x);
		else
		    apply_user_prio_prio |= apply_user_prio_prio_kind;
	    }
	}
    }
    
    if (apply_user_prio_prio & Reduce_Shift /* Reduce>Shift */ ) {
	/*   Reduce>Shift   En cas de conflit (1la ou unbounded), si l'ensemble 
	     FIRST_REDUCE est non vide, on supprime les items de
	     SHIFT_KIND U ACTION_KIND U NEXT_REDUCE */
	apply_user_prio_x = 0;
	
	while ((apply_user_prio_x = sxba_scan (sra_set, apply_user_prio_x)) > 0) {
	    if (current_state [apply_user_prio_x].attributes.kind != FIRST_REDUCE)
		SXBA_0_bit (sra_set, apply_user_prio_x);
	}
    }
    
    if (apply_user_prio_prio & Shift_Reduce /* Shift>Reduce */ ) {
	/*   Shift>Reduce   En cas de conflit (1la ou unbounded), si l'ensemble 
	     SHIFT_KIND U ACTION_KIND U NEXT_REDUCE est non vide, on
	     supprime les items de FIRST_REDUCE qui ont la clause 
	     Shift>Reduce. */
	
	/* On regarde d'abord s'il y a des SHIFT_KIND ou ACTION_KIND ou NEXT_REDUCE */
	apply_user_prio_x = 0;
	
	while ((apply_user_prio_x = sxba_scan (sra_set, apply_user_prio_x)) > 0) {
	    if (current_state [apply_user_prio_x].attributes.kind != FIRST_REDUCE) {
		apply_user_prio_x = 0;
		
		while ((apply_user_prio_x = sxba_scan (sra_set, apply_user_prio_x)) > 0) {
		    if (current_state [apply_user_prio_x].attributes.kind == FIRST_REDUCE)
			if (ers_disp [get_red_no (current_state [apply_user_prio_x].attributes.origine_index)].
			    priority_kind & Shift_Reduce)
			    SXBA_0_bit (sra_set, apply_user_prio_x);
		}
		
		break;
	    }
	}
    }
}


static	bool conflict_mngr (SXBA sra_set, 
			       SXINT conflict_mngr_conflict_mode, 
			       char conflict_mngr_prio /* bit (3) */)
{
    SXINT		conflict_mngr_x, conflict_mngr_j;
    SXINT		conflict_mngr_ner, choice, choice_priority, typ, class, orig, current_conflict_mode;
    bool	is_shift, is_action, is_reduce;
    char	kind /* bit (2) */ ;
    char	conflict_kind /* bit (5) */ ;

    sxinitialise(conflict_mngr_ner); /* pour faire taire gcc -Wuninitialized */
    sxinitialise(class); /* pour faire taire gcc -Wuninitialized */

/* On utilise en premier les priorites utilisateur pour resoudre les conflits.
   Si ca marche, on ne sort pas de diagnostics */

    current_conflict_mode = conflict_mngr_conflict_mode;

    if (conflict_mngr_prio) {
	/* Il y a des clauses "Priority" dans la grammaire,
	   il y a donc des "FIRST_REDUCE" dans le coup */
	apply_user_prio (sra_set, conflict_mngr_prio);

	if (!is_conflict (sra_set, &current_conflict_mode, &dum_char))
	    /* Tous les conflits ont ete resolus par les priorites de l'utilisateur */
	    return false;
    }


/* Il reste des conflits => application des regles systeme */

/* Les references aux items dans sra_set sont classees par origines
   croissantes */

    conflict_kind = 0;
    /* Shift || (first) Action || (next) Action || (first) Reduce || (next) Reduce */
    choice_priority = UNKNOWN;
    is_reduce = is_action = false;


/* Priorite au shift sinon priorite a l'action ou au reduce d'index min */
    sxinitialise (choice);
    conflict_mngr_j = 0;

    while ((conflict_mngr_j = sxba_scan (sra_set, conflict_mngr_j)) > 0) {
	kind = current_state [conflict_mngr_j].attributes.kind;
	orig = current_state [conflict_mngr_j].attributes.origine_index;
	typ = get_type (orig, kind);

	if (typ == PURE_ACTION || typ == DISGUISED_ACTION)
	    if (is_action) {
		if (class != ers [orig].lispro)
		    conflict_kind |= 4 /* (next) Action */ ;
	    }
	    else {
		is_action = true;
		class = ers [orig].lispro;
		conflict_kind |= 2 /* (first) Action */ ;
	    }
	else if (typ == REDUCE_CONFLICT)
	    if (is_reduce) {
		if (conflict_mngr_ner != orig)
		    conflict_kind |= 16 /* (next) Reduce */ ;
	    }
	    else {
		is_reduce = true;
		conflict_mngr_ner = orig;
		conflict_kind |= 8 /* (first) Reduce */ ;
	    }
	else
	    conflict_kind |= 1 /* Shift */ ;

	if (typ < choice_priority) {
	    choice_priority = typ;
	    choice = orig;
	}
	else if (typ == choice_priority)
	    if (orig < choice)
		choice = orig;
    }

    if (current_conflict_mode == ONELA_CONFLICT)
	if (current_state_kind & LA)
	    lecl_diagnoses_mixte (current_state, sra_set, fsa [xfsa1].seed_state,
				  xfsa1, conflict_kind, choice, &m_head_ptr);
	else
	    if (is_non_deterministic_automaton && (conflict_kind & 6))
		/* Conflit avec des actions */
		lecl_diagnoses_immediate (current_state, sra_set, xfsa1, conflict_kind,
					  choice, false);
	    else
		lecl_diagnoses_one_la (current_state, sra_set, xfsa1, conflict_kind,
				       choice, esc1_set);
    else
	 /* les conflits unbounded ne peuvent pas etre emis immediatement car les
	    chemins menant de l'etat germe a l'etat de detection peuvent encore etre
	    augmentes  de plus on regroupe les points de detection */
	 if (current_state_kind & LA)
	     lecl_diagnoses_unbounded (current_state, sra_set, fsa [xfsa1].seed_state,
				       xfsa1, conflict_kind, choice, &u_head_ptr);
	 else
	     lecl_diagnoses_immediate (current_state, sra_set, xfsa1, conflict_kind, choice, true);

    if (!is_non_deterministic_automaton) {
	is_reduce = is_action = is_shift = false;
	class = ers [choice].lispro;
	
	if (class >= 0)
	    is_shift = true;
	else if (class <= xactmin)
	    is_action = true;
	else
	    is_reduce = true;
	
	conflict_mngr_x = 0;
	
	while ((conflict_mngr_x = sxba_scan (sra_set, conflict_mngr_x)) > 0) {
	    /* Actions systeme:
	       Si Shift => on supprime tout ce qui n'est pas Shift
	       Si Action => on supprime tout ce qui n'est pas l'action choisie
	       Si Reduce => on supprime tout ce qui n'est pas le reduce choisi
	       */
	    orig = current_state [conflict_mngr_x].attributes.origine_index;
	    
	    if ((is_shift && ers [orig].lispro >= 0)
		|| (is_action && ers [orig].lispro == class)
		|| (is_reduce && orig == choice))
		;
	    else
		SXBA_0_bit (sra_set, conflict_mngr_x);
	}
    }

    return is_non_deterministic_automaton;
}



static	bool conflict_in_comments (SXBA sra_set)
{
    SXINT	sra_set_card, x1, x2;
    char	priority /* bit (3) */ ;
    bool	has_conflict;

    sra_set_card = sxba_cardinal (sra_set);

    if (sra_set_card < 2)
	return false;

    has_conflict = false;

    if (sra_set_card == 2) {
	/* Easy case */
	SXBA	/* max_re + 1 */ ctxt1_set, ctxt2_set;

	x1 = sxba_scan (sra_set, 0);
	x2 = sxba_scan (sra_set, x1);
	ctxt1_set = ctxt [current_state [x1].attributes.ctxt_no];
	ctxt2_set = ctxt [current_state [x2].attributes.ctxt_no];
	sxba_and (sxba_copy (ctxt_set, ctxt1_set), ctxt2_set);

	if (!sxba_is_empty (ctxt_set))
	    /* il y a conflit potentiel */
	    if (is_conflict (sra_set, &dum_int
			     /* out: doit rendre UNBOUNDED_CONFLICT */
					      , &priority)) {
		has_conflict |= conflict_mngr (sra_set, UNBOUNDED_CONFLICT, priority);
		sxba_not (ctxt_set);
		SXBA_0_bit (ctxt_set, 0);

		if (sxba_bit_is_set (sra_set, x1)) {
		    /* x1 n'est pas touche */
		    sxba_and (ctxt_set, ctxt2_set);
		    maj_ctxt (x2, ctxt_set, sra_set);
		}
		else {
		    sxba_and (ctxt_set, ctxt1_set);
		    maj_ctxt (x1, ctxt_set, sra_set);
		}
	    }
    }
    else {
	/* Hard case */
        /* Le 21/3/02 on change les tailles de xcurrent_state a max_items_no */
	SXBA	/* max_re */ re_set;
	SXBA	/* xcurrent_state non max_items_no */ it_set, item_set, have_changed;
	SXBA	/* xcurrent_state non max_items_no */ *sets /* 1:max_re */ ;
	SXBA	/* max_re */ *ctxt_sets /* 1:xcurrent_state non max_items_no */ ;
	SXINT	conflict_in_comments_x, conflict_in_comments_ner;

	sets = sxbm_calloc (max_re + 1, max_items_no + 1);
	ctxt_sets = sxbm_calloc (max_items_no + 1, max_re + 1);
	re_set = sxba_calloc (max_re + 1);
	item_set = sxba_calloc (max_items_no + 1);
	have_changed = sxba_calloc (max_items_no + 1);

	conflict_in_comments_x = 0;

	while ((conflict_in_comments_x = sxba_scan (sra_set, conflict_in_comments_x)) > 0) {
	    sxba_copy (ctxt_set, ctxt [current_state [conflict_in_comments_x].attributes.ctxt_no]);
	    sxba_copy (ctxt_sets [conflict_in_comments_x], ctxt_set);
	    conflict_in_comments_ner = 0;

	    while ((conflict_in_comments_ner = sxba_scan (ctxt_set, conflict_in_comments_ner)) > 0) {
		SXBA_1_bit (sets [conflict_in_comments_ner], conflict_in_comments_x);
		SXBA_1_bit (re_set, conflict_in_comments_ner);
	    }
	}

	conflict_in_comments_ner = 0;

	while ((conflict_in_comments_ner = sxba_scan (re_set, conflict_in_comments_ner)) > 0) {
	    sxba_empty (ctxt_set);
	    SXBA_0_bit (re_set, conflict_in_comments_ner);
	    SXBA_1_bit (ctxt_set, conflict_in_comments_ner);
	    it_set = sets [conflict_in_comments_ner];


/* On traite simultanement les contextes ayant meme it[em]_set */
	    conflict_in_comments_x = conflict_in_comments_ner;

	    while ((conflict_in_comments_x = sxba_scan (re_set, conflict_in_comments_x)) > 0) {
		if (sxba_first_difference (it_set, sets [conflict_in_comments_x]) == -1) {
		    SXBA_0_bit (re_set, conflict_in_comments_x);
		    SXBA_1_bit (ctxt_set, conflict_in_comments_x);
		}
	    }

	    sxba_copy (item_set, it_set);

	    if (is_conflict (it_set, &dum_int, &priority))
		has_conflict |= conflict_mngr (it_set, UNBOUNDED_CONFLICT, priority);

	    sxba_minus (item_set, it_set);
	    sxba_not (ctxt_set);
	    SXBA_0_bit (ctxt_set, 0);
	    conflict_in_comments_x = 0;

	    while ((conflict_in_comments_x = sxba_scan (item_set, conflict_in_comments_x)) > 0) {
		sxba_and (ctxt_sets [conflict_in_comments_x], ctxt_set);
		SXBA_1_bit (have_changed, conflict_in_comments_x);
	    }
	}

	sxba_minus (sra_set, have_changed);


/* Creation eventuelle de nouveaux contextes */
	conflict_in_comments_x = 0;

	while ((conflict_in_comments_x = sxba_scan (have_changed, conflict_in_comments_x)) > 0) {
	    maj_ctxt (conflict_in_comments_x, ctxt_sets [conflict_in_comments_x], sra_set);
	}

	sxfree (have_changed);
	sxfree (item_set);
	sxfree (re_set);
	sxbm_free (ctxt_sets);
	sxbm_free (sets);
    }

    return has_conflict;
}



static	bool unbounded_conflict (SXBA sra_set, 
				    char unbounded_conflict_prio /* bit (3) */)
{
    SXBA	/* xcurrent_state */ set;
    SXINT	unbounded_conflict_x, xindexes, unbounded_conflict_i, ind;
    bool		has_conflict;

    sxinitialise(unbounded_conflict_i); /* pour faire taire gcc -Wuninitialized */
/* On redecoupe par adresse de detection */

    xindexes = 0;
    unbounded_conflict_x = 0;

    while ((unbounded_conflict_x = sxba_scan (sra_set, unbounded_conflict_x)) > 0) {
	ind = current_state [unbounded_conflict_x].index;

	for (unbounded_conflict_i = 1; unbounded_conflict_i <= xindexes; unbounded_conflict_i++) {
	    if (indexes [unbounded_conflict_i] == ind)
		goto next;
	}

	unbounded_conflict_i = ++xindexes;
	sxba_empty (sra_s [unbounded_conflict_i]);
	indexes [xindexes] = ind;
next:	SXBA_1_bit (sra_s [unbounded_conflict_i], unbounded_conflict_x);
    }

    sxba_empty (sra_set);
    has_conflict = false;

    for (unbounded_conflict_x = 1; unbounded_conflict_x <= xindexes; unbounded_conflict_x++) {
	set = sra_s [unbounded_conflict_x];

	if (is_conflict (set, &dum_int, &unbounded_conflict_prio)) {
	    unbounded_conflict_i = ers_disp [ers [indexes [unbounded_conflict_x]].prolis].master_token;

	    if (unbounded_conflict_i == comments_re_no /* dans les commentaires */ )
		has_conflict |= conflict_in_comments (set);
	    else
		has_conflict |= conflict_mngr (set, UNBOUNDED_CONFLICT, unbounded_conflict_prio);
	}

	sxba_or (sra_set, set); /* recopie dans le parametre */
    }

    return has_conflict;
}



static	void maj_trans (void)
{
    bool	found;
    SXINT	maj_trans_x, cc_ref;

    found = false;

    for (maj_trans_x = xfsa_t2; maj_trans_x < xfsa_t && !found; maj_trans_x++) {
	if (fsa_trans [maj_trans_x].next_state_no == next_state
	    && fsa_trans [maj_trans_x].is_del == bdel
	    && fsa_trans [maj_trans_x].is_scan == scan) {
	    cc_ref = fsa_trans [maj_trans_x].cc_ref;
	    sxba_or (compound_classes [cc_ref], sc_set);
	    found = true;
	}
    }

    if (!found) {
	/* xcc<=xfsa_t */
	sxba_copy (compound_classes [++xcc], sc_set);
	put_new_trans (xcc, next_state, bdel, scan);
    }
}



static	void allouer (void)
{
    esc_to_xcurrent_state = sxbm_calloc (last_esc + 1, max_items_no + 1);
    current_state = (struct lr_marker*) sxalloc (max_items_no + 1, sizeof (struct lr_marker));
    new_ctxt = (SXINT*) sxalloc (max_items_no + 1, sizeof (SXINT));
    is_promoted = sxba_calloc (max_items_no + 1);
    is_new_ctxt = sxba_calloc (max_items_no + 1);
    to_be_sorted = (SXINT*) sxalloc (max_items_no + 1, sizeof (SXINT));
    xcurrent_state_set = sxba_calloc (max_items_no + 1);
    indexes = (SXINT*) sxalloc (max_items_no + 1, sizeof (SXINT));
    not_yet_processed = sxba_calloc (max_items_no + 1);
    store = (struct lr_marker*) sxalloc (max_items_no + 1, sizeof (struct lr_marker));
    sorted_actions = (SXINT*) sxalloc (max_items_no + 1, sizeof (SXINT));
    sra_s = sxbm_calloc (max_items_no + 1, max_items_no + 1);
}



static	void liberer (void)
{
    sxbm_free (sra_s);
    sxfree (sorted_actions);
    sxfree (store);
    sxfree (not_yet_processed);
    sxfree (indexes);
    sxfree (xcurrent_state_set);
    sxfree (to_be_sorted);
    sxfree (is_new_ctxt);
    sxfree (is_promoted);
    sxfree (new_ctxt);
    sxfree (current_state);
    sxbm_free (esc_to_xcurrent_state);
}



static bool	is_act_red (SXBA w_set, 
			    SXBA is_act_red_sc_set, 
			    SXINT is_act_red_next_state)
{
    /* is_act_red_next_state est un etat pur action.
       Cette fonction regarde si tous les items de l'etat courant (!LA) ayant
       un rapport avec is_act_red_sc_set (entre 1 et xcurrent_state dans current_state)
       qui correspondent aux actions de is_act_red_next_state sont des FIRST_REDUCE */

    SXINT	is_act_red_xitem, is_act_red_x, is_act_red_index, lim, is_act_red_k;

    lim = fsa [is_act_red_next_state + 1].item_ref;

    for (is_act_red_xitem = fsa [is_act_red_next_state].item_ref; is_act_red_xitem < lim; is_act_red_xitem++) {
	is_act_red_index = item [is_act_red_xitem].index;

	for (is_act_red_x = 1; is_act_red_x <= xcurrent_state; is_act_red_x++) {
	    is_act_red_k = ers [current_state [is_act_red_x].index].lispro;

	    if (is_act_red_k > 0) {
		sxba_and (sxba_copy (w_set, kod [is_act_red_k]), is_act_red_sc_set);

		if (!sxba_is_empty (w_set) && current_state [is_act_red_x].attributes.origine_index == is_act_red_index && current_state [is_act_red_x].
		     attributes.kind != FIRST_REDUCE) {
		    return false;
		}
	    }
	}
    }

    return true;
}



static void complete_trans (void)
{
    bool	is_sc_set_empty = false;

    if (has_predicate) {
	sxba_empty (sc_set);
	is_sc_set_empty = true;
	esc1 = 0;
	
	while ((esc1 = sxba_scan (esc1_set, esc1)) > 0) {
	    if (esc1 <= smax) {
		k = esc1;
		x = prdct_true_code;
	    }
	    else {
		k = esc_to_sc_prdct [esc1].simple_class;
		x = esc_to_sc_prdct [esc1].predicate;
	    }
	    
	    if (sxba_bit_is_set (has_prdct, k)) {
		set_sc_prdct_to_next (k, x);
	    }
	    else {
		SXBA_1_bit (sc_set, k);
		is_sc_set_empty = false;
	    }
	}
    }
    
    if (next_state_kind & LA)
	bdel = scan = false;
    
    if (!is_sc_set_empty)
	/* On est sur que les sc intervenant dans la transition
	   courante ne sont pas associees (meme dans d'autres
	   transitions) a des predicats, next_state est donc OK,
	   de plus esc1_set = sc_set */
	maj_trans ();
}

static SXINT create_nd_state (struct sc_prdct_to_next_item *p)
{
    /* traitement d'une liste d'actions non-deterministes. */
    SXINT				create_nd_state_x, create_nd_state_hsh, create_nd_state_i;
    struct lr_marker		create_nd_state_lrm;
    struct lr_marker	*item1_ptr, *item2_ptr;

    sxinitialise(item2_ptr); /* pour faire taire gcc -Wuninitialized */
    xstate = 0;

    do {
	create_nd_state_lrm.index = p->state_no;
	create_nd_state_lrm.attributes.origine_index = 0;
	create_nd_state_lrm.attributes.ctxt_no = p->is_scan ? 1 : 0;
	create_nd_state_lrm.attributes.kind = p->is_del ? 1 : 0;
	put (&create_nd_state_lrm);
	
	if ((create_nd_state_x = p->lnk))
	    p = &(next_state_no_list [create_nd_state_x]);
    } while (create_nd_state_x);

    xitem = xitem2;

    for (create_nd_state_i = 1; create_nd_state_i <= xstate; create_nd_state_i++) {
	to_be_sorted [create_nd_state_i] = create_nd_state_i;
    }

    if (xstate > 1)
	sort_by_tree (to_be_sorted, 1, xstate, less);

    create_nd_state_hsh = 0;

    for (create_nd_state_i = 1; create_nd_state_i <= xstate; create_nd_state_i++) {
	item1_ptr = state + to_be_sorted [create_nd_state_i];

	if (create_nd_state_i == 1 || !eq_lr_marker (item2_ptr, item1_ptr)) {
	    item2_ptr = item + xitem2;
	    *item2_ptr = *item1_ptr;
	    create_nd_state_hsh += item2_ptr->index;
	    maj (&xitem2, (SXINT)3);
	}
    }

    create_nd_state_hsh = -(create_nd_state_hsh % 127);

    next_state_kind = NONDETER;
    bdel = false;
    scan = false;
    
    if (!is_state_already_defined ((SXINT)0, (SXINT)0, bdel,
				   scan, &next_state, create_nd_state_hsh))
	set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, bdel,
		       scan, next_state_kind);
    
    return next_state;
}


void	lecl_fsa_construction (void)
{
    bool				is_nd_trans;
    struct sc_prdct_to_next_item	*p, *p1;
    
    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = true;
	}

	fputs ("FSA Construction\n", sxtty);
    }

    xnondetermax = 0;
    max_items_no = 100;
    /*     A L L O C A T I O N    */
    ctxt_set = sxba_calloc (max_re + 1);
    and_set = sxba_calloc (max_re + 1);
    sc1_set = sxba_calloc (smax + 1);
    esc_set = sxba_calloc (last_esc + 1);
    esc1_set = sxba_calloc (last_esc + 1);
    has_prdct = sxba_calloc (smax + 1);
    sc_set = sxba_calloc (smax + 1);
    has_true = sxba_calloc (smax + 1);
    sc_to_prdct = sxbm_calloc (smax + 1, xprdct_to_ate + 1);
    sc_prdct_to_next = (SXINT**) sxalloc (smax + 1, sizeof (SXINT*));

    {
	SXINT	lecl_fsa_construction_i;

	sc_prdct_to_next [0] = NULL;

	for (lecl_fsa_construction_i = 1; lecl_fsa_construction_i <= smax; lecl_fsa_construction_i++)
	    sc_prdct_to_next [lecl_fsa_construction_i] = (SXINT*) sxcalloc ((SXINT) (xprdct_to_ate + 1), sizeof (SXINT));

    }

    next_state_no_list_size = 100;
    next_state_no_list_top = 0;
    next_state_no_list = (struct sc_prdct_to_next_item *)
	    sxalloc (next_state_no_list_size + 1, sizeof (struct sc_prdct_to_next_item));
    
    sc_name = varstr_alloc (32);

/*   I N I T I A L I Z A T I O N S   */

    m_head_ptr = u_head_ptr = NULL;
    fsa_hsh_tbl = &(FSA_HSH_TBL [126]);
    /* translation d'adresses (-126:0) */
    compound_classes = sxbm_calloc (lgt [4] + 1, smax + 1);
    xfsa2 = 1;
    fsa [1].item_ref = 1;
    fsa [1].transition_ref = 1;
    xcc = 0;
    xitem2 = 1;
    xfsa_t = 1;
    erase_inconsistency = 0;

    for (i = -126; i <= 0; i++)
	fsa_hsh_tbl [i] = 0;


/* creation de l'etat initial */

    xsubstate = 0;

    for (i = 1; i <= max_re; i++) {
	j = ers_disp [i].prolon;

	if (j != ers_disp [i + 1].prolon)
	    if (ers_disp [i].reduc > 0 || ers [j].lispro == -2 || ers_disp [i].master_token == include_re_no)
		put_item_in_substate (j, (SXINT)0, i, UNDEF_KIND);
    }

    xstate = 0;
    subsuiv (true, false, false);

    if (xstate > max_items_no)
	max_items_no = xstate;

    allouer ();
    xitem = xitem2;
    build_state (&xitem2, &hsh, &next_state_kind, &ner, false);
    fsa_hsh_tbl [hsh] = xfsa2;
    fsa [xfsa2].lnk = 0;
    set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, false, false, next_state_kind);


/* on boucle tant qu'il y a un etat non traite */

    for (xfsa1 = 1; xfsa1 < xfsa2; xfsa1++) {
	if (sxverbosep) {
	    fputc ('S', sxtty);
	    sxttycol1p = false;
	}

	current_state_kind = fsa [xfsa1].state_kind;
	/* On traite en premier les cas ou il n'est pas necessaire
	   de connaitre les contextes */
	xsubstate = 0;
	xstate = 0;
	origine = fsa [xfsa1].item_ref;
	sommet = fsa [xfsa1 + 1].item_ref;
	items_no = sommet - origine;

	if ((current_state_kind & ACT) && !(current_state_kind & MIXTE)) {
	    /* a single action => no look_ahead is needed */
	    for (xitem = origine; xitem < sommet; xitem++) {
		maj (&xsubstate, (SXINT)1);
		substate [xsubstate] = item [xitem];
	    }

	    action_no = ers [item [origine].index].lispro;
	    subsuiv (true, false, false);
	    xitem = xitem2;
	    build_state (&xitem2, &hsh, &next_state_kind, &ner, false);

	    if (!is_state_already_defined ((SXINT)0, (SXINT)0, false, false, &next_state, hsh))
		set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, false, false, next_state_kind);

	    put_new_trans (action_no, next_state, false, false);
	    fsa [xfsa1 + 1].transition_ref = xfsa_t;
	}
	else if (items_no == 1 && (current_state_kind & SHIFT)) {
	    /* On n'est pas en look_ahead */
	    indx = item [origine].index;
	    predicate_no = ers [indx].prdct_no;
	    bdel = ers [indx].is_erased;
	    scan = true;

	    if (predicate_no != prdct_false_code) {
		maj (&xsubstate, (SXINT)1);
		substate [xsubstate] = item [origine];
		subsuiv (true, false, false);
	    }


/* Si &False => xstate = 0 */

	    xitem = xitem2;
	    build_state (&xitem2, &hsh, &next_state_kind, &ner, false);

	    if (!is_state_already_defined ((SXINT)0, (SXINT)0, bdel, scan, &next_state, hsh))
		set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, bdel, scan, next_state_kind);

	    if (predicate_no > prdct_false_code) {
		xsubstate = 0;
		put_item_in_substate (prdct_to_ers [predicate_no],
				      next_state,
				      (SXINT)(scan ? 1 : 0),
				      (char) (bdel ? 2 : 0));
		xstate = 0;
		subsuiv (true, false, false);
		xitem = xitem2;
		build_state (&xitem2, &hsh, &next_state_kind, &ner, false);
		next_state_kind = PRDCT;

		if (!is_state_already_defined ((SXINT)0, (SXINT)0, bdel, scan, &next_state, hsh))
		    set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, bdel, scan, next_state_kind);
	    }

	    sxba_copy (compound_classes [++xcc], kod [ers [indx].lispro]);
	    put_new_trans (xcc, next_state, bdel, scan);
	    fsa [xfsa1 + 1].transition_ref = xfsa_t;
	}
	else if (current_state_kind & PRDCT)
	    /* Il n'y a pas de conflit impliquant un predicat:
	       S'il y a un marqueur en position reduce, on suppose qu'il se trouve devant 
	       "&Else" */
	    predicates_processing (origine, items_no, (bool)(current_state_kind & LA));
	else if (current_state_kind & NONDETER) {
	    /* Traitement d'un etat non-deterministe */
	    i = 0;

	    for (xitem = origine; xitem < sommet; xitem++) {
		put_new_trans (++i,
			       item [xitem].index,
			       (bool) (item [xitem].attributes.kind == 1), /* del */
			       (bool) (item [xitem].attributes.ctxt_no == 1) /* scan */);
	    }

	    if (i > xnondetermax)
		xnondetermax = i;

	    fsa [xfsa1 + 1].transition_ref = xfsa_t;
	}
	else if ((current_state_kind & REDUCE) && !(current_state_kind & MIXTE) && is_optimize) {
	    /* On genere la reduction sur Any+Eof */
	    sxba_fill (sc1_set);
	    SXBA_0_bit (sc1_set, 0);
	    SXBA_0_bit (sc1_set, 1);
	    sxba_copy (compound_classes [++xcc], sc1_set);
	    put_new_trans (xcc, -ers [item [origine].index - 1].prolis, false, false);
	    fsa [xfsa1 + 1].transition_ref = xfsa_t;
	}
	else {
	    /* Cas general */
	    if (is_non_deterministic_automaton && (current_state_kind & MIXTE)) {
		/* On regarde si un marqueur LR reference une action apres fermeture */
		xstate = 0;
		xsubstate = 0;

		for (xitem = origine; xitem < sommet; xitem++) {
		    k = item [xitem].index;
		    
		    if (ers [k].lispro >= 0
			|| ers [k].lispro <= xactmin) {
			/* point en position shift ou
			   marqueur lr sur une specification d'action */
			if (current_state_kind & LA)
			    put (item + xitem);
			else {
			    lrm.attributes.origine_index = lrm.index = k;
			    lrm.attributes.ctxt_no = item [xitem].attributes.ctxt_no;
			    lrm.attributes.kind = ers [k].lispro >= 0 ? SHIFT_KIND : ACTION_KIND;
			    put (&lrm);
			}
		    }
		    else {
			/* il y a au moins un point en position reduce */
			if (current_state_kind & LA)
			    filsubstate (item + xitem);
			else {
			    lrm.attributes.origine_index = lrm.index = k;
			    lrm.attributes.ctxt_no = item [xitem].attributes.ctxt_no;
			    lrm.attributes.kind = FIRST_REDUCE;
			    filsubstate (&lrm);
			}
		    }
		}

		if (xsubstate > 0)
		    subsuiv (true, true, true);
	    
		/* state est rempli avec l'etat courant 
		   Il n'est pas trie et il peut y avoir des doublons
		   On regarde si une action est referencee */
		
		for (x = 1; x <= xstate; x++) {
		    if (ers [state [x].index].lispro <= xactmin)
			/* Un marqueur LR de look-ahead reference une action, comme
			   dans le cas du non-determinisme les actions sont executees
			   en look-ahead, on genere le non-determinisme
			   pour la branche correspondante de l'automate de look-ahead */
			break;
		}

		if (x <= xstate) {
		    /* Il y a des actions ds le coup... */
		    SXINT			orig, class, y;
		    
		    items_no = xstate;
		    
		    if (items_no > max_items_no) {
			liberer ();
			/* reallocation des max_items_no */
			max_items_no = items_no;
			allouer ();
		    }
		    
		    for (i = 1; i <= items_no; i++) {
			to_be_sorted [i] = i;
		    }
		    
		    if (items_no > 1)
			sort_by_tree (to_be_sorted, 1, items_no, less);
		    
		    sxba_empty (xcurrent_state_set);
		    xcurrent_state = 0;
		    
		    for (i = 1; i <= items_no; i++) {
			lr_marker_ptr = state + to_be_sorted [i];
			
			if (i > 1)
			    if (eq_lr_marker (current_state + xcurrent_state, lr_marker_ptr))
				continue;
			
			current_state [++xcurrent_state] = *lr_marker_ptr;
			SXBA_1_bit (xcurrent_state_set, xcurrent_state);
		    }
		    
		    /* L'etat courant est range dans current_state entre 1 et xcurrent_state */
		    
		    /* On regarde si apres application des priorites utilisateur, il reste
		       des actions. */
		    x = 0;

		    if (current_state_kind & REDUCE) {
			set_conflict_mode_prio (xcurrent_state_set, &conflict_mode, &prio);
			
			if (prio) {
			    /* Il y a des priorites utilisateur */
			    apply_user_prio (xcurrent_state_set, prio);
			    
			    /* Reste-t-il des actions ? */
			    while ((x = sxba_scan (xcurrent_state_set, x)) > 0) {
				if (ers [current_state [x].index].lispro <= xactmin) {
				    /* oui */
				    x = 0;
				    break;
				}
			    }

			}
		    }
		    
		    if (x == 0) {
			/* Il reste des actions */
			if ((current_state_kind & LA)) {
			    /* Pour les messages d'erreur donnant les points de detection,
			       on supprime les items dont l'index ne reference pas
			       une action. */
			    /* Rappelons que ds le cas unbounded, les points de detection
			       sont factorises pour tous les conflits. */
			    while ((x = sxba_scan (xcurrent_state_set, x)) > 0) {
				class =
				    ers [current_state [x].attributes.origine_index].lispro;
				
				if (class > xactmin &&
				    ers [current_state [x].index].lispro > xactmin)
				    /* pas une action */
				    current_state [x].index = 0; /* pour lecl_diagnose */
			    }
			}

			/* sortie du message */
			conflict_mode = (current_state_kind & LA)
			    ? UNBOUNDED_CONFLICT
				: ONELA_CONFLICT;
			conflict_mngr (xcurrent_state_set, conflict_mode, UNDEF_KIND);
			
			/* On produit du non-determinisme */
			if (current_state_kind & SHIFT) {
			    /* Le Shift */
			    if (current_state_kind & LA) {
				next_state = fsa [xfsa1].shift_state;
				bdel = fsa [xfsa1].del;
				scan = fsa [xfsa1].scan;
			    }
			    else {
				xstate = 0;
				
				for (xitem = origine; xitem < sommet; xitem++) {
				    if (ers [item [xitem].index].lispro >= 0)
					put (item + xitem);
				}
			    
				if (xstate > 0) {
				    xitem = xitem2;
				    build_state (&xitem2, &hsh, &next_state_kind, &ner, false);
				    
				    if (!is_state_already_defined ((SXINT)0, (SXINT)0, false, false,
								   &next_state, hsh))
					set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, false, false, next_state_kind);
				    
				    bdel = scan = false;
				}
				else
				    next_state = 0;
			    }
			    
			    if (next_state)
				set_sc_prdct_to_next ((SXINT)1, prdct_true_code) /* Any */;
			}
			
			if (current_state_kind & REDUCE) {
			    /* Les Reduce */
			    x = 0;
			    
			    while ((x = sxba_scan_reset (xcurrent_state_set, x)) > 0) {
				orig = current_state [x].attributes.origine_index;
				
				if ((class = ers [orig].lispro) < 0
				    && class > xactmin) {
				    ner = orig;
				    next_state = -ers [ner - 1].prolis;
				    y = x;
				    
				    while ((y = sxba_scan (xcurrent_state_set, y)) > 0)
					if (current_state [y].attributes.origine_index == ner)
					    SXBA_0_bit (xcurrent_state_set, y);
				    
				    bdel = false;
				    scan = false;
				    set_sc_prdct_to_next ((SXINT)1, prdct_true_code) /* Any */;
				}
			    }
			}
			
			if (current_state_kind & ACT) {
			    /* Les actions */
			    xstate = 0;
			    
			    for (xitem = origine; xitem < sommet; xitem++) {
				if (ers [item [xitem].index].lispro <= xactmin)
				    put (item + xitem);
			    }
			    
			    build_action_states (xstate /* passe par valeur */ );
			}
			
			/* On genere le non determinisme immediatement */
			p = &(next_state_no_list [sc_prdct_to_next [1] [prdct_true_code]]);
			next_state = create_nd_state (p);
			bdel = false;
			scan = false;
			sc_prdct_to_next [1] [prdct_true_code] = 0;
			sxba_fill (sc_set);
			SXBA_0_bit (sc_set, 0);
			SXBA_0_bit (sc_set, 1); /* Any+Eof */
			maj_trans ();
			fsa [xfsa1 + 1].transition_ref = xfsa_t;
			continue;
		    }
		}
	    }
	    
	    if (current_state_kind & ACT && !(current_state_kind & LA)) {
		/* on calcule les etats atteints par transition sur les actions */
		xstate = 0;
		
		for (xitem = origine; xitem < sommet; xitem++) {
		    k = item [xitem].index;
		    
		    if (ers [k].lispro < xactmin)
			put (item + xitem);
		}
		
		build_action_states (xstate /* passe par valeur */ );
		
	    }
	    
	    if (current_state_kind & LA)
		is_first_reduce = is_first_reduction (xfsa1);
	    /* Il existe dans l'etat courant au moins un item FIRST_REDUCE */
	    else
		is_first_reduce = false;
	    
	    xstate = 0;
	    xsubstate = 0;

	    for (xitem = origine; xitem < sommet; xitem++) {
		k = item [xitem].index;

		if (ers [k].lispro >= 0) {
		    /* point en position shift */
		    if (current_state_kind & LA)
			put (item + xitem);
		    else {
			lrm.attributes.origine_index = lrm.index = k;
			lrm.attributes.ctxt_no = item [xitem].attributes.ctxt_no;
			lrm.attributes.kind = SHIFT_KIND;
			/* Shift */
			put (&lrm);
		    }
		}
		else if (ers [k].lispro < xactmin) {
		    /* marqueur lr sur une specification de action */
		    if (current_state_kind & LA)
			put_item_in_substate (k, item [xitem].attributes.origine_index,
					      item [xitem].attributes.ctxt_no,
					      item [xitem].attributes.kind);
			/* Cette instruction doit etre inutile, en look_ahead aucun
			   marqueur ne reference d'action */
		    else
			put_item_in_substate (k, k, item [xitem].attributes.ctxt_no, ACTION_KIND);
		}
		else
		     /* il y a au moins un point en position reduce */
		    if (current_state_kind & LA) {
			lrm = item [xitem];

			if (lrm.attributes.kind != FIRST_REDUCE) {
			    if (is_first_reduce)
				lrm.attributes.kind = NEXT_REDUCE;
			    else
				lrm.attributes.kind = FIRST_REDUCE;
			}
			filsubstate (&lrm);
		    }
		    else {
			lrm.attributes.origine_index = lrm.index = k;
			lrm.attributes.ctxt_no = item [xitem].attributes.ctxt_no;
			lrm.attributes.kind = FIRST_REDUCE;
			filsubstate (&lrm);
		    }
	    }
	    
	    if (xsubstate > 0)
		subsuiv (false, true, is_first_reduce);
	    
	    
	    /* state est rempli avec l'etat courant, aucun marqueur ne refere une action 
	       Il n'est pas trie et il peut y avoir des doublons */
	    
	    items_no = xstate;
	    
	    if (items_no > max_items_no) {
		liberer ();
		/* reallocation des max_items_no */
		max_items_no = items_no;
		allouer ();
	    }
	    
	    
/*
  P R I N C I P E S   D E   L A   D E T E R M I N A T I O N  
  D E S
  L O O K _ A H E A D
  
  NON-DETERMINISME :
  Principe: Les predicats doivent pouvoir s'executer tout le temps (analyse
  normale, erreur, look-ahead), l'utilisateur etant prevenu de ce mode.
  La consequence est que les actions dont ils peuvent dependre doivent etre
  dans tous les cas executees avant.
  Une action pour s'executer ne doit pas avoir besoin de look-ahead
  (apres une action utilisateur, le ctxt peut ne pas etre celui que l'on
  croit, l'action ayant pu lire du texte source)
  Dans le cas general, une action forme donc une barriere impermeable a la
  connaissance de son contexte droit.
  Si une E.R. commence par une action, les E.R. qui peuvent la preceder doivent
  pouvoir se reconnaitre sans look-ahead
  On veut de plus que si une action @1 precede un predicat &1 (Ex @1 t &1),
  &1 peut dependre de @1.
  Pour y parvenir si un conflit implique (meme par fermeture) une action
  le resoudre par du non-determinisme.
  Les actions en look-ahead (meme celles qui ne demandent pas de contexte)
  vont donc produire la "resolution" du conflit initial par non-determinisme.


  CAS NORMAL :
  _ La presence d'action dans une ER ne doit pas a priori modifier le resultat
  de l'analyse
  
  - Un item SHIFT_KIND ou ACTION_KIND garde son type jusqu'a la fin de
  l'expression reguliere ou il se change alors en item de type FIRST_REDUCE
  s'il n'existe pas deja d'item de type FIRST_REDUCE dans le meme etat sinon
  il se change en item de type NEXT_REDUCE.
  Ils ne peuvent entrer en conflit  sous leur forme initiale qu'avec un item
  de type reduce.
  On peut retrouver leur type d'origine grace au champ origine_index
  Il ne peut y avoir conflit que s'il y a au moins un item reduce (vrai ou
  deguise).   De facon interne il n'y a donc pas de conflit Shift/Action
  , Action/Action il n'y a que des conflits Shift-Action-Reduce/Reduce
  
  
  _ Le mode de resolution d'un conflit est donne uniquement par les items de
  type reduce (FIRST_REDUCE U NEXT_REDUCE) y intervenant:
  un conflit est UNBOUNDED ssi tous les items reduce sont de type unbounded
  (ER suivie dans la grammaire d'une specif "Unbounded Context ....")
  sinon il est ONELA. Le mode de resolution d'un conflit peut donc passer
  de UNBOUNDED a ONELA lorsqu'un item shift ou action est promu en reduce
  
  - S'il y a un conflit on essaie:
  les priorites de l'utilisateur puis les regles systeme
  
  SIGNIFICATION des CLAUSES "PRIORITY" de LECL
  Reduce>Reduce  En cas de conflit (1la ou unbounded), on supprime l'ensemble
  des items de type FIRST_REDUCE dont l'ER d'origine n'a
  pas la clause Reduce>Reduce.
  
  Reduce>Shift   En cas de conflit (1la ou unbounded), si l'ensemble 
  FIRST_REDUCE est non vide, on supprime les items de
  SHIFT_KIND U ACTION_KIND U NEXT_REDUCE
  
  Shift>Reduce   En cas de conflit (1la ou unbounded), si l'ensemble 
  SHIFT_KIND U ACTION_KIND U NEXT_REDUCE est non vide, on
  supprime les items de FIRST_REDUCE qui ont la clause 
  Shift>Reduce.
  
  Si FIRST_REDUCE est vide (unbounded) alors les items de NEXT_REDUCE sont
  promus en FIRST_REDUCE.
  
  Si apres application des regles de l'utilisateur il reste des conflits, on
  applique les regles systeme
  
  REGLES SYSTEME (BUILTIN)
  _ Lorsqu'il est possible de le determiner,on donne priorite a la transition
  (Shift, Reduce, Action) qui assure la reconnaissance de la chaine la plus
  longue a l'interieur d'une meme ER. 
  Sinon, les priorites sont appliquees dans l'ordre suivant par priorite
  decroissante:
  1) Shift pur
  2) Action pure (d'index min)
  3) Shift deguise
  4) Action deguisee (d'index min)
  5) Reduce d'index min
*/
	    
/*
  Sur une detection de conflit 1la, au moins une des occurrences du
  look_ahead se trouve en debut d'ER.
  */
	    
	    for (i = 1; i <= items_no; i++) {
		to_be_sorted [i] = i;
	    }
	    
	    if (items_no > 1)
		sort_by_tree (to_be_sorted, 1, items_no, less);
	    
	    xcurrent_state = 0;
	    
	    for (i = 1; i <= items_no; i++) {
		lr_marker_ptr = state + to_be_sorted [i];
		
		if (i > 1)
		    if (eq_lr_marker (current_state + xcurrent_state, lr_marker_ptr))
			continue;
		
		current_state [++xcurrent_state] = *lr_marker_ptr;
	    }
	    
	    
	    /* L'etat courant est range dans current_state entre 1 et xcurrent_state */
	    
	    fsa [xfsa1 + 1].transition_ref = xfsa_t;
	    /* Pour get_action_state eventuel */

/* on cree l'ensemble esc_set des classes simples etendues valides pour
   l'etat courant et l'ensemble des conflits */

	    for (x = 1; x <= last_esc; x++)
		sxba_empty (esc_to_xcurrent_state [x]);

	    sxba_empty (esc_set);
	    sxba_empty (has_prdct);
	    has_predicate = false;
	    sxba_empty (has_true);

	    for (x = 1; x <= smax; x++)
		sxba_empty (sc_to_prdct [x]);

	    for (x = 1; x <= xcurrent_state; x++) {
		j = current_state [x].index;
		k = ers [j].lispro;

		if (k > 0)
		    if (ers [j].prdct_no == prdct_true_code)
			sxba_or (has_true, kod [k]);
	    }

	    {
		SXBA	lecl_fsa_construction_class_set;

		for (x = 1; x <= xcurrent_state; x++) {
		    j = current_state [x].index;
		    k = ers [j].lispro;

		    if (k > 0) {
			predicate_no = ers [j].prdct_no;

			if (predicate_no != prdct_false_code) {
			    lecl_fsa_construction_class_set = kod [k];
			    k = 0;

			    while ((k = sxba_scan (lecl_fsa_construction_class_set, k)) > 0) {
				if (!sxba_bit_is_set (has_true, k)
				    || predicate_no == prdct_true_code) {
				    if (predicate_no <= prdct_true_code) {
					esc = k;
					SXBA_1_bit (sc_to_prdct [k],
						    prdct_true_code /* Eh oui */ );
				    }
				    else {
					esc = sc_prdct_to_esc [k] [predicate_no];
					SXBA_1_bit (has_prdct, k);
					has_predicate = true;
					SXBA_1_bit (sc_to_prdct [k], predicate_no);
				    }

				    SXBA_1_bit (esc_to_xcurrent_state [esc], x);
				    SXBA_1_bit (esc_set, esc);
				}
			    }
			}
		    }
		}
	    }

	    xfsa_t2 = xfsa_t;


/* apres les actions */
	    esc = 0;

	    while ((esc = sxba_scan (esc_set, esc)) > 0) {
		SXBA_0_bit (esc_set, esc);
		sxba_copy (xcurrent_state_set, esc_to_xcurrent_state [esc]);
		sxba_empty (esc1_set);
		SXBA_1_bit (esc1_set, esc);
		sxba_empty (sc_set);
		esc1 = esc_to_sc_prdct [esc].simple_class;
		SXBA_1_bit (sc_set, esc1);
		esc1 = esc;

		while ((esc1 = sxba_scan (esc_set, esc1)) > 0) {
		    if (sxba_first_difference (esc_to_xcurrent_state [esc1],
					       xcurrent_state_set) == -1) {
			SXINT lecl_fsa_construction_sc;

			SXBA_1_bit (esc1_set, esc1);
			SXBA_0_bit (esc_set, esc1);
			lecl_fsa_construction_sc = esc_to_sc_prdct [esc1].simple_class;
			SXBA_1_bit (sc_set, lecl_fsa_construction_sc);
		    }
		}


/* esc1_set contient des esc dont on est sur que:
   chaque transition reference le meme etat */

		sxba_empty (is_promoted);
		sxba_empty (is_new_ctxt);


/* Il est impossible de modifier current_state pendant la resolution des
   conflits, chaque item pouvant etre "partage" entre plusieurs sous-ensembles */

		is_nd_trans = false;

		if (current_state_kind & (MIXTE + REDUCE))
		    if (is_conflict (xcurrent_state_set, &conflict_mode, &prio)) {
			if (conflict_mode == ONELA_CONFLICT)
			    is_nd_trans = conflict_mngr (xcurrent_state_set,
							    conflict_mode, prio);
			else {
			    is_nd_trans = unbounded_conflict (xcurrent_state_set, prio);
			    promote (xcurrent_state_set);
			}
		    }

		if (is_nd_trans) {
		    /* Non-determinisme : generation independante. */
		    bool	doit, lecl_fsa_construction_is_first;
		    SXINT		orig, class, lecl_fsa_construction_ner, cur_sc, cur_prdct;
		    
		    if (esc <= smax) {
			cur_sc = esc;
			cur_prdct = prdct_true_code;
		    }
		    else {
			cur_sc = esc_to_sc_prdct [esc].simple_class;
			cur_prdct = esc_to_sc_prdct [esc].predicate;
		    }
		    
		    next_state = 0;
		    
		    if (current_state_kind & LA) {
			next_state = fsa [xfsa1].shift_state;
			bdel = fsa [xfsa1].del;
			scan = true;
			next_state_kind = 0;
		    }
		    else if ((current_state_kind & SHIFT)) {
			/* etat principal ayant du shift */
			/* on calcule l'etat obtenu (next_state) par transition sur shift */
			xsubstate = 0;
			x = 0;
			
			while ((x = sxba_scan (xcurrent_state_set, x)) > 0) {
			    if (current_state [x].attributes.kind == SHIFT_KIND) {
				SXBA_0_bit (xcurrent_state_set, x);
				put_item_in_substate (current_state [x].index,
						      (SXINT)0,
						      sxba_bit_is_set (is_new_ctxt, x)
						      ? new_ctxt [x]
						      : current_state [x].attributes.ctxt_no,
						      UNDEF_KIND);
			    }
			}
			
			/* il y a au moins un point shift en position shift */
			xstate = 0;
			bdel = get_del (sc_set);
			scan = true;
			subsuiv (true, false, false);
			/* Si xstate==0, on a un etat vide (utilisation de &False),
			   qui sera traite lors de la generation de code */
			xitem = xitem2;
			build_state (&xitem2, &hsh, &next_state_kind, &lecl_fsa_construction_ner, false);
			/* on a forcement un etat principal */
			
			if (!is_state_already_defined ((SXINT)0, (SXINT)0, bdel, scan, &next_state, hsh))
			    set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, bdel, scan, next_state_kind);
		    }
		    
		    if (next_state != 0)
			set_sc_prdct_to_next (cur_sc, cur_prdct);
		    
		    while ((x = sxba_scan (xcurrent_state_set, 0)) > 0) {
			lecl_fsa_construction_is_first = true;
			next_state = 0;
			
			do {
			    orig = current_state [x].attributes.origine_index;
			    class = ers [orig].lispro;
			    
			    if (class >= 0)
				/* Shift (en look-ahead) already processed */
				doit = true;
			    else if (class <= xactmin) {
				if (current_state_kind & LA)
				    lecl_fsa_construction_ner = get_action_state (fsa [xfsa1].seed_state, class);
				else
				    lecl_fsa_construction_ner = get_action_state (xfsa1, class);

				if  (lecl_fsa_construction_is_first) {
				    lecl_fsa_construction_is_first = false;
				    doit = true;
				    next_state = lecl_fsa_construction_ner;
				}
				else
				    doit = lecl_fsa_construction_ner == next_state;
			    }
			    else {
				if (lecl_fsa_construction_is_first) {
				    lecl_fsa_construction_is_first = false;
				    lecl_fsa_construction_ner = orig;
				    next_state = -ers [lecl_fsa_construction_ner - 1].prolis;
				    doit = true;
				}
				else
				    doit = lecl_fsa_construction_ner == orig;
			    }

			    if (doit)
				SXBA_0_bit (xcurrent_state_set, x);

			} while ((x = sxba_scan (xcurrent_state_set, x)) > 0); 
			
			if (next_state != 0) {
			    bdel = false;
			    scan = false;
			    next_state_kind = 0;
			    set_sc_prdct_to_next (cur_sc, cur_prdct);
			}
		    }

		    /* On genere le non determinisme immediatement s'il n'y a aucun predicat
		       dans le coup, sinon l'epilogue s'en chargera... */
		    p = &(next_state_no_list [sc_prdct_to_next [cur_sc] [cur_prdct]]);
		    next_state = create_nd_state (p);
		    bdel = false;
		    scan = false;

		    if (SXBA_bit_is_set (has_prdct, cur_sc)) {
			p->state_no = next_state;
			p->is_del = bdel;
			p->is_scan = scan;
			p->lnk = 0;

			esc1 = esc;

			while ((esc1 = sxba_scan (esc1_set, esc1)) > 0) {
			    if (esc1 <= smax) {
				cur_sc = esc1;
				cur_prdct = prdct_true_code;
			    }
			    else {
				cur_sc = esc_to_sc_prdct [esc1].simple_class;
				cur_prdct = esc_to_sc_prdct [esc1].predicate;
			    }
		    
			    set_sc_prdct_to_next (cur_sc, cur_prdct);
			}
		    }
		    else {
			sc_prdct_to_next [cur_sc] [cur_prdct] = 0;
			/* sc_set is OK */
			maj_trans ();
		    }
		}
		else {
		    next_state = 0;
		    bdel = false;
		    scan = false;
		    
		    if ((current_state_kind & SHIFT) && !(current_state_kind & LA)) {
			/* etat principal shift ou shift_reduce */
			/* on calcule l'etat obtenu (next_state) par transition sur shift */
			xsubstate = 0;
			x = 0;
			
			while ((x = sxba_scan (xcurrent_state_set, x)) > 0) {
			    if (current_state [x].attributes.kind == SHIFT_KIND) {
				put_item_in_substate (current_state [x].index,
						      (SXINT)0,
						      sxba_bit_is_set (is_new_ctxt, x)
						      ? new_ctxt [x]
						      : current_state [x].attributes.ctxt_no,
						      UNDEF_KIND);
			    }
			}
			
			if (xsubstate > 0) {
			    /* il y a au moins un point shift en position shift */
			    xstate = 0;
			    bdel = get_del (sc_set);
			    scan = true;
			    subsuiv (true, false, false);
			    /* Si xstate==0, on a un etat vide (utilisation de &False),
			       qui sera traite lors de la generation de code */
			    xitem = xitem2;
			    build_state (&xitem2, &hsh, &next_state_kind, &ner, false);
			    
			    
			    /* on a forcement un etat principal */
			    
			    if (!is_state_already_defined ((SXINT)0, (SXINT)0, bdel, scan, &next_state, hsh))
				set_new_state ((SXINT)0, (SXINT)0, (SXINT)0, bdel, scan, next_state_kind);
			}
		    }
		    
		    if (current_state_kind & (MIXTE + REDUCE)) {
			xsubstate = 0;
			x = 0;
			
			while ((x = sxba_scan (xcurrent_state_set, x)) > 0) {
			    put_item_in_substate (current_state [x].index,
						  current_state [x].attributes.origine_index,
						  sxba_bit_is_set (is_new_ctxt, x)
						  ? new_ctxt [x]
						  : current_state [x].attributes.ctxt_no,
						  sxba_bit_is_set (is_promoted, x)
						  ? FIRST_REDUCE
						  : current_state [x].attributes.kind);
			}
			
			xstate = 0;
			subsuiv (is_non_deterministic_automaton, false, false);
			/* en look_ahead aucun marqueur lr ne reference d'action
			 sauf ds le cas non-deterministe */
			xitem = xitem2;
			build_state (&xitem2, &hsh, &next_state_kind, &ner, true);
			
			if (!(next_state_kind & MIXTE)) {
			    xitem2 = xitem;
			    
			    
			    /* l'etat est supprime */
			    
			    if (next_state_kind & SHIFT) {
				if (current_state_kind & LA) {
				    next_state = fsa [xfsa1].shift_state;
				    bdel = fsa [xfsa1].del;
				    scan = true;
				}
				/* else next_state, scan et bdel viennent d'etre positionnes */
			    }
			    else if (next_state_kind & ACT) {
				if (current_state_kind & LA)
				    next_state = get_action_state (fsa [xfsa1].seed_state, ner);
				else
				    next_state = get_action_state (xfsa1, ner);
			    }
			    else
				/* Reduce (prdct est impossible) */
				next_state = -ner;
			    
			    next_state_kind = 0;
			}
			else {
			    if (next_state_kind & LA)
				/* toujours vrai ? */
				if (current_state_kind & LA)
				    seed_state = fsa [xfsa1].seed_state;
				else
				    seed_state = xfsa1;
			    else
				seed_state = 0;
			    
			    if (current_state_kind & LA) {
				shift_state = fsa [xfsa1].shift_state;
				bdel = fsa [xfsa1].del;
				scan = fsa [xfsa1].scan;
			    }
			    else
				shift_state = next_state;
			    
			    if (!is_state_already_defined (seed_state, (SXINT)0, bdel,
							   scan, &next_state, hsh))
				set_new_state (seed_state, shift_state, (SXINT)0, bdel,
					       scan, next_state_kind);
			}
		    }
		    
		    complete_trans ();
		}
	    }
	    
	    
	    /* esc ^= 0  */
	    
	    if (has_predicate) {
		sc = 0;
		
		while ((sc = sxba_scan (has_prdct, sc)) > 0) {
		    xsubstate = 0;
		    else_state = 0;
		    sxba_empty (sc_set);
		    SXBA_1_bit (sc_set, sc);
		    bdel = false;
		    scan = false;
		    prdct_set = sc_to_prdct [sc];
		    /* On tente de detecter a priori les transitions vers le meme etat */
		    sc1 = sc;

		    while ((sc1 = sxba_scan (has_prdct, sc1)) > 0) {
			if (sxba_first_difference (sc_to_prdct [sc1], prdct_set) == -1) {
			    /* compatible predicats */
			    /* on verifie la compatibilite next_state */
			    is_the_same_trans = true;
			    predicate_no = 0;

			    while (is_the_same_trans
				   && (predicate_no = sxba_scan (prdct_set, predicate_no)) > 0) {
				p1 = &(next_state_no_list [sc_prdct_to_next [sc1] [predicate_no]]);
				p = &(next_state_no_list [sc_prdct_to_next [sc] [predicate_no]]);

				if (p->state_no != p1->state_no
				    || p->is_del != p1->is_del
				    || p->is_scan != p1->is_scan)
				    is_the_same_trans = false;
			    }

			    if (is_the_same_trans) {
				SXBA_0_bit (has_prdct, sc1);
				SXBA_1_bit (sc_set, sc1);
			    }
			}
		    }

		    process_predicates = true;

		    if (is_optimize) {
			is_first = true;
			is_the_same_trans = true;
			predicate_no = 0;

			while (is_the_same_trans
			       && (predicate_no = sxba_scan (prdct_set, predicate_no)) > 0) {
			    p = &(next_state_no_list [sc_prdct_to_next [sc] [predicate_no]]);

			    if (is_first) {
			      is_first = false;
			      next_state = p->state_no;
			      bdel = p->is_del;
			      scan = p->is_scan;

			      /* Changement le 4/5/93, je n'y comprend plus rien...
				 if (next_state >= 0)
				 if ((current_state_kind & ACT)
				 && (fsa [next_state].state_kind & ACT)
				 && !(fsa [next_state].state_kind & MIXTE)
				 && is_act_red (sc1_set, sc_set, next_state))
				 ;
				 else
				 is_the_same_trans = false;
			      */
			      /* Re_Changement le 3/11/03, je n'y comprend toujours pas grand chose, mais je remets comme avant
				 car le test du predicat p ds "a"&p s'il n'y a aucun probleme (pas d'autre "a" en //) disparait !!  
			      */
			      if (next_state >= 0) {
				if ((current_state_kind & ACT)
				    && (fsa [next_state].state_kind & ACT)
				    && !(fsa [next_state].state_kind & MIXTE)
				    && is_act_red (sc1_set, sc_set, next_state))
				  ;
				else
				  is_the_same_trans = false;
			      }
			    }
			    else if (next_state != p->state_no
				     || bdel != p->is_del
				     || scan != p->is_scan)
				is_the_same_trans = false;
			}

			if (is_the_same_trans) {

/* Changement le 4/5/93, je n'y comprend plus rien...
   On est en optimize. Les predicats ne sont pas testes dans le cas suivant:
       - Ils conduisent tous a un reduce vers la meme unite lexicale.
       - Ils conduisent tous a l'execution de la meme action qui est toujours
	 suivie par un reduce (pas forcement le meme)
*/

/* On est en optimize. Les predicats ne sont pas testes s'ils conduisent
   tous a des comportements identiques. */
			    process_predicates = false;
			    maj_trans ();

			    predicate_no = 0;

			    while ((predicate_no = sxba_scan (prdct_set, predicate_no)) > 0)
				sc_prdct_to_next [sc] [predicate_no] = 0;
			}
		    }

		    if (process_predicates) {
			predicate_no = 0;

			while ((predicate_no = sxba_scan (prdct_set, predicate_no)) > 0) {
			    p = &(next_state_no_list [sc_prdct_to_next [sc] [predicate_no]]);
			    sc_prdct_to_next [sc] [predicate_no] = 0;

			    if (predicate_no == prdct_true_code) {
				else_state = p->state_no;
				bdel = p->is_del;
				scan = p->is_scan;
			    }
			    else
				put_item_in_substate
				    (prdct_to_ers [predicate_no],
				     p->state_no,
				     (SXINT)(p->is_scan ? 1 : 0),
				     p->is_del ? 2 : 0);
			}

			xstate = 0;
			subsuiv (true, false, false);
			xitem = xitem2;
			build_state (&xitem2, &hsh, &next_state_kind, &ner, false);

			if (current_state_kind & LA)
			    seed_state = fsa [xfsa1].seed_state;
			else
			    seed_state = xfsa1;

			next_state_kind = current_state_kind & LA ? LA + PRDCT : PRDCT;

			if (!is_state_already_defined (seed_state, else_state, bdel,
						       scan, &next_state, hsh))
			    set_new_state (seed_state, (SXINT)0, else_state, bdel,
					   scan, next_state_kind);

			bdel = false;
			scan = false;
			maj_trans ();
		    }
		}

		clear_sc_prdct_to_next ();
	    }

	    fsa [xfsa1 + 1].transition_ref = xfsa_t;
	} /* cas general */
    }


/* xfsa1<xfsa2 */

    if (u_head_ptr != NULL || m_head_ptr != NULL) {
	lecl_diagnoses_unbounded_output (u_head_ptr, m_head_ptr);
    }

    liberer ();

    varstr_free (sc_name);
    sxfree (next_state_no_list);

    {
	SXINT	lecl_fsa_construction_i;

	for (lecl_fsa_construction_i = smax; lecl_fsa_construction_i > 0; lecl_fsa_construction_i--)
	    sxfree ((sc_prdct_to_next [lecl_fsa_construction_i]));
    }

    sxfree (sc_prdct_to_next);
    sxbm_free (sc_to_prdct);
    sxfree (has_true);
    sxfree (sc_set);
    sxfree (has_prdct);
    sxfree (esc1_set);
    sxfree (esc_set);
    sxfree (sc1_set);
    sxfree (and_set);
    sxfree (ctxt_set);
}
