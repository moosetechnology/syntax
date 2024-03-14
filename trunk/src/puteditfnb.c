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
#include "put_edit.h"

char WHAT_PUTEDITFNB[] = "@(#)SYNTAX - $Id: puteditfnb.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

void	put_edit_fnb (SXINT col, SXINT f, SXINT number)
{
    size_t	len;

    sprintf (_SXPE_str, "%ld", (SXINT) number);
    _SXPE_pos (col + f - (len = strlen (_SXPE_str)));
    fputs (_SXPE_str, _SXPE_file);
    _SXPE_col += len;
}
