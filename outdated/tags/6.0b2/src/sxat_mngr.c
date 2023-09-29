/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *                                                      *
   ******************************************************** */





/* Actions de parcours d'arbre pour la nouvelle version de SYNTAX (TM) */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 10:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 06-04-87 10:19 (pb):		Retourne NULL si "out of bounds"	*/
/************************************************************************/
/* 17-12-86 11:28 (phd):	"sxat_snv" est une macro dans SXUNIX.H	*/
/************************************************************************/
/* 21-10-86 11:27 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 21-10-86 11:27 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/




#include "sxunix.h"

char WHAT_SXAT_MNGR[] = "@(#)SYNTAX - $Id: sxat_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

NODE	*sxbrother (visited, n)
    register NODE	*visited;
    register int	n;
{
    register NODE	*brother;
    register int	pos;

    if (n >= (pos = visited->position)) {
	brother = visited;
    }
    else if (n > 0) {
	brother = (visited->father)->son;
	pos = 1;
    }
    else
	return NULL;

    while (pos++ < n && brother != NULL) {
	brother = brother->brother;
    }

    return brother;
}



NODE	*sxson (visited, n)
    NODE	*visited;
    register int	n;
{
    register NODE	*son = visited->son;
    register int	pos = 1;

    while (pos++ < n && son != NULL) {
	son = son->brother;
    }

    return son;
}
