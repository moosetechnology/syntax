/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)lecl_free.c	- SYNTAX [unix] - 31 Mai 1988";

#include "sxunix.h"
#include "S_tables.h"


VOID	lecl_free (lecl_tables_ptr)
    register struct lecl_tables_s	*lecl_tables_ptr;
{
    register int	i;

    sxfree (lecl_tables_ptr->S_is_a_keyword);
    sxfree (lecl_tables_ptr->S_is_a_generic_terminal);

    if (lecl_tables_ptr->S_check_kw != NULL)
	/* Dans LECL, c'est un VARSTR libere manu */
	sxfree (lecl_tables_ptr->S_check_kw);

    sxfree (lecl_tables_ptr->S_terlis);
    sxfree (lecl_tables_ptr->SXS_adrp);
    sxfree (lecl_tables_ptr->SXS_action_or_prdct_code);

    for (i = lecl_tables_ptr->S_tbl_size.S_last_state_no; i > 0; i--)
	sxfree (lecl_tables_ptr->S_transition_matrix [i]);

    sxfree (lecl_tables_ptr->S_transition_matrix);
    sxfree (lecl_tables_ptr->S_char_to_simple_class);
}
