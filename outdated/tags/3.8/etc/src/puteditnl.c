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


VOID	put_edit_nl (n)
    register int	n;
{
    while (n-- > 0)
	putc (NEWLINE, _SXPE_file);

    _SXPE_col = 1;
}
