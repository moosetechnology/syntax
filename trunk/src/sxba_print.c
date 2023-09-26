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

char WHAT_SXBAPRINT[] = "@(#)SYNTAX - $Id: sxba_print.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;

SXVOID	sxba_print (SXBA s)
        	  

/* affiche sur la sortie standard la chaine de bits s; procedure de service */

{
    SXBA_INDEX	i;

    for (i = 0; (SXBA_ELT)i < (*s); i++)
	putchar (sxba_bit_is_set (s, i) ? '1' : '0');

    putchar ('\n');
}
