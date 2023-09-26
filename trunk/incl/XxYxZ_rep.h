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
/* ------------------------- Include File XxYxZ_rep.h ------------------------- */

#ifndef XxYxZ_rep
#define XxYxZ_rep

/* Representation of the Abstract Data Type XxYxZ */

/* Definition of XxY_header and X_header */
#include		"XxY.h"

#define XxYxZ_7F		X_root_7F
#define XxYxZ_80		X_root_80

#if SXBITS_PER_LONG>=64
#define XxYxZ_HASH_FUN(x,y,z)	((x) ^ (y << 21) ^ (z << 42))
#else /* SXBITS_PER_LONG<64 */
#define XxYxZ_HASH_FUN(x,y,z)	((x) ^ (y << 10) ^ (z << 20))
#endif /* SXBITS_PER_LONG */


struct XxYxZ_elem {
    SXINT X, Y, Z;
};


typedef struct {
    X_root_header_s	X_VOID_NAME;
    struct XxYxZ_elem	*display;
    X_header		*X_hd [3];
    XxY_header		*XxY_hd [3];
    SXINT			*lnk_hd [6], *lnk [6];
    SXBOOLEAN		X_hd_is_static [3], XxY_hd_is_static [3];
} XxYxZ_header;

#endif
