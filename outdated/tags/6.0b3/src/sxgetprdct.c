/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */


/* retourne le numero du predicat (ou -1) associe' au couple (red_no, pos)
   ou red_no est un numero de production et pos l'index en partie droite
   (repere de gauche a droite a partir de 1). */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 31-03-93 14:53 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXGETPRDCT[] = "@(#)SYNTAX - $Id: sxgetprdct.c 1019 2008-02-13 08:38:28Z rlacroix $" WHAT_DEBUG;

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


