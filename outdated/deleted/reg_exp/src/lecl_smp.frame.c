/* ********************************************************************
   *  This program has been generated from lecl.at                    *
   *  on Thu May 19 11:07:37 1994                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



/*   I N C L U D E S   */
#define SXNODE struct lecl_node
#include "sxunix.h"

struct lecl_node {
    SXNODE_HEADER_S SXVOID_NAME;

/*
your attribute declarations...
*/
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define ABBREVIATION_n 2
#define ABBREVIATION_PRDCT_NAME_n 3
#define ABBREVIATION_RE_NAME_n 4
#define ABBREVIATION_S_n 5
#define ACTION_ERASE_n 6
#define ACTION_NO_n 7
#define ALTERNATIVE_n 8
#define BYTE_LENGTH_n 9
#define CLASS_n 10
#define CLASS_NAME_n 11
#define CLASS_S_n 12
#define COLLATING_S_n 13
#define COMMENTS_n 14
#define COMPONENT_n 15
#define COMPONENTS_S_n 16
#define COMPONENT_DEF_n 17
#define COMPONENT_NAME_DEF_n 18
#define COMPONENT_NAME_REF_n 19
#define COMPONENT_REF_n 20
#define CONTEXT_n 21
#define CONTEXT_COMMENTS_n 22
#define CONTEXT_EOF_n 23
#define CONTEXT_INCLUDE_n 24
#define CONTEXT_NAME_n 25
#define DECR_n 26
#define DENOTATION_S_n 27
#define ENVIRONMENT_S_n 28
#define EOF_n 29
#define ERASE_n 30
#define EXPRESSION_n 31
#define EXTENDED_CLASS_REF_n 32
#define ID_n 33
#define IDENTIFIER_n 34
#define ID_DENOTATION_n 35
#define INCLUDE_n 36
#define INCR_n 37
#define INTEGER_NUMBER_n 38
#define IS_FALSE_n 39
#define IS_FIRST_COL_n 40
#define IS_LAST_COL_n 41
#define IS_RESET_n 42
#define IS_SET_n 43
#define IS_TRUE_n 44
#define KEYWORDS_n 45
#define KEYWORDS_SPEC_n 46
#define KEYWORD_NAME_n 47
#define LEXICAL_UNIT_NAME_n 48
#define LOWER_CASE_n 49
#define LOWER_TO_UPPER_n 50
#define MARK_n 51
#define MINUS_n 52
#define NOT_n 53
#define NOT_KEYWORDS_n 54
#define NO_SPECIFIED_n 55
#define OCTAL_CODE_n 56
#define OPTION_n 57
#define PLUS_n 58
#define POST_ACTION_n 59
#define PRDCT_ET_n 60
#define PRDCT_EXPRESSION_n 61
#define PRDCT_NOT_n 62
#define PRDCT_OR_n 63
#define PREDICATE_NAME_n 64
#define PREDICATE_NO_n 65
#define PRIORITY_KIND_S_n 66
#define PROGRAM_ROOT_LECL_n 67
#define PUT_n 68
#define REDUCE_REDUCE_n 69
#define REDUCE_SHIFT_n 70
#define REF_TRANS_CLOSURE_n 71
#define REGULAR_EXPRESSION_n 72
#define RELEASE_n 73
#define REPR_SPEC_S_n 74
#define RESET_n 75
#define RESTRICTED_CONTEXT_n 76
#define SEQUENCE_n 77
#define SET_n 78
#define SHIFT_REDUCE_n 79
#define SLICE_n 80
#define STRING_n 81
#define STRING_DENOTATION_n 82
#define SYNONYM_S_n 83
#define TOKEN_n 84
#define TOKEN_DEF_n 85
#define TOKEN_REF_S_n 86
#define TOKEN_S_n 87
#define TRANS_CLOSURE_n 88
#define UNBOUNDED_CONTEXT_n 89
#define UNBOUNDED_RESTRICTED_CONTEXT_n 90
#define UPPER_CASE_n 91
#define UPPER_TO_LOWER_n 92
#define VOID_n 93
#define WORD_LENGTH_n 94
/*
E N D   N O D E   N A M E S
*/

static void lecl_pi () {

/*
I N H E R I T E D
*/

switch (SXVISITED->father->name) {

case ERROR_n :
break;

case ABBREVIATION_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ABBREVIATION_PRDCT_NAME_n, ABBREVIATION_RE_NAME_n} */
	break;

	case 2 :/* SXVISITED->name = {PRDCT_EXPRESSION_n, REGULAR_EXPRESSION_n} */
	break;

	}

break;

case ABBREVIATION_S_n :/* SXVISITED->name = ABBREVIATION_n */
break;

case ALTERNATIVE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, 
EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, 
REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n}
 */
	break;

	case 2 :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, 
INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, 
SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	break;

	}

break;

case CLASS_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = CLASS_NAME_n */
	break;

	case 2 :/* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	break;

	}

break;

case CLASS_S_n :/* SXVISITED->name = CLASS_n */
break;

case COLLATING_S_n :/* SXVISITED->name = {ID_n, INTEGER_NUMBER_n, NO_SPECIFIED_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
break;

case COMPONENT_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = COMPONENT_DEF_n */
	break;

	case 2 :/* SXVISITED->name = {ENVIRONMENT_S_n, VOID_n} */
	break;

	}

break;

case COMPONENTS_S_n :/* SXVISITED->name = COMPONENT_n */
break;

case COMPONENT_DEF_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = COMPONENT_NAME_DEF_n */
	break;

	case 2 :/* SXVISITED->name = REGULAR_EXPRESSION_n */
	break;

	}

break;

case COMPONENT_REF_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {CONTEXT_COMMENTS_n, CONTEXT_EOF_n, CONTEXT_INCLUDE_n, CONTEXT_NAME_n} */
	break;

	case 2 :/* SXVISITED->name = {COMPONENT_NAME_REF_n, VOID_n} */
	break;

	}

break;

case CONTEXT_n :/* SXVISITED->name = TOKEN_REF_S_n */
break;

case DENOTATION_S_n :/* SXVISITED->name = {ID_DENOTATION_n, STRING_DENOTATION_n} */
break;

case ENVIRONMENT_S_n :/* SXVISITED->name = {CONTEXT_n, KEYWORDS_SPEC_n, POST_ACTION_n, PRIORITY_KIND_S_n, RESTRICTED_CONTEXT_n, 
UNBOUNDED_CONTEXT_n, UNBOUNDED_RESTRICTED_CONTEXT_n} */
break;

case ERASE_n :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, INCR_n, 
LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SET_n, 
SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
break;

case EXPRESSION_n :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, 
EXTENDED_CLASS_REF_n, ID_n, INCR_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n
, NOT_n, OCTAL_CODE_n, OPTION_n, PRDCT_ET_n, PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n, PUT_n, 
REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n}
 */
break;

case EXTENDED_CLASS_REF_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ID_n, NOT_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
	break;

	case 2 :/* SXVISITED->name = {IS_FALSE_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, IS_TRUE_n, 
PREDICATE_NAME_n, PREDICATE_NO_n} */
	break;

	}

break;

case KEYWORDS_n :/* SXVISITED->name = KEYWORD_NAME_n */
break;

case KEYWORDS_SPEC_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {IDENTIFIER_n, VOID_n} */
	break;

	case 2 :/* SXVISITED->name = IDENTIFIER_n */
	break;

	case 3 :/* SXVISITED->name = {KEYWORDS_n, NOT_KEYWORDS_n, VOID_n} */
	break;

	}

break;

case MINUS_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	break;

	case 2 :/* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	break;

	}

break;

case NOT_n :/* SXVISITED->name = {ID_n, OCTAL_CODE_n, SLICE_n, STRING_n} */
break;

case NOT_KEYWORDS_n :/* SXVISITED->name = KEYWORD_NAME_n */
break;

case OPTION_n :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, 
EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, 
REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n}
 */
break;

