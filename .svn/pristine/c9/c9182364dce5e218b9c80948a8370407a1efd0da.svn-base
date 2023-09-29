/* ------------------------- Include File X_root_rep.h ------------------------- */

#ifndef X_root_rep
#define X_root_rep

/* Representation of the Abstract Data Type X_root */

#include	<stdio.h>
#include	"sxalloc.h"


#define X_root_7F		(((SXUINT) (~0))>>1)
#define X_root_80		(~X_root_7F)

#define XYZ_X			00
#define XYZ_Y			01
#define XYZ_Z			02
#define XYZ_XY			03
#define XYZ_XZ			04
#define XYZ_YZ			05
#define XYZ_XYZ			06


#define X_root_erase(h,x)	(h).hash_lnk[x] |= (h).has_free_buckets = X_80
#define X_root_is_erased(h,x)	((h).hash_lnk[x] & X_80)
#define X_root_is_static(h)	((h).is_static)
#define X_root_is_allocated(h)	((h).is_allocated)
#define X_root_size(h)		((h).size)
#define X_root_top(h)		((h).top)
#define X_root_X(h,x)		((h).display[x].X)
#define X_root_Y(h,x)		((h).display[x].Y)
#define X_root_Z(h,x)		((h).display[x].Z)
#define X_root_unlock(h)	((h).is_locked = FALSE)
#define X_root_foreach(h,x)	for (x=X_root_top(h);x>0;x--) \
                                   if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_Xforeach(h,i,x)	for (x=(h).lnk_hd[XYZ_X][X_is_set((h).X_hd[XYZ_X],i)];\
				     x!=0;x=(h).lnk[XYZ_X][x]) \
                                   if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_Yforeach(h,i,x)	for (x=(h).lnk_hd[XYZ_Y][X_is_set((h).X_hd[XYZ_Y],i)];\
				     x!=0;x=(h).lnk[XYZ_Y][x]) \
                                   if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_Zforeach(h,i,x)	for (x=(h).lnk_hd[XYZ_Z][X_is_set((h).X_hd[XYZ_Z],i)];\
				     x!=0;x=(h).lnk[XYZ_Z][x]) \
                                   if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_XYforeach(h,i,j,x)	for (x=(h).lnk_hd[XYZ_XY]\
					     [XxY_is_set(((h).XxY_hd-3)[XYZ_XY],i,j)];\
					     x!=0;x=(h).lnk[XYZ_XY][x]) \
                                           if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_XZforeach(h,i,j,x)	for (x=(h).lnk_hd[XYZ_XZ]\
					     [XxY_is_set(((h).XxY_hd-3)[XYZ_XZ],i,j)];\
					     x!=0;x=(h).lnk[XYZ_XZ][x]) \
                                           if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_YZforeach(h,i,j,x)	for (x=(h).lnk_hd[XYZ_YZ]\
					     [XxY_is_set(((h).XxY_hd-3)[XYZ_YZ],i,j)];\
					     x!=0;x=(h).lnk[XYZ_YZ][x]) \
                                           if (((h).hash_lnk[x] & X_root_80) == 0)
#define X_root_Xtotal_sizeof(h)						\
   (									\
    ((h).hash_display == NULL ? 0 : sizeof(SXINT)*(h).hash_size)+		\
    ((h).hash_lnk == NULL ? 0 : sizeof(SXINT)*(h).size)+ 			\
    ((h).hash_display == NULL ? 0 : sizeof(struct X_elem)*((h).size+1))	\
   )

#define X_root_Xused_sizeof(h)						\
   (									\
    ((h).hash_display == NULL ? 0 : sizeof(SXINT)*(h).hash_size)+		\
    ((h).hash_lnk == NULL ? 0 : sizeof(SXINT)*(h).top)+			\
    ((h).hash_display == NULL ? 0 : sizeof(struct X_elem)*((h).top+1))	\
   )

