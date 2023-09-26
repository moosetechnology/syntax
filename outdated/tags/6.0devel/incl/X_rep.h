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
/* ------------------------- Include File X_rep.h ------------------------- */

#ifndef X_rep
#define X_rep

/* Representation of the Abstract Data Type X */

#include	"X_root.h"

#define X_7F			X_root_7F
#define X_80			X_root_80

#define X_HASH_FUN(x)		(x)

struct X_elem {
    SXINT X;
};

typedef struct {
    X_root_header_s	X_VOID_NAME;
    struct X_elem	*display;
} X_header;

#endif

