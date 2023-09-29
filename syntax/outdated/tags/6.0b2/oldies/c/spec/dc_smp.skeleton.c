/* ********************************************************************
   *  This program has been generated from dc.at                      *
   *  on Tue Jan 26 14:05:51 1993                                     *
   *  by the SYNTAX (*) abstract tree constructor SEMAT               *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */



/*   I N C L U D E S   */
#define NODE struct dc_node
#include "sxunix.h"

struct dc_node {
    SXNODE_HEADER_S VOID_NAME;

/*
your attribute declarations...
*/
};
/*
N O D E   N A M E S
*/
#define ERROR_n 1
#define DECLARATIONS_n 2
#define DECLARATIONS_STRUCT_n 3
#define DECLARATION_INTERNAL_n 4
#define DECLARATION_PARAMETER_n 5
#define DECLARATION_SPECIFIERS_0_n 6
#define DECLARATION_SPECIFIERS_1_n 7
#define DECLARATION_STRUCT_n 8
#define DECLARATOR_n 9
#define DECLARATORS_DIRECT_ABSTRACT_n 10
#define DECLARATORS_INIT_n 11
#define DECLARATORS_STRUCT_n 12
#define DECLARATOR_ABSTRACT_n 13
#define DECLARATOR_ARRAY_n 14
#define DECLARATOR_DIRECT_FUNCTION_n 15
#define DECLARATOR_FUNCTION_n 16
#define DEFINITION_FUNCTION_n 17
#define DEFINITION_FUNCTION_NO_DECLARATION_n 18
#define DEFINITION_FUNCTION_NO_SPECIFIER_n 19
#define DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n 20
#define D_IDENTIFIER_n 21
#define ENUMERATOR_n 22
#define ENUMERATORS_n 23
#define ENUM_NAME_n 24
#define EXTERNAL_DECLARATION_n 25
#define FIELD_n 26
#define IDENTIFIER_n 27
#define IDENTIFIERS_n 28
#define INITIALIZER_n 29
#define INITIALIZERS_n 30
#define OPERATOR_ADDITIVE_n 31
#define OPERATOR_ASSIGNMENT_n 32
#define OPERATOR_BITWISE_n 33
#define OPERATOR_CAST_n 34
#define OPERATOR_COMMA_n 35
#define OPERATOR_CONDITIONAL_n 36
#define OPERATOR_DOT_n 37
#define OPERATOR_FUNCTION_CALL_n 38
#define OPERATOR_INDEXING_n 39
#define OPERATOR_LOGICAL_n 40
#define OPERATOR_MULTIPLICATIVE_n 41
#define OPERATOR_POST_DECR_n 42
#define OPERATOR_POST_INCR_n 43
#define OPERATOR_PRE_DECR_n 44
#define OPERATOR_PRE_INCR_n 45
#define OPERATOR_PTR_n 46
#define OPERATOR_RELATIONAL_n 47
#define OPERATOR_SHIFT_n 48
#define OPERATOR_SIZEOF_EXPR_n 49
#define OPERATOR_SIZEOF_TYPE_n 50
#define OPERATOR_UNARY_n 51
#define PARAMETERS_n 52
#define POINTER_n 53
#define SPECIFIER_ENUM_n 54
#define SPECIFIER_NAMED_ENUM_n 55
#define SPECIFIER_NAMED_STRUCT_n 56
#define SPECIFIER_QUALIFIERS_0_n 57
#define SPECIFIER_QUALIFIERS_1_n 58
#define SPECIFIER_STRUCT_n 59
#define STMT_n 60
#define STMT_BREAK_n 61
#define STMT_CASE_n 62
#define STMT_COMPOUND_n 63
#define STMT_CONTINUE_n 64
#define STMT_DEFAULT_n 65
#define STMT_DO_WHILE_n 66
#define STMT_FOR_n 67
#define STMT_GOTO_n 68
#define STMT_IF_THEN_n 69
#define STMT_IF_THEN_ELSE_n 70
#define STMT_LABELED_n 71
#define STMT_RETURN_n 72
#define STMT_SWITCH_n 73
#define STMT_WHILE_n 74
#define TAG_NAME_n 75
#define TRANSLATION_UNITS_n 76
#define TYPEDEF_NAME_n 77
#define TYPE_NAME_n 78
#define TYPE_QUALIFIERS_n 79
#define VOID_n 80
#define character_constant_n 81
#define floating_constant_n 82
#define integer_constant_n 83
#define string_literal_n 84
/*
E N D   N O D E   N A M E S
*/


