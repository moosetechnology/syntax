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
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 14:18 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 17-12-86 12:10 (phd):	Creation				*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXCHECKMAGIC[] = "@(#)SYNTAX - $Id: sxcheckmagic.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

VOID	sxcheck_magic_number (m1, m2, who)
    unsigned long	m1, m2;
    char	*who;
{
    if (m1 == m2) {
	return;
    }

    fprintf (sxstderr, "\n%s: Tables garbled, code %ld.\n", who, m1);
    SXEXIT (SEVERITIES);
}
