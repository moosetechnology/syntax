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

static char ME [] = "SXPARSACT";

char WHAT_SXPARSACT[] = "@(#)SYNTAX - $Id: sxparsact.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

/*
 * This is the default sxparser_action() function, which can be replaced by
 * the one explicitly defined by the user
 */

bool sxparser_action (SXINT what, SXINT action_no)
{
    switch (what) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
        /* do nothing */
	return SXANY_BOOL;

    case SXACTION:
	switch (action_no) {
#if 0
	case 1:
	case 2:
        /* etc */
#endif
	default:
	   fprintf (sxstderr, "%s: the default sxparser_action() function defined in sxparsact.c should not be invoked with (SXACTION, %ld) parameters\n", ME, action_no);
	   fprintf (sxstderr, "%s: in presence of syntactic actions, a specific sxparser_action() function must be defined explicitly\n", ME);
           sxexit (1);
	}
	break;

    case SXPREDICATE:
	switch (action_no) {
#if 0
	case 1:
	case 2:
        /* etc */
#endif
	default:
	   fprintf (sxstderr, "%s: the default sxparser_action() function defined in sxparsact.c should not be invoked with (SXPREDICATE, %ld) parameters\n", ME, action_no);
	   fprintf (sxstderr, "%s: in presence of syntactic predicates, a specific sxparser_action() function must be defined explicitly\n", ME);
           sxexit (1);
	}
	break;

    default:
        fprintf (sxstderr, "%s: invalid parameter %ld given to the default sxparser_action() function\n", ME, what);
        sxexit(1);
	break;
    }

    return SXANY_BOOL; /* pour faire taire gcc -Wreturn-type */
}

