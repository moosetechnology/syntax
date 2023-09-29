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
/* 24-11-87 17:12 (phd) :	Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxgc [] = "@(#)sxgc.c	- SYNTAX [unix] - 24 Novembre 1987";
#endif


#include "sxunix.h"
#define BELL	((char) 7)


VOID	sxgc ()
/* Must return to its caller only if memory map has changed */
{
    fprintf (sxstderr, "\tNO MORE ALLOCATION POSSIBLE...%c\n", BELL);
    SXEXIT (SEVERITIES);
}
