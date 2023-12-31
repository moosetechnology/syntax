/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)parser_write.c\t- SYNTAX [unix] - 31 Mai 1988";

#include "sxunix.h"
#include "P_tables.h"



#define WRITE(f,p,l)						\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


BOOLEAN		parser_write (parser_ag_ptr, langname)
    register struct parser_ag_item	*parser_ag_ptr;
    char	*langname;
{
    /* sortie des tables du parser */
    register int	bytes;
    register int	/* file descriptor */ F_pt;
    int		pt_version;
    char	parser_tables_name [128];
    static char		ME [] = "parser_write";

    if ((F_pt = creat (strcat (strcpy (parser_tables_name, langname), ".pt"), 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    pt_version = pt_num_version;
    /* numero de version des tables */

    WRITE (F_pt, &(pt_version), sizeof (int));
    WRITE (F_pt, &(parser_ag_ptr->bnf_modif_time), sizeof (long));
    WRITE (F_pt, &(parser_ag_ptr->constants), sizeof (struct P_constants));

    if (parser_ag_ptr->constants.mMred != 0)
	WRITE (F_pt, parser_ag_ptr->reductions + 1, -parser_ag_ptr->constants.mMred * sizeof (struct P_reduction));

    if (parser_ag_ptr->constants.Mtstate != 0)
	WRITE (F_pt, parser_ag_ptr->t_bases + 1, parser_ag_ptr->constants.Mtstate * sizeof (struct P_base));

    if (parser_ag_ptr->constants.Mntstate != 0)
	WRITE (F_pt, parser_ag_ptr->nt_bases + 1, parser_ag_ptr->constants.Mntstate * sizeof (struct P_base));

    if (parser_ag_ptr->constants.Mvec != 0)
	WRITE (F_pt, parser_ag_ptr->vector + 1, parser_ag_ptr->constants.Mvec * sizeof (struct P_item));

    if (parser_ag_ptr->constants.Mgotos != 0)
	WRITE (F_pt, parser_ag_ptr->gotos + 1, parser_ag_ptr->constants.Mgotos * sizeof (struct P_goto));

    if (parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred != 0)    
	WRITE (F_pt, parser_ag_ptr->prdcts + 1, (parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred) * sizeof (struct P_prdct));

    close (F_pt);
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (parser_tables_name);
    return FALSE;
}
