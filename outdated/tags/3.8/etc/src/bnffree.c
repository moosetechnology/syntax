/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)bnf_free.c\t- SYNTAX [unix] - 7 Avril 1988";




#include "sxunix.h"
#include "B_tables.h"


VOID	bnf_free (B)
    register struct bnf_ag_item		*B;
{
    if (B->NT_STRING != NULL)
	sxfree (B->NT_STRING);

    if (B->T_STRING != NULL)
	sxfree (B->T_STRING);

    if (B->FOLLOW != NULL)
	sxbm_free (B->FOLLOW);

    if (B->BVIDE != NULL)
	sxfree (B->BVIDE);

    if (B->IS_A_GENERIC_TERMINAL != NULL)
	sxfree (B->IS_A_GENERIC_TERMINAL);

    if (B->TNUMPD != NULL)
	sxfree (B->TNUMPD);

    if (B->NUMPD != NULL)
	sxfree (B->NUMPD);

    if (B->TPD != NULL)
	sxfree (B->TPD - B->WS_TBL_SIZE.xtmax - 1);

    if (B->ADR != NULL)
	sxfree (B->ADR - B->WS_TBL_SIZE.xtmax - 1);

    if (B->WS_NTMAX != NULL)
	sxfree (B->WS_NTMAX);

    if (B->XNT_TO_NT_PRDCT != NULL)
	sxfree (B->XNT_TO_NT_PRDCT);

    if (B->XT_TO_T_PRDCT != NULL)
	sxfree (B->XT_TO_T_PRDCT);

    if (B->WS_INDPRO != NULL)
	sxfree (B->WS_INDPRO);

    if (B->WS_NBPRO != NULL)
	sxfree (B->WS_NBPRO);
}
