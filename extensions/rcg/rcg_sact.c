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
#include "varstr.h"
#include <ctype.h>

char WHAT_RCGSACT[] = "@(#)SYNTAX - $Id: rcg_sact.c 4222 2024-09-03 08:02:15Z garavel $" WHAT_DEBUG;

extern SXTABLES	sxtables;

static VARSTR	vstr;

static _Noreturn void	gripe (void)
{
    fputs ("\nThe function \"sxscanner_action\" for rcg/drcg is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

/* ATTENTION : sxscanner_action peut etre appele au cours de la passe rcg puis au cours de la passe drcg.
   Dans ce cas on a la sequence
   sxscanner_action (SXOPEN) rcg ... sxscanner_action (SXACTION) rcg ... sxscanner_action (SXOPEN) drcg ...
   sxscanner_action (SXACTION) drcg ... sxscanner_action (SXCLOSE) drcg ... sxscanner_action (SXCLOSE) rcg ...
   et donc vstr est alloue 2 fois (pas trop grave) mais surtout on le libere 2 fois de suite...
   D'ou les tests a l'alloc et a la liberation */

bool sxscanner_action (SXINT code, SXINT act_no)
{
    SXINT 	x, kw;
    char	c;

    switch (code) {
    default:
	gripe ();
        /* NOTREACHED */

    case SXOPEN:
	if (vstr == NULL)
	    vstr = varstr_alloc (32);
	break;

    case SXCLOSE:
	if (vstr)
	    varstr_free (vstr), vstr = NULL;
	break;

    case SXBEGIN:
    case SXEND:
    case SXINIT:
    case SXFINAL:
	break;

    case SXACTION:
	switch (act_no) {
	default:
	    gripe ();
	    /* NOTREACHED */

	case 1:
	    /* On regarde si la version minuscule de token_string est un predicat predefini */
	    if (!sxkeywordp (&sxtables, sxsvar.sxlv.terminal_token.lahead)) {
		/* Ce n'est pas un predicat predefini ecrit en minuscule */
		varstr_raz (vstr);

		for (x = 0; x < sxsvar.sxlv.ts_lgth; x++) {
		    c = sxsvar.sxlv_s.token_string [x];

		    if (isupper (c)) {
			c = tolower (c);
		    }

		    varstr_catchar (vstr, c);
		}

		(void) varstr_complete (vstr);

		if ((kw = (*sxsvar.SXS_tables.S_check_keyword) (varstr_tostr (vstr), sxsvar.sxlv.ts_lgth)) > 0) {
		    /* C'est un predicat predefini */
		    sxsvar.sxlv.terminal_token.lahead = kw;
		    sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
		    sxsvar.sxlv.ts_lgth = 0;
		}
	    }
	}

	break;
    }

    return SXANY_BOOL;
}

