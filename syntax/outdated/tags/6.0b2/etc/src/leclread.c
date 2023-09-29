/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

#include "sxunix.h"
#include "S_tables.h"
char WHAT_LECLREAD[] = "@(#)SYNTAX - $Id: leclread.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error
#define BA_READ(f, b)	\
	if (!sxba_read (f,b))					\
		goto read_error

BOOLEAN		lecl_read (lecl_tables_ptr, langname)
    register struct lecl_tables_s	*lecl_tables_ptr;
    char	*langname;
{
    register int	bytes;
    register int	i;
    register int	/* file descriptor */ F_st;
    int		st_version;
    char	ent_name [128];
    static char		ME [] = "lecl_read";


/* allocation et lecture des tables du scanner */

    if ((F_st = open (strcat (strcpy (ent_name, langname), ".st"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_st, &(st_version), sizeof (int));

    if (st_version != st_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new lecl\n");
	return FALSE;
    }

    READ (F_st, &(lecl_tables_ptr->bnf_modif_time), sizeof (long));
    READ (F_st, &(lecl_tables_ptr->S_tbl_size), sizeof (struct S_tbl_size_s));
    READ (F_st, lecl_tables_ptr->S_char_to_simple_class = (unsigned char*) sxalloc (lecl_tables_ptr->S_tbl_size.
	 S_last_char_code + 1, sizeof (unsigned char)), (lecl_tables_ptr->S_tbl_size.S_last_char_code + 1) * sizeof (
	 unsigned char));
    lecl_tables_ptr->S_transition_matrix = (transition_matrix_s**) sxalloc (lecl_tables_ptr->S_tbl_size.
	 S_last_state_no + 1, sizeof (transition_matrix_s*));

    for (i = 1; i <= lecl_tables_ptr->S_tbl_size.S_last_state_no; i++) {
	READ (F_st, (lecl_tables_ptr->S_transition_matrix [i] = (transition_matrix_s*) sxalloc (lecl_tables_ptr->
	     S_tbl_size.S_last_simple_class_no + 1, sizeof (transition_matrix_s*))) + 1, lecl_tables_ptr->
	     S_tbl_size.S_last_simple_class_no * sizeof (transition_matrix_s));
    }

    READ (F_st, lecl_tables_ptr->SXS_action_or_prdct_code = (struct action_or_prdct_code_s*) sxalloc (lecl_tables_ptr->
	 S_tbl_size.S_xprod + 1, sizeof (struct action_or_prdct_code_s)), (lecl_tables_ptr->S_tbl_size.S_xprod + 1) *
	 sizeof (struct action_or_prdct_code_s));
    READ (F_st, lecl_tables_ptr->SXS_adrp = (struct S_adrp_s*) sxalloc (lecl_tables_ptr->S_tbl_size.
	 S_last_static_ste + 1, sizeof (struct S_adrp_s)), (lecl_tables_ptr->S_tbl_size.
	 S_last_static_ste + 1) * sizeof (struct S_adrp_s));
    READ (F_st, lecl_tables_ptr->S_terlis = (char*) sxalloc (lecl_tables_ptr->S_tbl_size.S_terlis_lgth + 1, sizeof (char)
	 ), (lecl_tables_ptr->S_tbl_size.S_terlis_lgth + 1) * sizeof (char));

    if (lecl_tables_ptr->S_tbl_size.S_check_kw_lgth != 0) {
	READ (F_st, lecl_tables_ptr->S_check_kw = (char*) sxalloc (lecl_tables_ptr->S_tbl_size.S_check_kw_lgth + 1, sizeof (char)
	 ), (lecl_tables_ptr->S_tbl_size.S_check_kw_lgth + 1) * sizeof (char));
    }
    else
	lecl_tables_ptr->S_check_kw = NULL;

    BA_READ (F_st, lecl_tables_ptr->S_is_a_generic_terminal = sxba_calloc (lecl_tables_ptr->S_tbl_size.S_termax + 1));
    BA_READ (F_st, lecl_tables_ptr->S_is_a_keyword = sxba_calloc (lecl_tables_ptr->S_tbl_size.S_termax + 1));
    close (F_st);
    return TRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
