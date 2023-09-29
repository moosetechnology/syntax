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
#define ppt_num_version 2

struct PP_constants {
	SXINT PP_indx_lgth, PP_schema_lgth;
	};

struct PP_ag_item {
	long		bnf_modif_time;

	struct PP_constants	PP_constants;

	SXINT		*PP_indx;		/* 1:PP_indx_lgth */

	struct SXPP_schema	*SXPP_schema;	/* 1:PP_schema_lgth */
	};

extern SXBOOLEAN paradis_write(struct PP_ag_item *PP_ag, char *langname);
extern SXVOID    paradis_free (struct PP_ag_item *PP_ag);
