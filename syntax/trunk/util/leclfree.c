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

#include "sxversion.h"
#include "sxunix.h"
#include "S_tables.h"

char WHAT_LECLFREE[] = "@(#)SYNTAX - $Id: leclfree.c 3146 2023-05-02 12:21:39Z garavel $" WHAT_DEBUG;

SXVOID	lecl_free (struct lecl_tables_s *lecl_tables_ptr)
{
    SXINT	i;

    sxfree (lecl_tables_ptr->S_is_a_keyword);
    sxfree (lecl_tables_ptr->S_is_a_generic_terminal);

    if (lecl_tables_ptr->S_check_kw != NULL)
	/* Dans LECL, c'est un VARSTR libere manu */
	sxfree (lecl_tables_ptr->S_check_kw);

    sxfree (lecl_tables_ptr->S_terlis);
    sxfree (lecl_tables_ptr->SXS_adrp);
    sxfree (lecl_tables_ptr->SXS_action_or_prdct_code);

    for (i = lecl_tables_ptr->S_tbl_size.S_last_state_no; i > 0; i--)
	sxfree (lecl_tables_ptr->S_transition_matrix [i]);

    sxfree (lecl_tables_ptr->S_transition_matrix);
    sxfree (lecl_tables_ptr->S_char_to_simple_class);
}
