/* ------------------------- Include File XxYxZ_rep.h ------------------------- */

#ifndef XxYxZ_rep
#define XxYxZ_rep

/* Representation of the Abstract Data Type XxYxZ */

/* Definition of XxY_header and X_header */
#include		"XxY.h"

#define XxYxZ_7F		X_root_7F
#define XxYxZ_80		X_root_80

#if SXBITS_PER_LONG>=64
#define XxYxZ_HASH_FUN(x,y,z)	((x) ^ (y << 21) ^ (z << 42))
#else /* SXBITS_PER_LONG<64 */
#define XxYxZ_HASH_FUN(x,y,z)	((x) ^ (y << 10) ^ (z << 20))
#endif /* SXBITS_PER_LONG */


struct XxYxZ_elem {
    SXINT X, Y, Z;
};


typedef struct {
    X_root_header_s	X_VOID_NAME;
    struct XxYxZ_elem	*display;
    X_header		*X_hd [3];
    XxY_header		*XxY_hd [3];
    SXINT			*lnk_hd [6], *lnk [6];
    BOOLEAN		X_hd_is_static [3], XxY_hd_is_static [3];
} XxYxZ_header;

#endif
