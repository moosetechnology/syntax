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

char WHAT_SXS_SRECOVERY32[] = "@(#)SYNTAX - $Id: sxs_srcvr32.c 4082 2024-06-20 11:42:05Z garavel $ SXS_SRCVR_32" /* WHAT_DEBUG */;

#define SXP_MAX (SHRT_MAX+1)
#define SXS_MAX (SHRT_MAX+1)
#define VARIANT_32 1

#include "sxs_srcvr.c"
