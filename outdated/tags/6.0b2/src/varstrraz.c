#include "varstr.h"
#include "sxunix.h"

char WHAT_VARSTRRAZ[] = "@(#)SYNTAX - $Id: varstrraz.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VARSTR	varstr_raz (varstr_ptr)
    VARSTR	varstr_ptr;
{
    varstr_ptr->current = varstr_ptr->first;
    *(varstr_ptr->first) = NUL;
    return varstr_ptr;
}
