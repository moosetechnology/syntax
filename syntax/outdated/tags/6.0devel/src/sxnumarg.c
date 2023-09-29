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

char WHAT_SXNUMARG[] = "@(#)SYNTAX - $Id: sxnumarg.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

SXBOOLEAN	sxnumarg (char *arg, SXINT *nb)
{
    SXINT	val = 0;
    char	c;

    switch (c = *arg) {
    case SXNUL:
	return SXFALSE;

    case '0':
	switch (c = *++arg) {
	case SXNUL:
	    break;

	case 'x':
	case 'X':
	    /* hexa */
          if ((c = *++arg))
		do {
		    val <<= 4;

		    if (c >= '0' && c <= '9')
			val += c - '0';
		    else if (c >= 'A' && c <= 'F')
			val += c - 'A' + 10;
		    else if (c >= 'a' && c <= 'f')
			val += c - 'a' + 10;
		    else
			return SXFALSE;
		} while ((c = *++arg));
	    else
		return SXFALSE;

	    break;

	default:
	    /* octal */
	    do {
		val <<= 3;

		if (c >= '0' && c <= '7')
		    val += c - '0';
		else
		    return SXFALSE;
	    } while ((c = *++arg));

	    break;
	}

	break;

    default:
	/* decimal */
	do {
	    val *= 10;

	    if (c >= '0' && c <= '9')
		val += c - '0';
	    else
		return SXFALSE;
	} while ((c = *++arg));

	break;
    }

    *nb = val;
    return SXTRUE;
}
