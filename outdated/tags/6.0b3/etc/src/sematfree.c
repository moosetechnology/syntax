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
#include "T_tables.h"
char WHAT_SEMATFREE[] = "@(#)SYNTAX - $Id: sematfree.c 1056 2008-02-15 13:40:31Z rlacroix $" WHAT_DEBUG;


VOID	semat_free (struct T_ag_item *T_ag)
{
    if (T_ag->SXT_node_info != NULL) {
	sxfree (T_ag->SXT_node_info), T_ag->SXT_node_info = NULL;
    }

    if (T_ag->T_stack_schema != NULL) {
	sxfree (T_ag->T_stack_schema), T_ag->T_stack_schema = NULL;
    }

    if (T_ag->T_nns_indx != NULL) {
	sxfree (T_ag->T_nns_indx), T_ag->T_nns_indx = NULL;
    }

    if (T_ag->T_ter_to_node_name != NULL) {
	sxfree (T_ag->T_ter_to_node_name), T_ag->T_ter_to_node_name = NULL;
    }

    if (T_ag->T_node_name_string != NULL) {
	sxfree (T_ag->T_node_name_string), T_ag->T_node_name_string = NULL;
    }
}
