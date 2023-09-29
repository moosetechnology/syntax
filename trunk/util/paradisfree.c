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

#include "PP_tables.h"

char WHAT_PARADISFREE[] = "@(#)SYNTAX - $Id: paradisfree.c 3146 2023-05-02 12:21:39Z garavel $" WHAT_DEBUG;


SXVOID	paradis_free (struct PP_ag_item *PP_ag)
{
    if (PP_ag->SXPP_schema != NULL) {
	sxfree (PP_ag->SXPP_schema), PP_ag->SXPP_schema = NULL;
    }

    if (PP_ag->PP_indx != NULL) {
	sxfree (PP_ag->PP_indx), PP_ag->PP_indx = NULL;
    }
}
