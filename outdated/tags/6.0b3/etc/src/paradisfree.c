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
char WHAT_PARADISFREE[] = "@(#)SYNTAX - $Id: paradisfree.c 1053 2008-02-15 13:39:59Z rlacroix $" WHAT_DEBUG;


VOID	paradis_free (struct PP_ag_item *PP_ag)
{
    if (PP_ag->SXPP_schema != NULL) {
	sxfree (PP_ag->SXPP_schema), PP_ag->SXPP_schema = NULL;
    }

    if (PP_ag->PP_indx != NULL) {
	sxfree (PP_ag->PP_indx), PP_ag->PP_indx = NULL;
    }
}
