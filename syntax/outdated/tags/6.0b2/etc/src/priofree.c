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
char WHAT_PRIOFREE[] = "@(#)SYNTAX - $Id: priofree.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;



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
