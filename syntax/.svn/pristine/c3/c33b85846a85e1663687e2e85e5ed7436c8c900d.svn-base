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
#define att_num_version 2

struct T_constants {
	SXINT	T_nbpro,
		T_nbnod,
		T_stack_schema_size,
		T_node_name_string_lgth,
		T_ter_to_node_name_size;
	};

struct T_ag_item {
	long		bnf_modif_time;

	struct T_constants	T_constants;

	struct SXT_node_info	*SXT_node_info;	/* 1:T_nbpro+1 */

	SXINT		*T_stack_schema,	/* 1:T_stack_schema_size */
			*T_nns_indx,	/* 0:T_nbnod+1 */
			*T_ter_to_node_name;/* 0:T_ter_to_node_name_size */

	char		*T_node_name_string;
	};
