/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
/*
N O D E   N A M E S
*/
#define ERROR_n ((SXINT)1)
#define ABBREVIATION_n ((SXINT)2)
#define ABBREVIATION_PRDCT_NAME_n ((SXINT)3)
#define ABBREVIATION_RE_NAME_n ((SXINT)4)
#define ABBREVIATION_S_n ((SXINT)5)
#define ACTION_ERASE_n ((SXINT)6)
#define ACTION_NO_n ((SXINT)7)
#define ALTERNATIVE_n ((SXINT)8)
#define BYTE_LENGTH_n ((SXINT)9)
#define CLASS_n ((SXINT)10)
#define CLASS_NAME_n ((SXINT)11)
#define CLASS_S_n ((SXINT)12)
#define COLLATING_S_n ((SXINT)13)
#define COMMENTS_n ((SXINT)14)
#define COMPONENT_n ((SXINT)15)
#define COMPONENTS_S_n ((SXINT)16)
#define COMPONENT_DEF_n ((SXINT)17)
#define COMPONENT_NAME_DEF_n ((SXINT)18)
#define COMPONENT_NAME_REF_n ((SXINT)19)
#define COMPONENT_REF_n ((SXINT)20)
#define CONTEXT_n ((SXINT)21)
#define CONTEXT_COMMENTS_n ((SXINT)22)
#define CONTEXT_EOF_n ((SXINT)23)
#define CONTEXT_INCLUDE_n ((SXINT)24)
#define CONTEXT_NAME_n ((SXINT)25)
#define DECR_n ((SXINT)26)
#define DENOTATION_S_n ((SXINT)27)
#define ENVIRONMENT_S_n ((SXINT)28)
#define EOF_n ((SXINT)29)
#define ERASE_n ((SXINT)30)
#define EXPRESSION_n ((SXINT)31)
#define EXTENDED_CLASS_REF_n ((SXINT)32)
#define ID_n ((SXINT)33)
#define IDENTIFIER_n ((SXINT)34)
#define ID_DENOTATION_n ((SXINT)35)
#define INCLUDE_n ((SXINT)36)
#define INCR_n ((SXINT)37)
#define INTEGER_NUMBER_n ((SXINT)38)
#define IS_FALSE_n ((SXINT)39)
#define IS_FIRST_COL_n ((SXINT)40)
#define IS_LAST_COL_n ((SXINT)41)
#define IS_RESET_n ((SXINT)42)
#define IS_SET_n ((SXINT)43)
#define IS_TRUE_n ((SXINT)44)
#define KEYWORDS_n ((SXINT)45)
#define KEYWORDS_SPEC_n ((SXINT)46)
#define KEYWORD_NAME_n ((SXINT)47)
#define LEXICAL_UNIT_NAME_n ((SXINT)48)
#define LOWER_CASE_n ((SXINT)49)
#define LOWER_TO_UPPER_n ((SXINT)50)
#define MARK_n ((SXINT)51)
#define MINUS_n ((SXINT)52)
#define NOT_n ((SXINT)53)
#define NOT_KEYWORDS_n ((SXINT)54)
#define NO_SPECIFIED_n ((SXINT)55)
#define OCTAL_CODE_n ((SXINT)56)
#define OPTION_n ((SXINT)57)
#define PLUS_n ((SXINT)58)
#define POST_ACTION_n ((SXINT)59)
#define PRDCT_ET_n ((SXINT)60)
#define PRDCT_EXPRESSION_n ((SXINT)61)
#define PRDCT_NOT_n ((SXINT)62)
#define PRDCT_OR_n ((SXINT)63)
#define PREDICATE_NAME_n ((SXINT)64)
#define PREDICATE_NO_n ((SXINT)65)
#define PRIORITY_KIND_S_n ((SXINT)66)
#define PROGRAM_ROOT_LECL_n ((SXINT)67)
#define PUT_n ((SXINT)68)
#define REDUCE_REDUCE_n ((SXINT)69)
#define REDUCE_SHIFT_n ((SXINT)70)
#define REF_TRANS_CLOSURE_n ((SXINT)71)
#define REGULAR_EXPRESSION_n ((SXINT)72)
#define RELEASE_n ((SXINT)73)
#define REPR_SPEC_S_n ((SXINT)74)
#define RESET_n ((SXINT)75)
#define RESTRICTED_CONTEXT_n ((SXINT)76)
#define SEQUENCE_n ((SXINT)77)
#define SET_n ((SXINT)78)
#define SHIFT_REDUCE_n ((SXINT)79)
#define SLICE_n ((SXINT)80)
#define STRING_n ((SXINT)81)
#define STRING_DENOTATION_n ((SXINT)82)
#define SYNONYM_S_n ((SXINT)83)
#define TOKEN_n ((SXINT)84)
#define TOKEN_DEF_n ((SXINT)85)
#define TOKEN_REF_S_n ((SXINT)86)
#define TOKEN_S_n ((SXINT)87)
#define TRANS_CLOSURE_n ((SXINT)88)
#define UNBOUNDED_CONTEXT_n ((SXINT)89)
#define UNBOUNDED_RESTRICTED_CONTEXT_n ((SXINT)90)
#define UPPER_CASE_n ((SXINT)91)
#define UPPER_TO_LOWER_n ((SXINT)92)
#define VOID_n ((SXINT)93)
#define WORD_LENGTH_n ((SXINT)94)
/*
E N D   N O D E   N A M E S
*/
