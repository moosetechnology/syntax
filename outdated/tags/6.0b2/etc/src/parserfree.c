/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

#include "sxunix.h"
#include "P_tables.h"
char WHAT_PARSERFREE[] = "@(#)SYNTAX - $Id: parserfree.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;



VOID	parser_free (parser_ag_ptr)
    register struct parser_ag_item	*parser_ag_ptr;
{
    if (parser_ag_ptr->prdct_list != NULL)
	sxfree (parser_ag_ptr->prdct_list);

    if (parser_ag_ptr->germe_to_gr_act != NULL)
	sxfree (parser_ag_ptr->germe_to_gr_act);

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

