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

/*   C O L L E C T _ S T _ S I Z E   */

#define SXNODE struct lecl_node

#define get_brother(x) (x->brother)


#include "sxversion.h"
#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "lecl_nn.h"

#include "lecl_node.h"
char WHAT_LECLCSS[] = "@(#)SYNTAX - $Id: lecl_css.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern bool	lecl_check_octal_code (SXINT, struct sxsource_coord, SXINT *);
static void 	prdct_processing (struct lecl_node *, char *, SXINT);
static SXINT	buck_no, val, cur_xcld_no, cur_kw_no;



static void act_processing (struct lecl_node *visited,
			    char *name,
			    SXINT param)
{
    action_size++;
    prdct_processing (visited, name, param);
}



static void	prdct_processing (struct lecl_node *visited,
				  char *name,
				  SXINT	param)
{
    if (param == 0)
	visited->token.string_table_entry = sxstrsave (name);
    else {
	char	*p, s [32];

	p = sxstrget (param);
	
	if (atoi (p) > 31) {
	    sxerror (visited->token.source_index,
		     err_titles [1][0],
		     "%sThis counter number seems too high...",
		     err_titles [1]+1);
	}
	
	s [0] = SXNUL;
	strcat (strcat (strcat (strcat (s, name), " ("), p), ")");
	visited->token.string_table_entry = sxstrsave (s);
    }
}



