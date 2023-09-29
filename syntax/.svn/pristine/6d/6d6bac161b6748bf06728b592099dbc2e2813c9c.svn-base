#include "varstr.h"
#include "sxunix.h"


VARSTR	varstr_raz (varstr_ptr)
    VARSTR	varstr_ptr;
{
    varstr_ptr->current = varstr_ptr->first;
    *(varstr_ptr->first) = NUL;
    return varstr_ptr;
}
