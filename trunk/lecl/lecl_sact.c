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

char WHAT_LECLSACT[] = "@(#)SYNTAX - $Id: lecl_sact.c 3603 2023-09-23 20:02:36Z garavel $" WHAT_DEBUG;

SXVOID	lecl_scan_act (SXINT what, SXINT act_no)
{
    switch (what) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	return;

    case SXACTION:
	switch (act_no) {
	case 1:
	    /* Dark Letter Check */
	    if (sxsrcmngr.current_char != sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1])
		sxerror (sxsrcmngr.source_coord,
			 sxsvar.sxtables->err_titles [1][0] /* Warning */ ,
			 "%sA dark symbol must be built up with the same character.",
			 sxsvar.sxtables->err_titles [1]+1 /* Warning */ );

	    return;

	case 2:
	    /* \nnn => char */
	    {
		char	val;
		char	c, *s, *t;

		t = s = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != SXNUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + 1;
		sxsvar.sxlv.mark.index = -1;
	    }

	    return;

	default:
	    /* valeur inattendu */
	    break;
	}
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    default:
	fputs ("The function \"lecl_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
