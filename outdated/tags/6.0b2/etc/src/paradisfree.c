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

#include "PP_tables.h"
char WHAT_PARADISFREE[] = "@(#)SYNTAX - $Id: paradisfree.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


VOID	paradis_free (PP_ag)
    register struct PP_ag_item	*PP_ag;
{
    if (PP_ag->SXPP_schema != NULL) {
	sxfree (PP_ag->SXPP_schema), PP_ag->SXPP_schema = NULL;
    }

    if (PP_ag->PP_indx != NULL) {
	sxfree (PP_ag->PP_indx), PP_ag->PP_indx = NULL;
    }
}
