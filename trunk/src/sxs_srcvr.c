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

#ifndef VARIANT_32
char WHAT_SXS_SRECOVERY[] = "@(#)SYNTAX - $Id: sxs_srcvr.c 2489 2023-01-21 20:46:41Z garavel $" WHAT_DEBUG;
#endif

#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])

static SXBOOLEAN	recovery (SXINT state_no, unsigned char *class)
{
    SXBOOLEAN	is_error_in_la;

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
	return SXFALSE;
    }
    else {
	*class = char_to_class (is_error_in_la ? sxlanext_char () : sxnext_char ());
	return SXTRUE;
    }
}



SXBOOLEAN		sxssrecovery (SXINT sxssrecovery_what, SXINT state_no, unsigned char *class)
{
    switch (sxssrecovery_what) {
    case SXACTION:
	return recovery (state_no, class);

    case SXCLOSE:
	break;

    default:
	fprintf (sxstderr, "The function \"sxssrecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return SXTRUE;
}
