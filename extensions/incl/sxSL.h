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
/* Permet de stocker des listes (ou des ensembles) et de leur assigner
   un identifiant (entier >= 0) NON unique.
   Fonctionnalites semblables a XH.
*/
   
/*
Avant de faire:
#include "sxSL.h"

il faut definir le type

typedef struct {
   ...
} sxSL_elems;

des elements manipule's
*/

/*
   Une liste est identifie'e par un entier positif ou nul affecte' au
   parametre l de sxSL_seek (h, l).
   Une liste de longueur l dont la taille des elements est n octets
   occupe a peu pres
   (3*sizeof (SXINT) + l*n) octets.

   Comparaison (sur 10000 listes aleatoires de taille max 200 prenant leurs
                elements sur [0..9999]):
                XH	XT	sxlist	sxsubset	sxSL
   Temps	1.2	7.3	2.7	2.4		1
   Place	1	4.1	3.0	3.1		1

   Avantages :
      - Re'cupere effectivement la place occupe'e par les listes qui ont e'te'
        "released", sans changer les identifiants des listes actives.
      - Traite' uniquement par macros (sauf sxSL_GC ()

   Inconve'nients :
      - Ne supporte pas la construction simultane'e de plusieurs listes.
      - Ne partage aucune partie commune

*/
#ifndef sxSL_h
#define sxSL_h

#include "sxalloc.h"

typedef struct sxSL_header sxSL_header;

struct sxSL_header {
    struct sxSL_display {
	SXINT	sorted,
	        X,
	        lgth;
    }			*display;

    sxSL_elems	*elems;

    void		(*move) (sxSL_header *header, 
				 SXINT old_indx,
				 SXINT new_indx, 
				 SXINT lgth );

    SXINT			(*oflw) (sxSL_header *header, SXINT new_size);

    SXINT			sorted_top,
                        display_top,
                        display_size,
                        elems_top,
                        elems_size,
                        free;

    bool		has_erased;
} /* sxSL_header */ ;


#define sxSL_7F	SXINT_MAX
#define sxSL_80	(~sxSL_7F)


/* (h,n,s,o,m) == (header, list_nb, average_list_size, SXINT (*oflw)(header, new_size),
                   void (*move)(header, old_indx, new_indx, lgth)) */
/* User procedure oflw is executed when a list overflow occurs
   User procedure move is called when on GCs */
/* Example for move: */
#if 0
static void move (header, x, y, lgth)
    sxSL_header	*header;
    SXINT			x, y, lgth;
{
    /* La liste list est reimplantee en y */
    /* Aucun debordement, on peut utiliser des pointeurs. */
    SXINT	d;
    sxSL_elems	*p, *q;

    p = &(sxSL_elem (*header, x));
    q = &(sxSL_elem (*header, y));

    for (d = 0; d < lgth; d++)
	*q++ = *p++;
}
#endif

#define sxSL_alloc(h,n,s,o,m)							\
          (h).display = (struct sxSL_display*) sxalloc ((h).display_size = n,	\
					   sizeof (struct sxSL_display)),	\
          (h).sorted_top = (h).display_top = 0,					\
          (h).elems = (sxSL_elems*) sxalloc ((h).elems_size = n*s,		\
					      sizeof (sxSL_elems)),		\
          (h).elems_top = 0,							\
          (h).free = -1,							\
          (h).oflw = o,								\
          (h).move = m,								\
          (h).has_erased = false

/* Free the whole structure */
#define sxSL_free(h)		(sxfree ((h).display), (h).display = NULL,	\
				 sxfree ((h).elems), (h).elems = NULL)


/* Seek a new list identifier in l */
#define sxSL_seek(h,l)								\
        (((l = (h).display_top) < (h).display_size				\
	  ? (h).display_top++ 							\
	  : ((l = (h).free) != -1						\
            ? (h).free = (h).display [l].X					\
	    : (sxSL_GC (&(h))							\
	      ? NULL								\
              : ((h).display = (struct sxSL_display*)				\
				 sxrealloc ((h).display,			\
					    (h).display_size *= 2,		\
					    sizeof (struct sxSL_display)),	\
	       (((h).oflw != NULL)						\
	         ? (*(h).oflw) ((h).display_size)				\
                 : NULL),							\
	      l = (h).display_top++)))),					\
         (h).display [(h).sorted_top++].sorted = l,				\
	 (h).display [l].X = (h).elems_top,					\
	 (h).display [l].lgth = 0)

/* Put a (new) element v in list l */
#define sxSL_put(h,l,v)								\
        (((h).elems_top==(h).elems_size						\
	  ? (h).elems = (sxSL_elems*) sxrealloc ((h).elems,			\
						    (h).elems_size *= 2,	\
						    sizeof (sxSL_elems))	\
	  : NULL),								\
	 (h).display [l].lgth++,						\
	 (h).elems[(h).elems_top++] = v)


/* Empty the structure */
#define sxSL_clear(h)								\
         (h).sorted_top = (h).display_top = (h).elems_top = 0,			\
         (h).free = -1,								\
         (h).has_erased = false

/* Length of list l */
#define sxSL_lgth(h,l)		(h).display [l].lgth

/* (maximal) list number */
#define sxSL_size(h)		(h).display_size

/* (maximal) cumulated list elem number */
#define sxSL_elems_size(h)	(h).elems_size

/* list number */
#define sxSL_top(h)		(h).display_top

/* cumulated list elem number */
#define sxSL_elems_top(h)	(h).elems_top

/* Index of the first element of list l */
#define sxSL_X(h,l)		(h).display [l].X

/* Reference to element b */
#define sxSL_elem(h,b)		(h).elems [b]

/* Model for scanning list l with possible reallocation within the body
#define sxSL_foreach(h,l,d)	for (d = sxSL_lgth(h,l) - 1;			\
                                     d >= 0;					\
                                     d--) {					\
                                        sxSL_elem (h, sxSL_X (h, l) + d)	\
					...					\
				     }
*/

/* Erase (list l */
#define sxSL_erase(h,l)		((h).display [l].X |= sxSL_80,			\
                                (h).has_erased = true)

/* Is list l erased ? */
#define sxSL_is_erased(h,l)	((h).display [l].X & sxSL_80)




extern bool			sxSL_GC (sxSL_header *header);

extern void 			sxSL_stat (sxSL_header	*header, 
					   FILE		*file,
					   SXINT	sizeof_elem );

#endif

