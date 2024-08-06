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

/*
 * a simple module for processing lexical level errors : sxssrecovery() only
 * deletes the erroneous character, while the standard recovery function
 * sxsrecovery() performs more advanced correction
*/
#include "sxversion.h"
#include "sxunix.h"

#ifndef VARIANT_32
char WHAT_SXS_SRECOVERY[] = "@(#)SYNTAX - $Id: sxs_srcvr.c 4083 2024-06-20 11:47:40Z garavel $" WHAT_DEBUG;
#endif

#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])

static bool	recovery (SXINT state_no, unsigned char *class)
{
    bool	is_error_in_la;

    if (!(is_error_in_la = sxsvar.SXS_tables.S_transition_matrix [state_no] [1]) &&
	!sxsvar.sxlv.mode.is_silent) {
	char	*s;

	switch (sxsrcmngr.current_char) {
	case EOF:
	    s = sxsvar.SXS_tables.S_global_mess [4];
	    break;

	case SXNEWLINE:
	    s = sxsvar.SXS_tables.S_global_mess [3];
	    break;

	default:
	    s = sxc_to_str [sxsrcmngr.current_char];
	    break;
	}

	sxerror (sxsrcmngr.source_coord, 
		 sxsvar.sxtables->err_titles [2][0],
		 sxsvar.SXS_tables.S_global_mess [1], 
		 sxsvar.sxtables->err_titles [2]+1, 
		 s);
    }

    if (sxsrcmngr.current_char == EOF) {
	sxsvar.sxlv.terminal_token.lahead = sxsvar.SXS_tables.S_termax /* End Of File */ ;
	*class = 2;
	return false;
    }
    else {
	*class = char_to_class (is_error_in_la ? sxlanext_char () : sxnext_char ());
	return true;
    }
}



bool		sxssrecovery (SXINT what, SXINT state_no, unsigned char *class)
{
    switch (what) {
    case SXACTION:
	return recovery (state_no, class);

    case SXCLOSE:
	break;

    default:
	fprintf (sxstderr, "The function \"sxssrecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return true;
}