static dc_pi () {

/*
I N H E R I T E D
*/

switch (VISITED->father->name) {

case ERROR_n :
break;

case DECLARATIONS_n :/* VISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
break;

case DECLARATIONS_STRUCT_n :/* VISITED->name = DECLARATION_STRUCT_n */
break;

case DECLARATION_INTERNAL_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	break;

	case 2 :/* VISITED->name = DECLARATORS_INIT_n */
	break;

	}

break;

case DECLARATION_PARAMETER_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	break;

	case 2 :/* VISITED->name = {DECLARATOR_n, DECLARATOR_ABSTRACT_n, DECLARATOR_FUNCTION_n, POINTER_n, VOID_n} */
	break;

	}

break;

case DECLARATION_SPECIFIERS_0_n :/* VISITED->name = VOID_n */
break;

case DECLARATION_SPECIFIERS_1_n :/* VISITED->name = {DECLARATION_SPECIFIERS_0_n, ENUM_NAME_n, SPECIFIER_ENUM_n, 
SPECIFIER_NAMED_ENUM_n, SPECIFIER_NAMED_STRUCT_n, SPECIFIER_STRUCT_n, TAG_NAME_n, TYPEDEF_NAME_n, VOID_n} */
break;

case DECLARATION_STRUCT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {SPECIFIER_QUALIFIERS_0_n, SPECIFIER_QUALIFIERS_1_n} */
	break;

	case 2 :/* VISITED->name = DECLARATORS_STRUCT_n */
	break;

	}

break;

case DECLARATOR_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {POINTER_n, VOID_n} */
	break;

	case 2 :/* VISITED->name = {DECLARATOR_n, DECLARATOR_ARRAY_n, D_IDENTIFIER_n} */
	break;

	}

break;

case DECLARATORS_DIRECT_ABSTRACT_n :/* VISITED->name = {DECLARATOR_ABSTRACT_n, IDENTIFIER_n, OPERATOR_ADDITIVE_n, 
OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, 
OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, 
OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, PARAMETERS_n, POINTER_n, VOID_n, character_constant_n, 
floating_constant_n, integer_constant_n, string_literal_n} */
break;

case DECLARATORS_INIT_n :/* VISITED->name = {DECLARATOR_n, DECLARATOR_FUNCTION_n, INITIALIZER_n} */
break;

case DECLARATORS_STRUCT_n :/* VISITED->name = {DECLARATOR_n, DECLARATOR_FUNCTION_n, FIELD_n, IDENTIFIER_n, OPERATOR_ADDITIVE_n, 
OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, 
OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, 
OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
break;

case DECLARATOR_ABSTRACT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {POINTER_n, VOID_n} */
	break;

	case 2 :/* VISITED->name = DECLARATORS_DIRECT_ABSTRACT_n */
	break;

	}

break;

case DECLARATOR_ARRAY_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {DECLARATOR_n, DECLARATOR_ARRAY_n, DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n, 
D_IDENTIFIER_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, VOID_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case DECLARATOR_DIRECT_FUNCTION_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {DECLARATOR_n, DECLARATOR_ARRAY_n, DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n, 
D_IDENTIFIER_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIERS_n, PARAMETERS_n, VOID_n} */
	break;

	}

break;

case DECLARATOR_FUNCTION_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {POINTER_n, VOID_n} */
	break;

	case 2 :/* VISITED->name = {DECLARATOR_ARRAY_n, DECLARATOR_DIRECT_FUNCTION_n, DECLARATOR_FUNCTION_n} */
	break;

	}

break;

case DEFINITION_FUNCTION_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	break;

	case 2 :/* VISITED->name = DECLARATOR_FUNCTION_n */
	break;

	case 3 :/* VISITED->name = DECLARATIONS_n */
	break;

	case 4 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case DEFINITION_FUNCTION_NO_DECLARATION_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n} */
	break;

	case 2 :/* VISITED->name = DECLARATOR_FUNCTION_n */
	break;

	case 3 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case DEFINITION_FUNCTION_NO_SPECIFIER_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = DECLARATOR_FUNCTION_n */
	break;

	case 2 :/* VISITED->name = DECLARATIONS_n */
	break;

	case 3 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = DECLARATOR_FUNCTION_n */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case ENUMERATOR_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = D_IDENTIFIER_n */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case ENUMERATORS_n :/* VISITED->name = {D_IDENTIFIER_n, ENUMERATOR_n} */
