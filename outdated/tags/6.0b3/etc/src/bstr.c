/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1990 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *             Produit de l'�quipe ATOLL :              *
   *  ATelier d'Outils Logiciels pour le Langage naturel  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030513 15:44 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/
/* peut-�tre 19900917 (pb):	Cr�ation				*/
/************************************************************************/

#include "sxunix.h"
#include "varstr.h"    
#include "bstr.h"
char WHAT_BSTR[] = "@(#)SYNTAX - $Id: bstr.c 1050 2008-02-15 13:39:16Z rlacroix $" WHAT_DEBUG;

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



VOID	bstr_free (struct bstr *bstr)
{
    varstr_free (bstr->vstr);
    sxfree (bstr);
}


VOID	bstr_get_attr (struct bstr *bstr, SXINT *lgth, SXINT *col)
{
    *col = bstr->col;
    *lgth = varstr_length (bstr->vstr);
}



struct bstr	*bstr_shrink (struct bstr *bstr, SXINT new_lgth, SXINT new_col)
{
    /* Assume new_lgth <= varstr_length (vstr) */

    bstr->vstr->current = bstr->vstr->first + new_lgth;
    *(bstr->vstr->current) = NUL;
    bstr->col = new_col;
    return bstr;
}



/* BoundedSTRingCATenate : concatene str a bstr. */

struct bstr	*bstr_cat_str (struct bstr *bstr, char *str)
{
    SXINT	strl;

    if (bstr->col + (strl = strlen (str)) >= bstr->rmargin) {
	bstr->vstr = varstr_catenate (bstr->vstr, "\n");
	bstr->vstr = varstr_catenate (bstr->vstr, blanks - (bstr->col = bstr->lmargin));
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
	bstr->vstr = varstr_catenate (bstr->vstr, blanks - (bstr->col = bstr->lmargin));
    }
    else {
	bstr->col += 1;
	bstr->vstr = varstr_catenate (bstr->vstr, " ");
    }

    return bstr;
}
