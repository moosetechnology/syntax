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
#include "R_tables.h"
char WHAT_RECORREAD[] = "@(#)SYNTAX - $Id: recorread.c 1215 2008-03-13 14:06:31Z rlacroix $" WHAT_DEBUG;


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error; else ((void)0)
#define BA_READ(f, b)	{\
	if (!sxba_read (f,b))					\
		goto read_error; }


BOOLEAN		recor_read (struct R_tables_s *R_tables, char *langname)
{
    SXINT	bytes;
    SXINT	i;
    filedesc_t	/* file descriptor */ F_rt;
    SXINT		rt_version;
    char	ent_name [128];
    static char		ME [] = "recor_read";


/* allocation et lecture des tables de recor */

    if ((F_rt = open (strcat (strcpy (ent_name, langname), ".rt"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_rt, &(rt_version), sizeof (SXINT));

    if (rt_version != rt_num_version) {
	fprintf (sxstderr, "tables format has changed: please use the new recor\n");
	return FALSE;
    }

    READ (F_rt, (char*) &R_tables->bnf_modif_time, sizeof (long));
    READ (F_rt, (char*) &R_tables->R_tbl_size, sizeof (struct R_tbl_size_s));
    R_tables->S_lregle = R_tables->R_tbl_size.S_nbcart == 0 ? NULL : (SXINT**) sxalloc (R_tables->R_tbl_size.S_nbcart + 1, sizeof (SXINT*));

    for (i = 1; i <= R_tables->R_tbl_size.S_nbcart; i++)
	READ (F_rt, R_tables->S_lregle [i] = (SXINT*) sxalloc (R_tables->R_tbl_size.S_maxlregle + 1, sizeof (SXINT)), (
	     R_tables->R_tbl_size.S_maxlregle + 1) * sizeof (SXINT));

    R_tables->SXS_local_mess = R_tables->R_tbl_size.S_nbcart == 0 ? NULL : (struct local_mess*) sxalloc (R_tables->R_tbl_size.S_nbcart + 1, sizeof (struct local_mess))
	 ;

    for (i = 1; i <= R_tables->R_tbl_size.S_nbcart; i++)
	READ (F_rt, &(R_tables->SXS_local_mess [i].param_no), sizeof (SXINT));

    for (i = 1; i <= R_tables->R_tbl_size.S_nbcart; i++)
	READ (F_rt, R_tables->SXS_local_mess [i].string_ref = (struct string_ref*) sxalloc (R_tables->R_tbl_size.S_maxparam
	     + 1, sizeof (struct string_ref)), (R_tables->R_tbl_size.S_maxparam + 1) * sizeof (struct string_ref));

    READ (F_rt, R_tables->S_global_mess = (struct global_mess*) sxalloc (6, sizeof (struct global_mess)), 6 * sizeof (
	 struct global_mess));
    READ (F_rt, R_tables->S_string_mess = sxalloc (R_tables->R_tbl_size.S_lstring_mess, sizeof (char)), R_tables->
	 R_tbl_size.S_lstring_mess * sizeof (char));

    if (R_tables->R_tbl_size.S_nbcart != 0) {
	BA_READ (F_rt, R_tables->S_no_delete = sxba_calloc (R_tables->R_tbl_size.S_last_char_code + 1));
	BA_READ (F_rt, R_tables->S_no_insert = sxba_calloc (R_tables->R_tbl_size.S_last_char_code + 1));
    }
    else
	R_tables->S_no_delete = R_tables->S_no_insert = NULL;
	
    R_tables->P_lregle = R_tables->R_tbl_size.P_nbcart == 0 ? NULL : (SXINT**) sxalloc (R_tables->R_tbl_size.P_nbcart + 1, sizeof (SXINT*));

    for (i = 1; i <= R_tables->R_tbl_size.P_nbcart; i++)
	READ (F_rt, R_tables->P_lregle [i] = (SXINT*) sxalloc (R_tables->R_tbl_size.P_maxlregle + 1, sizeof (SXINT)), (
	     R_tables->R_tbl_size.P_maxlregle + 1) * sizeof (SXINT));

    R_tables->P_right_ctxt_head = R_tables->R_tbl_size.P_nbcart == 0 ? NULL : (SXINT*) sxalloc (R_tables->R_tbl_size.P_nbcart + 1, sizeof (SXINT));

    if (R_tables->R_tbl_size.P_nbcart != 0) {
	READ (F_rt, R_tables->P_right_ctxt_head, (R_tables->R_tbl_size.P_nbcart + 1) * sizeof (SXINT));
    }

    R_tables->SXP_local_mess = (struct local_mess*) sxalloc (R_tables->R_tbl_size.P_nbcart + 2, sizeof (struct local_mess))
	 ;

    for (i = 1; i <= R_tables->R_tbl_size.P_nbcart + 1; i++)
	READ (F_rt, &(R_tables->SXP_local_mess [i].param_no), sizeof (SXINT));

    for (i = 1; i <= R_tables->R_tbl_size.P_nbcart + 1; i++)
	READ (F_rt, R_tables->SXP_local_mess [i].string_ref = (struct string_ref*) sxalloc (R_tables->R_tbl_size.P_maxparam
	     + 1, sizeof (struct string_ref)), (R_tables->R_tbl_size.P_maxparam + 1) * sizeof (struct string_ref));

    READ (F_rt, R_tables->P_global_mess = (struct global_mess*) sxalloc (6, sizeof (struct global_mess)), 6 * sizeof (
	 struct global_mess));
    READ (F_rt, R_tables->P_string_mess = sxalloc (R_tables->R_tbl_size.P_lstring_mess, sizeof (char)), R_tables->
	 R_tbl_size.P_lstring_mess * sizeof (char));

    if (R_tables->R_tbl_size.P_nbcart != 0) {
	BA_READ (F_rt, R_tables->P_no_delete = sxba_calloc (R_tables->R_tbl_size.P_tmax + 1));
	BA_READ (F_rt, R_tables->P_no_insert = sxba_calloc (R_tables->R_tbl_size.P_tmax + 1));
    }
    else
	R_tables->P_no_delete = R_tables->P_no_insert = NULL;

    BA_READ (F_rt, R_tables->PER_tset = sxba_calloc (R_tables->R_tbl_size.P_tmax + 1));
    R_tables->E_titles = (char**) sxalloc (R_tables->R_tbl_size.E_nb_kind + 1, sizeof (char*));

    for (i = 1; i <= R_tables->R_tbl_size.E_nb_kind; i++)
	READ (F_rt, R_tables->E_titles [i] = sxalloc (R_tables->R_tbl_size.E_maxltitles, sizeof (char)), R_tables->
	     R_tbl_size.E_maxltitles * sizeof (char));

    if (R_tables->R_tbl_size.E_labstract != 0)
        READ (F_rt, R_tables->E_abstract = sxalloc (R_tables->R_tbl_size.E_labstract, sizeof (char)), R_tables->R_tbl_size.E_labstract * sizeof (char));
    else
	R_tables->E_abstract = NULL;

    close (F_rt);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
