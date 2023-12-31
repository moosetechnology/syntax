/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe ChLoE.(pb)			  *
   *                                                      *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 02-03-94 10:37 (pb):	Ajout de cette rubrique				*/
/************************************************************************/

#include "sxdynam_scanner.h"
char WHAT_TABLESOUTDSTBL[] = "@(#)SYNTAX - $Id: out_DS_tbl.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

static char	ME [] = "DYNAMIC_SCANNER_read";

struct sxdynam_scanner	ds;


BOOLEAN
ds_read (language_name)
     char *language_name;
{
    int		file_descr	/* file descriptor */ ;
    char	file_name [128];

    if (sxverbosep)
	fputs ("\tDynamic Scanner Tables\n", sxtty);

    if ((file_descr = open (strcat (strcpy (file_name, language_name), ".ds"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open ", ME);
	sxperror (file_name);
	return FALSE;
    }

    SXDS = &(ds);

    if (!sxre_read (file_descr))
    {
	fprintf (sxstderr, "%s: cannot read ", ME);
	sxperror (file_name);
	return FALSE;
    }

    close (file_descr);

    fprintf (sxstdout, "\n#include \"sxdynam_scanner.h\"\n");

    sxre_to_c (sxstdout, "DYNAM_SCANNER", FALSE);

    return TRUE;
}

