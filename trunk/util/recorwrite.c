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
#include "R_tables.h"

char WHAT_RECORWRITE[] = "@(#)SYNTAX - $Id: recorwrite.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#define WRITE(f,p,l)		\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error
#define BA_WRITE(f,b)	\
	if (!sxba_write (f, b))					\
		goto write_error


bool		recor_write (struct R_tables_s *R_tables, char *langname)
{
    SXINT	bytes;
    SXINT	i;
    sxfiledesc_t	/* file descriptor */ F_rt;
    SXINT		rt_version;
    char	ent_name [128];
    static char		ME [] = "recor_write";


/* sortie des tables de recor */

    if ((F_rt = open (strcat (strcpy (ent_name, langname), ".rt"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    rt_version = rt_num_version;
    /* numero de version des tables */

    WRITE (F_rt, &(rt_version), sizeof (SXINT));
    WRITE (F_rt, &R_tables->bnf_modif_time, sizeof (SXINT));
    WRITE (F_rt, &R_tables->R_tbl_size, sizeof (struct R_tbl_size_s));

    for (i = 1; i <= R_tables->R_tbl_size.S_nbcart; i++)
	WRITE (F_rt, R_tables->S_lregle [i], (R_tables->R_tbl_size.S_maxlregle + 1) * sizeof (SXINT));

    for (i = 1; i <= R_tables->R_tbl_size.S_nbcart; i++)
	WRITE (F_rt, &(R_tables->SXS_local_mess [i].param_no), sizeof (SXINT));

    for (i = 1; i <= R_tables->R_tbl_size.S_nbcart; i++)
	WRITE (F_rt, R_tables->SXS_local_mess [i].string_ref, (R_tables->R_tbl_size.S_maxparam + 1) * sizeof (struct
	     string_ref));

    WRITE (F_rt, R_tables->S_global_mess, 6 * sizeof (struct global_mess));
    WRITE (F_rt, R_tables->S_string_mess, R_tables->R_tbl_size.S_lstring_mess * sizeof (char));

    if (R_tables->S_no_delete != NULL)
	BA_WRITE (F_rt, R_tables->S_no_delete);

    if (R_tables->S_no_insert != NULL)
	BA_WRITE (F_rt, R_tables->S_no_insert);

    for (i = 1; i <= R_tables->R_tbl_size.P_nbcart; i++)
	WRITE (F_rt, R_tables->P_lregle [i], (R_tables->R_tbl_size.P_maxlregle + 1) * sizeof (SXINT));

    if (R_tables->R_tbl_size.P_nbcart != 0)
	WRITE (F_rt, R_tables->P_right_ctxt_head, (R_tables->R_tbl_size.P_nbcart + 1) * sizeof (SXINT));

    for (i = 1; i <= R_tables->R_tbl_size.P_nbcart + 1; i++)
	WRITE (F_rt, &(R_tables->SXP_local_mess [i].param_no), sizeof (SXINT));

    for (i = 1; i <= R_tables->R_tbl_size.P_nbcart + 1; i++)
	WRITE (F_rt, R_tables->SXP_local_mess [i].string_ref, (R_tables->R_tbl_size.P_maxparam + 1) * sizeof (struct
	     string_ref));

    WRITE (F_rt, R_tables->P_global_mess, 6 * sizeof (struct global_mess));
    WRITE (F_rt, R_tables->P_string_mess, R_tables->R_tbl_size.P_lstring_mess * sizeof (char));

    if (R_tables->S_no_delete != NULL)
	BA_WRITE (F_rt, R_tables->P_no_delete);

    if (R_tables->P_no_insert != NULL)
	BA_WRITE (F_rt, R_tables->P_no_insert);

    BA_WRITE (F_rt, R_tables->PER_tset);

    for (i = 1; i <= R_tables->R_tbl_size.E_nb_kind; i++)
	WRITE (F_rt, R_tables->E_titles [i], R_tables->R_tbl_size.E_maxltitles * sizeof (char));

    if (R_tables->R_tbl_size.E_labstract != 0)
	WRITE (F_rt, R_tables->E_abstract, R_tables->R_tbl_size.E_labstract * sizeof (char));

    close (F_rt);
    return true;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (ent_name);
    return false;
}
