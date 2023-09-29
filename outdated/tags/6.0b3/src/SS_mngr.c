/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1990 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030512 10:59 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 26-02-90 10:36 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxcommon.h"
#include "sxalloc.h"

char WHAT_SS_MNGR[] = "@(#)SYNTAX - $Id: SS_mngr.c 934 2008-01-17 10:16:58Z rlacroix $" WHAT_DEBUG;

SXINT *SS_alloc (SXINT size)
{
    /* Suppose que size >= 2*/
    SXINT *t;

    t = (SXINT*) sxalloc (size + 2, sizeof (SXINT)) + 1;
    t [-1] = size;
    t [0] = t [1] = 2;
    return t;
}

SXINT SS_sature (SXINT **t)
{
    *t = (SXINT*) sxrealloc (*t - 1, ((*t)[-1] *= 2) + 2, sizeof (SXINT)) + 1;
    return 0;
}
