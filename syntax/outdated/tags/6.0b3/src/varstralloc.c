#include "varstr.h"
#include "sxunix.h"

char WHAT_VARSTRALLOC[] = "@(#)SYNTAX - $Id: varstralloc.c 939 2008-01-17 15:01:08Z rlacroix $" WHAT_DEBUG;

VARSTR	varstr_alloc (SXINT size)
{
    VARSTR	varstr_ptr;

    varstr_ptr = (VARSTR) sxalloc (1, sizeof (struct varstr_hd));
    varstr_ptr->current = varstr_ptr->first = sxalloc (size, sizeof (char));
    *(varstr_ptr->first) = NUL;
    varstr_ptr->last = varstr_ptr->first + size;
    return varstr_ptr;
}
