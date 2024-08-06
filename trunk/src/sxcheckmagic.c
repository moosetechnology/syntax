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

char WHAT_SXCHECKMAGIC[] = "@(#)SYNTAX - $Id: sxcheckmagic.c 3975 2024-05-27 12:27:34Z garavel $" WHAT_DEBUG;

void	sxcheck_magic_number (SXUINT m1, SXUINT m2, char *who)
{
    if (m1 == m2) {
	return;
    }

    fprintf (sxstderr, "\n%s: Tables garbled (code %lu vs %lu)\n", who, m1, m2);
    sxexit (SXSEVERITIES);
}
