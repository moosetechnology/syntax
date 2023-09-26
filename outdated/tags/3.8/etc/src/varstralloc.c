#include "varstr.h"
#include "sxunix.h"

VARSTR	varstr_alloc (size)
    int		size;
{
    VARSTR	varstr_ptr;

    varstr_ptr = (VARSTR) sxalloc (1, sizeof (struct varstr_hd));
    varstr_ptr->current = varstr_ptr->first = sxalloc (size, sizeof (char));
    *(varstr_ptr->first) = NUL;
    varstr_ptr->last = varstr_ptr->first + size;
    return varstr_ptr;
}
