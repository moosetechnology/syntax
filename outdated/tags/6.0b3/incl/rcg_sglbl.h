#ifndef rcg_sglbl_h
#define rcg_sglbl_h

/* rcg_sglbl.h */

/* Options des analyseurs RCG (positionnes par RCG_main). */

BOOLEAN			is_default_semantics, is_print_time, is_no_semantics, is_parse_tree_number;
BOOLEAN			is_guiding, is_full_guiding;
SXINT			forest_level, debug_level, best_tree_number;

#define TIME_INIT	0
#define TIME_FINAL	1

#define	FL_source		((SXINT)1)
#define	FL_clause		((SXINT)2)
#define	FL_lhs_prdct		((SXINT)4)
#define	FL_lhs_clause		((SXINT)8)
#define	FL_rhs			((SXINT)(16+4))
#define	FL_n			((SXINT)32)
#define	FL_supertagger		((SXINT)64)

#endif

#ifndef CURRENT_RCG_PARSER_RELEASE
#define CURRENT_RCG_PARSER_RELEASE ((SXINT)4)
#endif
