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

/*   T R E E _ T O _ S T R I N G   */

#define SXNODE struct lecl_node

#include "sxversion.h"
#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "lecl_node.h"
#include "lecl_nn.h"

char WHAT_LECLTTS[] = "@(#)SYNTAX - $Id: lecl_tts.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

static VARSTR	varstr_ptr;
static char	*mark;
static SXINT	item_no;
static struct lecl_node		*node_ptr;


static void	tree_to_string (struct lecl_node *visited)
{
    switch (visited->name) {
    case ERROR_n :
	break;

    case ABBREVIATION_n :
	break;

    case ABBREVIATION_PRDCT_NAME_n :
	break;

    case ABBREVIATION_RE_NAME_n :
	break;

    case ABBREVIATION_S_n :
	break;

    case ACTION_ERASE_n :
	goto a_leaf;

    case ACTION_NO_n :
	goto a_leaf;

    case ALTERNATIVE_n :
	tree_to_string (sxson (visited, 1));
	varstr_catenate (varstr_ptr, " | ");
	tree_to_string (sxson (visited, 2));
	break;

    case BYTE_LENGTH_n :
	break;

    case CLASS_n :
	break;

    case CLASS_NAME_n :
	break;

    case CLASS_S_n :
	break;

    case COLLATING_S_n :
	break;

    case COMMENTS_n :
	break;

    case COMPONENT_n :
	break;

    case COMPONENTS_S_n :
	break;

    case COMPONENT_DEF_n :
	break;

    case COMPONENT_NAME_DEF_n :
	break;

    case COMPONENT_NAME_REF_n :
	break;

    case COMPONENT_REF_n :
	break;

    case CONTEXT_n :
	break;

    case CONTEXT_COMMENTS_n :
	break;

    case CONTEXT_EOF_n :
	break;

    case CONTEXT_INCLUDE_n :
	break;

    case CONTEXT_NAME_n :
	break;

    case DECR_n :
	goto a_leaf;

    case DENOTATION_S_n :
	break;

    case ENVIRONMENT_S_n :
	break;

    case EOF_n :
	break;

    case ERASE_n :
	varstr_catenate (varstr_ptr, "-");
	tree_to_string (sxson (visited, 1));
	break;

    case EXPRESSION_n :
	varstr_catenate (varstr_ptr, "(");
	tree_to_string (sxson (visited, 1));
	varstr_catenate (varstr_ptr, ")");
	break;

    case EXTENDED_CLASS_REF_n :
	tree_to_string (sxson (visited, 1));
	tree_to_string (sxson (visited, 2));
	break;

    case ID_n :
	if (visited->item_code >= 0)
	    /* class_name */
	    goto a_leaf;


/* abbreviation_name */

	node_ptr = abbrev [visited->item_code].subtree_ptr;

	if (item_no <= visited->d_item_no)
	    item_no = item_no - visited->i_item_no + 1;

	if (node_ptr->son->name == ALTERNATIVE_n) {
	    varstr_catenate (varstr_ptr, "(");
	    tree_to_string (node_ptr);
	    varstr_catenate (varstr_ptr, ")");
	}
	else
	    tree_to_string (node_ptr);

	break;

    case IDENTIFIER_n :
	break;

    case ID_DENOTATION_n :
	break;

    case INCLUDE_n :
	break;

    case INCR_n :
	goto a_leaf;

    case INTEGER_NUMBER_n :
	break;

    case IS_FALSE_n :
	goto a_predicate;

    case IS_FIRST_COL_n :
	goto a_predicate;

    case IS_LAST_COL_n :
	goto a_predicate;

    case IS_RESET_n :
	goto a_predicate;

    case IS_SET_n :
	goto a_predicate;

    case IS_TRUE_n :
	goto a_predicate;

    case KEYWORD_NAME_n :
	break;

    case KEYWORDS_n :
	break;

    case KEYWORDS_SPEC_n :
	break;

    case LEXICAL_UNIT_NAME_n :
	break;

    case LOWER_CASE_n :
	goto a_leaf;

    case LOWER_TO_UPPER_n :
	goto a_leaf;

    case MARK_n :
	goto a_leaf;

    case MINUS_n :
	break;

    case NOT_n :
	tree_to_string (sxson (visited, 1));
	break;

    case NOT_KEYWORDS_n :
	break;

    case NO_SPECIFIED_n :
	break;

    case OCTAL_CODE_n :
	goto a_leaf;

    case OPTION_n :
	varstr_catenate (varstr_ptr, "[");
	tree_to_string (sxson (visited, 1));
	varstr_catenate (varstr_ptr, "]");
	break;

    case PLUS_n :
	break;

    case POST_ACTION_n :
	break;

    case PRDCT_ET_n :
	break;

    case PRDCT_EXPRESSION_n :
	break;

    case PRDCT_NOT_n :
	break;

    case PRDCT_OR_n :
	break;

    case PREDICATE_NAME_n :
	goto a_predicate;

    case PREDICATE_NO_n :
	goto a_predicate;

    case PRIORITY_KIND_S_n :
	break;

    case PROGRAM_ROOT_LECL_n :
	break;

    case PUT_n :
        if (visited->i_item_no == item_no
)    	varstr_catenate (varstr_ptr, mark);
    
    	varstr_catenate (varstr_ptr, "@Put (\"");
        varstr_catenate (varstr_ptr, SXCHAR_TO_STRING (*(sxstrget (visited->token.string_table_entry) + 6)));
    	varstr_catenate (varstr_ptr, "\")");
        return;

    case REDUCE_REDUCE_n :
	break;

    case REDUCE_SHIFT_n :
	break;

    case REF_TRANS_CLOSURE_n :
	varstr_catenate (varstr_ptr, "{");
	tree_to_string (sxson (visited, 1));
	varstr_catenate (varstr_ptr, "}");
	break;

    case REGULAR_EXPRESSION_n :
	tree_to_string (sxson (visited, 1));
	break;

    case RELEASE_n :
	goto a_leaf;

    case REPR_SPEC_S_n :
	break;

    case RESET_n :
	goto a_leaf;

    case RESTRICTED_CONTEXT_n :
	break;

    case SEQUENCE_n :
	tree_to_string (sxson (visited, 1));
	varstr_catenate (varstr_ptr, " ");
	tree_to_string (sxson (visited, 2));
	break;

    case SET_n :
	goto a_leaf;

    case SHIFT_REDUCE_n :
	break;

    case SLICE_n :
	goto a_leaf;

    case STRING_n :
	goto a_leaf;

    case STRING_DENOTATION_n :
	break;

    case SYNONYM_S_n :
	break;

    case TOKEN_n :
	break;

    case TOKEN_DEF_n :
	break;

    case TOKEN_REF_S_n :
	break;

    case TOKEN_S_n :
	break;

    case TRANS_CLOSURE_n :
	varstr_catenate (varstr_ptr, "{");
	tree_to_string (sxson (visited, 1));
	varstr_catenate (varstr_ptr, "}+");
	break;

    case UNBOUNDED_CONTEXT_n :
	break;

    case UNBOUNDED_RESTRICTED_CONTEXT_n :
	break;

    case UPPER_CASE_n :
	goto a_leaf;

    case UPPER_TO_LOWER_n :
	goto a_leaf;

    case VOID_n :
	break;

    case WORD_LENGTH_n :
	break;
    default:
#if EBUG
      sxtrap("lecl_tts","unknown switch case #1");
#endif
      break;

/* zzzz */
    }

    return;

a_predicate:
    varstr_catenate (varstr_ptr, sxstrget (visited->token.string_table_entry));
    return;

a_leaf:
    if (visited->i_item_no == item_no)
	varstr_catenate (varstr_ptr, mark);

    varstr_cat_cstring (varstr_ptr, sxstrget (visited->token.string_table_entry));
    return;
}



VARSTR	lecl_tree_to_string (VARSTR vs_ptr, 
			     struct lecl_node *visited, 
			     SXINT x, 
			     char *MARK)
{
    mark = MARK;
    varstr_ptr = vs_ptr;
    item_no = x;
    tree_to_string (visited);

    if (visited->son->d_item_no < item_no)
	varstr_catenate (varstr_ptr, MARK);

    return varstr_ptr; /* end lecl_tree_to_string */
}
