/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)parser_free.c\t- SYNTAX [unix] - 26 Novembre 1987";

#include "sxunix.h"
#include "P_tables.h"



VOID	parser_free (parser_ag_ptr)
    register struct parser_ag_item	*parser_ag_ptr;
{
    if (parser_ag_ptr->gotos != NULL)
	sxfree (parser_ag_ptr->gotos);

    if (parser_ag_ptr->prdcts != NULL)
	sxfree (parser_ag_ptr->prdcts);

    if (parser_ag_ptr->reductions != NULL)
	sxfree (parser_ag_ptr->reductions);

    if (parser_ag_ptr->vector != NULL)
	sxfree (parser_ag_ptr->vector);

    if (parser_ag_ptr->nt_bases != NULL)
	sxfree (parser_ag_ptr->nt_bases);

    if (parser_ag_ptr->t_bases != NULL)
	sxfree (parser_ag_ptr->t_bases);
}

