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
/* 20030512 10:59 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 26-02-90 10:36 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxcommon.h"
#include "sxalloc.h"

char WHAT_SS_MNGR[] = "@(#)SYNTAX - $Id: SS_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

int *SS_alloc (size)
    int size;
{
    /* Suppose que size >= 2*/
    register int *t;

    t = (int*) sxalloc (size + 2, sizeof (int)) + 1;
    t [-1] = size;
    t [0] = t [1] = 2;
    return t;
}

int SS_sature (t)
    int	**t;
{
    *t = (int*) sxrealloc (*t - 1, ((*t)[-1] *= 2) + 2, sizeof (int)) + 1;
    return 0;
}