#define X_root_XYtotal_sizeof(h)						\
   (										\
    ((h).hash_display == NULL ? 0 : sizeof(SXINT)*(h).hash_size)+			\
    ((h).hash_lnk == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).display == NULL ? 0 : sizeof(struct XxY_elem)*((h).size+1))+		\
    (2*sizeof (X_header))+							\
    ((h).X_hd [0] == 0 ? 0 : X_root_Xtotal_sizeof(*((h).X_hd [0])))+		\
    ((h).X_hd [1] == 0 ? 0 : X_root_Xtotal_sizeof(*((h).X_hd [1])))+		\
    ((h).lnk_hd [0] == NULL ? 0 : sizeof(SXINT)*(X_size(*((h).X_hd [0]))+1))+	\
    ((h).lnk_hd [1] == NULL ? 0 : sizeof(SXINT)*(X_size(*((h).X_hd [1]))+1))+	\
    ((h).lnk [0] == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).lnk [1] == NULL ? 0 : sizeof(SXINT)*((h).size+1))			\
   )

#define X_root_XYused_sizeof(h)							\
   (										\
    ((h).hash_display == NULL ? 0 : sizeof(SXINT)*(h).hash_size)+			\
    ((h).hash_lnk == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).display == NULL ? 0 : sizeof(struct XxY_elem)*((h).top+1))+		\
    (2*sizeof (X_header))+							\
    ((h).X_hd [0] == 0 ? 0 : X_root_Xused_sizeof(*((h).X_hd [0])))+		\
    ((h).X_hd [1] == 0 ? 0 : X_root_Xused_sizeof(*((h).X_hd [1])))+		\
    ((h).lnk_hd [0] == NULL ? 0 : sizeof(SXINT)*(X_top(*((h).X_hd [0]))+1))+	\
    ((h).lnk_hd [1] == NULL ? 0 : sizeof(SXINT)*(X_top(*((h).X_hd [1]))+1))+	\
    ((h).lnk [0] == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).lnk [1] == NULL ? 0 : sizeof(SXINT)*((h).top+1))				\
   )


#define X_root_XYZtotal_sizeof(h)						\
   (										\
    ((h).hash_display == NULL ? 0 : sizeof(SXINT)*(h).hash_size)+			\
    ((h).hash_lnk == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).display == NULL ? 0 : sizeof(struct XxYxZ_elem)*((h).size+1))+		\
    (3*sizeof (X_header))+							\
    ((h).X_hd [0] == 0 ? 0 : X_root_Xtotal_sizeof(*((h).X_hd [0])))+		\
    ((h).X_hd [1] == 0 ? 0 : X_root_Xtotal_sizeof(*((h).X_hd [1])))+		\
    ((h).X_hd [2] == 0 ? 0 : X_root_Xtotal_sizeof(*((h).X_hd [2])))+		\
    (3*sizeof (XxY_header))+							\
    ((h).XxY_hd [0] == 0 ? 0 : X_root_XYtotal_sizeof(*((h).XxY_hd [0])))+	\
    ((h).XxY_hd [1] == 0 ? 0 : X_root_XYtotal_sizeof(*((h).XxY_hd [1])))+	\
    ((h).XxY_hd [2] == 0 ? 0 : X_root_XYtotal_sizeof(*((h).XxY_hd [2])))+	\
    ((h).lnk_hd [0] == NULL ? 0 : sizeof(SXINT)*(X_size(*((h).X_hd [0]))+1))+	\
    ((h).lnk_hd [1] == NULL ? 0 : sizeof(SXINT)*(X_size(*((h).X_hd [1]))+1))+	\
    ((h).lnk_hd [2] == NULL ? 0 : sizeof(SXINT)*(X_size(*((h).X_hd [2]))+1))+	\
    ((h).lnk_hd [3] == NULL ? 0 : sizeof(SXINT)*(XxY_size(*((h).XxY_hd [0]))+1))+	\
    ((h).lnk_hd [4] == NULL ? 0 : sizeof(SXINT)*(XxY_size(*((h).XxY_hd [1]))+1))+	\
    ((h).lnk_hd [5] == NULL ? 0 : sizeof(SXINT)*(XxY_size(*((h).XxY_hd [2]))+1))+	\
    ((h).lnk [0] == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).lnk [1] == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).lnk [2] == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).lnk [3] == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).lnk [4] == NULL ? 0 : sizeof(SXINT)*((h).size+1))+			\
    ((h).lnk [5] == NULL ? 0 : sizeof(SXINT)*((h).size+1))			\
   )