case PLUS_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	break;

	case 2 :/* SXVISITED->name = {ID_n, MINUS_n, OCTAL_CODE_n, PLUS_n, SLICE_n, STRING_n} */
	break;

	}

break;

case PRDCT_ET_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n
, PREDICATE_NAME_n, PREDICATE_NO_n} */
	break;

	case 2 :/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_NOT_n, 
PREDICATE_NAME_n, PREDICATE_NO_n} */
	break;

	}

break;

case PRDCT_EXPRESSION_n :/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, 
PRDCT_NOT_n, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
break;

case PRDCT_NOT_n :/* SXVISITED->name = {IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PREDICATE_NO_n} */
break;

case PRDCT_OR_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n
, PRDCT_OR_n, PREDICATE_NAME_n, PREDICATE_NO_n} */
	break;

	case 2 :/* SXVISITED->name = {EXPRESSION_n, IS_FIRST_COL_n, IS_LAST_COL_n, IS_RESET_n, IS_SET_n, PRDCT_ET_n, PRDCT_NOT_n
, PREDICATE_NAME_n, PREDICATE_NO_n} */
	break;

	}

break;

case PRIORITY_KIND_S_n :/* SXVISITED->name = {REDUCE_REDUCE_n, REDUCE_SHIFT_n, SHIFT_REDUCE_n} */
break;

case PROGRAM_ROOT_LECL_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {CLASS_S_n, VOID_n} */
	break;

	case 2 :/* SXVISITED->name = {ABBREVIATION_S_n, VOID_n} */
	break;

	case 3 :/* SXVISITED->name = {TOKEN_S_n, VOID_n} */
	break;

	case 4 :/* SXVISITED->name = {SYNONYM_S_n, VOID_n} */
	break;

	case 5 :/* SXVISITED->name = {REPR_SPEC_S_n, VOID_n} */
	break;

	}

break;

case PUT_n :/* SXVISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
break;

case REF_TRANS_CLOSURE_n :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, 
EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, 
REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n}
 */
break;

case REGULAR_EXPRESSION_n :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, 
EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, 
REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n}
 */
break;

case REPR_SPEC_S_n :/* SXVISITED->name = {BYTE_LENGTH_n, COLLATING_S_n, WORD_LENGTH_n} */
break;

case RESTRICTED_CONTEXT_n :/* SXVISITED->name = {TOKEN_REF_S_n, VOID_n} */
break;

case SEQUENCE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, 
INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, 
SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	break;

	case 2 :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, DECR_n, ERASE_n, EXPRESSION_n, EXTENDED_CLASS_REF_n, ID_n, 
INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, 
SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n} */
	break;

	}

break;

case SLICE_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
	break;

	case 2 :/* SXVISITED->name = {ID_n, OCTAL_CODE_n, STRING_n} */
	break;

	}

break;

case SYNONYM_S_n :/* SXVISITED->name = DENOTATION_S_n */
break;

case TOKEN_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = TOKEN_DEF_n */
	break;

	case 2 :/* SXVISITED->name = {ENVIRONMENT_S_n, VOID_n} */
	break;

	}

break;

case TOKEN_DEF_n :
	switch (SXVISITED->position) {
	case 1 :/* SXVISITED->name = {COMMENTS_n, EOF_n, INCLUDE_n, LEXICAL_UNIT_NAME_n} */
	break;

	case 2 :/* SXVISITED->name = {COMPONENTS_S_n, REGULAR_EXPRESSION_n} */
	break;

	}

break;

case TOKEN_REF_S_n :/* SXVISITED->name = COMPONENT_REF_n */
break;

case TOKEN_S_n :/* SXVISITED->name = TOKEN_n */
break;

case TRANS_CLOSURE_n :/* SXVISITED->name = {ACTION_ERASE_n, ACTION_NO_n, ALTERNATIVE_n, DECR_n, ERASE_n, EXPRESSION_n, 
EXTENDED_CLASS_REF_n, ID_n, INCR_n, LOWER_CASE_n, LOWER_TO_UPPER_n, MARK_n, NOT_n, OCTAL_CODE_n, OPTION_n, PUT_n, 
REF_TRANS_CLOSURE_n, RELEASE_n, RESET_n, SEQUENCE_n, SET_n, SLICE_n, STRING_n, TRANS_CLOSURE_n, UPPER_CASE_n, UPPER_TO_LOWER_n}
 */
