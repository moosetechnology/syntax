/* ------------------------- Include File XxY_rep.h ------------------------- */

#ifndef XxY_rep
#define XxY_rep

/* Representation of the Abstract Data Type XxY */

#include		"X.h"

#ifndef SXBA_ELT
#define SXBA_ELT	unsigned long int
typedef SXBA_ELT	*SXBA;
#include		"sxba.h"
#endif

extern SXBA *sxbm_calloc (), *sxbm_resize ();

#define XxY_7F			X_root_7F
#define XxY_80			X_root_80

#define XxY_HASH_FUN(x,y)	((x) ^ (y << 16))

#define XY_X			00
#define XY_Y			01


struct XxY_elem {
    int X, Y;
};

typedef struct {
    X_root_header_s	X_VOID_NAME;
    struct XxY_elem	*display;
    X_header		*X_hd [2];
    int			*lnk_hd [2], *lnk [2];
} XxY_header;

#endif
