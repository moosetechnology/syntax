/* ********************************************************
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

#include "sxunix.h"

#include "PP_tables.h"
char WHAT_PARADISWRITE[] = "@(#)SYNTAX - $Id: paradiswrite.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


BOOLEAN		paradis_write (PP_ag, langname)
    register struct PP_ag_item	*PP_ag;
    char	*langname;
{
    register int	bytes;
    register int	/* file descriptor */ F_ppt;
    int		ppt_version;
    char	ent_name [128];
    static char		ME [] = "paradis_write";


/* sortie des tables de paradis */

    if ((F_ppt = open (strcat (strcpy (ent_name, langname), ".ppt"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    ppt_version = ppt_num_version;
    /* numero de version des tables */

    WRITE (F_ppt, &(ppt_version), sizeof (int));
    WRITE (F_ppt, &PP_ag->bnf_modif_time, sizeof (long));
    WRITE (F_ppt, &PP_ag->PP_constants, sizeof (struct PP_constants));
    WRITE (F_ppt, PP_ag->PP_indx, (PP_ag->PP_constants.PP_indx_lgth + 1) * sizeof (int));
    WRITE (F_ppt, PP_ag->SXPP_schema, (PP_ag->PP_constants.PP_schema_lgth + 1) * sizeof (struct SXPP_schema));
    close (F_ppt);
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
