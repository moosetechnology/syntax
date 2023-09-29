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

/* THIS FILE IS NOT PART OF THE SYNTAX LIBRARY */

/*
   D�claration de toutes les variables globales de SYNTAX, pour �viter
   les duplicatas (si, "s" au pluriel, voir ��les rectifications de
   l'orthographe du fran�ais�� de 1990).
*/
/* commentaire ci-dessus incompr�hensible, et laiss� pour investigations ult�rieures */

#define SX_DFN_EXT_VAR	/* C'est �a qui fait tout */
#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h"

char WHAT_SXVARS[] = "@(#)SYNTAX - $Id: sxvars.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;
