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

/*
   Declaration de toutes les variables globales de SYNTAX, pour eviter
   les definitions dupliquees entre les fichiers .h et .c
*/

/* variables pour la partie non-déterministe */

#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"

#undef SX_GLOBAL_VAR
#undef SX_INIT_VAL
#define SX_DFN_EXT_VAR	/* C'est ça qui fait tout */

#include "udag_scanner.h"
#include "earley.h"

char WHAT_SXVARS2[] = "@(#)SYNTAX - $Id: sxvars2.c 1508 2008-08-19 15:58:13Z sagot $" WHAT_DEBUG;
