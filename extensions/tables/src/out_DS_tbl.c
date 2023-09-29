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
#include "sxdynam_scanner.h"

char WHAT_TABLESOUTDSTBL[] = "@(#)SYNTAX - $Id: out_DS_tbl.c 3305 2023-05-26 09:00:36Z garavel $" WHAT_DEBUG;

static char	ME [] = "DYNAMIC_SCANNER_read";

struct sxdynam_scanner	ds;


SXBOOLEAN
ds_read (char *ln)
{
    sxfiledesc_t	file_descr	/* file descriptor */ ;
    char	file_name [128];

    if (sxverbosep)
	fputs ("\tDynamic Scanner Tables\n", sxtty);

    if ((file_descr = open (strcat (strcpy (file_name, ln), ".ds"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open ", ME);
	sxperror (file_name);
	return SXFALSE;
    }

    SXDS = &(ds);

    if (!sxre_read (file_descr))
    {
	fprintf (sxstderr, "%s: cannot read ", ME);
	sxperror (file_name);
	return SXFALSE;
    }

    close (file_descr);

    fprintf (sxstdout, "\n#include \"sxdynam_scanner.h\"\n");

    sxre_to_c (sxstdout, "DYNAM_SCANNER", SXFALSE);

    return SXTRUE;
}

