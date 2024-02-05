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

#define SXNODE struct lecl_node

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "lecl_nn.h"
#include "lecl_node.h"
#include "varstr.h"
#include "lecl_ag.h"

char WHAT_LECLPD[] = "@(#)SYNTAX - $Id: lecl_pd.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

static SXINT	node_no;
static SXBA	/* max_re_lgth */ next_set;
static SXBA	/* max_re_lgth */ first_set;



static void	set_current_pos (SXINT re_no, 
				 SXINT *current_pos, 
				 SXINT code_no, 
				 SXINT prdct_no, 
				 SXBA next, 
				 struct sxsource_coord *designator, 
				 bool is_erased)
{
    struct ers_item	*ers_ptr;

    if (re_no > 0 && code_no > 0 /* token & class */ )
	SXBA_1_bit (is_class_used_in_re, code_no);

    ers_ptr = ers + *current_pos;
    ers_ptr->lispro = code_no;
    ers_ptr->prdct_no = prdct_no;
    ers_ptr->prolis = re_no;

    if (designator != NULL)
	ers_ptr->liserindx = *designator;

    ers_ptr->is_erased = is_erased;
    sxba_copy (suivant [*current_pos], next);
    (*current_pos)++;
}



static bool	is_path (bool one_path, 
			 SXINT debut, 
			 SXINT fin, 
			 SXBA /* re_lgth */ not_already_seen, 
			 SXINT *to_be_processed /* 1:re_lgth */)
{
    /* Si one_path retourne vrai s'il existe au moins un chemin, sinon
       retourne vrai si tous les chemins */
    SXBA	/* max_re_lgth */ next;
    SXINT	x, xs, xl;

    sxba_fill (not_already_seen);
    SXBA_0_bit (not_already_seen, 0);
    x = 1;
    to_be_processed [1] = debut;

    while (x > 0) {
	next = suivant [to_be_processed [x]];
	x--;
	xs = 0;

	while ((xs = sxba_scan (next, xs)) > 0) {
	    xl = xlis + xs;

	    if (xl == fin) {
		if (one_path) {
		    return (true);
		}
	    }
	    else    
	    if (ers [xl].lispro <= xactmin) {
		if (sxba_bit_is_set (not_already_seen, xs)) {
		    SXBA_0_bit (not_already_seen, xs);
		    to_be_processed [++x] = xl;
		}
	    }
	    else {
	        if (!one_path) {
		    return false;
		}
	    }
	    
	}
    }

    return !one_path;
}



static void	bitassign (SXBA lhs, SXINT x, SXINT lgth, SXBA rhs)
{
    x += lgth;

    while (lgth > 0) {
	x--;

	if (sxba_bit_is_set (rhs, lgth))
	    SXBA_1_bit (lhs, x);
	else
	    SXBA_0_bit (lhs, x);

	lgth--;
    }
}

static void	st_set_debutant (SXBA first, SXINT item_no, SXINT item_code)
{
    SXINT		origine, delta;

    sxba_empty (first);

    if (item_code > 0)
	/* class or error(symbol not declared) */
	SXBA_1_bit (first, item_no);
    else if (item_code < 0) {
	/* abbreviation */
	origine = abbrev [item_code].pront;
	delta = abbrev [item_code - 1].pront - origine - 1;
	/* on enleve le EOT eventuel */
	bitassign (first, item_no, delta, suivant [origine]);
    }
}



static void	st_set_lispro (SXINT re_no, 
			       SXINT item_no, 
			       SXBA next, 
			       SXINT item_code, 
			       SXINT prdct_no, 
			       bool is_erased, 
			       struct sxsource_coord *designator)
{
    /* xlis est l'index dans lispro du debut de l'E.R. en cours de construction;
   on a deja verifie qu'elle tient dans lispro */
    struct ers_item	*ers_ptr;
    SXINT		delta, i, j, current_pos, origine;
    SXBA	/* max_re_lgth */ st_set_lispro_follow;

