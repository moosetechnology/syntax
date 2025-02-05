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

char WHAT_PPCSACT[] = "@(#)SYNTAX - $Id: ppc_sact.c 4244 2024-09-05 13:07:19Z garavel $";

extern SXTABLES	ppc_args_tables, *ppc_tables;
extern SXINT	ppc_TYPE;

/* forward */ SXSCANACT_FUNCTION ppc_scan_act;
/* forward */ SXSCANACT_FUNCTION ppc_args_scan_act;

static void	gripe (SXTABLES *sxtables, SXINT act_no)
{
    sxuse (act_no);
    sxerror (sxsvar.sxlv.terminal_token.source_index,
	     sxtables->err_titles [2][0],
	     "%sInternal inconsistency: Action %d not yet implemented.\nExecution aborted.",
	     sxtables->err_titles [2]+1);
    sxexit (3);
}



static bool	*user_types;
static SXINT	typ_lgth;


bool ppc_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXBEGIN:
	user_types = (bool*) sxcalloc (typ_lgth = sxstrmngr.tablength, sizeof (bool));
	return SXANY_BOOL;

    case SXEND:
	sxfree (user_types);
	return SXANY_BOOL;

    case SXOPEN:
    case SXCLOSE:
	return SXANY_BOOL;

    case SXINIT:
    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	if (typ_lgth < sxstrmngr.top) {
	    SXINT	i = typ_lgth;

	    user_types = (bool*) sxrealloc (user_types, typ_lgth = sxstrmngr.tablength, sizeof (bool));

	    while (i < typ_lgth) {
		user_types [i++] = false;
	    }
	}

	switch (act_no) {
	case 0:
	    /* -TYPE */
	    user_types [SXSTACKtoken (SXSTACKtop ()).string_table_entry] = true;
	    break;

	case 1:
	    /* {lower}+ */
	    if (sxkeywordp (ppc_tables, sxsvar.sxlv.terminal_token.lahead)) {
		return SXANY_BOOL;
	    }
            /* FALLTHROUGH */ /* added par H. Garavel, confirmed by P. Boullier */

	case 2:
	    /* %NAME */
	    if (user_types [sxsvar.sxlv.terminal_token.string_table_entry]) {
		sxsvar.sxlv.terminal_token.lahead = ppc_TYPE;
	    }
	    break;

	default:
	    gripe (ppc_tables, act_no);
            /* NOTREACHED */
	}
	return SXANY_BOOL;

    default:
	gripe (ppc_tables, code);
        /* NOTREACHED */
	return SXANY_BOOL;
    }
}


bool ppc_args_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	return SXANY_BOOL;

    case SXACTION:
	switch (act_no) {
	    SXINT	length;
	    char	*mark, *value;

	case 3:
	    if ((value = getenv (mark = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index)) == NULL) {
		sxerror (sxsvar.sxlv.mark.source_coord,
			 ppc_args_tables.err_titles[2][0],
			 "%s%s is not in the environment.\n",
			 ppc_args_tables.err_titles[2]+1,
			 mark);
		length = 0;
	    }
	    else {
		if (sxsvar.sxlv_s.ts_lgth_use <= sxsvar.sxlv.mark.index + (length = strlen (value))) {
		    sxsvar.sxlv_s.token_string = sxrealloc (sxsvar.sxlv_s.token_string, sxsvar.sxlv_s.ts_lgth_use += length + 1
			 , sizeof (char));
		}

		strcpy (mark, value);
	    }

	    sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + length;
	    break;

	default:
	    gripe (&ppc_args_tables, act_no);
	}
	break;
    default:
	gripe (&ppc_args_tables, code);
    }
    return SXANY_BOOL;
}
