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
#ifndef XH_h
#define XH_h

#include	<stdio.h>
#include        "sxalloc.h"


struct XH_elem {
    SXINT			lnk;
    SXUINT	scrmbl;
    SXINT			X;
};

typedef struct {
    char		*name;
    SXINT			*hash_display;
    struct XH_elem	*display;
    SXINT			*list;
    SXINT			hash_size, list_size;
    SXUINT	scrmbl;
    void		(*oflw) (SXINT, SXINT);
    FILE		*stat_file;
    SXBOOLEAN		is_locked;
    SXBOOLEAN		is_allocated;
} XH_header;


extern void			XH_alloc (XH_header *header, 
					  char *name, 
					  SXINT init_elem_nb, 
					  SXINT average_list_nb_per_bucket, 
					  SXINT average_list_size, 
					  void (*oflw) (SXINT, SXINT), 
					  FILE *stat_file);
extern void			XH_free (XH_header *header);
extern void			XH_stat (FILE *stat_file, XH_header *header);
extern void			XH_clear (XH_header *header);
extern void			XH_lock (XH_header *header);
extern SXINT			XH_is_set (XH_header *header);
extern SXBOOLEAN			XH_set (XH_header *header, SXINT *result);
extern SXBOOLEAN			XH_oflw (XH_header *header);
extern void			XH_pack (XH_header *header, SXINT local_n, void (*swap_ft) (SXINT, SXINT));
extern void                     XH2c (
				      XH_header *header, 
				      FILE *F_XH, 
				      char *name, 
				      SXBOOLEAN is_static);

/*
  Les anciennes définitions de .._7F et .._80 étaient comme ca :

  #define XH_7F			(((SXUINT) (~0))>>1)
  #define XH_80			(~XH_7F)

  Le problème est que 7F a le type SXUINT, ce qui soulève des warnings lint
  lorsqu'il est associé à des variables de type SXINT

  Les nouvelles définitions ont la même valeur qu'avant, mais sont de type SXINT
  = signed long. Elles n'utilisent pas les decalages

*/

#define XH_7F SXINT_MAX
#define XH_80 (~XH_7F)

#define XH_push(h,x)		((h).scrmbl+=(h).list[*((h).list)]=x, \
				 (++*((h).list)>(h).list_size) ? XH_oflw(&(h)) : SXFALSE)
#define XH_pop(h,x)		((h).scrmbl-=x=(h).list[--*((h).list)])
#define XH_raz(h)		((h).scrmbl = 0, \
				 *((h).list) = (h).display [(h).display->lnk].X)
#define XH_erase(h,x)		(h).display[x].lnk |= XH_80
#define XH_is_erased(h,x)	((h).display[x].lnk & XH_80)
#define XH_size(h)		((h).display->X)
#define XH_top(h)		((h).display->lnk)
#define XH_unlock(h)		((h).is_locked = SXFALSE)
#define XH_X(h,x)		((h).display[x].X)
#define XH_list_top(h)		((h).list[0])
#define XH_list_size(h)		((h).list_size)
#define XH_list_elem(h,x)	((h).list[x])
#define XH_list_lgth(h,x)	(XH_X(h,(x)+1)-XH_X(h,x))
#define XH_is_allocated(h)	((h).is_allocated)


#endif
