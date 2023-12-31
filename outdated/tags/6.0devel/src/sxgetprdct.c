/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/






/* retourne le numero du predicat (ou -1) associe' au couple (red_no, pos)
   ou red_no est un numero de production et pos l'index en partie droite
   (repere de gauche a droite a partir de 1). */



#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXGETPRDCT[] = "@(#)SYNTAX - $Id: sxgetprdct.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

SXINT		sxgetprdct (SXINT red_no, SXINT pos)
/*
   Cette fonction rend la valeur du predicat associe au couple (red_no, pos)
   si ce couple existe dans le tableau trie sxplocals.SXP_tables.prdct_list
   de taille N = sxplocals.SXP_tables.Mprdct_list (tableau est indice de 1 a
   N). Marche par dichotomie.
*/
{
    SXINT				low, high /* bornes inf et sup */, mid;
    struct SXP_prdct_list	*ap, *lim;

    if ((high = sxplocals.SXP_tables.Mprdct_list) > 0) {
	low = 1;
	
	while (low < high) {
	    mid = (low + high) >> 1 /* div 2 */ ;

	    if (red_no <= (SXINT) sxplocals.SXP_tables.prdct_list [mid].red_no)
		high = mid;
	    else
		low = mid + 1;
	}
	
	/* low == high */
	/* repere l'occurrence la plus basse de red_no (s'il y en a). */
	ap = sxplocals.SXP_tables.prdct_list + low;
	lim = sxplocals.SXP_tables.prdct_list + sxplocals.SXP_tables.Mprdct_list;
	
	while (red_no == ((SXINT) ap->red_no) && ((SXINT) ap->pos) <= pos) {
	    if (((SXINT) ap->pos) == pos)
		return (SXINT) ap->prdct;
	    
	    if (++ap > lim)
		break;
	}
    }
    
    return -1;
}


