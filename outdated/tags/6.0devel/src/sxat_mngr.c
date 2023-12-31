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









/* Actions de parcours d'arbre pour la nouvelle version de SYNTAX (TM) */






#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXAT_MNGR[] = "@(#)SYNTAX - $Id: sxat_mngr.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

SXNODE	*sxbrother (/* struct sxnode_header_s */ void *visited_void, SXINT n)
{
    SXNODE	*brother;
    SXINT	pos;
    struct sxnode_header_s *visited = (struct sxnode_header_s *) visited_void;

    if (n >= (pos = visited->position)) {
	brother = visited;
    }
    else if (n > 0) {
	brother = (visited->father)->son;
	pos = 1;
    }
    else
	return NULL;

    while (pos++ < n && brother != NULL) {
	brother = brother->brother;
    }

    return brother;
}



SXNODE	*sxson (/* struct sxnode_header_s */ void *visited, SXINT n)
{
    SXNODE	*son = ((struct sxnode_header_s*)visited)->son;
    SXINT	pos = 1;

    while (pos++ < n && son != NULL) {
	son = son->brother;
    }

    return son;
}
