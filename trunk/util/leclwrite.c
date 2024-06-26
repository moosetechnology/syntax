/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/

#include "sxversion.h"
#include "sxunix.h"
#include "S_tables.h"

char WHAT_LECLWRITE[] = "@(#)SYNTAX - $Id: leclwrite.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;


#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error
#define BA_WRITE(f,b)	\
	if (!sxba_write (f, b))					\
		goto write_error


bool		lecl_write (struct lecl_tables_s *lecl_tables_ptr, char *langname)
{
    SXINT	bytes;
    SXINT	i;
    sxfiledesc_t	/* file descriptor */ F_st;
    SXINT		st_version;
    char	ent_name [128];
    static char		ME [] = "lecl_write";


/* sortie des tables du scanner */

    if ((F_st = open (strcat (strcpy (ent_name, langname), ".st"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    st_version = st_num_version /* numero de version des tables */ ;

    WRITE (F_st, &(st_version), sizeof (SXINT));
    WRITE (F_st, &(lecl_tables_ptr->bnf_modif_time), sizeof (SXINT));
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
    return true;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return false;
}