    st_set_lispro_follow = sxba_calloc (max_re_lgth + 1);
    current_pos = xlis + item_no;

    if (item_code < 0 /* abbreviations */ ) {
	origine = abbrev [item_code].pront;
	delta = abbrev [item_code - 1].pront - origine - 1;

	for (j = 1; j <= delta; j++) {
	    sxba_empty (st_set_lispro_follow);
	    i = origine + j;
	    bitassign (st_set_lispro_follow, item_no, delta, suivant [i]);

	    if (sxba_bit_is_set (suivant [i], delta + 1))
		sxba_or (st_set_lispro_follow, next);

	    ers_ptr = ers + i;
	    set_current_pos (re_no, &current_pos, ers_ptr->lispro, ers_ptr->prdct_no, st_set_lispro_follow, &(ers_ptr->liserindx), 
			     (bool) (ers_ptr->is_erased || is_erased));
	}
    }
    else
	set_current_pos (re_no, &current_pos, item_code, prdct_no, next, designator, is_erased);

    sxfree (st_set_lispro_follow);
}

static void	st_set_lispro_action (SXINT re_no, 
				      SXINT item_no, 
				      SXBA next, 
				      SXINT item_code, 
				      SXINT prdct_no, 
				      bool is_erased, 
				      struct sxsource_coord *designator)
{
    /* xlis est l'index dans lispro du debut de l'E.R. en cours de construction;
   on a deja verifie qu'elle tient dans lispro */
    SXINT		current_pos;

    current_pos = xlis + item_no;
    set_current_pos (re_no, &current_pos, item_code, prdct_no, next, designator, is_erased);
}

static void	st_initialize_lispro (SXINT re_no, SXBA next)
{
    /* xlis est l'index dans lispro du debut de l'E.R. en cours de construction;
   on a deja verifie qu'elle tient dans lispro */
    SXINT		current_pos;

    current_pos = xlis;
    set_current_pos (re_no, &current_pos,  (SXINT)-1, (SXINT)0, next, (struct sxsource_coord *)NULL, false);
}

static void	st_check_token (SXINT *st_check_token_xlis, 
				SXINT item_no, 
				bool is_empty, 
				struct sxsource_coord *designator)
{
    /* This procedure checks that in the current regular expression
   - The empty string cannot be generated
   - There is no path through the RE involving only actions
   - There is no loop involving only actions
   - @Erase occurs only at EOT
     */

    SXINT		EOT, re_lgth, xl;

    re_lgth = item_no + 1;
    EOT = *st_check_token_xlis + re_lgth;

    if (is_empty)
	sxerror (*designator, err_titles [2][0], "%sA token specification must not generate an empty string.", err_titles [2]+1);
    else {
	SXBA	/* re_lgth */ not_already_seen;
	SXINT	*to_be_processed /* 1:re_lgth */ ;

	not_already_seen = sxba_calloc (re_lgth + 1);
	to_be_processed = (SXINT*) sxalloc ((SXINT) re_lgth + 1, sizeof (SXINT));

	if (is_path (true, *st_check_token_xlis, EOT, not_already_seen, to_be_processed))
	    sxerror (*designator,
		     err_titles [2][0],
		     "%sIllegal token specification : it exists a path involving only actions.",
		     err_titles [2]+1);

	for (xl = *st_check_token_xlis + 1; xl < EOT; xl++) {
	    if (ers [xl].lispro <= xactmin) {
		if (is_path (true, xl, xl, not_already_seen, to_be_processed))
		    sxerror (ers [xl].liserindx,
			     err_titles [2][0],
			     "%sIllegal token specification : it exists a loop involving only actions.", 
			     err_titles [2]+1);
	}
    }
	sxfree (to_be_processed);
	sxfree (not_already_seen);
    }

    *st_check_token_xlis = EOT;
    /* OK */
}



