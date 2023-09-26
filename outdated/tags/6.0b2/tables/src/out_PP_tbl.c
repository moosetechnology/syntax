/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'équipe Langages et Traducteurs.        *
   *							  *
   ******************************************************** */





/* Sortie des tables du paragrapheur syntaxique produit par SYNTAX */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 17:24 (phd):	Ajout de l'élément -1 de PP_tables	*/
/************************************************************************/

#include "sxunix.h"
#include "tables.h"
#include "out.h"
char WHAT_TABLESOUTPPTBL[] = "@(#)SYNTAX - $Id: out_PP_tbl.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;



static VOID	out_schema ()
{
    register int	j, i;
    register int	d, nbt;

    out_struct ("SXPP_schema", "");
    nbt = PPC.PP_schema_lgth / slice_length2;
    d = 1;

    for (i = 1; i <= nbt; i++) {
	for (j = d, d += slice_length2; j < d; j++) {
	    printf ("{%d,%d},", PPS [j].PP_codop, PPS [j].PP_act);
	}

	putchar (NEWLINE);
    }

    for (j = d; j <= PPC.PP_schema_lgth; j++) {
	printf ("{%d,%d},", PPS [j].PP_codop, PPS [j].PP_act);
    }

    out_end_struct ();
}



static VOID	out_tables ()
{
    register int	i;

    puts ("\nstatic struct SXPP_schema *PP_tables[]={NULL,");

    for (i = 1; i <= PPC.PP_indx_lgth; i++) {
	printf ("&SXPP_schema[%d],\n", PP.PP_indx [i] - 1);
    }

    out_end_struct ();
}


SXVOID
out_PP_tables ()
{
    if (sxverbosep) {
	fputs ("\tPretty-Printer Tables\n", sxtty);
    }

    out_schema ();
    out_tables ();
}
