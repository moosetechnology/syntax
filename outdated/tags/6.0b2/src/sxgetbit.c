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
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 22-02-93 11:41 (pb):		Retourne un entier (indice ds tab) au	*/
/*				lieu d'un booleen.			*/
/************************************************************************/
/* 21-10-86 11:32 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 21-10-86 11:32 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXGETBIT[] = "@(#)SYNTAX - $Id: sxgetbit.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

int		sxgetbit (tab, val)
    register int	*tab;
    register int	val;
/*
   Cette fonction rend vrai si "val" existe dans le tableau trie "tab" dont le
   premier element est la taille du tableau (N si le tableau est indice de
   zero a N). Marche par dichotomie. Utilisee pour recuperer la valeur d'un
   bit dans une chaine representee par le tableau des indices des bits a 1,
   comme dans les tables de SYNTAX en C.
*/
{
    register int	low = 1, high = tab [0] /* bornes inf et sup */ ;
    register int	mid;

    while (low < high) {
	if (val <= tab [mid = (low + high) >> 1 /* div 2 */ ])
	    high = mid;
	else
	    low = mid + 1;
    }

    return (low == high && val == tab [low]) ? low : 0;
}
