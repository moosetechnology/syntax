
#include	<stdio.h>

#ifndef BOOLEAN
# define BOOLEAN		char
# define FALSE			(BOOLEAN) 0
# define TRUE			(BOOLEAN) 1	/* (!FALSE) */
#endif

extern SXBA			sxba_calloc (), sxba_resize ();

extern void			XT_alloc (); /* (header_ptr,
						name,
						init_elem_nb,
						average_list_nb_per_bucket,
						average_list_size,
						oflw,
						stat_file) */
extern void			XT_free (); /* (header_ptr) */
extern BOOLEAN			XT_set (); /* (header_ptr, list, &result) */
extern int			XT_list_lgth (); /* (header_ptr, list) */

#define XT_erase(h,l)		X_erase((h).list,l)
#define XT_clear(h)		(XxY_clear (&((h).tree)), X_clear (&((h).list)))
#define XT_stat(f,h)		(XxY_stat (f,&((h).tree)), X_stat (f,&((h).list) ))
#define XT_push(h,p,x,r)	XxY_set (&((h).tree), p, x, r)
#define XT_is_set(h,l)		(X_is_set (&((h).list, l)))

#define XT_size(h)		X_size ((h).list)
#define XT_top(h)		X_top ((h).list)
#define XT_list_size(h)		XxY_size ((h).tree)
#define XT_list_top(h)		XxY_top ((h).tree)
#define XT_lock(h)		(XxY_lock (&((h).tree)), X_lock (&((h).list)))
#define XT_unlock(h)		(XxY_unlock ((h).tree), X_unlock ((h).list) )
#define XT_list_elem(h,x)	XxY_Y ((h).tree, x)
#define XT_foreach(h,x)		for (x=X_X((h).list,x);x>0;x=XxY_X((h).tree,x))

typedef struct {
    XxY_header		tree;
    X_header		list;
    SXBA		unreachable_set;
    int			unreachable_set_size;
} XT_header;



