/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

/*   G E N _ N A M E   */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"
#include "varstr.h"
#include "lecl_ag.h"

char WHAT_LECLGN[] = "@(#)SYNTAX - $Id: lecl_gn.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

static SXBA	/* char_max */ char_set = NULL, /* smax */ sc_set = NULL,
/* max_prdct_no */ prdct_set = NULL;


static VARSTR	gen_sc_name (SXBA gen_sc_name_char_set, VARSTR varstr_ptr)
{
    SXINT		i, j, l;
    bool	is_first, is_slice;

    is_slice = false;
    is_first = true;
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
		is_first = false;
	    else {
		if (!is_slice)
		    varstr_catenate (varstr_ptr, "\"");

		varstr_catenate (varstr_ptr, " + ");
	    }

	    varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_catenate (varstr_ptr, "\""),
		 SXCHAR_TO_STRING (i - 1)), "\"..\""), SXCHAR_TO_STRING (i + l - 2)), "\"");
	    is_slice = true;
	}
	else {
	    if (is_slice)
		varstr_catenate (varstr_ptr, " + \"");
	    else if (is_first) {
		varstr_catenate (varstr_ptr, "\"");
		is_first = false;
	    }

	    for (j = 0; j < l; j++)
		varstr_catenate (varstr_ptr, SXCHAR_TO_STRING (i + j - 1));

	    is_slice = false;
	}

	i += l - 1;
    }

    if (!is_slice)
	varstr_catenate (varstr_ptr, "\"");

    return varstr_ptr;
}



static VARSTR	gen_cc_name (SXBA *SC_TO_CHAR_SET, 
			     SXBA gen_cc_name_sc_set, 
			     VARSTR varstr_ptr)
{
    SXINT		i;
    bool	not_empty;

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



static VARSTR	gen_ecc (SXBA *SC_TO_CHAR_SET, 
			 SXBA esc_set, 
			 SXBA gen_ecc_prdct_set, 
			 VARSTR varstr_ptr)
{
    SXINT		prdct, esc;
    bool	firstp;

    if (sc_set == NULL)
	sc_set = sxba_calloc (smax + 1);

    firstp = true;
    prdct = 0;

    while ((prdct = sxba_scan (gen_ecc_prdct_set, prdct)) > 0) {
	sxba_empty (sc_set);
	esc = smax;

	while ((esc = sxba_scan (esc_set, esc)) > 0) {
	    SXINT sc;

	    if (esc_to_sc_prdct [esc].predicate == prdct) {
		sc = esc_to_sc_prdct [esc].simple_class;
		SXBA_1_bit (sc_set, sc);
	    }
	}

	if (firstp)
	    firstp = false;
	else
	    varstr_catenate (varstr_ptr, " + ");

	varstr_catenate (gen_cc_name (SC_TO_CHAR_SET, sc_set, varstr_ptr), sxstrget (action_or_prdct_to_ate [prdct]));
    }

    return varstr_ptr;
}



VARSTR	lecl_gen_ecc (SXBA *SC_TO_CHAR_SET, 
		      SXBA esc_set, 
		      SXINT max_prdct_no, 
		      VARSTR varstr_ptr)
{
    bool	prdct_setp = false;
    SXINT		esc;

    if (prdct_set == NULL)
	prdct_set = sxba_calloc (max_prdct_no + 1);
    else
	sxba_empty (prdct_set);

    esc = smax;

    while ((esc = sxba_scan (esc_set, esc)) > 0) {
	SXINT prdct;

	prdct_setp = true;
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



VARSTR	lecl_gen_cc (SXBA *SC_TO_CHAR_SET, 
		     SXBA esc_set, 
		     VARSTR varstr_ptr)
{
    return gen_cc_name (SC_TO_CHAR_SET, esc_set, varstr_ptr);
}

VARSTR	lecl_gen_sc (SXBA *SC_TO_CHAR_SET, 
		     SXINT simple_class, 
		     VARSTR varstr_ptr)
{
    if (simple_class == 2 /* EOF */ )
	varstr_catenate (varstr_ptr, "Eof");
    else
	gen_sc_name (SC_TO_CHAR_SET [simple_class], varstr_ptr);

    return varstr_ptr;
}



void	lecl_gen_free (void)
{
    if (char_set)
	sxfree (char_set);

    if (sc_set)
	sxfree (sc_set);

    if (prdct_set)
	sxfree (prdct_set);

    char_set = sc_set = prdct_set = NULL;
}
