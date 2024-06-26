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
#include "T_tables.h"

char WHAT_SEMATFREE[] = "@(#)SYNTAX - $Id: sematfree.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;


void	semat_free (struct T_ag_item *T_ag)
{
    if (T_ag->SXT_node_info != NULL) {
	sxfree (T_ag->SXT_node_info), T_ag->SXT_node_info = NULL;
    }

    if (T_ag->T_stack_schema != NULL) {
	sxfree (T_ag->T_stack_schema), T_ag->T_stack_schema = NULL;
    }

    if (T_ag->T_nns_indx != NULL) {
	sxfree (T_ag->T_nns_indx), T_ag->T_nns_indx = NULL;
    }

    if (T_ag->T_ter_to_node_name != NULL) {
	sxfree (T_ag->T_ter_to_node_name), T_ag->T_ter_to_node_name = NULL;
    }

    if (T_ag->T_node_name_string != NULL) {
	sxfree (T_ag->T_node_name_string), T_ag->T_node_name_string = NULL;
    }
}
