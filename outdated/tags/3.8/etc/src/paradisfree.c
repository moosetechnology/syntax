/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)paradis_free.c\t- SYNTAX [unix] - 31 Aout 1988";

#include "sxunix.h"

#ifdef	MSDOS
#include "Q_tables.h"
#else
#include "PP_tables.h"
#endif


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
