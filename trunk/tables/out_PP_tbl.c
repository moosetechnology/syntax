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

/* Sortie des tables du paragrapheur syntaxique produit par SYNTAX */

#include "sxversion.h"
#include "sxunix.h"
#include "tables.h"
#include "out.h"

char WHAT_TABLESOUTPPTBL[] = "@(#)SYNTAX - $Id: out_PP_tbl.c 3650 2023-12-23 07:32:10Z garavel $" WHAT_DEBUG;


static void	out_schema (void)
{
    SXINT	j, i;
    SXINT	d, nbt;

    out_struct ("SXPP_schema", "");
    nbt = PPC.PP_schema_lgth / slice_length2;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length2; j < d; j++) {
	    printf ("{%ld,%ld},", PPS [j].PP_codop, PPS [j].PP_act);
	}

	putchar (SXNEWLINE);
    }

    for (j = d; j <= PPC.PP_schema_lgth; j++) {
	printf ("{%ld,%ld},", PPS [j].PP_codop, PPS [j].PP_act);
    }

    out_end_struct ();
}



static void	out_tables (void)
{
    SXINT	i;

    puts ("\nstatic struct SXPP_schema *PP_tables[]={NULL,");

    for (i = 1; i <= PPC.PP_indx_lgth; i++) {
	printf ("&SXPP_schema[%ld],\n", (SXINT) PP.PP_indx [i] - 1);
    }

    out_end_struct ();
}


void
out_PP_tables (void)
{
    if (sxverbosep) {
	fputs ("\tPretty-Printer Tables\n", sxtty);
    }

    out_schema ();
    out_tables ();
}