break;

case UNBOUNDED_CONTEXT_n :/* SXVISITED->name = TOKEN_REF_S_n */
break;

case UNBOUNDED_RESTRICTED_CONTEXT_n :/* SXVISITED->name = {TOKEN_REF_S_n, VOID_n} */
break;



/*
Z Z Z Z
*/

default:
break;
}
/* end lecl_pi */
}

static void lecl_pd () {

/*
D E R I V E D
*/

switch (SXVISITED->name) {

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
break;

case ACTION_NO_n :
break;

case ALTERNATIVE_n :
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
break;

case DENOTATION_S_n :
break;

case ENVIRONMENT_S_n :
break;

case EOF_n :
break;

case ERASE_n :
break;

case EXPRESSION_n :
break;

case EXTENDED_CLASS_REF_n :
break;

case ID_n :
break;

case IDENTIFIER_n :
break;

case ID_DENOTATION_n :
break;

case INCLUDE_n :
break;

case INCR_n :
break;

case INTEGER_NUMBER_n :
break;

case IS_FALSE_n :
break;

case IS_FIRST_COL_n :
break;

case IS_LAST_COL_n :
break;

case IS_RESET_n :
break;

case IS_SET_n :
break;

case IS_TRUE_n :
break;

case KEYWORDS_n :
break;

case KEYWORDS_SPEC_n :
break;

case KEYWORD_NAME_n :
break;

case LEXICAL_UNIT_NAME_n :
break;

case LOWER_CASE_n :
break;

case LOWER_TO_UPPER_n :
break;

case MARK_n :
break;

case MINUS_n :
break;

case NOT_n :
break;

case NOT_KEYWORDS_n :
break;

case NO_SPECIFIED_n :
break;

case OCTAL_CODE_n :
break;

case OPTION_n :
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
break;

case PREDICATE_NO_n :
break;

case PRIORITY_KIND_S_n :
break;

case PROGRAM_ROOT_LECL_n :
break;

case PUT_n :
break;

case REDUCE_REDUCE_n :
break;

case REDUCE_SHIFT_n :
break;

case REF_TRANS_CLOSURE_n :
break;

case REGULAR_EXPRESSION_n :
break;

case RELEASE_n :
break;

case REPR_SPEC_S_n :
break;

case RESET_n :
break;

case RESTRICTED_CONTEXT_n :
break;

case SEQUENCE_n :
break;

case SET_n :
break;

case SHIFT_REDUCE_n :
break;

case SLICE_n :
break;

case STRING_n :
break;

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
break;

case UNBOUNDED_CONTEXT_n :
break;

case UNBOUNDED_RESTRICTED_CONTEXT_n :
break;

case UPPER_CASE_n :
break;

case UPPER_TO_LOWER_n :
break;

case VOID_n :
break;

case WORD_LENGTH_n :
break;



/*
Z Z Z Z
*/

default:
break;
}
/* end lecl_pd */
}

static void smpopen (sxtables_ptr)
struct sxtables *sxtables_ptr;
{
sxatcvar.atc_lv.node_size = sizeof (struct lecl_node);
}

static void smppass ()
{

/*   I N I T I A L I S A T I O N S   */
/* ........... */

/*   A T T R I B U T E S    E V A L U A T I O N   */
sxsmp (sxatcvar.atc_lv.abstract_tree_root, lecl_pi, lecl_pd);

/*   F I N A L I S A T I O N S   */
/* ........... */

}

lecl_smp (what, sxtables_ptr)int what;
struct sxtables *sxtables_ptr;
{
switch (what) {
case SXOPEN:
smpopen (sxtables_ptr);
break;
case SXSEMPASS: /* For [sub-]tree evaluation during parsing */
break;
case SXACTION:
smppass ();
break;
case SXCLOSE:
break;
default:
break;
}
}
