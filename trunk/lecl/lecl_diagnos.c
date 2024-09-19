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

/*   D I A G N O S E S  */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"
#include "lecl_ag.h"

char WHAT_LECLDIAGNOS[] = "@(#)SYNTAX - $Id: lecl_diagnos.c 4295 2024-09-14 14:28:48Z garavel $" WHAT_DEBUG;

extern VARSTR	lecl_gen_ecc (SXBA *SC_TO_CHAR_SET, 
			      SXBA esc_set, 
			      SXINT max_prdct_no, 
			      VARSTR varstr_ptr);
extern VARSTR	lecl_gen_cc (SXBA *SC_TO_CHAR_SET, 
			     SXBA esc_set, 
			     VARSTR varstr_ptr);
extern VARSTR	lecl_tree_to_string (VARSTR vs_ptr, 
				     struct lecl_node *visited, 
				     SXINT x, 
				     char *MARK);
static char	*names [6] = {" ", "Shift", "Action", "Action", "Reduce", "Reduce"};

#define marge_lgth 60

static char	marge [marge_lgth + 1] = "\n                                                           ";
static struct lr_marker		*current_state;
static VARSTR	avarstr;
static SXINT	origs_no;

struct item {
    SXINT		origine, index;
};
static struct item	*items;



static char	*get_origine_kind (SXINT get_origine_kind_x)
{
    SXINT	get_origine_kind_k;

    get_origine_kind_k = ers [get_origine_kind_x].lispro;
    return names [get_origine_kind_k >= 0 ? 1 : (get_origine_kind_k <= xactmin ? 2 : 4)];
}



static VARSTR	system_re (VARSTR varstr_ptr, 
			   SXINT xl, 
			   SXINT lim, 
			   SXINT mark_index, 
			   char *mark_char)
{
    for (; xl <= lim; xl++) {
	if (ers [xl].is_erased)
	    varstr_catenate (varstr_ptr, "-");

	if (xl == mark_index)
	    varstr_catenate (varstr_ptr, mark_char);

	varstr_cat_cstring (varstr_ptr, sxstrget (abbrev_or_class_to_ate [ers [xl].lispro]));
	varstr_catenate (varstr_ptr, " ");
    }

    if (xl == mark_index)
	varstr_catenate (varstr_ptr, mark_char);

    return varstr_ptr;
}



static VARSTR	cat_re_name (VARSTR varstr_ptr, SXINT ner)
{
    SXINT		mner, red;

    mner = ers_disp [ner].master_token;
    red = ers_disp [ner].reduc;

    if (red < 0)
	red = -red;

    if (mner == ner)
	return varstr_catenate (varstr_ptr, sxstrget (red));
    else {
	SXINT	ate;

	ate = ers_disp [mner].reduc;

	if (ate < 0)
	    ate = -ate;

	return varstr_catenate (varstr_catenate (varstr_catenate (varstr_ptr, sxstrget (ate)), "."), sxstrget (red));
    }
}



static VARSTR	build_re_string (VARSTR varstr_ptr, SXINT mark_index, char *mark_char, SXINT hd/* , tl */)
{
    SXINT		re_no;

    if (ers [mark_index].lispro == -1 || ers [mark_index].lispro == -2)
	re_no = ers [mark_index - 1].prolis;
    else
	re_no = ers [mark_index].prolis;

    varstr_catenate (cat_re_name (varstr_catenate (varstr_ptr, marge + marge_lgth - hd), re_no), " = ");

    if (ers_disp [re_no].subtree_ptr != NULL)
	lecl_tree_to_string (varstr_ptr, ers_disp [re_no].subtree_ptr, mark_index - ers_disp [re_no].prolon, mark_char);
    else
	system_re (varstr_ptr, ers_disp [re_no].prolon + 1, ers_disp [re_no + 1].prolon - 1, mark_index, mark_char);

    return varstr_catenate (varstr_ptr, " ;");
}



