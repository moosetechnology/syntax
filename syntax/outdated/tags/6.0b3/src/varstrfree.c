#include "varstr.h"
#include "sxunix.h"

char WHAT_VARSTRFREE[] = "@(#)SYNTAX - $Id: varstrfree.c 939 2008-01-17 15:01:08Z rlacroix $" WHAT_DEBUG;

void varstr_free (VARSTR varstr_ptr)
{
    sxfree (varstr_ptr->first);
    sxfree (varstr_ptr);
}
