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


/* Manipulation de chaines de bits simples */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 21-10-86 11:32 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 21-10-86 11:32 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#ifndef lint
char	what_sxgetbit [] = "@(#)sxgetbit.c	- SYNTAX [unix] - 21 Avril 1986";
#endif




#include "sxunix.h"

BOOLEAN		sxgetbit (tab, val)
    register short	*tab;
    register short	val;
/*
   Cette fonction rend vrai si "val" existe dans le tableau trie "tab" dont le
   premier element est la taille du tableau (N si le tableau est indice de
   zero a N). Marche par dichotomie. Utilisee pour recuperer la valeur d'un
   bit dans une chaine representee par le tableau des indices des bits a 1,
   comme dans les tables de SYNTAX en C.
*/
{
    register int	low = 1, high = tab [0];
    /* bornes inf et sup */
    register int	mid;

    while (low < high) {
	if (val <= tab [mid = (low + high) >> 1 /* div 2 */ ])
	    high = mid;
	else
	    low = mid + 1;
    }

    return low == high && val == tab [low];
}