static VARSTR	cat_conflict_name (VARSTR varstr_ptr, char kind /* bit (5) */)
{
    SXINT		cat_conflict_name_i, cat_conflict_name_x;
    bool	is_first = true;

    for (cat_conflict_name_x = 1, cat_conflict_name_i = 1; cat_conflict_name_i < 6; cat_conflict_name_x <<= 1, cat_conflict_name_i++)
	if (kind & cat_conflict_name_x) {
	    if (is_first)
		is_first = false;
	    else
		varstr_catenate (varstr_ptr, "/");

	    varstr_catenate (varstr_ptr, names [cat_conflict_name_i]);
	}

    return varstr_ptr;
}



static void given_priority (VARSTR varstr_ptr, SXINT choice)
{
    SXINT		given_priority_k;

    if (is_non_deterministic_automaton)
	varstr_catenate (varstr_ptr, "The set of possibilities is kept");
    else {
	varstr_catenate (varstr_ptr, "Priority is given to ");
	given_priority_k = ers [choice].lispro;
	
	if (given_priority_k >= 0)
	    varstr_catenate (varstr_ptr, names [1]);
	else if (given_priority_k <= xactmin)
	    varstr_catenate (varstr_catenate (varstr_ptr, "the action "), sxstrget (action_or_prdct_to_ate [given_priority_k]));
	else
	    cat_re_name (varstr_catenate (varstr_ptr, "the reduction "), ers [choice - 1].prolis);
    }

    sxtmsg (sxsrcmngr.source_coord.file_name, "%s%s.", err_titles [1]+1, varstr_tostr (varstr_ptr));
}



static VARSTR	detection_mess (VARSTR varstr_ptr, SXINT n, struct detection_record *p)
{
    struct detection_record	*q;

    for (q = p; q != NULL; q = q->next) {
	if (q->index > 0)
	    break;
    }

    if (q != NULL) {
	/* Au moins 1 non nul */
	varstr_catenate (varstr_catenate (varstr_ptr, marge + marge_lgth - n), "detected on :\n");
	
	for (; p != NULL; p = p->next) {
	    if (p->index > 0)
	      varstr_catenate (build_re_string (varstr_ptr, p->index, "|\b^", (SXINT)11/* , 79 */), "\n");
	}
    }

    return varstr_ptr;
}



static bool	less_origine_index (SXINT less_origine_index_i, SXINT less_origine_index_j)
{
    return current_state [less_origine_index_i].attributes.origine_index < current_state [less_origine_index_j].attributes.origine_index;
}



static void put (struct conflict_record *conflict_record_ptr, SXINT put_i, SXINT put_j)
{
    /* prend le point de detection repere par l'index put_i de to_be_sorted et le
       range dans detection_items [put_j].detection_ptr */
    struct detection_record	*detection_record_ptr, *p;
    SXINT		d_point;

    d_point = current_state [put_i].index;
    p = NULL;

    for (detection_record_ptr = conflict_record_ptr->detection_items [put_j].detection_ptr;
	 detection_record_ptr;
	 detection_record_ptr = detection_record_ptr->next) {
	if (detection_record_ptr->index == d_point)
	    return;

	p = detection_record_ptr;

	if (d_point < detection_record_ptr->index) {
	    detection_record_ptr = (struct detection_record*) sxcalloc (1, sizeof (struct detection_record));
	    *detection_record_ptr = *p;
	    p->next = detection_record_ptr;
	    p->index = d_point;
	    return;
	}
    }

    detection_record_ptr = (struct detection_record*) sxcalloc (1, sizeof (struct detection_record));
    detection_record_ptr->index = d_point;
    detection_record_ptr->next = NULL;

    if (p == NULL)
	conflict_record_ptr->detection_items [put_j].detection_ptr = detection_record_ptr;
    else
	p->next = detection_record_ptr;
}



