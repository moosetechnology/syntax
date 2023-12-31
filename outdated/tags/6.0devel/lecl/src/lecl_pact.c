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
#include "lecl_td.h"

char WHAT_LECLPACT[] = "@(#)SYNTAX - $Id: lecl_pact.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static SXINT	NOT_ste, KEYWORD_ste;

SXINT	lecl_pars_act (SXINT entry, SXINT action_no)
{
    switch (entry) {
    case SXOPEN:
    case SXCLOSE:
    case SXFINAL:
	return 0;

    case SXINIT:
	/* The keywords "NOT" and "KEYWORD" are not reserved. */
	NOT_ste = sxstrsave ("NOT");
	KEYWORD_ste = sxstrsave ("KEYWORD");
	return 0;

    case SXPREDICATE: {
	switch (action_no) {
	case 1:
	    return sxget_token (sxplocals.ptok_no)->string_table_entry == NOT_ste &&
		sxget_token (sxplocals.ptok_no + 1)->string_table_entry == KEYWORD_ste;

	case 2:
	    return sxget_token (sxplocals.ptok_no)->string_table_entry == KEYWORD_ste &&
		sxget_token (sxplocals.ptok_no + 1)->lahead != EQUAL_code;

	default:
	    break;
	}
	break;
    }
    default:
	break;
    }

    fputs ("The function \"lecl_pars_act\" is out of date \
with respect to its specification.\n", sxstderr);
    sxexit(1);
    return 0; /* pour faire taire gcc -Wreturn-type */
}
