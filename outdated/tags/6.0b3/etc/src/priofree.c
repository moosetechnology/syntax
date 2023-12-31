/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

#include "sxunix.h"
#include "D_tables.h"
char WHAT_PRIOFREE[] = "@(#)SYNTAX - $Id: priofree.c 1054 2008-02-15 13:40:10Z rlacroix $" WHAT_DEBUG;



VOID	prio_free (struct priority **t_priorities, struct priority **r_priorities)
{
    if (*t_priorities != NULL) {
	sxfree (*t_priorities), *t_priorities = NULL;
    }

    if (*r_priorities != NULL) {
	sxfree (*r_priorities), *r_priorities = NULL;
    }
}
