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
#include "T_tables.h"
char WHAT_SEMATWRITE[] = "@(#)SYNTAX - $Id: sematwrite.c 1215 2008-03-13 14:06:31Z rlacroix $" WHAT_DEBUG;


#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


BOOLEAN		semat_write (struct T_ag_item *T_ag, char *langname)
{
    SXINT	bytes;
    filedesc_t	/* file descriptor */ F_att;
    SXINT		att_version;
    char	ent_name [128];
    static char		ME [] = "semat_write";


/* sortie des tables de semat */

    if ((F_att = open (strcat (strcpy (ent_name, langname), ".att"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    att_version = att_num_version;
    /* numero de version des tables */

    WRITE (F_att, &(att_version), sizeof (SXINT));
    WRITE (F_att, &T_ag->bnf_modif_time, sizeof (long));
    WRITE (F_att, &T_ag->T_constants, sizeof (struct T_constants));
    WRITE (F_att, T_ag->SXT_node_info, (T_ag->T_constants.T_nbpro + 2) * sizeof (struct SXT_node_info));
    WRITE (F_att, T_ag->T_stack_schema, (T_ag->T_constants.T_stack_schema_size + 1) * sizeof (SXINT));
    WRITE (F_att, T_ag->T_nns_indx, (T_ag->T_constants.T_nbnod + 2) * sizeof (SXINT));
    WRITE (F_att, T_ag->T_ter_to_node_name, (T_ag->T_constants.T_ter_to_node_name_size + 1) * sizeof (SXINT));
    WRITE (F_att, T_ag->T_node_name_string, T_ag->T_constants.T_node_name_string_lgth * sizeof (char));
    close (F_att);
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
