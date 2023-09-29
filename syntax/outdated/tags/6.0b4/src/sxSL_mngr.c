/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1989 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ChLoE.     		  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 10:59 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 05-09-94 16:15 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/




#include	<stdio.h>

#include "sxcommon.h"

typedef SXINT	sxSL_elems; /* Bidon */

#include	"sxSL.h"

char WHAT_SXSL_MNGR[] = "@(#)SYNTAX - $Id: sxSL_mngr.c 953 2008-01-22 10:22:13Z rlacroix $" WHAT_DEBUG;

BOOLEAN	sxSL_GC (sxSL_header *header)
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

	header->has_erased = FALSE;

	if (deltax != 0) {
	    header->sorted_top -= deltax;
	    sxSL_elems_top (*header) -= deltalist;

	    return TRUE;
	}
    }

    return FALSE;
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
	    (long)total_sizeof,
	    (long)(100*used_sizeof)/total_sizeof);
}    

