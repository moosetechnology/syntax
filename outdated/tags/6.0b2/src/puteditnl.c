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

char WHAT_PUTEDITNL[] = "@(#)SYNTAX - $Id: puteditnl.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	put_edit_nl (n)
    register int	n;
{
    while (n-- > 0)
	putc (NEWLINE, _SXPE_file);

    _SXPE_col = 1;
}
