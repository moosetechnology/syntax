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
#include "varstr.h"
#include "sxunix.h"

char WHAT_VARSTRALLOC[] = "@(#)SYNTAX - $Id: varstralloc.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;

VARSTR	varstr_alloc (SXINT size)
{
    VARSTR	varstr_ptr;

    varstr_ptr = (VARSTR) sxalloc (1, sizeof (struct varstr_hd));
    varstr_ptr->current = varstr_ptr->first = sxalloc (size+1, sizeof (char));
    *(varstr_ptr->first) = SXNUL;
    varstr_ptr->last = varstr_ptr->first + size;
    *(varstr_ptr->last) = SXNUL;
    return varstr_ptr;
}