static void unbounded_or_mixte (struct lr_marker *current_state_ptr, 
				SXBA sra_set, 
				SXINT seed_state, 
				SXINT detection_state,
				char conflict_kind /* bit (5) */, 
				SXINT choice, 
				struct conflict_record **head_ptr, 
				bool is_unbounded)
{
    SXINT		*to_be_sorted /* 1:n */ ;
    SXINT		n, unbounded_or_mixte_x, unbounded_or_mixte_i, orig1, orig2;
    struct conflict_record	*current_conflict_record_ptr, *conflict_record_ptr;

    current_state = current_state_ptr;
    n = sxba_cardinal (sra_set);
    to_be_sorted = (SXINT*) sxalloc ((SXINT) (n + 1), sizeof (SXINT));
    unbounded_or_mixte_i = 0;
    unbounded_or_mixte_x = 0;

    while ((unbounded_or_mixte_x = sxba_scan (sra_set, unbounded_or_mixte_x)) > 0)
	to_be_sorted [++unbounded_or_mixte_i] = unbounded_or_mixte_x;

    sort_by_tree (to_be_sorted, 1, n, less_origine_index);
    origs_no = 1;
    orig1 = current_state [to_be_sorted [1]].attributes.origine_index;

    for (unbounded_or_mixte_x = 2; unbounded_or_mixte_x <= n; unbounded_or_mixte_x++) {
	orig2 = current_state [to_be_sorted [unbounded_or_mixte_x]].attributes.origine_index;

	if (orig2 != orig1) {
	    orig1 = orig2;
	    origs_no++;
	}
    }

    current_conflict_record_ptr = NULL;

    for (conflict_record_ptr = *head_ptr;
	 conflict_record_ptr;
	 conflict_record_ptr = conflict_record_ptr->lnk) {
	if (conflict_record_ptr->seed_state == seed_state
	    && conflict_record_ptr->detection_state == detection_state
	    && conflict_record_ptr->choice == choice
	    && conflict_record_ptr->kind == conflict_kind
	    && conflict_record_ptr->origs_number == origs_no) {
	    unbounded_or_mixte_i = 1;
	    orig1 = current_state [to_be_sorted [1]].attributes.origine_index;

	    if (conflict_record_ptr->detection_items [1].orig == orig1) {
		for (unbounded_or_mixte_x = 2; unbounded_or_mixte_x <= n; unbounded_or_mixte_x++) {
		    orig2 = current_state [to_be_sorted [unbounded_or_mixte_x]].attributes.origine_index;

		    if (orig2 != orig1) {
			unbounded_or_mixte_i++;

			if (conflict_record_ptr->detection_items [unbounded_or_mixte_i].orig != orig2)
			    goto next_conflict;

			orig1 = orig2;
		    }
		}

		goto fill_detections;
	    }
	}

next_conflict:
	current_conflict_record_ptr = conflict_record_ptr;
    }

    conflict_record_ptr = (struct conflict_record*) sxalloc (1, sizeof (struct conflict_record));
    conflict_record_ptr->origs_number = origs_no;
    conflict_record_ptr->detection_items =
	(struct detection_item*) sxalloc ((SXINT) (origs_no + 1), sizeof (struct detection_item));

    if (current_conflict_record_ptr != NULL)
	current_conflict_record_ptr->lnk = conflict_record_ptr;
    else
      *head_ptr = conflict_record_ptr;

    conflict_record_ptr->lnk = NULL;
    conflict_record_ptr->seed_state = seed_state;
    conflict_record_ptr->detection_state = detection_state;
    conflict_record_ptr->choice = choice;
    conflict_record_ptr->kind = conflict_kind;
    unbounded_or_mixte_i = 1;
    conflict_record_ptr->detection_items [1].orig = orig1 =
	current_state [to_be_sorted [1]].attributes.origine_index;
    conflict_record_ptr->detection_items [1].detection_ptr = NULL;

    for (unbounded_or_mixte_x = 2; unbounded_or_mixte_x <= n; unbounded_or_mixte_x++) {
	orig2 = current_state [to_be_sorted [unbounded_or_mixte_x]].attributes.origine_index;

	if (orig2 != orig1) {
	    unbounded_or_mixte_i++;
	    conflict_record_ptr->detection_items [unbounded_or_mixte_i].orig = orig2;
	    conflict_record_ptr->detection_items [unbounded_or_mixte_i].detection_ptr = NULL;
	    orig1 = orig2;
	}
    }

fill_detections:
    unbounded_or_mixte_i = 1;
    orig1 = current_state [to_be_sorted [1]].attributes.origine_index;
    put (conflict_record_ptr, to_be_sorted [1], (SXINT)1);

    for (unbounded_or_mixte_x = 2; unbounded_or_mixte_x <= n; unbounded_or_mixte_x++) {
	orig2 = current_state [to_be_sorted [unbounded_or_mixte_x]].attributes.origine_index;

	if (orig2 != orig1) {
	    unbounded_or_mixte_i++;
	    orig1 = orig2;
	}

	put (conflict_record_ptr, to_be_sorted [unbounded_or_mixte_x], is_unbounded ? 1 : unbounded_or_mixte_i);
    }

    sxfree (to_be_sorted);
}



