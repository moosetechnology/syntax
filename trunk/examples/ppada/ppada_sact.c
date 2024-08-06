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
#include "ppada_td.h"
#include <ctype.h>

char WHAT_PPADASACT[] = "@(#)SYNTAX - $Id: ppada_sact.c 4125 2024-07-29 10:59:13Z garavel $";

extern struct sxtables	ppada_args_tables, ppada_tables;



static void	gripe (struct sxtables *sxtables, SXINT act_no)
{
    sxuse (act_no);
    sxerror (sxsvar.sxlv.terminal_token.source_index,
	     sxtables->err_titles [2][0],
	     "%sInternal inconsistency: Action %d not yet implemented.\nExecution aborted.",
	     sxtables->err_titles [2]+1);
    sxexit (3);
}


bool ppada_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe (&ppada_tables, code);
	    /*NOTREACHED*/

    case SXBEGIN:
    case SXEND:
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	break;

    case SXACTION:
	switch (act_no) {
	case 1:
	    /* {lower} UPPER {LETTER} */
	    switch (sxsvar.sxlv.ts_lgth) {
		static char	string [10];

	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
		{
		    char	c, *str1, *str2;

		    str1 = sxsvar.sxlv_s.token_string, str2 = string;

		    for (; ; ) {
			if (isupper (c = *str1++)) {
			    *str2++ = c - 'A' + 'a';
			    continue;
			}

			if ((*str2++ = c) == SXNUL)
			    break;
		    }
		}

		if ((*sxsvar.SXS_tables.S_check_keyword) (string, sxsvar.sxlv.ts_lgth)) {
		    strcpy (sxsvar.sxlv_s.token_string, string);
		}

	    default:
		break;
	    }

	    break;

	case 4:
	    /* comments: rtrim (ESPACE, TAB, EOL) */
	    for (; ; ) {
		switch (sxsvar.sxlv_s.token_string [--sxsvar.sxlv.ts_lgth]) {
		case '\n':
		case '\t':
		case ' ':
		    if (sxsvar.sxlv.ts_lgth == 0) {
			sxsvar.sxlv_s.token_string [0] = SXNUL;
			break;
		    }

		    continue;

		default:
		    sxsvar.sxlv_s.token_string [++sxsvar.sxlv.ts_lgth] = '\n';
		    sxsvar.sxlv_s.token_string [++sxsvar.sxlv.ts_lgth] = SXNUL;
		    break;
		}

		break;
	    }

	    break;

	default:
	    gripe (&ppada_tables, act_no);
		/*NOTREACHED*/
	}

	break;

    case SXPREDICATE:
	switch (act_no) {
	case 1:
	    switch (sxsvar.sxlv.terminal_token.lahead) {
	    default:
		return false;

	    case IDENTIFIER:
	    case RP:
		return true;
	    }
	default:
	    gripe (&ppada_tables, act_no);
		/*NOTREACHED*/
	}
    }

    return SXANY_BOOL;
}


bool ppada_args_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe (&ppada_args_tables, code);

    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	break;

    case SXACTION:
	switch (act_no) {
	    SXINT	length;
	    char	*mark, *value;

	case 3:
	    if ((value = getenv (mark = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index)) == NULL) {
		sxerror (sxsvar.sxlv.mark.source_coord,
			 ppada_args_tables.err_titles [2][0],
			 "%s%s is not in the environment.\n",
			 ppada_args_tables.err_titles [2]+1,
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
	    gripe (&ppada_args_tables, act_no);
		/*NOTREACHED*/
	}
	break;
    }
    return SXANY_BOOL;
}
