/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *							  *
   ******************************************************** */




/* Arret brutal et immediat */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 24-11-87 17:12 (phd) :	Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxexit [] = "@(#)sxexit.c	- SYNTAX [unix] - 24 Novembre 1987";
#endif


#include "sxunix.h"

VOID	sxexit (sevlev)
int sevlev;
/* Must never return to its caller */
{
    exit (sevlev);
}
