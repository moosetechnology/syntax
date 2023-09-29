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
#include "sxcommon.h"
#include "sxalloc.h"

char WHAT_SS_MNGR[] = "@(#)SYNTAX - $Id: SS_mngr.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;

SXINT *SS_alloc (SXINT size)
{
    /* Suppose que size >= 2*/
    SXINT *t;

    t = (SXINT*) sxalloc (size + 2, sizeof (SXINT)) + 1;
    t [-1] = size;
    t [0] = t [1] = 2;
    return t;
}

//#if 0
SXINT SS_sature (SXINT **t)
{
    *t = (SXINT*) sxrealloc (*t - 1, ((*t)[-1] *= 2) + 2, sizeof (SXINT)) + 1;
    return 0;
}
//#endif /* 0 */
