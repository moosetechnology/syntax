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

char WHAT_PUTEDITPOS[] = "@(#)SYNTAX - $Id: puteditpos.c 938 2008-01-17 15:00:37Z rlacroix $" WHAT_DEBUG;

VOID	_SXPE_pos (size_t col)
{
    size_t	nb_tabs, nb_spaces;
    static char		hts [] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", *TABS = &hts [sizeof
	 hts - 1];
    static char		sps [] = "        ", *SPACES = &sps [sizeof sps - 1];

    if (col < _SXPE_col) {
	putc (NEWLINE, _SXPE_file);
	_SXPE_col = 1;
    }

    if (col > _SXPE_col) {
	nb_tabs = (col - 1) / TAB_INTERVAL - (_SXPE_col - 1) / TAB_INTERVAL;

	if (nb_tabs > 0) {
	    fputs (TABS - nb_tabs, _SXPE_file);
	    nb_spaces = (col - 1) % TAB_INTERVAL;
	}
	else
	    nb_spaces = col - _SXPE_col;

	fputs (SPACES - nb_spaces, _SXPE_file);
	_SXPE_col = col;
    }
}
