/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/



#include "sxunix.h"
#include "varstr.h"    
#include "bstr.h"
char WHAT_BSTR[] = "@(#)SYNTAX - $Id: bstr.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static char	blancs [ /* 132 */ ] = "                                                                                                                                    " ;
static char	*blanks = blancs + sizeof blancs - 1;


struct bstr	*bstr_raz (struct bstr *bstr)
{
    varstr_raz (bstr->vstr);
    bstr->col = 0;
    return bstr;
}



struct bstr	*bstr_init (struct bstr *bstr, SXINT lmargin, SXINT rmargin)
{
    bstr_raz (bstr);
    bstr->lmargin = lmargin;
    bstr->rmargin = rmargin;
    return bstr;
}



struct bstr	*bstr_alloc (SXINT size, SXINT lmargin, SXINT rmargin)
{
    struct bstr	*bstr;

    bstr = (struct bstr*) sxalloc (1, sizeof (struct bstr));
    bstr->vstr = varstr_alloc (size);
    return bstr_init (bstr, lmargin, rmargin);
}



SXVOID	bstr_free (struct bstr *bstr)
{
    varstr_free (bstr->vstr);
    sxfree (bstr);
}


SXVOID	bstr_get_attr (struct bstr *bstr, SXINT *lgth, SXINT *col)
{
    *col = bstr->col;
    *lgth = varstr_length (bstr->vstr);
}



struct bstr	*bstr_shrink (struct bstr *bstr, SXINT new_lgth, SXINT new_col)
{
    /* Assume new_lgth <= varstr_length (vstr) */

    bstr->vstr->current = bstr->vstr->first + new_lgth;
    *(bstr->vstr->current) = SXNUL;
    bstr->col = new_col;
    return bstr;
}



/* BoundedSTRingCATenate : concatene str a bstr. */

struct bstr	*bstr_cat_str (struct bstr *bstr, char *str)
{
    SXINT	strl;

    if (bstr->col + (strl = strlen (str)) >= bstr->rmargin) {
	bstr->vstr = varstr_catenate (bstr->vstr, "\n");
        bstr->col = bstr->lmargin;
	bstr->vstr = varstr_catenate (bstr->vstr, blanks - bstr->lmargin);
    }

    bstr->col += strl;
    bstr->vstr = varstr_catenate (bstr->vstr, str);
    return bstr;
}



/* BoundedSTRingCATenateSTRingSPace : concatene str et un blanc a bstr. */

struct bstr	*bstr_cat_strsp (struct bstr *bstr, char *str)
{
    bstr = bstr_cat_str (bstr, str);

    if (bstr->col + 1 >= bstr->rmargin) {
	bstr->vstr = varstr_catenate (bstr->vstr, "\n");
        bstr->col = bstr->lmargin;
	bstr->vstr = varstr_catenate (bstr->vstr, blanks - bstr->lmargin);
    }
    else {
	bstr->col += 1;
	bstr->vstr = varstr_catenate (bstr->vstr, " ");
    }

    return bstr;
}
