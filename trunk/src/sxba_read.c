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

/* Lecture de tableaux de bits pour SYNTAX */

#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"
#include <unistd.h>

char WHAT_SXBA_READ[] = "@(#)SYNTAX - $Id: sxba_read.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;


SXBOOLEAN
sxba_read (sxfiledesc_t file, SXBA bits_array)

/*
 * "sxba_read" reads the already allocated "bits_array" on "file".
 * "file" must be the result of "open".
 */

{
    SXBA_ELT	bits_number, bytes_number;

    if (read (file, &bits_number, sizeof (SXBA_ELT)) != sizeof (SXBA_ELT) || bits_number != SXBASIZE (bits_array)) {
	return SXFALSE;
    }

    bytes_number = SXNBLONGS (bits_number) * sizeof (SXBA_ELT);
    return (SXBA_ELT)(read (file, bits_array + 1, bytes_number)) == bytes_number;
}