static void st_cs (struct lecl_node *visited)
{
    struct lecl_node	*brother, *son_1;
    SXINT		old_exclude_size, old_keyword_size;;

    sxinitialise(brother); /* pour faire taire gcc -Wuninitialized */
    switch (visited->name) {
    case ERROR_n  :
	return;

    case ABBREVIATION_n  :
	goto second_son;

    case ABBREVIATION_PRDCT_NAME_n  :
	return;

    case ABBREVIATION_RE_NAME_n  :
	return;

    case ABBREVIATION_S_n  :
	abbrev_size = visited->degree;
	goto list;

    case ACTION_ERASE_n  :
	act_processing (visited, "@Erase", (SXINT)0);
	return;

    case ACTION_NO_n  :
	action_size++;
	return;

    case ALTERNATIVE_n  :
	goto two_sons;

    case BYTE_LENGTH_n  :
	byte_length = atoi (sxstrget (visited->token.string_table_entry));

	if (byte_length == 0 || byte_length > 8) {
	    sxerror (visited->token.source_index,
		     err_titles [1][0],
		     "%s%ld is outside the range one to eight allowed for a byte length specification; the value eight is taken.",
		     err_titles [1]+1, 
		     (SXINT) byte_length);
	    byte_length = 8;
	}

	return;

    case CLASS_n  :
	buck_no = 1;
	st_cs (sxson (visited, 2));
	max_bucket_no = max_bucket_no < buck_no ? buck_no : max_bucket_no;
	return;

    case CLASS_NAME_n  :
	return;

    case CLASS_S_n  :
	class_size = class_size + visited->degree;
	goto list;

    case COLLATING_S_n  :
	goto list;

    case COMMENTS_n  :
	are_comments_defined = true;
	return;

    case COMPONENT_n  :
	st_cs (sxson (visited, 1));
	st_cs (sxson (visited, 2));
	exclude_size += cur_xcld_no;
	keyword_size += cur_kw_no;
	return;

    case COMPONENTS_S_n  :
	goto list;

    case COMPONENT_DEF_n  :
	token_size++;
	goto second_son;

    case COMPONENT_NAME_DEF_n  :
	return;

    case COMPONENT_NAME_REF_n  :
	return;

    case COMPONENT_REF_n  :
	return;

    case CONTEXT_n  :
	goto first_son;

    case CONTEXT_COMMENTS_n  :
	return;

    case CONTEXT_EOF_n  :
	return;

    case CONTEXT_INCLUDE_n  :
	return;

    case CONTEXT_NAME_n  :
	return;

    case DECR_n  :
	act_processing (visited, "@Decr", visited->token.string_table_entry);
	return;

    case DENOTATION_S_n  :
	synonym_no += visited->degree;
	goto list;

    case ENVIRONMENT_S_n  :
	goto list;

    case EOF_n  :
	return;

    case ERASE_n  :
	goto first_son;

    case EXPRESSION_n  :
	goto first_son;

    case EXTENDED_CLASS_REF_n  :
	predicate_size++;
	goto two_sons;

    case ID_n  :
	return;

    case IDENTIFIER_n :
	return;

    case ID_DENOTATION_n  :
    {
	SXINT l;
	char *str = (char*) sxalloc ((l = sxstrlen (visited->token.string_table_entry) + 1) + 2, sizeof (char));

	str [0] = '\"';
	strcpy (str + 1, sxstrget (visited->token.string_table_entry));
	str [l] = '\"';
	str [l + 1] = SXNUL;
	visited->token.string_table_entry = sxstr2save (str, l + 1);
       /* forme normale des synonymes */
	sxfree (str);
    }
	return;

    case INCLUDE_n  :
	return;

    case INCR_n  :
	act_processing (visited, "@Incr", visited->token.string_table_entry);
	return;

    case INTEGER_NUMBER_n  :
	return;

    case IS_FALSE_n  :
	visited->token.string_table_entry = false_ate;
	return;

    case IS_FIRST_COL_n  :
	prdct_processing (visited, "&Is_First_Col", (SXINT)0);
	return;

    case IS_LAST_COL_n  :
	prdct_processing (visited, "&Is_Last_Col", (SXINT)0);
	return;

    case IS_RESET_n  :
	prdct_processing (visited, "&Is_Reset", visited->token.string_table_entry);
	return;

    case IS_SET_n  :
	prdct_processing (visited, "&Is_Set", visited->token.string_table_entry);
	return;

    case IS_TRUE_n  :
	visited->token.string_table_entry = true_ate;
	return;

    case KEYWORD_NAME_n :
	return;

    case KEYWORDS_n :
	keyword_size += visited->degree;
	return;

    case KEYWORDS_SPEC_n :
	st_cs (sxson (visited, 3));
	return;

    case LEXICAL_UNIT_NAME_n  :
	return;

    case LOWER_CASE_n  :
	act_processing (visited, "@Lower_Case", (SXINT)0);
	return;

    case LOWER_TO_UPPER_n  :
	act_processing (visited, "@Lower_To_Upper", (SXINT)0);
	return;

    case MARK_n  :
	act_processing (visited, "@Mark", (SXINT)0);
	return;

    case MINUS_n  :
	buck_no++;
	goto two_sons;

    case NOT_n  :
	goto first_son;

    case NOT_KEYWORDS_n  :
	keyword_size += visited->degree;
	return;

    case NO_SPECIFIED_n  :
	return;

    case OCTAL_CODE_n  :
	class_size++;
	brother = visited;
	goto octal_code;

    case OPTION_n  :
	goto first_son;

    case PLUS_n  :
	buck_no++;
	goto two_sons;

    case POST_ACTION_n  :
	action_size++;
	return;

    case PRDCT_ET_n /* 52 */ :
	predicate_size++;
	goto two_sons;

    case PRDCT_EXPRESSION_n  :
	predicate_size++;
	goto first_son;

    case PRDCT_NOT_n  :
	{
	    char	s [32];

	    son_1 = sxson (visited, 1);
	    st_cs (son_1);
	    son_1->token.string_table_entry = sxstrsave (strcat (strcpy (s, "^"), sxstrget (son_1->token.
		 string_table_entry)));
	}

	return;

    case PRDCT_OR_n  :
	predicate_size++;
	goto two_sons;

    case PREDICATE_NAME_n  :
	return;

    case PREDICATE_NO_n  :
	return;

    case PRIORITY_KIND_S_n  :
	return;

    case PROGRAM_ROOT_LECL_n  :
	brother = sxson (visited, 1);
	st_cs (brother);
	/* CLASSES */

	brother = get_brother (brother);
	st_cs (brother);
	/* ABBREVIATIONS */

	brother = get_brother (brother);
	/* ZOMBIES */

	brother = get_brother (brother);
	are_comments_defined = false;
	st_cs (brother);
	/* TOKENS */

	brother = get_brother (brother);
	is_synonym_list = brother->name != VOID_n;
	synonym_no = synonym_list_no = 0;
	st_cs (brother);
	/* SYNONYMS */

	st_cs (get_brother (brother));
	/* REPR_SPEC_s */
	return;

    case PUT_n  :
	action_size++;
	/* Le nouveau string_table_entry sera positionne dans lecl_stc apres verification */
	return;

    case REDUCE_REDUCE_n  :
	return;

    case REDUCE_SHIFT_n  :
	return;

    case REF_TRANS_CLOSURE_n  :
	goto first_son;

    case REGULAR_EXPRESSION_n  :
	goto first_son;

    case RELEASE_n  :
	act_processing (visited, "@Release", (SXINT)0);
	return;

    case REPR_SPEC_S_n  :
	goto list;

    case RESET_n  :
	act_processing (visited, "@Reset", visited->token.string_table_entry);
	return;

    case RESTRICTED_CONTEXT_n  :
	goto first_son;

    case SEQUENCE_n  :
	goto two_sons;

    case SET_n  :
	act_processing (visited, "@Set", visited->token.string_table_entry);
	return;

    case SHIFT_REDUCE_n  :
	return;

    case SLICE_n  :
	{
	    char	s [32];

	    class_size++;
	    brother = sxson (visited, 2);
	    strcpy (s, sxstrget (sxson (visited, 1)->token.string_table_entry));
	    strcat (s, "..");
	    strcat (s, sxstrget (brother->token.string_table_entry));
	    visited->token.string_table_entry = sxstrsave (s);

	    if (brother->name == OCTAL_CODE_n)
		goto octal_code;
	}

	return;

    case STRING_n  :
	class_size++;
	return;

    case STRING_DENOTATION_n  :
	return;

    case SYNONYM_S_n  :
	synonym_list_no = visited->degree + 1;
	goto list;

    case TOKEN_n  :
	/* On visite en premier la specif d'environnement pour recuperer le nombre 
   de contextes */
	old_exclude_size = exclude_size;
	old_keyword_size = keyword_size;
	st_cs (sxson (visited, 2));
	cur_xcld_no = exclude_size - old_exclude_size;
	cur_kw_no = keyword_size - old_keyword_size;
	st_cs (sxson (visited, 1));
	return;

    case TOKEN_DEF_n   :
	token_size++;
	/* pour are_comments_defined */
	goto two_sons;

    case TOKEN_REF_S_n  :
	exclude_size += visited->degree;
	return;

    case TOKEN_S_n  :
	goto list;

    case TRANS_CLOSURE_n  :
	goto first_son;

    case UNBOUNDED_CONTEXT_n  :
	goto first_son;

    case UNBOUNDED_RESTRICTED_CONTEXT_n  :
	goto first_son;

    case UPPER_CASE_n  :
	act_processing (visited, "@Upper_Case", (SXINT)0);
	return;

    case UPPER_TO_LOWER_n  :
	act_processing (visited, "@Upper_To_Lower", (SXINT)0);
	return;

    case VOID_n  :
	return;

    case WORD_LENGTH_n  :
	word_length = atoi (sxstrget (visited->token.string_table_entry));

	if (word_length > 32 || word_length == 0) {
	    sxerror (visited->token.source_index,
		     err_titles [1][0],
		     "%s%ld is outside the range one to thirty-two allowed for a word length specification; the value thirty_two is taken.",
		     err_titles [1]+1,
		     (SXINT) word_length);
	    word_length = 32;
	}
	return;

    case ZOMBIE_NAME_n :
        return;

    case ZOMBIE_S_n :
        return;

    default:
#if EBUG
      sxtrap("lecl_css","unknown switch case #1");
#endif
      break;

/* zzzz */
    }

octal_code:
    if (lecl_check_octal_code (brother->token.string_table_entry, brother->token.source_index, &val))
	char_max = char_max < val ? val : char_max;

    return;

first_son:
    st_cs (sxson (visited, 1));
    return;

second_son:
    st_cs (sxson (visited, 2));
    return;

two_sons:
    brother = sxson (visited, 1);
    st_cs (brother);
    st_cs (get_brother (brother));
    return;

list:
    for (brother = sxson (visited, 1); brother != NULL; brother = get_brother (brother)) {
	st_cs (brother);
    }

    return;
}



void lecl_css (struct lecl_node *visited)

{
    token_size = exclude_size = keyword_size = synonym_list_no = synonym_no = abbrev_size = predicate_size = action_size = class_size = char_max =
	 max_bucket_no = 0;
    byte_length = 8;
    word_length = 32;
    st_cs (visited);
}
