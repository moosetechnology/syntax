/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */


#include "sxunix.h"
#include "put_edit.h"

char WHAT_PUTEDITFNB[] = "@(#)SYNTAX - $Id: puteditfnb.c 938 2008-01-17 15:00:37Z rlacroix $" WHAT_DEBUG;

VOID	put_edit_fnb (SXINT col, SXINT f, SXINT number)
{
    size_t	len;

    sprintf (_SXPE_str, "%ld", (long)number);
    _SXPE_pos (col + f - (len = strlen (_SXPE_str)));
    fputs (_SXPE_str, _SXPE_file);
    _SXPE_col += len;
}
