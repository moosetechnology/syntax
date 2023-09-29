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


#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXVOID[] = "@(#)SYNTAX - $Id: sxvoid.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

void
sxvoid (void)
/* procedure ne faisant rien */
{
}

SXBOOLEAN
sxbvoid (void)
/* procedure ne faisant rien */
{
  return SXFALSE;
}

SXINT
sxivoid (void)
/* procedure ne faisant rien */
{
  return 0;
}

SXINT
sxivoid_int (SXINT i)
/* procedure ne faisant rien */
{
  sxuse (i);
  return 0;
}
