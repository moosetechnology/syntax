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




/* Compactage memoire */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 24-11-87 17:12 (phd) :	Creation				*/
/************************************************************************/


#include "sxunix.h"

char WHAT_SXGC[] = "@(#)SYNTAX - $Id: sxgc.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

#define BELL	((char) 7)


VOID	sxgc ()
/* Must return to its caller only if memory map has changed */
{
    fprintf (sxstderr, "\tNO MORE ALLOCATION POSSIBLE...%c\n", BELL);
    SXEXIT (SEVERITIES);
}
