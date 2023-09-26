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

/* Compactage memoire */

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXGC[] = "@(#)SYNTAX - $Id: sxgc.c 3234 2023-05-15 16:52:27Z garavel $" WHAT_DEBUG;

SXVOID	sxgc (void)
/* Must return to its caller only if memory map has changed */
{
  fprintf (sxstderr, "\tNO MORE ALLOCATION POSSIBLE...%c\n", sxbell);

    if (sxgc_recovery && mem_signature_mode)
      sxgc_recovery ();
    else
      sxexit (SXSEVERITIES);
}
