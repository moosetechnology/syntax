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

/* Manipulation de chaines de bits simples */

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXGETBIT[] = "@(#)SYNTAX - $Id: sxgetbit.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;

SXINT		sxgetbit (SXINT *tab, SXINT val)
/*
   Cette fonction rend vrai si "val" existe dans le tableau trie "tab" dont le
   premier element est la taille du tableau (N si le tableau est indice de
   zero a N). Marche par dichotomie. Utilisee pour recuperer la valeur d'un
   bit dans une chaine representee par le tableau des indices des bits a 1,
   comme dans les tables de SYNTAX en C.
*/
{
    SXINT	low = 1, high = tab [0] /* bornes inf et sup */ ;
    SXINT	mid;

    while (low < high) {
	if (val <= tab [mid = (low + high) >> 1 /* div 2 */ ])
	    high = mid;
	else
	    low = mid + 1;
    }

    return (low == high && val == tab [low]) ? low : 0;
}
