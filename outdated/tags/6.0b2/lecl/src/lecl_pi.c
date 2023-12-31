/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
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
/* 20030516 13:07 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/******************************************************************/
/*                                                                */
/*                                                                */
/*  This program has been translated from lecl_pass_inherited.pl1 */
/*  on Friday the twenty-first of March, 1986, at 9:01:31,        */
/*  on the Multics system at INRIA,                               */
/*  by the release 3.3g PL1_C translator of INRIA,                */
/*         developped by the "Langages et Traducteurs" team,      */
/*         using the SYNTAX (*), FNC and Paradis systems.         */
/*                                                                */
/*                                                                */
/******************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                            */
/******************************************************************/

#define NODE struct lecl_node

#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"

#include "lecl_node.h"

#include "lecl_nn.h"
char WHAT_LECLPI[] = "@(#)SYNTAX - $Id: lecl_pi.c 564 2007-05-15 15:19:42Z rlacroix $" WHAT_DEBUG;

extern VOID lecl_ced ();



VOID	lecl_pi ()
{
    register struct lecl_node	*visited = VISITED;
    struct lecl_node	*brother_2;


#ifdef DEBUG
    printf ("PI: father_node = %d, visited_node = %d\n", (visited->father)->name, visited->name);
#endif

/*
I N H E R I T E D
*/

    switch ((visited->father)->name) {
    case ERROR_n  :
	sxat_snv (DERIVED, (visited->father));
	return;

    case ABBREVIATION_n  :
	switch (visited->position) {
	case 1:
	    /* VISITED->name = {ABBREVIATION_PRDCT_NAME_n, ABBREVIATION_RE_NAME_n} */
	    current_re_no = (visited->father)->item_code;

	    /* Afin de remplir prolis avec l'oppose (sinon pb avec is_class_used_in_re
   -Cf set_current_pos dans lecl_pass_derived) du code interne du predicat
    que l'on va traiter */
	    if (visited->name == ABBREVIATION_PRDCT_NAME_n)
		current_re_no = -abbrev [current_re_no].prdct_no;

	case 2:
	    /* VISITED->name = {PRDCT_EXPRESSION_n, REGULAR_EXPRESSION_n} */
	    return;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #1");
#endif
	  break;
	}

    case ABBREVIATION_S_n  :
	/* VISITED->name = ABBREVIATION_n */
	return;

    case ALTERNATIVE_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case CLASS_n  :
	    /* VISITED->name = CLASS_NAME_n */
	    /* VISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	return;

    case CLASS_S_n  :
	/* VISITED->name = CLASS_n */
	return;

    case COLLATING_S_n  :
	/* VISITED->name = {ID_n, INTEGER_NUMBER_n, NO_SPECIFIED_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	return;

    case COMPONENT_n  :
	    /* VISITED->name = COMPONENT_DEF_n */
	    /* VISITED->name = {ENVIRONMENT_S_n, VOID_n} */
	return;

    case COMPONENTS_S_n  :
	/* VISITED->name = COMPONENT_n */
	return;

    case COMPONENT_DEF_n  :
	    /* VISITED->name = COMPONENT_NAME_DEF_n */
	    /* VISITED->name = REGULAR_EXPRESSION_n */
	current_re_no = (visited->father)->item_code;
	return;

    case COMPONENT_REF_n  :
	    /* VISITED->name = {CONTEXT_COMMENTS_n, CONTEXT_EOF_n, CONTEXT_INCLUDE_n, CONTEXT_NAME_n} */
	    /* VISITED->name = {COMPONENT_NAME_REF_n, VOID_n} */
	return;

    case CONTEXT_n  :
	/* VISITED->name = TOKEN_REF_S_n */
	return;

    case DENOTATION_S_n  :
	/* VISITED->name = {ID_DENOTATION_n, STRING_DENOTATION_n} */
	return;

    case ENVIRONMENT_S_n  :
/* VISITED->name = {CONTEXT_n, KEYWORDS_SPECn, POST_ACTION_n, PRIORITY_KIND_S_n, RESTRICTED_CONTEXT_n, UNBOUNDED_CONTEXT_n, UNBOUNDED_RESTRICTED_CONTEXT_n} */
	return;

    case ERASE_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case EXPRESSION_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case EXTENDED_CLASS_REF_n  :
	switch (visited->position) {
	case 1:
	    /* VISITED->name = {ID_n, NOT_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	    goto propagate;

	case 2:
/* VISITED->name = {IS_FALSE_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, IS_TRUE_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	    return;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #2");
#endif
	  break;
	}

    case KEYWORDS_n :/* VISITED->name = KEYWORD_NAME_n */
	return;

    case KEYWORDS_SPEC_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, VOID_n} */
	    break;

	case 2 :/* VISITED->name = IDENTIFIER_n */
	    break;

	case 3 :/* VISITED->name = {KEYWORDS_n, NOT_KEYWORDS_n, VOID_n} */
	    break;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #3");
#endif
	  break;

	}

	return;

    case MINUS_n  :
	    /* VISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	return;

    case NOT_n  :
	/* VISITED->name = {ID_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	goto propagate;

    case NOT_KEYWORDS_n :/* VISITED->name = KEYWORD_NAME_n */
	return;

    case OPTION_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	goto propagate;

    case PLUS_n  :
	    /* VISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	return;

    case PRDCT_ET_n  :
	switch (visited->position) {
	case 1:
/* VISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	    if (visited->not_is_first_visit)
		sxba_copy (tnext [visited->node_no], tfirst [sxbrother (visited, 2)->node_no]);

	    return;

	case 2:
/* VISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_NOT_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	    goto propagate;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #4");
#endif
	  break;
	}

    case PRDCT_EXPRESSION_n  :
/* VISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	return;

    case PRDCT_NOT_n  :
	/* VISITED->name = {IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PREDICATE_NO_n} */
	goto propagate;

    case PRDCT_OR_n  :
/* VISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
/* VISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	goto propagate;

    case PRIORITY_KIND_S_n  :
	/* VISITED->name = {REDUCE_REDUCE_n, REDUCE_SHIFT_n, SHIFT_REDUCE_n} */
	return;

    case PROGRAM_ROOT_LECL_n  :
	switch (visited->position) {
	case 1:
	    /* VISITED->name = {CLASS_S_n, VOID_n} */
	    ers_disp [1].pexcl = xexclude;
	    ers_disp [1].pkw = xkw;
	    lecl_ced ((visited->father));
	    sxat_snv (INHERITED, sxbrother (visited, 2));
	    return;

	case 2:
	    /* VISITED->name = {ABBREVIATION_S_n, VOID_n} */
	    ers = (struct ers_item*) sxalloc ((int) ers_size + 1, sizeof (struct ers_item));
	    suivant = sxbm_calloc (ers_size + 1,  max_re_lgth + 1);
	    tfirst = sxbm_calloc (max_node_no_in_re + 1,  max_re_lgth + 1);
	    tnext = sxbm_calloc (max_node_no_in_re + 1,  max_re_lgth + 1);
	    xlis = 1;
	    ers [1].lispro = -1;
	    return;

	case 3:
	    /* VISITED->name = {TOKEN_S_n, VOID_n} */
	    return;

	case 4:
	    /* VISITED->name = {SYNONYM_S_n, VOID_n} */
	    return;

	case 5:
	    /* VISITED->name = {REPR_SPEC_S_n, VOID_n} */
	    sxat_snv (DERIVED, (visited->father));
	    return;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #5");
#endif
	  break;
	}

    case PUT_n  :
	/* VISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
	sxat_snv (DERIVED, (visited->father));
	return;

    case REF_TRANS_CLOSURE_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	if (visited->not_is_first_visit)
	    sxba_or (sxba_copy (tnext [visited->node_no], tnext [(visited->father)->node_no]), tfirst [visited->node_no]);

	return;

    case REGULAR_EXPRESSION_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	return;

    case REPR_SPEC_S_n  :
	/* VISITED->name = {BYTE_LENGTH_n, COLLATING_S_n, WORD_LENGTH_n} */
	return;

    case RESTRICTED_CONTEXT_n  :
	/* VISITED->name = {TOKEN_REF_S_n, VOID_n} */
	return;

    case SEQUENCE_n  :
	switch (visited->position) {
	case 1:
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	    if (visited->not_is_first_visit) {
		brother_2 = sxbrother (visited, 2);

		if (brother_2->is_empty)
		    sxba_or (sxba_copy (tnext [visited->node_no], tfirst [brother_2->node_no]), tnext [(visited->father)->node_no]);
		else
		    sxba_copy (tnext [visited->node_no], tfirst [brother_2->node_no]);
	    }

	    return;

	case 2:
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	    goto propagate;
	default:
#if EBUG
	  sxtrap("lecl_pi","unknown switch case #6");
#endif
	  break;
	}

    case SLICE_n  :
	    /* VISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
	sxat_snv (DERIVED, (visited->father));
	return;

    case SYNONYM_S_n  :
	/* VISITED->name = DENOTATION_S_n */
	return;

    case TOKEN_n  :
	    /* VISITED->name = TOKEN_DEF_n */
	    /* VISITED->name = {ENVIRONMENT_S_n, VOID_n} */
	return;

    case TOKEN_DEF_n  :
	    /* VISITED->name = {COMMENTS_n, EOF_n, INCLUDE_n, LEXICAL_UNIT_NAME_n} */
	    /* VISITED->name = {COMPONENTS_S_n, REGULAR_EXPRESSION_n} */
	current_re_no = (visited->father)->item_code;
	return;

    case TOKEN_REF_S_n  :
	/* VISITED->name = COMPONENT_REF_n */
	return;

    case TOKEN_S_n  :
	/* VISITED->name = TOKEN_n */
	return;

    case TRANS_CLOSURE_n  :
/* VISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	if (visited->not_is_first_visit)
	    sxba_or (sxba_copy (tnext [visited->node_no], tnext [(visited->father)->node_no]), tfirst [visited->node_no]);

	return;

    case UNBOUNDED_CONTEXT_n  :
	/* VISITED->name = TOKEN_REF_S_n */
	return;

    case UNBOUNDED_RESTRICTED_CONTEXT_n  :
	/* VISITED->name = {TOKEN_REF_S_n, VOID_n} */
	return;
    default:
#if EBUG
      sxtrap("lecl_pi","unknown switch case #7");
#endif
      break;

    /* zzzz */
    }

propagate:
    if (visited->not_is_first_visit)
	sxba_copy (tnext [visited->node_no], tnext [(visited->father)->node_no]);

    return;
}