break;

case ENUM_NAME_n :/* VISITED->name = IDENTIFIER_n */
break;

case EXTERNAL_DECLARATION_n :/* VISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n
, DECLARATORS_INIT_n, DEFINITION_FUNCTION_n, DEFINITION_FUNCTION_NO_DECLARATION_n, DEFINITION_FUNCTION_NO_SPECIFIER_n, 
DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n} */
break;

case FIELD_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = DECLARATOR_n */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case IDENTIFIERS_n :/* VISITED->name = D_IDENTIFIER_n */
break;

case INITIALIZER_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = DECLARATOR_n */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, INITIALIZERS_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, 
OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, 
OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, 
OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case INITIALIZERS_n :/* VISITED->name = {IDENTIFIER_n, INITIALIZERS_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, 
OPERATOR_BITWISE_n, OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, 
OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, 
OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
break;

case OPERATOR_ADDITIVE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_ASSIGNMENT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
	break;

	case 2 :/* VISITED->name = VOID_n */
	break;

	case 3 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_BITWISE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_CAST_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = TYPE_NAME_n */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_COMMA_n :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
break;

case OPERATOR_CONDITIONAL_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	case 3 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_DOT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
	break;

	case 2 :/* VISITED->name = IDENTIFIER_n */
	break;

	}

break;

case OPERATOR_FUNCTION_CALL_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	break;

	}

break;

case OPERATOR_INDEXING_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
	break;

	case 2 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	}

break;

case OPERATOR_LOGICAL_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_MULTIPLICATIVE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_POST_DECR_n :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
break;

case OPERATOR_POST_INCR_n :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
break;

case OPERATOR_PRE_DECR_n :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
break;

case OPERATOR_PRE_INCR_n :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
break;

case OPERATOR_PTR_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
	break;

	case 2 :/* VISITED->name = IDENTIFIER_n */
	break;

	}

break;

case OPERATOR_RELATIONAL_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_SHIFT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	}

break;

case OPERATOR_SIZEOF_EXPR_n :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
break;

case OPERATOR_SIZEOF_TYPE_n :/* VISITED->name = TYPE_NAME_n */
break;

case OPERATOR_UNARY_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = VOID_n */
	break;

	case 2 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_COMMA_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, 
OPERATOR_INDEXING_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, 
OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n
, string_literal_n} */
	break;

	}

break;

case PARAMETERS_n :/* VISITED->name = DECLARATION_PARAMETER_n */
break;

case POINTER_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = TYPE_QUALIFIERS_n */
	break;

	case 2 :/* VISITED->name = {POINTER_n, VOID_n} */
	break;

	}

break;

case SPECIFIER_ENUM_n :/* VISITED->name = ENUMERATORS_n */
break;

case SPECIFIER_NAMED_ENUM_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = IDENTIFIER_n */
	break;

	case 2 :/* VISITED->name = ENUMERATORS_n */
	break;

	}

break;

case SPECIFIER_NAMED_STRUCT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = VOID_n */
	break;

	case 2 :/* VISITED->name = IDENTIFIER_n */
	break;

	case 3 :/* VISITED->name = DECLARATIONS_STRUCT_n */
	break;

	}

break;

case SPECIFIER_QUALIFIERS_0_n :/* VISITED->name = VOID_n */
break;

case SPECIFIER_QUALIFIERS_1_n :/* VISITED->name = {ENUM_NAME_n, SPECIFIER_ENUM_n, SPECIFIER_NAMED_ENUM_n, 
SPECIFIER_NAMED_STRUCT_n, SPECIFIER_QUALIFIERS_0_n, SPECIFIER_STRUCT_n, TAG_NAME_n, TYPEDEF_NAME_n, VOID_n} */
break;

case SPECIFIER_STRUCT_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = VOID_n */
	break;

	case 2 :/* VISITED->name = DECLARATIONS_STRUCT_n */
	break;

	}

break;

