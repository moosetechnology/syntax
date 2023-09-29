/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/



#include "sxunix.h"
#include "R_tables.h"
char WHAT_RECORFREE[] = "@(#)SYNTAX - $Id: recorfree.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


SXVOID	recor_free (struct R_tables_s *R_tables)
{
    SXINT	i;

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
