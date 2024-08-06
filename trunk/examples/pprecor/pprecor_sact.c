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

#include "sxunix.h"

char WHAT_PPRECORSACT[] = "@(#)SYNTAX - $Id: pprecor_sact.c 4125 2024-07-29 10:59:13Z garavel $" WHAT_DEBUG;

bool pp_recor_scan_act (SXINT what, SXINT act_no)
{
    switch (what) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 1:
	    if (sxsrcmngr.current_char != sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1])
		sxerror (sxsrcmngr.source_coord,
			 sxsvar.sxtables->err_titles [1][0],
			 "%sA dark symbol must be built up with the same character.",
			 sxsvar.sxtables->err_titles [1]+1);

	    return SXANY_BOOL;

	case 2:
	    do {
		switch (sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]) {
		case ' ':
		case '\t':
		case '\n':
		    sxsvar.sxlv.ts_lgth--;
		    continue;

		default:
		    sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth++] = '\n';
		    break;
		}

		break;
	    } while (sxsvar.sxlv.ts_lgth > 0);

	    return SXANY_BOOL;

	default:
	    /* value inattendue */
	    break;
	}
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    default:
	fputs ("The function \"pp_recor_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
