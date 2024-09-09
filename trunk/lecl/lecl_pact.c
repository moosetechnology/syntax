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

#include "sxversion.h"
#include "sxunix.h"
#include "lecl_td.h"

char WHAT_LECLPACT[] = "@(#)SYNTAX - $Id: lecl_pact.c 4227 2024-09-03 09:47:35Z garavel $" WHAT_DEBUG;

static SXINT	NOT_ste, KEYWORD_ste;

bool sxparser_action (SXINT entry, SXINT action_no)
{
    switch (entry) {
    case SXOPEN:
    case SXCLOSE:
    case SXFINAL:
	return SXANY_BOOL;

    case SXINIT:
	/* The keywords "NOT" and "KEYWORD" are not reserved. */
	NOT_ste = sxstrsave ("NOT");
	KEYWORD_ste = sxstrsave ("KEYWORD");
	return SXANY_BOOL;

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

    fputs ("The function \"sxparser_action\" for lecl/pplecl is out of date \
with respect to its specification.\n", sxstderr);
    sxexit(1);
    return SXANY_BOOL; /* pour faire taire gcc -Wreturn-type */
}
