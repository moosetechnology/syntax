/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)parser_read.c\t- SYNTAX [unix] - 31 Mai 1988";

#include "sxunix.h"
#include "P_tables.h"



#define READ(f,p,l)						\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


BOOLEAN		parser_read (parser_ag_ptr, langname)
    register struct parser_ag_item	*parser_ag_ptr;
    char	*langname;
{
    register int	bytes;
    register int	/* file descriptor */ F_pt;
    int		pt_version;
    char	parser_tables_name [128];
    static char		ME [] = "parser_read";


/* allocation et lecture des tables du parser */

    if ((F_pt = open (strcat (strcpy (parser_tables_name, langname), ".pt"), 0)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_pt, &(pt_version), sizeof (int));

    if (pt_version != pt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new csynt\n");
	return (FALSE);
    }

    READ (F_pt, &(parser_ag_ptr->bnf_modif_time), sizeof (long));
    READ (F_pt, &(parser_ag_ptr->constants), sizeof (struct P_constants));

    if (parser_ag_ptr->constants.mMred != 0) {
	parser_ag_ptr->reductions = (struct P_reduction*) sxalloc (-parser_ag_ptr->constants.mMred + 1, sizeof (struct P_reduction));
	READ (F_pt, parser_ag_ptr->reductions + 1, -parser_ag_ptr->constants.mMred * sizeof (struct P_reduction));
    }
    else
	parser_ag_ptr->reductions = NULL;

    if (parser_ag_ptr->constants.Mtstate != 0) {
	parser_ag_ptr->t_bases = (struct P_base*) sxalloc (parser_ag_ptr->constants.Mtstate + 1, sizeof (struct P_base));
	READ (F_pt, parser_ag_ptr->t_bases + 1, parser_ag_ptr->constants.Mtstate * sizeof (struct P_base));
    }
    else
	parser_ag_ptr->t_bases = NULL;

    if (parser_ag_ptr->constants.Mntstate != 0) {
	parser_ag_ptr->nt_bases = (struct P_base*) sxalloc (parser_ag_ptr->constants.Mntstate + 1, sizeof (struct P_base));
	READ (F_pt, parser_ag_ptr->nt_bases + 1, parser_ag_ptr->constants.Mntstate * sizeof (struct P_base));
    }
    else
	parser_ag_ptr->nt_bases = NULL;

    if (parser_ag_ptr->constants.Mvec != 0) {
	parser_ag_ptr->vector = (struct P_item*) sxalloc (parser_ag_ptr->constants.Mvec + 1, sizeof (struct P_item));
	READ (F_pt, parser_ag_ptr->vector + 1, parser_ag_ptr->constants.Mvec * sizeof (struct P_item));
    }
    else
	parser_ag_ptr->vector = NULL;

    if (parser_ag_ptr->constants.Mgotos != 0) {
	parser_ag_ptr->gotos = (struct P_goto*) sxalloc (parser_ag_ptr->constants.Mgotos + 1, sizeof (struct P_goto));
	READ (F_pt, parser_ag_ptr->gotos + 1, parser_ag_ptr->constants.Mgotos * sizeof (struct P_goto));
    }
    else
	parser_ag_ptr->gotos = NULL;

    if (parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred != 0)     {
	parser_ag_ptr->prdcts = (struct P_prdct*) sxalloc ((parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred) + 1, sizeof (struct P_prdct));
	READ (F_pt, parser_ag_ptr->prdcts + 1, (parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred) * sizeof (struct P_prdct));
    }
    else
	parser_ag_ptr->prdcts = NULL;

    close (F_pt);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (parser_tables_name);
    return FALSE;
}
