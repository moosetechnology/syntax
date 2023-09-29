/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */



static char	what [] = "@(#)recor_free.c\t- SYNTAX [unix] - 31 Mai 1988";

#include "sxunix.h"
#include "R_tables.h"


VOID	recor_free (R_tables)
    register struct R_tables_s	*R_tables;
{
    register int	i;

    if (R_tables->E_abstract != NULL)
	sxfree (R_tables->E_abstract);

    if (R_tables->E_titles != NULL) {
	for (i = R_tables->R_tbl_size.E_nb_kind; i >= 1; i--)
	    sxfree (R_tables->E_titles [i]);

	sxfree (R_tables->E_titles);
    }

    if (R_tables->PER_tset != NULL)
	sxfree (R_tables->PER_tset);

    if (R_tables->P_no_insert != NULL)
	sxfree (R_tables->P_no_insert);

    if (R_tables->P_no_delete != NULL)
	sxfree (R_tables->P_no_delete);

    if (R_tables->P_string_mess != NULL)
	sxfree (R_tables->P_string_mess);

    if (R_tables->P_global_mess != NULL)
	sxfree (R_tables->P_global_mess);

    if (R_tables->SXP_local_mess != NULL) {
	for (i = R_tables->R_tbl_size.P_nbcart + 1; i >= 1; i--)
	    sxfree (R_tables->SXP_local_mess [i].string_ref);

	sxfree (R_tables->SXP_local_mess);
    }

    if (R_tables->P_right_ctxt_head != NULL)
	sxfree (R_tables->P_right_ctxt_head);

    if (R_tables->P_lregle != NULL) {
	for (i = R_tables->R_tbl_size.P_nbcart; i >= 1; i--)
	    sxfree (R_tables->P_lregle [i]);

	sxfree (R_tables->P_lregle);
    }

    if (R_tables->S_no_insert != NULL)
	sxfree (R_tables->S_no_insert);

    if (R_tables->S_no_delete != NULL)
	sxfree (R_tables->S_no_delete);

    if (R_tables->S_string_mess != NULL)
	sxfree (R_tables->S_string_mess);

    if (R_tables->S_global_mess != NULL)
	sxfree (R_tables->S_global_mess);

    if (R_tables->SXS_local_mess != NULL) {
	for (i = R_tables->R_tbl_size.S_nbcart; i >= 1; i--)
	    sxfree (R_tables->SXS_local_mess [i].string_ref);

	sxfree (R_tables->SXS_local_mess);
    }

    if (R_tables->S_lregle != NULL) {
	for (i = R_tables->R_tbl_size.S_nbcart; i >= 1; i--)
	    sxfree (R_tables->S_lregle [i]);

	sxfree (R_tables->S_lregle);
    }
}
