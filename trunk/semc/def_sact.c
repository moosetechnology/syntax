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

char WHAT_SEMC_DEFSACT[] = "@(#)SYNTAX - $Id: def_sact.c 4248 2024-09-05 15:54:05Z garavel $" WHAT_DEBUG;

extern void    bnf_skip_rule (void);
extern void    bnf_found_bad_beginning_of_rule (void);

/* forward */ SXSCANACT_FUNCTION def_scan_act;

bool def_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXINIT:
    case SXFINAL:
    case SXOPEN:
    case SXCLOSE:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	case 0:
	    bnf_found_bad_beginning_of_rule ();
	    return SXANY_BOOL;

	case 1:
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

	    return SXANY_BOOL;

	case 2:
	    sxerror (sxsrcmngr.source_coord,
		     sxsvar.sxtables->err_titles [1][0],
		     "%s\"%s)\" is forced before End Of Line.",
		     sxsvar.sxtables->err_titles [1]+1, 
		     SXCHAR_TO_STRING (sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]));
	    return SXANY_BOOL;

	case 3:
	    bnf_skip_rule ();
	    return SXANY_BOOL;
	default:
#if EBUG
	  sxtrap("def_sact","unknown switch case #1");
#endif
	  break;
	}
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    default:
	fputs ("\nThe function \"def_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
	return SXANY_BOOL; /* pour faire taire gcc -Wreturn-type */
    }
}
