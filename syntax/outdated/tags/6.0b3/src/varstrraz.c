#include "varstr.h"
#include "sxunix.h"

char WHAT_VARSTRRAZ[] = "@(#)SYNTAX - $Id: varstrraz.c 939 2008-01-17 15:01:08Z rlacroix $" WHAT_DEBUG;

VARSTR	varstr_raz (VARSTR varstr_ptr)
{
    varstr_ptr->current = varstr_ptr->first;
    *(varstr_ptr->first) = NUL;
    return varstr_ptr;
}
