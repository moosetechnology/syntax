/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'�quipe Langages et Traducteurs.        *
   *							  *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030522 17:06 (phd):	Ajout de l'�l�ment -1 de SXT_node_info,	*/
/*				de T_stack_schema et de T_node_name	*/
/************************************************************************/

#include "sxunix.h"
#include "tables.h"
#include "out.h"
char WHAT_TABLESOUTTTBL[] = "@(#)SYNTAX - $Id: out_T_tbl.c 1120 2008-02-28 15:53:59Z rlacroix $" WHAT_DEBUG;



static VOID	out_tables (void)
{
    puts ("\nstatic struct SXT_tables SXT_tables=\n\
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};");
}



static VOID	out_T_node_info (void)
{
    SXINT	j, i;
    SXINT	d, nbt;

    out_struct ("SXT_node_info", "{0,0},");
    nbt = (TC.T_nbpro + 1) / slice_length2;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length2; j < d; j++) {
	    printf ("{%ld,%ld},", (long)TN [j].T_node_name, (long)TN [j].T_ss_indx);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= TC.T_nbpro + 1; j++) {
	printf ("{%ld,%ld},", (long)TN [j].T_node_name, (long)TN [j].T_ss_indx);
    }

    out_end_struct ();
}



static VOID	out_T_ter_to_node_name (void)
{
    SXINT	j, i;
    SXINT	nbt, d;

    out_int ("T_ter_to_node_name", "");
    nbt = TC.T_ter_to_node_name_size / slice_length;
    d = 0;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%ld,", (long)T.T_ter_to_node_name [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= TC.T_ter_to_node_name_size; j++) {
	printf ("%ld,", (long)T.T_ter_to_node_name [j]);
    }

    out_end_struct ();
}



static VOID	out_T_node_name (void)
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



static VOID	out_T_stack_schema (void)
{
    SXINT	j, i;
    SXINT	nbt, d;

    out_char ("T_stack_schema", "0,");
    nbt = TC.T_stack_schema_size / slice_length;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%ld,", (long)T.T_stack_schema [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= TC.T_stack_schema_size; j++) {
	printf ("%ld,", (long)T.T_stack_schema [j]);
    }

    out_end_struct ();
}


SXVOID
out_T_tables (void)
{
    if (sxverbosep)
	fputs ("\tAbstract Tree Tables\n", sxtty);

    out_T_node_info ();
    out_T_ter_to_node_name ();
    out_T_node_name ();
    out_ext_int_newstyle ("sempass(SXINT what, struct sxtables *sxtables_ptr)");
    out_T_stack_schema ();
    out_tables ();
}
