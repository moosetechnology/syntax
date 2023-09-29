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
    BOOLEAN		is_locked;
    BOOLEAN		is_allocated;
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
extern BOOLEAN			XH_set (XH_header *header, SXINT *result);
extern BOOLEAN			XH_oflw (XH_header *header);
extern void			XH_pack (XH_header *header, SXINT local_n, void (*swap_ft) (SXINT, SXINT));
extern void                     XH2c (
				      XH_header *header, 
				      FILE *F_XH, 
				      char *name, 
				      BOOLEAN is_static);

#define XH_7F			(((SXUINT) (~0))>>1)
#define XH_80			(~XH_7F)

#define XH_push(h,x)		((h).scrmbl+=(h).list[*((h).list)]=x, \
				 (++*((h).list)>(h).list_size) ? XH_oflw(&(h)) : FALSE)
#define XH_pop(h,x)		((h).scrmbl-=x=(h).list[--*((h).list)])
#define XH_raz(h)		((h).scrmbl = 0, \
				 *((h).list) = (h).display [(h).display->lnk].X)
#define XH_erase(h,x)		(h).display[x].lnk |= XH_80
#define XH_is_erased(h,x)	((h).display[x].lnk & XH_80)
#define XH_size(h)		((h).display->X)
#define XH_top(h)		((h).display->lnk)
#define XH_unlock(h)		((h).is_locked = FALSE)
#define XH_X(h,x)		((h).display[x].X)
#define XH_list_top(h)		((h).list[0])
#define XH_list_size(h)		((h).list_size)
#define XH_list_elem(h,x)	((h).list[x])
#define XH_list_lgth(h,x)	(XH_X(h,(x)+1)-XH_X(h,x))
#define XH_is_allocated(h)	((h).is_allocated)


#endif