static bool	less_item (SXINT less_item_i, SXINT less_item_j)
{
    struct item	*pi, *pj;

    pi = items + less_item_i;
    pj = items + less_item_j;

    if (pi->origine < pj->origine)
	return true;

    if (pi->origine > pj->origine)
	return false;

    return pi->index < pj->index;
}



static void	immediate_or_one_la (VARSTR varstr_ptr /* ATTENTION : freed !!! */, 
				     struct lr_marker *current_state_ptr, 
				     SXBA sra_set,
				     /* detection_state, conflict_kind, */
				     SXINT choice, 
				     bool with_detection_point)
{
    SXINT		*to_be_sorted /* 1:n */ ;
    SXINT		orig1, orig2, immediate_or_one_la_i, immediate_or_one_la_x, y, n;

    current_state = current_state_ptr;
    n = sxba_cardinal (sra_set);
    to_be_sorted = (SXINT*) sxalloc ((SXINT) (n + 1), sizeof (SXINT));
    items = (struct item*) sxalloc ((SXINT) (n + 1), sizeof (struct item));
    immediate_or_one_la_i = 0;
    immediate_or_one_la_x = 0;

    while ((immediate_or_one_la_x = sxba_scan (sra_set, immediate_or_one_la_x)) > 0) {
	items [++immediate_or_one_la_i].origine = current_state [immediate_or_one_la_x].attributes.origine_index;
	items [immediate_or_one_la_i].index = current_state [immediate_or_one_la_x].index;
	to_be_sorted [immediate_or_one_la_i] = immediate_or_one_la_i;
    }

    sort_by_tree (to_be_sorted, 1, n, less_item);
    orig1 = items [to_be_sorted [1]].origine;

    for (immediate_or_one_la_x = 1; immediate_or_one_la_x <= n; ) {
	varstr_catenate (build_re_string (varstr_catenate (varstr_catenate
							   (varstr_ptr, get_origine_kind (orig1)),
							   " :\n"), orig1, "|\b^", (SXINT)5/* , 79 */), "\n");

	if (ers [orig1].lispro < 0) {
	    if (with_detection_point && items [to_be_sorted [immediate_or_one_la_x]].index > 0)
		varstr_catenate (build_re_string (varstr_catenate
						  (varstr_ptr, "     detected on :\n"),
						  items [to_be_sorted [immediate_or_one_la_x]].index, "|\b^", (SXINT)11/* , 79*/),
				 "\n");

	    for (++immediate_or_one_la_x; immediate_or_one_la_x <= n; immediate_or_one_la_x++) {
		y = to_be_sorted [immediate_or_one_la_x];
		orig2 = items [y].origine;

		if (orig2 == orig1) {
		    if (with_detection_point && items [y].index)
			varstr_catenate (build_re_string
					 (varstr_ptr, items [y].index, "|\b^", (SXINT)11/* , 79 */), "\n");
		}
		else {
		    orig1 = orig2;
		    goto next_orig;
		}
	    }
	}
	else {
	    immediate_or_one_la_x++;

	    if (immediate_or_one_la_x <= n)
		orig1 = items [to_be_sorted [immediate_or_one_la_x]].origine;
	}

next_orig:
	;
    }

    given_priority (varstr_ptr, choice);
    sxfree (items);
    sxfree (to_be_sorted);
    varstr_free (varstr_ptr);
}



