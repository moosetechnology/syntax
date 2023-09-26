/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   * Produit de l'équipe ATOLL (Langages et Traducteurs). *
   *							  *
   ******************************************************** */



/* This package allows the handling of indexes for vectors or matrices.
   Vector indexes are integers and Matrix indexes are couples of integers
   ( V [i] or M [i] [j]).
   Brand new indexes can be seeked or already used indexes can be released.
   The user can manage the (concurrent) arrays (V or M), since he is warned when an
   overflow occurs.
   In the Matrix case, when an overflow occurs, the number of lines is increased. */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 13:31 (phd):	Utilisation de SX{ante,post}.h		*/
/************************************************************************/



#ifndef sxindex_h
#define sxindex_h
#include "SXante.h"

#include "sxalloc.h"
#include <stdio.h>

typedef struct sxindex_header sxindex_header;

struct sxindex_header {
    SXINT		*set;
    SXINT		size, index;
    SXINT		mask, shift;
    SXINT		line_nb, column_nb;
    SXINT		free;
    void	(*oflw) (sxindex_header *, SXINT, SXINT);
    BOOLEAN	is_static;
} /* sxindex_header */ ;


extern void	sxindex_alloc (sxindex_header *header,
			       SXINT line_nb,
			       SXINT column_nb,
			       void (*user_oflw) (sxindex_header *, SXINT, SXINT));
/* If line_nb equals zero, we are in the Vector case. */

extern SXINT	sxindex_seek (sxindex_header *header);
extern BOOLEAN	sxindex_write (sxindex_header *header, filedesc_t file_descr);
extern BOOLEAN	sxindex_read (sxindex_header *header,
			      filedesc_t file_descr,
			      void (*user_oflw) (sxindex_header *, SXINT, SXINT));
extern void	sxindex_array_to_c (sxindex_header *header, FILE *file, char *name);
extern void	sxindex_header_to_c (sxindex_header *header, FILE *file, char *name);
extern void	sxindex_to_c (sxindex_header *header, FILE *file, char *name, BOOLEAN is_static);
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
