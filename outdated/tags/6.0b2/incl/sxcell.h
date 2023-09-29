/* This package allows the handling of dynamic arrays of structures whose fields
   are integer cells. */

#ifndef sxcell_h
#define sxcell_h

#include "sxalloc.h"

typedef struct {
    int		*set;
    int		size, top, free, nb_of_int_per_bucket;
    int		(*oflw)();
} sxcell_header;


#define sxcell_alloc(h,n,s,o)				\
      (h).size=s,					\
      (h).nb_of_int_per_bucket=n,			\
      (h).top=0,					\
      (h).free=-1,					\
      (h).oflw=o,					\
      (h).set=(int*)sxalloc((h).size,sizeof(int)*(h).nb_of_int_per_bucket)
                                
#define sxcell_access(h)	((h).set)

#define sxcell_seek(h,x)	((h).top<(h).size?x=(h).top++:((h).free==-1? 	     \
  ((h).set=(int*)sxrealloc((h).set,(h).size*=2,sizeof(int)*(h).nb_of_int_per_bucket),\
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
