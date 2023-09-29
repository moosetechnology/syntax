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
#ifndef rcg_sglbl_h
#define rcg_sglbl_h
#include "SXante.h"

#ifndef SX_GLOBAL_VAR_RCG
#  ifdef SX_DFN_EXT_VAR_RCG
/* Cas particulier : Il faut DÉFINIR les variables globales. */
#    define SX_GLOBAL_VAR_RCG   /* rien */
#    define SX_INIT_VAL_RCG(v)  = v
#    define SX_DFN_EXT_VAR_RCG2
#  else
/* Cas général : Ce qu'on veut, c'est juste DÉCLARER les variables globales, pas les DÉFINIR. */
#    define SX_GLOBAL_VAR_RCG   extern
#    define SX_INIT_VAL_RCG(v)  /*rien*/
#  endif /* #ifndef SX_DFN_EXT_VAR_RCG */
#endif /* #ifndef SX_GLOBAL_VAR_RCG */

/* rcg_sglbl.h */

/* Options des analyseurs RCG (positionnes par RCG_main). */

SX_GLOBAL_VAR SXBOOLEAN			is_default_semantics, is_print_time, is_no_semantics, is_parse_tree_number;
SX_GLOBAL_VAR SXBOOLEAN			is_guiding, is_full_guiding;
SX_GLOBAL_VAR SXINT			forest_level, debug_level, best_tree_number;
SX_GLOBAL_VAR FILE                      *dag_yield_file;			
SX_GLOBAL_VAR char                      *dag_yield_file_name;			

#define TIME_INIT	0
#define TIME_FINAL	1

#define	FL_source		((SXINT)1)
#define	FL_clause		((SXINT)2)
#define	FL_lhs_prdct		((SXINT)4)
#define	FL_lhs_clause		((SXINT)8)
#define	FL_rhs			((SXINT)(16+4))
#define	FL_n			((SXINT)32)
#define	FL_supertagger		((SXINT)64)


#define NL_AND                  ((SXINT)0X10000)
#define NL_CAT                  ((SXINT)0X20000)
#define NL_OR                   ((SXINT)0X30000)
#define NL_LOAD                 ((SXINT)0X40000)
#define NL_DUP                  ((SXINT)0X50000)
#define NL_TERM                 ((SXINT)0X60000)
#define NL_OR_START             ((SXINT)0X70000)

#endif

#ifndef CURRENT_RCG_PARSER_RELEASE
#define CURRENT_RCG_PARSER_RELEASE ((SXINT)7)
#endif
