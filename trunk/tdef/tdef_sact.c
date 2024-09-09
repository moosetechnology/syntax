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

char WHAT_TDEFSACT[] = "@(#)SYNTAX - $Id: tdef_sact.c 4224 2024-09-03 08:06:07Z garavel $" WHAT_DEBUG;

bool sxscanner_action (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXOPEN:
    case SXINIT:
    case SXFINAL:
    case SXCLOSE:
	break;

    case SXACTION:
	switch (act_no) {
	case 1:
	    /* \nnn => char */
	    {
		char    val;
		char	c, *s, *t;

		t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != SXNUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
		sxsvar.sxlv.mark.index = -1;
	    }
	    return SXANY_BOOL;

	default:
	    break;
	}
        break;

    default:
	fputs ("The function \"sxscanner_action\" for tdef is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }

    return SXANY_BOOL;
}
