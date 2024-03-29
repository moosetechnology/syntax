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
#include "varstr.h"
#include "lecl_ag.h"
#include "lecl_node.h"
#include "lecl_nn.h"

char WHAT_LECLPI[] = "@(#)SYNTAX - $Id: lecl_pi.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

extern void lecl_ced (struct lecl_node *visited);



void	lecl_pi (void)
{
    struct lecl_node	*visited = SXVISITED;
    struct lecl_node	*brother_2;


#ifdef DEBUG
    printf ("PI: father_node = %d, visited_node = %d\n", (visited->father)->name, visited->name);
#endif

/*
I N H E R I T E D
*/

    switch ((visited->father)->name) {
    case ERROR_n  :
	sxat_snv (SXDERIVED, (visited->father));
	return;

    case ABBREVIATION_n  :
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = {ABBREVIATION_PRDCT_NAME_n, ABBREVIATION_RE_NAME_n} */
	    current_re_no = (visited->father)->item_code;

	    /* Afin de remplir prolis avec l'oppose (sinon pb avec is_class_used_in_re
   -Cf set_current_pos dans lecl_pass_derived) du code interne du predicat
    que l'on va traiter */
	    if (visited->name == ABBREVIATION_PRDCT_NAME_n)
		current_re_no = -abbrev [current_re_no].prdct_no;

	case 2:
	    /* SXVISITED->name = {PRDCT_EXPRESSION_n, REGULAR_EXPRESSION_n} */
	    return;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #1");
#endif
	  break;
	}
	return;

    case ABBREVIATION_S_n  :
	/* SXVISITED->name = ABBREVIATION_n */
	return;

    case ALTERNATIVE_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case CLASS_n  :
	    /* SXVISITED->name = CLASS_NAME_n */
	    /* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	return;

    case CLASS_S_n  :
	/* SXVISITED->name = CLASS_n */
	return;

    case COLLATING_S_n  :
	/* SXVISITED->name = {ID_n, INTEGER_NUMBER_n, NO_SPECIFIED_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	return;

    case COMPONENT_n  :
	    /* SXVISITED->name = COMPONENT_DEF_n */
	    /* SXVISITED->name = {ENVIRONMENT_S_n, VOID_n} */
	return;

    case COMPONENTS_S_n  :
	/* SXVISITED->name = COMPONENT_n */
	return;

    case COMPONENT_DEF_n  :
	    /* SXVISITED->name = COMPONENT_NAME_DEF_n */
	    /* SXVISITED->name = REGULAR_EXPRESSION_n */
	current_re_no = (visited->father)->item_code;
	return;

    case COMPONENT_REF_n  :
	    /* SXVISITED->name = {CONTEXT_COMMENTS_n, CONTEXT_EOF_n, CONTEXT_INCLUDE_n, CONTEXT_NAME_n} */
	    /* SXVISITED->name = {COMPONENT_NAME_REF_n, VOID_n} */
	return;

    case CONTEXT_n  :
	/* SXVISITED->name = TOKEN_REF_S_n */
	return;

    case DENOTATION_S_n  :
	/* SXVISITED->name = {ID_DENOTATION_n, STRING_DENOTATION_n} */
	return;

    case ENVIRONMENT_S_n  :
/* SXVISITED->name = {CONTEXT_n, KEYWORDS_SPECn, POST_ACTION_n, PRIORITY_KIND_S_n, RESTRICTED_CONTEXT_n, UNBOUNDED_CONTEXT_n, UNBOUNDED_RESTRICTED_CONTEXT_n} */
	return;

    case ERASE_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case EXPRESSION_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case EXTENDED_CLASS_REF_n  :
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = {ID_n, NOT_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	    goto propagate;

	case 2:
/* SXVISITED->name = {IS_FALSE_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, IS_TRUE_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	    return;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #2");
#endif
	  break;
	}
	return;

    case KEYWORDS_n :
        /* SXVISITED->name = KEYWORD_NAME_n */
	return;

    case KEYWORDS_SPEC_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, VOID_n} */
	    break;

	case 2 :/* SXVISITED->name = IDENTIFIER_n */
	    break;

	case 3 :/* SXVISITED->name = {KEYWORDS_n, NOT_KEYWORDS_n, VOID_n} */
	    break;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #3");
#endif
	  break;
	}
	return;

    case MINUS_n  :
	    /* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	return;

    case NOT_n  :
	/* SXVISITED->name = {ID_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	goto propagate;

    case NOT_KEYWORDS_n :/* SXVISITED->name = KEYWORD_NAME_n */
	return;

    case OPTION_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case PLUS_n  :
	    /* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	return;

    case PRDCT_ET_n  :
	switch (visited->position) {
	case 1:
/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	    if (visited->not_is_first_visit)
		sxba_copy (tnext [visited->node_no], tfirst [sxbrother (visited, 2)->node_no]);

	    return;

	case 2:
/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_NOT_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	    goto propagate;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #4");
#endif
	  break;
	}
	return;

    case PRDCT_EXPRESSION_n  :
/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	return;

    case PRDCT_NOT_n  :
	/* SXVISITED->name = {IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PREDICATE_NO_n} */
	goto propagate;

    case PRDCT_OR_n  :
