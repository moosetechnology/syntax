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
char WHAT_PARADISREAD[] = "@(#)SYNTAX - $Id: paradisread.c 1215 2008-03-13 14:06:31Z rlacroix $" WHAT_DEBUG;


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


BOOLEAN		paradis_read (struct PP_ag_item *PP_ag, char *langname)
{
    SXINT	bytes;
    filedesc_t	/* file descriptor */ F_ppt;
    SXINT		ppt_version;
    char	ent_name [128];
    static char		ME [] = "paradis_read";


/* allocation et lecture des tables de paradis */

    if ((F_ppt = open (strcat (strcpy (ent_name, langname), ".ppt"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_ppt, &(ppt_version), sizeof (SXINT));

    if (ppt_version != ppt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new paradis\n");
	return (FALSE);
    }

    READ (F_ppt, &PP_ag->bnf_modif_time, sizeof (long));
    READ (F_ppt, &PP_ag->PP_constants, sizeof (struct PP_constants));
    READ (F_ppt, PP_ag->PP_indx = (SXINT*) sxalloc (PP_ag->PP_constants.PP_indx_lgth + 1, sizeof (SXINT)), (PP_ag->
	 PP_constants.PP_indx_lgth + 1) * sizeof (SXINT));
    READ (F_ppt, PP_ag->SXPP_schema = (struct SXPP_schema*) sxalloc (PP_ag->PP_constants.PP_schema_lgth + 1, sizeof (struct
	 SXPP_schema)), (PP_ag->PP_constants.PP_schema_lgth + 1) * sizeof (struct SXPP_schema));
    close (F_ppt);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
