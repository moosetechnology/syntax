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
/* 26-02-90 10:36 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


static char	what [] = "@(#)SS_mngr.c	- SYNTAX [unix] - 26 Fevrier 1990";
static char	ME [] = "SS_mngr";

#include "sxunix.h"

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

VOID SS_sature (t)
    int	**t;
{
    *t = (int*) sxrealloc (*t - 1, ((*t)[-1] *= 2) + 2, sizeof (int)) + 1;
}