/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	goto propagate;

    case PRIORITY_KIND_S_n  :
	/* SXVISITED->name = {REDUCE_REDUCE_n, REDUCE_SHIFT_n, SHIFT_REDUCE_n} */
	return;

    case PROGRAM_ROOT_LECL_n  :
	switch (visited->position) {
	case 1:
	    /* SXVISITED->name = {CLASS_S_n, VOID_n} */
	    ers_disp [1].pexcl = xexclude;
	    ers_disp [1].pkw = xkw;
	    lecl_ced ((visited->father));
	    sxat_snv (SXINHERITED, sxbrother (visited, 2));
	    return;

	case 2:
	    /* SXVISITED->name = {ABBREVIATION_S_n, VOID_n} */
	    ers = (struct ers_item*) sxcalloc ((SXINT) ers_size + 1, sizeof (struct ers_item));
	    suivant = sxbm_calloc (ers_size + 1,  max_re_lgth + 1);
	    tfirst = sxbm_calloc (max_node_no_in_re + 1,  max_re_lgth + 1);
	    tnext = sxbm_calloc (max_node_no_in_re + 1,  max_re_lgth + 1);
	    xlis = 1;
	    ers [1].lispro = -1;
	    return;

	case 3:
	    /* SXVISITED->name = {VOID_n, ZOMBIE_S_n} */
	    return;

	case 4:
	    /* SXVISITED->name = {TOKEN_S_n, VOID_n} */
	    return;

	case 5:
	    /* SXVISITED->name = {SYNONYM_S_n, VOID_n} */
	    return;

	case 6:
	    /* SXVISITED->name = {REPR_SPEC_S_n, VOID_n} */
	    sxat_snv (SXDERIVED, (visited->father));
	    return;

	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #5");
#endif
	  break;
	}
	return; /* added by Hubert Garavel and Wendelin Serwe */

    case PUT_n  :
	/* SXVISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
	sxat_snv (SXDERIVED, (visited->father));
	return;

    case REF_TRANS_CLOSURE_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	if (visited->not_is_first_visit)
	    sxba_or (sxba_copy (tnext [visited->node_no], tnext [(visited->father)->node_no]), tfirst [visited->node_no]);

	return;

    case REGULAR_EXPRESSION_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	return;

    case REPR_SPEC_S_n  :
	/* SXVISITED->name = {BYTE_LENGTH_n, COLLATING_S_n, WORD_LENGTH_n} */
	return;

    case RESTRICTED_CONTEXT_n  :
	/* SXVISITED->name = {TOKEN_REF_S_n, VOID_n} */
	return;

    case SEQUENCE_n  :
	switch (visited->position) {
	case 1:
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	    if (visited->not_is_first_visit) {
		brother_2 = sxbrother (visited, 2);

		if (brother_2->is_empty)
		    sxba_or (sxba_copy (tnext [visited->node_no], tfirst [brother_2->node_no]), tnext [(visited->father)->node_no]);
		else
		    sxba_copy (tnext [visited->node_no], tfirst [brother_2->node_no]);
	    }

	    return;

	case 2:
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	    goto propagate;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #6");
#endif
	  break;
	}
	return; /* added by Hubert Garavel and Wendelin Serwe */

    case SLICE_n  :
	    /* SXVISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
	sxat_snv (SXDERIVED, (visited->father));
	return;

    case SYNONYM_S_n  :
	/* SXVISITED->name = DENOTATION_S_n */
	return;

    case TOKEN_n  :
	    /* SXVISITED->name = TOKEN_DEF_n */
	    /* SXVISITED->name = {ENVIRONMENT_S_n, VOID_n} */
	return;

    case TOKEN_DEF_n  :
	    /* SXVISITED->name = {COMMENTS_n, EOF_n, INCLUDE_n, LEXICAL_UNIT_NAME_n} */
	    /* SXVISITED->name = {COMPONENTS_S_n, REGULAR_EXPRESSION_n} */
	current_re_no = (visited->father)->item_code;
	return;

    case TOKEN_REF_S_n  :
	/* SXVISITED->name = COMPONENT_REF_n */
	return;

    case TOKEN_S_n  :
	/* SXVISITED->name = TOKEN_n */
	return;

    case TRANS_CLOSURE_n  :
/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	if (visited->not_is_first_visit)
	    sxba_or (sxba_copy (tnext [visited->node_no], tnext [(visited->father)->node_no]), tfirst [visited->node_no]);

	return;

    case UNBOUNDED_CONTEXT_n  :
	/* SXVISITED->name = TOKEN_REF_S_n */
	return;

    case UNBOUNDED_RESTRICTED_CONTEXT_n  :
	/* SXVISITED->name = {TOKEN_REF_S_n, VOID_n} */
	return;

    case ZOMBIE_S_n :
        /* SXVISITED->name = ZOMBIE_NAME_n */
        return;
 
    default:
#if EBUG
      sxtrap("lecl_pi","unknown switch case #7");
#endif
      break;
    /* zzzz */
    }
    return;  /* added by Hubert Garavel and Wendelin Serwe */

propagate:
    if (visited->not_is_first_visit)
	sxba_copy (tnext [visited->node_no], tnext [(visited->father)->node_no]);

    return;
}