static SXINT get_transition_name_refs (SXBA_INDEX get_transition_name_refs_x, SXBA_INDEX y, char **p)
{
    SXINT		get_transition_name_refs_j, ate, class, first_trans, last_trans;
    bool	is_prdct;

    sxinitialise (ate);
    *p = NULL;
    first_trans = fsa [get_transition_name_refs_x].transition_ref;
    last_trans = fsa [get_transition_name_refs_x + 1].transition_ref;
    is_prdct = fsa [get_transition_name_refs_x].state_kind & PRDCT;

    for (get_transition_name_refs_j = first_trans; get_transition_name_refs_j < last_trans; get_transition_name_refs_j++) {
	if (fsa_trans [get_transition_name_refs_j].next_state_no == y) {
	    class = fsa_trans [get_transition_name_refs_j].cc_ref;

	    if (class != 0) {
		if (class > 0 && !is_prdct) {
		    varstr_raz (avarstr);
		    ate = sxstrsave (lecl_gen_cc (sc_to_char_set, compound_classes [class], avarstr)->first);
		}
		else {
		    if (is_prdct && class == prdct_true_code) {
			if (last_trans - first_trans == 2) {
			    for (get_transition_name_refs_j = first_trans; get_transition_name_refs_j < last_trans; get_transition_name_refs_j++) {
				if (fsa_trans [get_transition_name_refs_j].next_state_no != y) {
				    /* on engendre ^&..., sauf si &...=^&... */
				    *p = sxstrget (action_or_prdct_to_ate [fsa_trans [get_transition_name_refs_j].cc_ref]);

				    if (**p == '^') {
					(*p)++;
					return 0;
				    }
				    else {
					varstr_raz (avarstr);
					ate = sxstrsave (varstr_tostr (varstr_catenate (varstr_catenate (avarstr, "^"), *p)));
				    }
				}
			    }
			}
			else {
			    char	*s;


/* cardinalite > 2, on engendre ^(&i|...) */

			    varstr_catenate (varstr_raz (avarstr), "^(");

			    for (get_transition_name_refs_j = first_trans; get_transition_name_refs_j < last_trans; get_transition_name_refs_j++) {
				if (fsa_trans [get_transition_name_refs_j].next_state_no != y) {
				    varstr_catenate (varstr_catenate (avarstr, sxstrget (action_or_prdct_to_ate [
					 fsa_trans [get_transition_name_refs_j].cc_ref])), "|");
				}
			    }

			    *((s = varstr_tostr (avarstr)) + varstr_length (avarstr) - 1) = ')';
			    ate = sxstrsave (s);
			}
		    }
		    else {
			ate = action_or_prdct_to_ate [class];
		    }
		}

		*p = sxstrget (ate);
	    }

	    return 0;
	}
    }
    return 0;
}



