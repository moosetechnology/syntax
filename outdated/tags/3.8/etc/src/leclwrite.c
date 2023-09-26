/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */

static char	what [] = "@(#)lecl_write.c\t- SYNTAX [unix] - 31 Mai 1988";

#include "sxunix.h"
#include "S_tables.h"


#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error
#define BA_WRITE(f,b)	\
	if (!sxba_write (f, b))					\
		goto write_error


BOOLEAN		lecl_write (lecl_tables_ptr, langname)
    register struct lecl_tables_s	*lecl_tables_ptr;
    char	*langname;
{
    register int	bytes;
    register int	i;
    register int	/* file descriptor */ F_st;
    int		st_version;
    char	ent_name [128];
    static char		ME [] = "lecl_write";


/* sortie des tables du scanner */

    if ((F_st = creat (strcat (strcpy (ent_name, langname), ".st"), 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    st_version = st_num_version /* numero de version des tables */ ;

    WRITE (F_st, &(st_version), sizeof (int));
    WRITE (F_st, &(lecl_tables_ptr->bnf_modif_time), sizeof (long));
    WRITE (F_st, &(lecl_tables_ptr->S_tbl_size), sizeof (struct S_tbl_size_s));
    WRITE (F_st, lecl_tables_ptr->S_char_to_simple_class, (lecl_tables_ptr->S_tbl_size.S_last_char_code + 1) * sizeof (
	 unsigned char));

    for (i = 1; i <= lecl_tables_ptr->S_tbl_size.S_last_state_no; i++) {
	WRITE (F_st, lecl_tables_ptr->S_transition_matrix [i] + 1, lecl_tables_ptr->S_tbl_size.S_last_simple_class_no *
	     sizeof (transition_matrix_s));
    }

    WRITE (F_st, lecl_tables_ptr->SXS_action_or_prdct_code, (lecl_tables_ptr->S_tbl_size.S_xprod + 1) * sizeof (struct
	 action_or_prdct_code_s));
    WRITE (F_st, lecl_tables_ptr->SXS_adrp, (lecl_tables_ptr->S_tbl_size.S_last_static_ste + 1) * sizeof (
	 struct S_adrp_s));
    WRITE (F_st, lecl_tables_ptr->S_terlis, (lecl_tables_ptr->S_tbl_size.S_terlis_lgth + 1) * sizeof (char));

    if (lecl_tables_ptr->S_tbl_size.S_check_kw_lgth != 0) {
	WRITE (F_st, lecl_tables_ptr->S_check_kw, (lecl_tables_ptr->S_tbl_size.S_check_kw_lgth + 1) * sizeof (char));
    }

    BA_WRITE (F_st, lecl_tables_ptr->S_is_a_generic_terminal);
    BA_WRITE (F_st, lecl_tables_ptr->S_is_a_keyword);
    close (F_st);
    return TRUE;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return FALSE;
}
