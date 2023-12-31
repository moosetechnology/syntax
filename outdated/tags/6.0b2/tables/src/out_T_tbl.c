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
char WHAT_TABLESOUTTTBL[] = "@(#)SYNTAX - $Id: out_T_tbl.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;



static VOID	out_tables ()
{
    puts ("\nstatic struct SXT_tables SXT_tables=\n\
{SXT_node_info, T_ter_to_node_name, T_stack_schema, sempass, T_node_name};");
}



static VOID	out_T_node_info ()
{
    register int	j, i;
    register int	d, nbt;

    out_struct ("SXT_node_info", "{0,0},");
    nbt = (TC.T_nbpro + 1) / slice_length2;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length2; j < d; j++) {
	    printf ("{%d,%d},", TN [j].T_node_name, TN [j].T_ss_indx);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= TC.T_nbpro + 1; j++) {
	printf ("{%d,%d},", TN [j].T_node_name, TN [j].T_ss_indx);
    }

    out_end_struct ();
}



static VOID	out_T_ter_to_node_name ()
{
    register int	j, i;
    register int	nbt, d;

    out_int ("T_ter_to_node_name", "");
    nbt = TC.T_ter_to_node_name_size / slice_length;
    d = 0;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%d,", T.T_ter_to_node_name [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= TC.T_ter_to_node_name_size; j++) {
	printf ("%d,", T.T_ter_to_node_name [j]);
    }

    out_end_struct ();
}



static VOID	out_T_node_name ()
{
    register int	i, lg;

    out_char ("*T_node_name", P0);

    for (i = 1; i <= TC.T_nbnod; i++) {
	if ((lg = T.T_nns_indx [i + 1] - T.T_nns_indx [i]) != 0) {
	    printf ("\"%.*s\",\n", lg, T.T_node_name_string + T.T_nns_indx [i]);
	}
	else
	    puts ("\"\",");
    }

    out_end_struct ();
}



static VOID	out_T_stack_schema ()
{
    register int	j, i;
    register int	nbt, d;

    out_char ("T_stack_schema", "0,");
    nbt = TC.T_stack_schema_size / slice_length;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length; j < d; j++) {
	    printf ("%d,", T.T_stack_schema [j]);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= TC.T_stack_schema_size; j++) {
	printf ("%d,", T.T_stack_schema [j]);
    }

    out_end_struct ();
}


SXVOID
out_T_tables ()
{
    if (sxverbosep)
	fputs ("\tAbstract Tree Tables\n", sxtty);

    out_T_node_info ();
    out_T_ter_to_node_name ();
    out_T_node_name ();
    out_ext_int ("sempass");
    out_T_stack_schema ();
    out_tables ();
}
