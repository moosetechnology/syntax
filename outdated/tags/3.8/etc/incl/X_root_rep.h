/* ------------------------- Include File X_root_rep.h ------------------------- */

#ifndef X_root_rep
#define X_root_rep

/* Representation of the Abstract Data Type X_root */

#include	<stdio.h>

#ifndef BOOLEAN
# undef  FALSE
# undef  TRUE
# define BOOLEAN		char
# define FALSE			(BOOLEAN) 0
# define TRUE			(BOOLEAN) 1	/* (!FALSE) */
#endif

#define X_root_7F		(((unsigned int) (~0))>>1)
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


/* to have the definition of X_root_header_s look like normal C */
#define X_VOID_NAME

#define X_root_header_s				\
    char		*name;			\
    int			*hash_display;		\
    int			*hash_lnk;		\
    int			hash_size;		\
    int			top;			\
    int			size;			\
    int			free_buckets;		\
    int			has_free_buckets;	\
    int			(*system_oflw) ();	\
    int			(*user_oflw) ();	\
    int			(*cmp) ();		\
    unsigned int	(*scrmbl) ();		\
    int			(*suppress) ();		\
    int			(*assign) ();		\
    FILE		*stat_file;		\
    BOOLEAN		is_locked

typedef struct {
    X_root_header_s	X_VOID_NAME;
} X_root_header;

#ifdef lint

extern char *malloc (), *calloc (), *realloc ();
extern void free ();

#  define sxalloc(NB, SZ)		malloc ((unsigned)((NB)*(SZ)))
#  define sxcalloc(NB, SZ)		calloc ((unsigned)(NB), (unsigned)(SZ))
#  define sxrealloc(PTR, NB, SZ)	realloc ((char*)(PTR), (unsigned)((NB)*(SZ)))
#  define sxfree(PTR)			free ((char*)(PTR))

#else

extern char *sxcont_malloc (), *sxcont_alloc (), *sxcont_realloc ();
extern void sxcont_free ();

#  define sxalloc(NB, SZ)		sxcont_malloc ((NB)*(SZ))
#  define sxcalloc(NB, SZ)		sxcont_alloc (NB, SZ)
#  define sxrealloc(PTR, NB, SZ)	sxcont_realloc (PTR, (NB)*(SZ))
#  define sxfree(PTR)			sxcont_free (PTR)

#endif /* lint */


#endif