#define X_root_XYZused_sizeof(h)						\
   (										\
    ((h).hash_display == NULL ? 0 : sizeof(SXINT)*(h).hash_size)+			\
    ((h).hash_lnk == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).display == NULL ? 0 : sizeof(struct XxYxZ_elem)*((h).top+1))+		\
    (3*sizeof (X_header))+							\
    ((h).X_hd [0] == 0 ? 0 : X_root_Xused_sizeof(*((h).X_hd [0])))+		\
    ((h).X_hd [1] == 0 ? 0 : X_root_Xused_sizeof(*((h).X_hd [1])))+		\
    ((h).X_hd [2] == 0 ? 0 : X_root_Xused_sizeof(*((h).X_hd [2])))+		\
    (3*sizeof (XxY_header))+							\
    ((h).XxY_hd [0] == 0 ? 0 : X_root_XYused_sizeof(*((h).XxY_hd [0])))+	\
    ((h).XxY_hd [1] == 0 ? 0 : X_root_XYused_sizeof(*((h).XxY_hd [1])))+	\
    ((h).XxY_hd [2] == 0 ? 0 : X_root_XYused_sizeof(*((h).XxY_hd [2])))+	\
    ((h).lnk_hd [0] == NULL ? 0 : sizeof(SXINT)*(X_top(*((h).X_hd [0]))+1))+	\
    ((h).lnk_hd [1] == NULL ? 0 : sizeof(SXINT)*(X_top(*((h).X_hd [1]))+1))+	\
    ((h).lnk_hd [2] == NULL ? 0 : sizeof(SXINT)*(X_top(*((h).X_hd [2]))+1))+	\
    ((h).lnk_hd [3] == NULL ? 0 : sizeof(SXINT)*(XxY_top(*((h).XxY_hd [0]))+1))+	\
    ((h).lnk_hd [4] == NULL ? 0 : sizeof(SXINT)*(XxY_top(*((h).XxY_hd [1]))+1))+	\
    ((h).lnk_hd [5] == NULL ? 0 : sizeof(SXINT)*(XxY_top(*((h).XxY_hd [2]))+1))+	\
    ((h).lnk [0] == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).lnk [1] == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).lnk [2] == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).lnk [3] == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).lnk [4] == NULL ? 0 : sizeof(SXINT)*((h).top+1))+			\
    ((h).lnk [5] == NULL ? 0 : sizeof(SXINT)*((h).top+1))				\
   )


/* to have the definition of X_root_header_s look like normal C */
#define X_VOID_NAME

#define X_root_header_s				\
    char		*name;			\
    SXINT			*hash_display;		\
    SXINT			*hash_lnk;		\
    SXINT			hash_size;		\
    SXINT			top;			\
    SXINT			size;			\
    SXINT			free_buckets;		\
    SXINT			has_free_buckets;	\
    void			(*system_oflw) (SXINT);	\
    void			(*user_oflw) (SXINT,SXINT);	\
    SXINT			(*cmp) (SXINT);		\
    SXUINT                      (*scrmbl) (SXINT);	\
    SXINT			(*suppress) (SXINT);	\
    SXINT			(*assign) (SXINT);	\
    FILE		*stat_file;		\
    BOOLEAN		is_locked;		\
    BOOLEAN		is_static;		\
    BOOLEAN		is_allocated

typedef struct {
    X_root_header_s	X_VOID_NAME;
} X_root_header;

#endif
