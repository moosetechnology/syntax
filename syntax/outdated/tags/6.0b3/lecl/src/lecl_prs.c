/*   P R O C E S S _ R E P R _ S P E C   */

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

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from lecl_prs.pl1       */
/*  on Tuesday the twenty-fifth of March, 1986, at 11:52:00, */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/

#define NODE struct lecl_node

#define get_brother(x) (x->brother)

#include "sxunix.h"

#include "lecl_node.h"

#include "varstr.h"
#include "lecl_ag.h"
#include "lecl_nn.h"
char WHAT_LECLPRS[] = "@(#)SYNTAX - $Id: lecl_prs.c 1114 2008-02-27 15:13:51Z rlacroix $" WHAT_DEBUG;



static SXINT	xclass, i, codofcar, class_no, xtc;
static SXBA	/* char_max */ char_set;
static struct lecl_node		*brother;



static VOID	maj (struct lecl_node *visited)
{
    if (++xclass >= target_collate_length) {
	sxput_error (visited->token.source_index, "%sThe length of the target collating sequence cannot exceed %-d .",
	     target_collate_length, err_titles [2] /* error */ );
    }
}



static VOID	process (struct lecl_node *visited)
{
    switch (visited->name) {
    case ERROR_n  :
	return;

    case ABBREVIATION_n  :
	return;

    case ABBREVIATION_PRDCT_NAME_n  :
	return;

    case ABBREVIATION_RE_NAME_n  :
	return;

    case ABBREVIATION_S_n  :
	return;

    case ACTION_ERASE_n  :
	return;

    case ACTION_NO_n  :
	return;

    case ALTERNATIVE_n  :
	return;

    case BYTE_LENGTH_n  :
	return;

    case CLASS_n  :
	return;

    case CLASS_NAME_n  :
	return;

    case CLASS_S_n  :
	return;

    case COLLATING_S_n  :
	xclass = 1;
	xtc = 0;

	for (brother = sxson (visited, 1); brother != NULL; brother = get_brother (brother)) {
	    process (brother);
	}


/* we check that each character (defined in the source language) has a
   representation in the target machine */

	{
	    char	string [256];

	    *string = NUL;

	    for (i = 0; i <= char_max; i++) {
		if (simple_classe [i] != 1 && target_code [i] == 1)
		    strcat (string, CHAR_TO_STRING (i));
	    }

	    if (*string) {
		sxtmsg (sxsrcmngr.source_coord.file_name,
		     "%sNo representation in the target machine has been defined for \"%s\".", err_titles [2] /* error */
		     , string);
	    }
	}

	return;

    case COMMENTS_n  :
	return;

    case COMPONENT_n  :
	return;

    case COMPONENTS_S_n  :
	return;

    case COMPONENT_DEF_n  :
	return;

    case COMPONENT_NAME_DEF_n  :
	return;

    case COMPONENT_NAME_REF_n  :
	return;

    case COMPONENT_REF_n  :
	return;

    case CONTEXT_n  :
	return;

    case CONTEXT_COMMENTS_n  :
	return;

    case CONTEXT_EOF_n  :
	return;

    case CONTEXT_INCLUDE_n  :
	return;

    case CONTEXT_NAME_n  :
	return;

    case DECR_n  :
	return;

    case DENOTATION_S_n  :
	return;

    case ENVIRONMENT_S_n  :
	return;

    case EOF_n  :
	return;

    case ERASE_n  :
	return;

    case EXPRESSION_n  :
	return;

    case EXTENDED_CLASS_REF_n  :
	return;

    case ID_n  :
	goto class_ref;

    case IDENTIFIER_n :
	return;

    case ID_DENOTATION_n  :
	return;

    case INCLUDE_n  :
	return;

    case INCR_n  :
	return;

    case INTEGER_NUMBER_n  :
	if (visited->token.string_table_entry != 0)
	    for (i = 1; i <= atol (sxstrget (visited->token.string_table_entry)); i++) { 
		maj (visited);
		classe [xclass] = 1;
		/* illegal char */
		target_collate [xtc++] = 0;
	    }

	return;

    case IS_FALSE_n  :
	return;

    case IS_FIRST_COL_n  :
	return;

    case IS_LAST_COL_n  :
	return;

    case IS_RESET_n  :
	return;

    case IS_SET_n  :
	return;

    case IS_TRUE_n  :
	return;

    case KEYWORD_NAME_n :
	return;

    case KEYWORDS_n :
	return;

    case LEXICAL_UNIT_NAME_n  :
	return;

    case LOWER_CASE_n  :
	return;

    case LOWER_TO_UPPER_n  :
	return;

    case MARK_n  :
	return;

    case MINUS_n  :
	return;

    case NOT_n  :
	return;

    case NOT_KEYWORDS_n :
	return;

    case NO_SPECIFIED_n  :
	maj (visited);
	classe [xclass] = 1;
	/* illegal char */
	target_collate [xtc++] = 0;
	return;

    case OCTAL_CODE_n  :
	goto class_ref;

    case OPTION_n  :
	return;

    case PLUS_n  :
	return;

    case POST_ACTION_n  :
	return;

    case PRDCT_ET_n  :
	return;

    case PRDCT_EXPRESSION_n  :
	return;

    case PRDCT_NOT_n  :
	return;

    case PRDCT_OR_n  :
	return;

    case PREDICATE_NAME_n  :
	return;

    case PREDICATE_NO_n  :
	return;

    case PRIORITY_KIND_S_n  :
	return;

    case PROGRAM_ROOT_LECL_n  :
	for (i = 0; i < target_collate_length; i++) {
	    if (i < char_max)
		classe [i] = simple_classe [i];
	    else
		classe [i] = 1;

	    target_code [i] = i;
	}

	process (sxson (visited, 5));
	return;

    case PUT_n  :
	return;

    case REDUCE_REDUCE_n  :
	return;

    case REDUCE_SHIFT_n /* 62 */ :
	return;

    case REF_TRANS_CLOSURE_n  :
	return;

    case REGULAR_EXPRESSION_n  :
	return;

    case RELEASE_n  :
	return;

    case REPR_SPEC_S_n  :
	goto list;

    case RESET_n  :
	return;

    case RESTRICTED_CONTEXT_n  :
	return;

    case SEQUENCE_n  :
	return;

    case SET_n  :
	return;

    case SHIFT_REDUCE_n  :
	return;

    case SLICE_n  :
	goto class_ref;

    case STRING_n  :
	goto class_ref;

    case STRING_DENOTATION_n  :
	return;

    case SYNONYM_S_n  :
	return;

    case TOKEN_n  :
	return;

    case TOKEN_DEF_n  :
	return;

    case TOKEN_REF_S_n  :
	return;

    case TOKEN_S_n  :
	return;

    case TRANS_CLOSURE_n  :
	return;

    case UNBOUNDED_CONTEXT_n  :
	return;

    case UNBOUNDED_RESTRICTED_CONTEXT_n  :
	return;

    case UPPER_CASE_n  :
	return;

    case UPPER_TO_LOWER_n  :
	return;

    case VOID_n  :
	return;

    case WORD_LENGTH_n  :
	return;
    default:
#if EBUG
      sxtrap("lecl_prs","unknown switch case #1");
#endif
      break;

/* zzzz */
    }

list:
    for (brother = sxson (visited, 1); brother != NULL; brother = get_brother (brother)) {
	process (brother);
    }

    return;

class_ref:
    char_set = class_to_char_set [visited->item_code];


/* On verifie que les codes sont legaux */

    i = 0;

    while ((i = sxba_scan (char_set, i)) > 0) {
	codofcar = i - 1;
	class_no = simple_classe [codofcar];

	if (class_no == 1 /* illegal char */ )
	    sxput_error (visited->token.source_index, "%sThe character \"%s\" is illegal.", err_titles [1] /* warning */ , CHAR_TO_STRING (codofcar));

	maj (visited);
	classe [xclass] = class_no;
	target_code [codofcar] = xclass;
	target_collate [xtc++] = codofcar;
    }
}



VOID	lecl_prs (struct lecl_node *visited)
{
    process (visited);
}
