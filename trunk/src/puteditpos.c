/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/



#include "sxversion.h"
#include "sxunix.h"
#include "put_edit.h"

char WHAT_PUTEDITPOS[] = "@(#)SYNTAX - $Id: puteditpos.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;

SXVOID	_SXPE_pos (size_t col)
{
    size_t	nb_tabs, nb_spaces;
    static char		hts [] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t", *TABS = &hts [sizeof
	 hts - 1];
    static char		sps [] = "        ", *SPACES = &sps [sizeof sps - 1];

    if (col < _SXPE_col) {
	putc (SXNEWLINE, _SXPE_file);
	_SXPE_col = 1;
    }

    if (col > _SXPE_col) {
	nb_tabs = (col - 1) / SXTAB_INTERVAL - (_SXPE_col - 1) / SXTAB_INTERVAL;

	if (nb_tabs > 0) {
	    fputs (TABS - nb_tabs, _SXPE_file);
	    nb_spaces = (col - 1) % SXTAB_INTERVAL;
	}
	else
	    nb_spaces = col - _SXPE_col;

	fputs (SPACES - nb_spaces, _SXPE_file);
	_SXPE_col = col;
    }
}
