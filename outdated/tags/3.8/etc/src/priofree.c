/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)prio_free.c	- SYNTAX [unix] - 15 Decembre 1986";

#include "sxunix.h"
#include "D_tables.h"



VOID	prio_free (t_priorities, r_priorities)
    struct priority	**t_priorities, **r_priorities;
{
    if (*t_priorities != NULL) {
	sxfree (*t_priorities), *t_priorities = NULL;
    }

    if (*r_priorities != NULL) {
	sxfree (*r_priorities), *r_priorities = NULL;
    }
}
