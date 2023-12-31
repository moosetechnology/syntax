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

char WHAT_PUTEDITFNB[] = "@(#)SYNTAX - $Id: puteditfnb.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	put_edit_fnb (col, f, number)
    int		col, f, number;
{
    register int	len;

    sprintf (_SXPE_str, "%d", number);
    _SXPE_pos (col + f - (len = strlen (_SXPE_str)));
    fputs (_SXPE_str, _SXPE_file);
    _SXPE_col += len;
}
