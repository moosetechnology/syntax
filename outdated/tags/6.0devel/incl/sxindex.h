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







/* This package allows the handling of indexes for vectors or matrices.
   Vector indexes are integers and Matrix indexes are couples of integers
   ( V [i] or M [i] [j]).
   Brand new indexes can be seeked or already used indexes can be released.
   The user can manage the (concurrent) arrays (V or M), since he is warned when an
   overflow occurs.
   In the Matrix case, when an overflow occurs, the number of lines is increased. */





#ifndef sxindex_h
#define sxindex_h
#include "SXante.h"

#include "sxalloc.h"
#include <stdio.h>

typedef struct sxindex_header sxindex_header;

typedef void (*sxoflw2_t) (struct sxindex_header *, SXINT, SXINT);

struct sxindex_header {
    SXINT		*set;
    SXINT		size, index;
    SXINT		mask, shift;
    SXINT		line_nb, column_nb;
    SXINT		free;
    void	(*oflw) (sxindex_header *, SXINT, SXINT);
    SXBOOLEAN	is_static;
} /* sxindex_header */ ;

extern void	sxindex_alloc (sxindex_header *header,
			       SXINT line_nb,
			       SXINT column_nb,
			       void (*user_oflw) (sxindex_header *, SXINT, SXINT)); /* i.e., sxoflw2_t user_oflw */
/* If line_nb equals zero, we are in the Vector case. */

extern SXINT	sxindex_seek (sxindex_header *header);
extern SXBOOLEAN	sxindex_write (sxindex_header *header, sxfiledesc_t file_descr);
extern SXBOOLEAN	sxindex_read (sxindex_header *header,
			      sxfiledesc_t file_descr,
			      void (*user_oflw) (sxindex_header *, SXINT, SXINT));
extern void	sxindex_array_to_c (sxindex_header *header, FILE *file, char *name);
extern void	sxindex_header_to_c (sxindex_header *header, FILE *file, char *name);
extern void	sxindex_to_c (sxindex_header *header, FILE *file, char *name, SXBOOLEAN is_static);
extern void	sxindex_reuse (sxindex_header *header,
			       void (*oflw) (sxindex_header *, SXINT, SXINT));

#define sxindex_free(h)		sxfree ((h).set), (h).set = NULL
#define sxindex_clear(h)	((h).free = -1, (h).index = 0)
#define sxindex_i(h,x)		((x) >> (h).shift)
#define sxindex_j(h,x)		((x) & (h).mask)
#define sxindex_release(h,x)	((h).set [x] = (h).free, (h).free = x)
#define sxindex_is_released(h,x) (x >= 0 && x < (h).index && (h).set [x] != -2)
#define sxindex_size(h)		(h).size
#define sxindex_top(h)		(h).index
#define sxindex_line_nb(h)	(h).line_nb
#define sxindex_column_nb(h)	(h).column_nb
#define sxindex_foreach(h,x)	for(x=0;x < (h).index;x++)	\
                                   if ((h).set [x] == -2)
#define sxindex_is_static(h)	(h).is_static
#define sxindex_total_sizeof(h)	(sizeof (SXINT) * (h).size)
#define sxindex_used_sizeof(h)	(sizeof (SXINT) * (h).index)
#define sxindex_is_allocated(h) ((h).set != NULL)

#include "SXpost.h"
#endif /* sxindex_h */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
