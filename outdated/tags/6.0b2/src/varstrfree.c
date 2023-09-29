#include "varstr.h"
#include "sxunix.h"

char WHAT_VARSTRFREE[] = "@(#)SYNTAX - $Id: varstrfree.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

void varstr_free (varstr_ptr)
    VARSTR	varstr_ptr;
{
    sxfree (varstr_ptr->first);
    sxfree (varstr_ptr);
}