case STMT_n :/* VISITED->name = {OPERATOR_COMMA_n, STMT_BREAK_n, STMT_CASE_n, STMT_COMPOUND_n, STMT_CONTINUE_n, STMT_DEFAULT_n, 
STMT_DO_WHILE_n, STMT_FOR_n, STMT_GOTO_n, STMT_IF_THEN_n, STMT_IF_THEN_ELSE_n, STMT_LABELED_n, STMT_RETURN_n, STMT_SWITCH_n, 
STMT_WHILE_n, VOID_n} */
break;

case STMT_CASE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {IDENTIFIER_n, OPERATOR_ADDITIVE_n, OPERATOR_ASSIGNMENT_n, OPERATOR_BITWISE_n, 
OPERATOR_CAST_n, OPERATOR_COMMA_n, OPERATOR_CONDITIONAL_n, OPERATOR_DOT_n, OPERATOR_FUNCTION_CALL_n, OPERATOR_INDEXING_n, 
OPERATOR_LOGICAL_n, OPERATOR_MULTIPLICATIVE_n, OPERATOR_POST_DECR_n, OPERATOR_POST_INCR_n, OPERATOR_PRE_DECR_n, 
OPERATOR_PRE_INCR_n, OPERATOR_PTR_n, OPERATOR_RELATIONAL_n, OPERATOR_SHIFT_n, OPERATOR_SIZEOF_EXPR_n, OPERATOR_SIZEOF_TYPE_n, 
OPERATOR_UNARY_n, character_constant_n, floating_constant_n, integer_constant_n, string_literal_n} */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case STMT_COMPOUND_n :/* VISITED->name = {DECLARATION_INTERNAL_n, DECLARATION_SPECIFIERS_0_n, DECLARATION_SPECIFIERS_1_n, STMT_n
, VOID_n} */
break;

case STMT_DEFAULT_n :/* VISITED->name = {STMT_n, VOID_n} */
break;

case STMT_DO_WHILE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	case 2 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	}

break;

case STMT_FOR_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	break;

	case 2 :/* VISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	break;

	case 3 :/* VISITED->name = {OPERATOR_COMMA_n, VOID_n} */
	break;

	case 4 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case STMT_GOTO_n :/* VISITED->name = IDENTIFIER_n */
break;

case STMT_IF_THEN_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case STMT_IF_THEN_ELSE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	case 3 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case STMT_LABELED_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = IDENTIFIER_n */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case STMT_RETURN_n :/* VISITED->name = {OPERATOR_COMMA_n, VOID_n} */
break;

case STMT_SWITCH_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case STMT_WHILE_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = OPERATOR_COMMA_n */
	break;

	case 2 :/* VISITED->name = {STMT_n, VOID_n} */
	break;

	}

break;

case TAG_NAME_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = VOID_n */
	break;

	case 2 :/* VISITED->name = IDENTIFIER_n */
	break;

	}

break;

case TRANSLATION_UNITS_n :/* VISITED->name = {EXTERNAL_DECLARATION_n, VOID_n} */
break;

case TYPE_NAME_n :
	switch (VISITED->position) {
	case 1 :/* VISITED->name = {SPECIFIER_QUALIFIERS_0_n, SPECIFIER_QUALIFIERS_1_n} */
	break;

	case 2 :/* VISITED->name = {DECLARATOR_ABSTRACT_n, POINTER_n, VOID_n} */
	break;

	}

break;

case TYPE_QUALIFIERS_n :/* VISITED->name = VOID_n */
break;



/*
Z Z Z Z
*/

default:
break;
}
/* end dc_pi */
}

