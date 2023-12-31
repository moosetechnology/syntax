/*   G E N _ N A M E   */

/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

/***************************************************************/
/*                                                             */
/*                                                             */
/*  This program has been translated from lecl_gen_name.pl1    */
/*  on Wednesday the twenty-sixth of March, 1986, at 10:01:38, */
/*  on the Multics system at INRIA,                            */
/*  by the release 3.3g PL1_C translator of INRIA,             */
/*         developped by the "Langages et Traducteurs" team,   */
/*         using the SYNTAX (*), FNC and Paradis systems.      */
/*                                                             */
/*                                                             */
/***************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                         */
/***************************************************************/


#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"
#include "lecl_ag.h"
char WHAT_LECLGN[] = "@(#)SYNTAX - $Id: lecl_gn.c 601 2007-05-30 10:43:19Z rlacroix $" WHAT_DEBUG;



static SXBA	/* char_max */ char_set = NULL, /* smax */ sc_set = NULL,
/* max_prdct_no */ prdct_set = NULL;




static VARSTR	gen_sc_name (gen_sc_name_char_set, varstr_ptr)
    SXBA	gen_sc_name_char_set;
    VARSTR	varstr_ptr;
{
    int		i, j, l;
    BOOLEAN	is_first, is_slice;

    is_slice = FALSE;
    is_first = TRUE;
    i = 0;

    while ((i = sxba_scan (gen_sc_name_char_set, i)) > 0) {
	l = sxba_0_lrscan (gen_sc_name_char_set, i);

	if (l == -1)
	    l = char_max - i + 1;
	else
	    l -= i;

	if (l > 7) {
	    /* On genere une slice */
	    if (is_first)
		is_first = FALSE;
	    else {
		if (!is_slice)
		    varstr_catenate (varstr_ptr, "\"");

		varstr_catenate (varstr_ptr, " + ");
	    }

	    varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_ptr, "\""),
		 CHAR_TO_STRING (i - 1)), "\"..\""), CHAR_TO_STRING (i + l - 2)), "\"");
	    is_slice = TRUE;
	}
	else {
	    if (is_slice)
		varstr_catenate (varstr_ptr, " + \"");
	    else if (is_first) {
		varstr_catenate (varstr_ptr, "\"");
		is_first = FALSE;
	    }

	    for (j = 0; j < l; j++)
		varstr_catenate (varstr_ptr, CHAR_TO_STRING (i + j - 1));

	    is_slice = FALSE;
	}

	i += l - 1;
    }

    if (!is_slice)
	varstr_catenate (varstr_ptr, "\"");

    return varstr_ptr;
}



static VARSTR	gen_cc_name (SC_TO_CHAR_SET, gen_cc_name_sc_set, varstr_ptr)
    SXBA	*SC_TO_CHAR_SET;
    SXBA	gen_cc_name_sc_set;
    VARSTR	varstr_ptr;
{
    int		i;
    BOOLEAN	not_empty;

    if (char_set == NULL)
	char_set = sxba_calloc (char_max + 1);
    else
	sxba_empty (char_set);

    i = 0;

    while ((i = sxba_scan (gen_cc_name_sc_set, i)) > 0 && i <= smax) {
	sxba_or (char_set, SC_TO_CHAR_SET [i]);
    }

    not_empty = !sxba_is_empty (char_set);

    if (sxba_bit_is_set (gen_cc_name_sc_set, 2) /* EOF */ ) {
	varstr_catenate (varstr_ptr, "Eof");

	if (not_empty)
	    varstr_catenate (varstr_ptr, " + ");
    }

    if (not_empty)
	gen_sc_name (char_set, varstr_ptr);

    return varstr_ptr;
}



static VARSTR	gen_ecc (SC_TO_CHAR_SET, esc_set, gen_ecc_prdct_set, varstr_ptr)
    SXBA	*SC_TO_CHAR_SET;
    SXBA	esc_set, gen_ecc_prdct_set;
    VARSTR	varstr_ptr;
{
    int		prdct, esc;
    BOOLEAN	firstp;

    if (sc_set == NULL)
	sc_set = sxba_calloc (smax + 1);

    firstp = TRUE;
    prdct = 0;

    while ((prdct = sxba_scan (gen_ecc_prdct_set, prdct)) > 0) {
	sxba_empty (sc_set);
	esc = smax;

	while ((esc = sxba_scan (esc_set, esc)) > 0) {
	    int sc;

	    if (esc_to_sc_prdct [esc].predicate == prdct) {
		sc = esc_to_sc_prdct [esc].simple_class;
		SXBA_1_bit (sc_set, sc);
	    }
	}

	if (firstp)
	    firstp = FALSE;
	else
	    varstr_catenate (varstr_ptr, " + ");

	varstr_catenate (gen_cc_name (SC_TO_CHAR_SET, sc_set, varstr_ptr), sxstrget (action_or_prdct_to_ate [prdct]));
    }

    return varstr_ptr;
}




VARSTR	lecl_gen_ecc (SC_TO_CHAR_SET, esc_set, max_prdct_no, varstr_ptr)
    SXBA	*SC_TO_CHAR_SET;
    SXBA	esc_set;
    int		max_prdct_no;
    VARSTR	varstr_ptr;
{
    BOOLEAN	prdct_setp = FALSE;
    int		esc;

    if (prdct_set == NULL)
	prdct_set = sxba_calloc (max_prdct_no + 1);
    else
	sxba_empty (prdct_set);

    esc = smax;

    while ((esc = sxba_scan (esc_set, esc)) > 0) {
	int prdct;

	prdct_setp = TRUE;
	prdct = esc_to_sc_prdct [esc].predicate;
	SXBA_1_bit (prdct_set, prdct);
    }

    if (sxba_scan (esc_set, 0) <= smax) {
	gen_cc_name (SC_TO_CHAR_SET, esc_set, varstr_ptr);

	if (prdct_setp)
	    varstr_catenate (varstr_ptr, " + ");
    }

    if (prdct_setp)
	gen_ecc (SC_TO_CHAR_SET, esc_set, prdct_set, varstr_ptr);

    return varstr_ptr;
}



VARSTR	lecl_gen_cc (SC_TO_CHAR_SET, esc_set, varstr_ptr)
    SXBA	*SC_TO_CHAR_SET;
    SXBA	esc_set;
    VARSTR	varstr_ptr;
{
    return gen_cc_name (SC_TO_CHAR_SET, esc_set, varstr_ptr);
}
VARSTR	lecl_gen_sc (SC_TO_CHAR_SET, simple_class, varstr_ptr)
    SXBA	*SC_TO_CHAR_SET;
    int		simple_class;
    VARSTR	varstr_ptr;
{
    if (simple_class == 2 /* EOF */ )
	varstr_catenate (varstr_ptr, "Eof");
    else
	gen_sc_name (SC_TO_CHAR_SET [simple_class], varstr_ptr);

    return varstr_ptr;
}



VOID	lecl_gen_free ()
{
    if (char_set)
	sxfree (char_set);

    if (sc_set)
	sxfree (sc_set);

    if (prdct_set)
	sxfree (prdct_set);

    char_set = sc_set = prdct_set = NULL;
}
