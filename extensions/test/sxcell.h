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
/* This package allows the handling of dynamic arrays of structures whose fields
   are integer cells. */

#ifndef sxcell_h
#define sxcell_h

#include "sxalloc.h"

typedef struct {
    SXINT		*set;
    SXINT		size, top, free, nb_of_int_per_bucket;
    SXINT		(*oflw)(SXINT);
} sxcell_header;


#define sxcell_alloc(h,n,s,o)				\
      (h).size=s,					\
      (h).nb_of_int_per_bucket=n,			\
      (h).top=0,					\
      (h).free=-1,					\
      (h).oflw=o,					\
      (h).set=(SXINT*)sxalloc((h).size,sizeof(SXINT)*(h).nb_of_int_per_bucket)
                                
#define sxcell_access(h)	((h).set)

#define sxcell_seek(h,x)	((h).top<(h).size?x=(h).top++:((h).free==-1? 	     \
  ((h).set=(SXINT*)sxrealloc((h).set,(h).size*=2,sizeof(SXINT)*(h).nb_of_int_per_bucket),\
   (h).oflw == NULL? 0 : (*(h).oflw)((h).size),					     \
   x=(h).top++):								     \
  (x=(h).free, (h).free=(h).set[(x)*(h).nb_of_int_per_bucket])))

#define sxcell_free(h)		sxfree ((h).set), (h).set = NULL
#define sxcell_clear(h)		((h).free = -1, (h).top = 0)

#define sxcell_release(h,x)	((h).set[(x)*(h).nb_of_int_per_bucket]=(h).free,     \
				 (h).free=x)
#define sxcell_size(h)		(h).size
#define sxcell_top(h)		(h).top

#endif