static dc_pd () {

/*
D E R I V E D
*/

switch (VISITED->name) {

case ERROR_n :
break;

case DECLARATIONS_n :
break;

case DECLARATIONS_STRUCT_n :
break;

case DECLARATION_INTERNAL_n :
break;

case DECLARATION_PARAMETER_n :
break;

case DECLARATION_SPECIFIERS_0_n :
break;

case DECLARATION_SPECIFIERS_1_n :
break;

case DECLARATION_STRUCT_n :
break;

case DECLARATOR_n :
break;

case DECLARATORS_DIRECT_ABSTRACT_n :
break;

case DECLARATORS_INIT_n :
break;

case DECLARATORS_STRUCT_n :
break;

case DECLARATOR_ABSTRACT_n :
break;

case DECLARATOR_ARRAY_n :
break;

case DECLARATOR_DIRECT_FUNCTION_n :
break;

case DECLARATOR_FUNCTION_n :
break;

case DEFINITION_FUNCTION_n :
break;

case DEFINITION_FUNCTION_NO_DECLARATION_n :
break;

case DEFINITION_FUNCTION_NO_SPECIFIER_n :
break;

case DEFINITION_FUNCTION_NO_SPECIFIER_NO_DECLARATION_n :
break;

case D_IDENTIFIER_n :
break;

case ENUMERATOR_n :
break;

case ENUMERATORS_n :
break;

case ENUM_NAME_n :
break;

case EXTERNAL_DECLARATION_n :
break;

case FIELD_n :
break;

case IDENTIFIER_n :
break;

case IDENTIFIERS_n :
break;

case INITIALIZER_n :
break;

case INITIALIZERS_n :
break;

case OPERATOR_ADDITIVE_n :
break;

case OPERATOR_ASSIGNMENT_n :
break;

case OPERATOR_BITWISE_n :
break;

case OPERATOR_CAST_n :
break;

case OPERATOR_COMMA_n :
break;

case OPERATOR_CONDITIONAL_n :
break;

case OPERATOR_DOT_n :
break;

case OPERATOR_FUNCTION_CALL_n :
break;

case OPERATOR_INDEXING_n :
break;

case OPERATOR_LOGICAL_n :
break;

case OPERATOR_MULTIPLICATIVE_n :
break;

case OPERATOR_POST_DECR_n :
break;

case OPERATOR_POST_INCR_n :
break;

case OPERATOR_PRE_DECR_n :
break;

case OPERATOR_PRE_INCR_n :
break;

case OPERATOR_PTR_n :
break;

case OPERATOR_RELATIONAL_n :
break;

case OPERATOR_SHIFT_n :
break;

case OPERATOR_SIZEOF_EXPR_n :
break;

case OPERATOR_SIZEOF_TYPE_n :
break;

case OPERATOR_UNARY_n :
break;

case PARAMETERS_n :
break;

case POINTER_n :
break;

case SPECIFIER_ENUM_n :
break;

case SPECIFIER_NAMED_ENUM_n :
break;

case SPECIFIER_NAMED_STRUCT_n :
break;

case SPECIFIER_QUALIFIERS_0_n :
break;

case SPECIFIER_QUALIFIERS_1_n :
break;

case SPECIFIER_STRUCT_n :
break;

case STMT_n :
break;

case STMT_BREAK_n :
break;

case STMT_CASE_n :
break;

case STMT_COMPOUND_n :
break;

case STMT_CONTINUE_n :
break;

case STMT_DEFAULT_n :
break;

case STMT_DO_WHILE_n :
break;

case STMT_FOR_n :
break;

case STMT_GOTO_n :
break;

case STMT_IF_THEN_n :
break;

case STMT_IF_THEN_ELSE_n :
break;

case STMT_LABELED_n :
break;

case STMT_RETURN_n :
break;

case STMT_SWITCH_n :
break;

case STMT_WHILE_n :
break;

case TAG_NAME_n :
break;

case TRANSLATION_UNITS_n :
break;

case TYPEDEF_NAME_n :
break;

case TYPE_NAME_n :
break;

case TYPE_QUALIFIERS_n :
break;

case VOID_n :
break;

case character_constant_n :
break;

case floating_constant_n :
break;

case integer_constant_n :
break;

case string_literal_n :
break;



/*
Z Z Z Z
*/

default:
break;
}
/* end dc_pd */
}

static smpopen (sxtables_ptr)
struct sxtables *sxtables_ptr;
{
sxatcvar.atc_lv.node_size = sizeof (struct dc_node);
}

static smppass ()
{

/*   I N I T I A L I S A T I O N S   */
/* ........... */

/*   A T T R I B U T E S    E V A L U A T I O N   */
sxsmp (sxatcvar.atc_lv.abstract_tree_root, dc_pi, dc_pd);

/*   F I N A L I S A T I O N S   */
/* ........... */

}

dc_smp (what, sxtables_ptr)int what;
struct sxtables *sxtables_ptr;
{
switch (what) {
case OPEN:
smpopen (sxtables_ptr);
break;
case ACTION:
smppass ();
break;
}
}
