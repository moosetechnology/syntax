/* ------------------------- Include File X_rep.h ------------------------- */

#ifndef X_rep
#define X_rep

/* Representation of the Abstract Data Type X */

#include	"X_root.h"

#define X_7F			X_root_7F
#define X_80			X_root_80

#define X_HASH_FUN(x)		(x)

struct X_elem {
    SXINT X;
};

typedef struct {
    X_root_header_s	X_VOID_NAME;
    struct X_elem	*display;
} X_header;

#endif

