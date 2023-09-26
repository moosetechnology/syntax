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








#include "sxversion.h"
#include "sxunix.h"
char WHAT_TABCDEFSACT[] = "@(#)SYNTAX - $Id: def_sact.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


extern SXVOID    bnf_skip_rule (void);
extern SXVOID    bnf_found_bad_beginning_of_rule (void);



SXINT def_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXINIT:
    case SXFINAL:
    case SXOPEN:
    case SXCLOSE:
	return 0;

    case SXACTION:
	switch (act_no) {
	case 0:
	    bnf_found_bad_beginning_of_rule ();
	    return 0;

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

	    return 0;

	case 2:
	    sxerror (sxsrcmngr.source_coord,
		     sxsvar.sxtables->err_titles [1][0],
		     "%s\"%s)\" is forced before End Of Line.",
		     sxsvar.sxtables->err_titles [1]+1, 
		     SXCHAR_TO_STRING (sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth - 1]));
	    return 0;

	case 3:
	    bnf_skip_rule ();
	    return 0;
	default:
#if EBUG
	  sxtrap("def_sact","unknown switch case #1");
#endif
	  break;
	}

    default:
	fputs ("\nThe function \"def_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
	return 0; /* pour faire taire gcc -Wreturn-type */
    }
}