bool		has_la_trans (SXBA *M, SXINT has_la_trans_state)
{
    bool	is_la;
    SXINT		has_la_trans_j, has_la_trans_x;
    char	state_kind;

    is_la = false;

    for (has_la_trans_j = fsa [has_la_trans_state].transition_ref; has_la_trans_j < fsa [has_la_trans_state + 1].transition_ref; has_la_trans_j++) {
	has_la_trans_x = fsa_trans [has_la_trans_j].next_state_no;

	if (has_la_trans_x >= 1) {
	    state_kind = fsa [has_la_trans_x].state_kind;

	    if (state_kind & (LA + PRDCT + NONDETER)) {
	    /* if ((state_kind & LA) || ((state_kind & PRDCT) && has_la_trans (M, has_la_trans_x))) {
	       Cette version peut boucler... si boucle en look-ahead ds l'automate. */
		SXBA_1_bit (M [has_la_trans_state], has_la_trans_x);
		is_la = true;
	    }
	}
    }

    return is_la;
}



void	lecl_diagnoses_unbounded (struct lr_marker *current_state_ptr, 
				  SXBA sra_set, 
				  SXINT seed_state, 
				  SXINT detection_state, 
				  char conflict_kind /* bit (5) */, 
				  SXINT choice, 
				  struct conflict_record **u_head_ptr)
{
    unbounded_or_mixte (current_state_ptr, sra_set, seed_state, detection_state, conflict_kind, choice, u_head_ptr, true);
}



void	lecl_diagnoses_mixte (struct lr_marker *current_state_ptr, 
			      SXBA sra_set, 
			      SXINT seed_state, 
			      SXINT detection_state, 
			      char conflict_kind /* bit (5) */, 
			      SXINT choice, 
			      struct conflict_record **m_head_ptr)
{
    unbounded_or_mixte (current_state_ptr, sra_set, seed_state, detection_state, conflict_kind, choice, m_head_ptr, false);
}



void	lecl_diagnoses_unbounded_output (struct conflict_record *u_head_ptr, struct conflict_record *m_head_ptr)
{
    bool	is_unbounded;
    SXINT		lecl_diagnoses_unbounded_output_i, lecl_diagnoses_unbounded_output_x, lecl_diagnoses_unbounded_output_orig;
    char	s1 [8], s2 [8];
    struct conflict_record	*head_ptr, *conflict_record_ptr;
    VARSTR	varstr_ptr;
    SXBA	/* xfsa2 */ *M, *M_plus /* 1:xfsa2 */ ;
    extern VARSTR	fsa_to_re (VARSTR,
				   SXBA *,
				   SXBA *,
				   SXINT,
				   SXINT,
				   SXINT,
				   SXINT (*) (SXBA_INDEX, SXBA_INDEX, char **));

    M = sxbm_calloc (xfsa2 + 1, xfsa2 + 1);
    M_plus = sxbm_calloc (xfsa2 + 1, xfsa2 + 1);


/* On ne conserve que les transitions allant d'un etat mixte vers un etat en
   look_ahead en conservant les etats "PRDCT" intermediaires */

    for (lecl_diagnoses_unbounded_output_i = 1; lecl_diagnoses_unbounded_output_i < xfsa2; lecl_diagnoses_unbounded_output_i++) {
	if (fsa [lecl_diagnoses_unbounded_output_i].state_kind & (MIXTE + LA + PRDCT + NONDETER))
	/* if (fsa [lecl_diagnoses_unbounded_output_i].state_kind & MIXTE) */
	    has_la_trans (M, lecl_diagnoses_unbounded_output_i);
    }

    for (lecl_diagnoses_unbounded_output_i = 1; lecl_diagnoses_unbounded_output_i < xfsa2; lecl_diagnoses_unbounded_output_i++)
	sxba_copy (M_plus [lecl_diagnoses_unbounded_output_i], M [lecl_diagnoses_unbounded_output_i]);

    fermer (M_plus, xfsa2 + 1);
    varstr_ptr = varstr_alloc (2048);
    avarstr = varstr_alloc (64);

    for (lecl_diagnoses_unbounded_output_i = 2; lecl_diagnoses_unbounded_output_i > 0; lecl_diagnoses_unbounded_output_i--) {
	if (lecl_diagnoses_unbounded_output_i == 2) {
	    head_ptr = u_head_ptr;
	    is_unbounded = true;
	}
	else {
	    head_ptr = m_head_ptr;
	    is_unbounded = false;
	}

	for (conflict_record_ptr = head_ptr;
	     conflict_record_ptr != NULL;
	     conflict_record_ptr = conflict_record_ptr->lnk) {
	    varstr_raz (varstr_ptr);

	    if (is_unbounded)
		varstr_catenate (varstr_ptr, "Unbounded ");

	    sprintf (s1, "%-ld", (SXINT) conflict_record_ptr->seed_state);
	    sprintf (s2, "%-ld", (SXINT) conflict_record_ptr->detection_state);
	    varstr_ptr = varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (
		 cat_conflict_name (varstr_ptr, conflict_record_ptr->kind), " conflict in state "), s1),
		 " detected in state "), s2), " by transition on\n     ");
	    varstr_ptr = fsa_to_re (varstr_ptr, M, M_plus, xfsa2, conflict_record_ptr->seed_state, conflict_record_ptr->
		 detection_state, get_transition_name_refs);
	    varstr_ptr = varstr_catenate (varstr_ptr, "\nbetween\n");

	    for (lecl_diagnoses_unbounded_output_x = 1; lecl_diagnoses_unbounded_output_x <= conflict_record_ptr->origs_number; lecl_diagnoses_unbounded_output_x++) {
		lecl_diagnoses_unbounded_output_orig = conflict_record_ptr->detection_items [lecl_diagnoses_unbounded_output_x].orig;
		varstr_ptr = varstr_catenate (build_re_string (varstr_catenate (varstr_catenate (varstr_ptr,
			 get_origine_kind (lecl_diagnoses_unbounded_output_orig)), " :\n     "), lecl_diagnoses_unbounded_output_orig, "|\b^", (SXINT)1/* , 79*/), "\n");

		if (!is_unbounded)
		    detection_mess (varstr_ptr,
				    (SXINT)5,
				    conflict_record_ptr->detection_items [lecl_diagnoses_unbounded_output_x].detection_ptr);
	    }


	    if (is_unbounded)
		detection_mess (varstr_ptr,
				(SXINT)0,
				conflict_record_ptr->detection_items [1].detection_ptr);

	    given_priority (varstr_ptr, conflict_record_ptr->choice);
	}
    }

    varstr_free (avarstr);
    varstr_free (varstr_ptr);
    sxbm_free (M_plus);
    sxbm_free (M);
}



