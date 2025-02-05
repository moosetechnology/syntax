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
#include "tables.h"
#include "out.h"

char WHAT_TABLESOUTTTBL[] = "@(#)SYNTAX - $Id: out_T_tbl.c 4076 2024-06-19 10:31:35Z garavel $" WHAT_DEBUG;


static void	out_tables (void)
{
    puts ("\nstatic struct SXT_tables SXT_tables={");
    puts ("SXT_node_info, T_ter_to_node_name, T_stack_schema, SEMPASS, T_node_name");
    puts ("};");
}



static void	out_T_node_info (void)
{
    SXINT	j, i;
    SXINT	d, nbt;

    out_struct ("SXT_node_info", "{0,0},");
    nbt = (TC.T_nbpro + 1) / slice_length2;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length2; j < d; j++) {
	    printf ("{%ld,%ld},", (SXINT) TN [j].T_node_name, (SXINT) TN [j].T_ss_indx);
	}

	putchar (SXNEWLINE);
    }

    for (j = d; j <= TC.T_nbpro + 1; j++) {
	printf ("{%ld,%ld},", (SXINT) TN [j].T_node_name, (SXINT) TN [j].T_ss_indx);
    }

    out_end_struct ();
}



static void	out_T_ter_to_node_name (void)
{
    SXINT	j, i;
    SXINT	nbt, d;

    out_int ("T_ter_to_node_name", "");
    nbt = TC.T_ter_to_node_name_size / slice_length;
    d = 0;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%ld,", (SXINT) T.T_ter_to_node_name [j]);
	}

	putchar (SXNEWLINE);
    }

    for (j = d; j <= TC.T_ter_to_node_name_size; j++) {
	printf ("%ld,", (SXINT) T.T_ter_to_node_name [j]);
    }

    out_end_struct ();
}



static void	out_T_node_name (void)
{
    SXINT	i, lg;

    out_char ("*T_node_name", P0);

    for (i = 1; i <= TC.T_nbnod; i++) {
	if ((lg = T.T_nns_indx [i + 1] - T.T_nns_indx [i]) != 0) {
	    /* LINTED this cast from long to int is needed by printf() */
	    printf ("\"%.*s\",\n", (int)lg, T.T_node_name_string + T.T_nns_indx [i]);
	}
	else
	    puts ("\"\",");
    }

    out_end_struct ();
}



static void	out_T_stack_schema (void)
{
    SXINT	j, i;
    SXINT	nbt, d;

    out_char ("T_stack_schema", "0,");
    nbt = TC.T_stack_schema_size / slice_length;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%ld,", (SXINT) T.T_stack_schema [j]);
	}

	putchar (SXNEWLINE);
    }

    for (j = d; j <= TC.T_stack_schema_size; j++) {
	printf ("%ld,", (SXINT) T.T_stack_schema [j]);
    }

    out_end_struct ();
}


void
out_T_tables (void)
{
    if (sxverbosep)
	fputs ("\tAbstract Tree Tables\n", sxtty);

    out_T_node_info ();
    out_T_ter_to_node_name ();
    out_T_node_name ();
    puts ("extern SXSEMPASS_FUNCTION SEMPASS;");
    out_T_stack_schema ();
    out_tables ();
}
