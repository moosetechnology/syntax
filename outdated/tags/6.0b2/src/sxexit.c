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
/* 20030505 14:18 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 24-11-87 17:12 (phd) :	Creation				*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXEXIT[] = "@(#)SYNTAX - $Id: sxexit.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	sxexit (int sevlev)
/* Must never return to its caller */
{
    exit (sevlev);
}