static SXINT	get_prdct_value (struct lecl_node *visited)
{
    struct lecl_node	*dad, *kid;

    if ((visited->father)->name == NOT_n) {
	dad = (visited->father)->father;
	kid = (visited->father);
    }
    else {
	dad = (visited->father);
	kid = visited;
    }

    if (dad->name != EXTENDED_CLASS_REF_n)
	return (0);

    return (kid->brother->item_code);
}



void	lecl_pd (void)
{
    struct lecl_node	*visited = SXVISITED;
    struct lecl_node	*son_1, *son_2;

    sxinitialise(son_2);
#ifdef DEBUG
    printf ("PD: visited_node = %d\n", visited->name);
#endif

/*
D E R I V E D
*/

    switch (visited->name) {
    case ERROR_n  :
	break;

    case ABBREVIATION_n  :
	break;

    case ABBREVIATION_PRDCT_NAME_n  :
	break;

    case ABBREVIATION_RE_NAME_n  :
	break;

    case ABBREVIATION_S_n  :
	break;

    case ACTION_ERASE_n  :
	goto action;

    case ACTION_NO_n  :
	goto action;

    case ALTERNATIVE_n  :
	if (!visited->not_is_first_visit) {
	    sxba_or (sxba_copy (tfirst [visited->node_no], tfirst [(son_1 = sxson (visited, 1))->node_no]), tfirst [(son_2 = sxson (
		 visited, 2))->node_no]);
	    visited->is_empty = son_1->is_empty || son_2->is_empty;
	}

	break;

    case BYTE_LENGTH_n  :
	break;

    case CLASS_n  :
	break;

    case CLASS_NAME_n  :
	break;

    case CLASS_S_n  :
	break;

    case COLLATING_S_n  :
	break;

    case COMMENTS_n  :
	break;

    case COMPONENT_n  :
	break;

    case COMPONENTS_S_n  :
	break;

    case COMPONENT_DEF_n  :
	break;

    case COMPONENT_NAME_DEF_n  :
	break;

    case COMPONENT_NAME_REF_n  :
	break;

    case COMPONENT_REF_n  :
	break;

    case CONTEXT_n  :
	break;

    case CONTEXT_COMMENTS_n  :
	break;

    case CONTEXT_EOF_n  :
	break;

    case CONTEXT_INCLUDE_n  :
	break;

    case CONTEXT_NAME_n  :
	break;

    case DECR_n  :
	goto action;

    case DENOTATION_S_n  :
	break;

    case ENVIRONMENT_S_n  :
	break;

    case EOF_n  :
	break;

    case ERASE_n  :
	goto propagate;

    case EXPRESSION_n  :
	goto propagate;

    case EXTENDED_CLASS_REF_n  :
	goto propagate;

    case ID_n  :
	goto sc_ref_in_re;

    case IDENTIFIER_n :
	break;

    case ID_DENOTATION_n  :
	break;

    case INCLUDE_n  :
	break;

    case INCR_n  :
	goto action;

    case INTEGER_NUMBER_n  :
	break;

    case IS_FALSE_n  :
	break;

    case IS_FIRST_COL_n  :
	goto predicate;

    case IS_LAST_COL_n  :
	goto predicate;

    case IS_RESET_n  :
	goto predicate;

    case IS_SET_n  :
	goto predicate;

    case IS_TRUE_n  :
	break;

    case KEYWORD_NAME_n :
 	break;

    case KEYWORDS_n :
 	break;
 
    case KEYWORDS_SPEC_n :
 	break;

    case LEXICAL_UNIT_NAME_n  :
	break;

    case LOWER_CASE_n  :
	goto action;

    case LOWER_TO_UPPER_n  :
	goto action;

    case MARK_n  :
	goto action;

    case MINUS_n  :
	break;

    case NOT_n  :
	goto propagate;

    case NOT_KEYWORDS_n :
	break;

    case NO_SPECIFIED_n  :
	break;

    case OCTAL_CODE_n  :
	goto sc_ref_in_re;

    case OPTION_n  :
	if (!visited->not_is_first_visit) {
	    visited->is_empty = true;
	    sxba_copy (tfirst [visited->node_no], tfirst [sxson (visited, 1)->node_no]);
	}

	break;

    case PLUS_n  :
	break;

    case POST_ACTION_n  :
	break;

    case PRDCT_ET_n  :
	goto propagate;

    case PRDCT_EXPRESSION_n  :
	son_1 = sxson (visited, 1);

	if (!visited->not_is_first_visit) {
	    SXINT din;

	    node_no = son_1->node_no;
	    next_set = tnext [node_no];
	    sxba_empty (next_set);
	    din = son_1->d_item_no + 1;
	    SXBA_1_bit (next_set, din);
	    first_set = tfirst [node_no];
	    st_initialize_lispro (current_re_no, first_set);
	    sxat_snv (SXINHERITED, sxson (visited, 1));
	}
	else {
	    abbrev [(visited->father)->item_code].is_empty = false;
	    xlis += son_1->d_item_no + 1;
	    ers [xlis].lispro = -1;
	}

	break;

    case PRDCT_NOT_n  :
	goto propagate;

    case PRDCT_OR_n  :
	if (!visited->not_is_first_visit)
	    sxba_or (sxba_copy (tfirst [visited->node_no], tfirst [sxson (visited, 1)->node_no]), tfirst [sxson (visited, 2)->node_no])
		 ;

	break;

    case PREDICATE_NAME_n  :
	goto predicate;

    case PREDICATE_NO_n  :
	goto predicate;

    case PRIORITY_KIND_S_n  :
	break;

    case PROGRAM_ROOT_LECL_n  :
	break;

    case PUT_n  :
	goto action;

    case REDUCE_REDUCE_n  :
	break;

    case REDUCE_SHIFT_n  :
	break;

    case REF_TRANS_CLOSURE_n  :
	if (!visited->not_is_first_visit) {
	    visited->is_empty = true;
	    sxba_copy (tfirst [visited->node_no], tfirst [sxson (visited, 1)->node_no]);
	}

	break;

    case REGULAR_EXPRESSION_n  :
	son_1 = sxson (visited, 1);

	if (!visited->not_is_first_visit) {
	    SXINT din;

	    node_no = son_1->node_no;
	    next_set = tnext [node_no];
	    sxba_empty (next_set);
	    din = son_1->d_item_no + 1;
	    SXBA_1_bit (next_set, din);
	    first_set = tfirst [node_no];

	    if (visited->father->son->name == EOF_n) {
		/* PB si EOF est defini par union!!, il serait preferable de pouvoir utiliser
   la classe de caractere predefinie EOF et de laisser l'utilisateur se 
   debrouiller avec sa definition du token EOF */
		SXBA_1_bit (first_set, 1 /* item_no */ );
		st_initialize_lispro (current_re_no, first_set);
		st_set_lispro (current_re_no, (SXINT)1 /* item_no */ , next_set, (SXINT)2
			       /* eof code */
									   , (SXINT)0 /* prdct_no */ , true, &(son_1->token.
		     source_index));
	    }
	    else if (son_1->is_empty) {
		sxba_or (sxba_copy (tfirst [visited->node_no], first_set), next_set);
		st_initialize_lispro (current_re_no, tfirst [visited->node_no]);
	    }
	    else
		st_initialize_lispro (current_re_no, first_set);

	    sxat_snv (SXINHERITED, son_1);
	}
	else {
	    if ((visited->father)->name != ABBREVIATION_n)
		st_check_token (&xlis, son_1->d_item_no, son_1->is_empty, &(son_1->token.source_index));
	    else {
		abbrev [(visited->father)->item_code].is_empty = son_1->is_empty;
		xlis += son_1->d_item_no + 1;
	    }

	    ers [xlis].lispro = -1;
	}

	break;

    case RELEASE_n  :
	release_code = visited->item_code;
	goto action;

    case REPR_SPEC_S_n  :
	break;

    case RESET_n  :
	goto action;

    case RESTRICTED_CONTEXT_n  :
	break;

    case SEQUENCE_n  :
	if (!visited->not_is_first_visit) {
	    visited->is_empty = (son_1 = sxson (visited, 1))->is_empty && (son_2 = sxson (visited, 2))->is_empty;

	    if (son_1->is_empty)
		sxba_or (sxba_copy (tfirst [visited->node_no], tfirst [son_1->node_no]), tfirst [son_2->node_no]);
	    else
		sxba_copy (tfirst [visited->node_no], tfirst [son_1->node_no]);
	}

	break;

    case SET_n  :
	goto action;

    case SHIFT_REDUCE_n  :
	break;

    case SLICE_n  :
	goto sc_ref_in_re;

    case STRING_n  :
	goto sc_ref_in_re;

    case STRING_DENOTATION_n  :
	break;

    case SYNONYM_S_n  :
	break;

    case TOKEN_n  :
	break;

    case TOKEN_DEF_n  :
	break;

    case TOKEN_REF_S_n  :
	break;

    case TOKEN_S_n  :
	break;

    case TRANS_CLOSURE_n  :
	goto propagate;

    case UNBOUNDED_CONTEXT_n  :
	break;

    case UNBOUNDED_RESTRICTED_CONTEXT_n  :
	break;

    case UPPER_CASE_n  :
	goto action;

    case UPPER_TO_LOWER_n  :
	goto action;

    case VOID_n  :
	break;

    case WORD_LENGTH_n  :
	break;

    case ZOMBIE_NAME_n :
        break;

    case ZOMBIE_S_n :
        break;

    default:
#if EBUG
      sxtrap("lecl_pd","unknown switch case #1");
#endif
      break;
      
/* zzzz */
    }

visited_node_kind_end:
    visited->not_is_first_visit = true;
    return;

action:
    node_no = visited->node_no;

    if (!visited->not_is_first_visit) {
	/* visited -> is_empty = false; init_value */
	sxba_empty (tfirst [node_no]);
	SXBA_1_bit (tfirst [node_no], visited->i_item_no);
    }
    else
	st_set_lispro_action (current_re_no, visited->i_item_no, tnext [node_no], visited->item_code, (SXINT)0 /* prdct_no */ ,
	     false
			      /* is_erased */
		  , &(visited->token.source_index));

    goto visited_node_kind_end;

predicate:
    if ((visited->father)->name == EXTENDED_CLASS_REF_n)
	goto visited_node_kind_end;


/* on est dans une RE */
/* sinon en sequence */

sc_ref_in_re:
    node_no = visited->node_no;

    if (!visited->not_is_first_visit) {
	if (visited->item_code < 0) {
	    /* abbrev */
	    visited->is_empty = abbrev [visited->item_code].is_empty;

	    if ((visited->father)->name == EXTENDED_CLASS_REF_n
		/* le cas NOT_n a deja ete verifie */
							       )
		sxerror (visited->brother->token.source_index,
			 err_titles [2][0],
			 "%sA predicate may only be associated with a class designator.", 
			 err_titles [2]+1);
	}


/*        else visited -> is_empty = false; init_value */

	st_set_debutant (tfirst [node_no], visited->i_item_no, visited->item_code);
    }
    else
	st_set_lispro (current_re_no, visited->i_item_no, tnext [node_no], visited->item_code, get_prdct_value (visited),
	     visited->is_erased, &(visited->token.source_index));

    goto visited_node_kind_end;

propagate:
    if (!visited->not_is_first_visit) {
	visited->not_is_first_visit = true;
	visited->is_empty = (son_1 = sxson (visited, 1))->is_empty;
	sxba_copy (tfirst [visited->node_no], tfirst [son_1->node_no]);
    }
}
