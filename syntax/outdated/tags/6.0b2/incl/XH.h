#ifndef XH_h
#define XH_h

#include	<stdio.h>
#include        "sxalloc.h"

extern void			XH_alloc (); /* (header_ptr,
						name,
						init_elem_nb,
						average_list_nb_per_bucket,
						average_list_size,
						oflw,
						stat_file) */
extern void			XH_free (); /* (header_ptr) */
extern void			XH_stat (); /* (stat_file, header_ptr) */
extern void			XH_clear (); /* (header_ptr) */
extern void			XH_lock (); /* (header_ptr) */
extern int			XH_is_set (); /* (header_ptr) */
extern BOOLEAN			XH_set (); /* (header_ptr, result_ptr) */
extern BOOLEAN			XH_oflw (); /* (header_ptr) */
extern void			XH_pack (); /* (header_ptr,
					       from_value,
					       swap_func) */
extern void                     XH2c (); /* (header_ptr, output_file, name, is_static) */

#define XH_7F			(((unsigned int) (~0))>>1)
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


struct XH_elem {
    int			lnk;
    unsigned int	scrmbl;
    int			X;
};

typedef struct {
    char		*name;
    int			*hash_display;
    struct XH_elem	*display;
    int			*list;
    int			hash_size, list_size;
    unsigned int	scrmbl;
    int			(*oflw) ();
    FILE		*stat_file;
    BOOLEAN		is_locked;
    BOOLEAN		is_allocated;
} XH_header;

#endif
