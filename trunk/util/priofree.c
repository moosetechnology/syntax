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
#include "D_tables.h"

char WHAT_PRIOFREE[] = "@(#)SYNTAX - $Id: priofree.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

void	prio_free (struct priority **t_priorities, struct priority **r_priorities)
{
    if (*t_priorities != NULL) {
	sxfree (*t_priorities), *t_priorities = NULL;
    }

    if (*r_priorities != NULL) {
	sxfree (*r_priorities), *r_priorities = NULL;
    }
}