void	lecl_diagnoses_immediate (struct lr_marker *current_state_ptr, 
				  SXBA sra_set, 
				  SXINT detection_state, 
				  char conflict_kind /* bit (5) */,
				  SXINT choice, 
				  bool with_detection_point)
{
    char	s [8];

    sprintf (s, "%-ld", (SXINT) detection_state);
    immediate_or_one_la (varstr_catenate (varstr_catenate (varstr_catenate (cat_conflict_name (varstr_catenate (varstr_alloc (2048), "immediate "), conflict_kind), " conflict in state "), s), " between\n"),
			 current_state_ptr, sra_set, /* detection_state, conflict_kind, */choice, with_detection_point);
}



void	lecl_diagnoses_one_la (struct lr_marker *current_state_ptr, 
			       SXBA sra_set, 
			       SXINT detection_state, 
			       char conflict_kind /* bit (5) */, 
			       SXINT choice, 
			       SXBA esc_set)
{
    char	s [8];

    sprintf (s, "%-ld", (SXINT) detection_state);
    immediate_or_one_la (varstr_catenate (lecl_gen_ecc (sc_to_char_set, esc_set, xprdct_to_ate, varstr_catenate (varstr_catenate (varstr_catenate (cat_conflict_name (varstr_catenate (varstr_alloc (2048), "1_look_ahead "), conflict_kind), " conflict in state "), s), " on ")), " between\n"),
			 current_state_ptr, sra_set, /* detection_state, conflict_kind, */choice, true);
}

