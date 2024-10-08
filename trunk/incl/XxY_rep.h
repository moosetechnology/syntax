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
/* ------------------------- Include File XxY_rep.h ------------------------- */

#ifndef XxY_rep
#define XxY_rep

/* Representation of the Abstract Data Type XxY */

#include "X.h"
#include "sxba.h"

#define XxY_7F			X_root_7F
#define XxY_80			X_root_80

#if SXBITS_PER_LONG>=64
#define XxY_HASH_FUN(x,y)	((x) ^ (y << 32))
#else /* SXBITS_PER_LONG<64 */
#define XxY_HASH_FUN(x,y)	((x) ^ (y << 16))
#endif /* SXBITS_PER_LONG */

#define XY_X			00
#define XY_Y			01


struct XxY_elem {
    SXINT X, Y;
};

typedef struct {
    X_root_header_s	X_VOID_NAME;
    struct XxY_elem	*display;
    X_header		*X_hd [2];
    SXINT			*lnk_hd [2], *lnk [2];
    bool		X_hd_is_static [2];
} XxY_header;

#endif
