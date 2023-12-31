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
   *  Produit de l'�quipe Langages et Traducteurs.        *
   *							  *
   ******************************************************** */





/* Sortie des tables du paragrapheur syntaxique produit par SYNTAX */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030527 17:24 (phd):	Ajout de l'�l�ment -1 de PP_tables	*/
/************************************************************************/

#include "sxunix.h"
#include "tables.h"
#include "out.h"
char WHAT_TABLESOUTPPTBL[] = "@(#)SYNTAX - $Id: out_PP_tbl.c 1120 2008-02-28 15:53:59Z rlacroix $" WHAT_DEBUG;



static VOID	out_schema (void)
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

	putchar (NEWLINE);
    }

    for (j = d; j <= PPC.PP_schema_lgth; j++) {
	printf ("{%ld,%ld},", PPS [j].PP_codop, PPS [j].PP_act);
    }

    out_end_struct ();
}



static VOID	out_tables (void)
{
    SXINT	i;

    puts ("\nstatic struct SXPP_schema *PP_tables[]={NULL,");

    for (i = 1; i <= PPC.PP_indx_lgth; i++) {
	printf ("&SXPP_schema[%ld],\n", (long)PP.PP_indx [i] - 1);
    }

    out_end_struct ();
}


SXVOID
out_PP_tables (void)
{
    if (sxverbosep) {
	fputs ("\tPretty-Printer Tables\n", sxtty);
    }

    out_schema ();
    out_tables ();
}
