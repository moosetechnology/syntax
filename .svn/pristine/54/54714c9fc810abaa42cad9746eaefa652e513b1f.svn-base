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

typedef struct {
    int		*set;
    int		size, index;
    int		mask, shift;
    int		line_nb, column_nb;
    int		free;
    void	(*oflw) (/* sxindex_header *header */);
    BOOLEAN	is_static;
} sxindex_header;


extern void	sxindex_alloc (/* sxindex_header *header,
				  int line_nb, column_nb,
				  void (*user_oflw) (sxindex_header *header,
				                     int old_line_nb,
				                     int old_size) */);
/* If line_nb equals zero, we are in the Vector case. */

extern int	sxindex_seek (/* sxindex_header *header */);
extern BOOLEAN	sxindex_write (/* sxindex_header *header;
				  int file */);
extern BOOLEAN	sxindex_read (/* sxindex_header *header;
				 int file */);
extern void	sxindex_array_to_c (/* sxindex_header *header;
				       FILE *file;
				       char *name */);
extern void	sxindex_header_to_c (/* sxindex_header *header;
					FILE *file;
					char *name */);
extern void	sxindex_to_c (/* sxindex_header *header;
				 FILE *file;
				 char *name;
				 BOOLEAN is_static */);
extern void	sxindex_reuse (/* sxindex_header *header;
				  void (*oflw) () */);

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
#define sxindex_total_sizeof(h)	(sizeof (int) * (h).size)
#define sxindex_used_sizeof(h)	(sizeof (int) * (h).index)

#include "SXpost.h"
#endif /* sxindex_h */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
