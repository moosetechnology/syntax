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
#include "sxcommon.h"

typedef SXINT	sxSL_elems; /* Bidon */

#include "sxSL.h"

char WHAT_SXSL_MNGR[] = "@(#)SYNTAX - $Id: sxSL_mngr.c 3917 2024-04-23 08:33:57Z garavel $" WHAT_DEBUG;

bool	sxSL_GC (sxSL_header *header)
{
    /* Cette procedure tasse les listes "erased" sans changer l'identifiant des autres.
       Si "swap_ft" est non NULL, elle est appelee a chaque deplacement. */

    SXINT			lim, deltax, x, deltalist, list, lgth, xelem;

    if (header->has_erased) {
	lim = header->sorted_top;
	deltax = 0;
	deltalist = 0;

	for (x = 0; x < lim; x++) {
	    list = header->display [x].sorted;
	    lgth = sxSL_lgth (*header, list);

	    if (sxSL_is_erased (*header, list)) {
		deltax++;
		deltalist += lgth;
		header->display [list].X = header->free;
		header->free = list;
	    }
	    else if (deltax != 0) {
		/* On deplace de x vers x-deltax */
		header-> display [x - deltax].sorted = list;
		xelem = header->display [list].X;
		(*header->move) (header, xelem, header->display [list].X -= deltalist, lgth);
	    }
	}

	header->has_erased = false;

	if (deltax != 0) {
	    header->sorted_top -= deltax;
	    sxSL_elems_top (*header) -= deltalist;

	    return true;
	}
    }

    return false;
}

void sxSL_stat (sxSL_header *header, FILE *file, SXINT sizeof_elem)
{
    SXINT	total_sizeof, used_sizeof;

    total_sizeof = sizeof (sxSL_header) +
	sizeof (struct sxSL_display) * sxSL_size (*header) +
	    sizeof_elem * sxSL_elems_size (*header);
    used_sizeof = sizeof (sxSL_header) +
	sizeof (struct sxSL_display) * header->sorted_top +
	    sizeof_elem * sxSL_elems_top (*header);

    fprintf (file, "sxSL_header: Total_sizeof (byte) = %ld (%ld%%)\n",
	    (SXINT) total_sizeof,
	    (SXINT) (100*used_sizeof)/total_sizeof);
}    

