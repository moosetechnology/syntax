#include "varstr.h"
#include "sxunix.h"


void varstr_free (varstr_ptr)
    VARSTR	varstr_ptr;
{
    sxfree (varstr_ptr->first);
    sxfree (varstr_ptr);
}
