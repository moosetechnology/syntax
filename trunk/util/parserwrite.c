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
#include "P_tables.h"

char WHAT_PARSERWRITE[] = "@(#)SYNTAX - $Id: parserwrite.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;


#define WRITE(f,p,l)						\
	if (bytes = (l), (write (f, p, bytes) != bytes))	\
		goto write_error


bool		parser_write (struct parser_ag_item *parser_ag_ptr, char *langname)
{
    /* sortie des tables du parser */
    SXINT	bytes;
    sxfiledesc_t	/* file descriptor */ F_pt;
    SXINT		pt_version;
    char	parser_tables_name [128];
    static char		ME [] = "parser_write";

    if ((F_pt = open (strcat (strcpy (parser_tables_name, langname), ".pt"), O_WRONLY+O_CREAT+O_BINARY, 0666)) < 0) {
	fprintf (sxstderr, "%s: cannot open (create) ", ME);
	goto file_error;
    }

    pt_version = pt_num_version;
    /* numero de version des tables */

    WRITE (F_pt, &(pt_version), sizeof (SXINT));
    WRITE (F_pt, &(parser_ag_ptr->bnf_modif_time), sizeof (SXINT));
    WRITE (F_pt, &(parser_ag_ptr->constants), sizeof (struct P_constants));

    if (parser_ag_ptr->constants.mMred != 0)
	WRITE (F_pt, parser_ag_ptr->reductions + 1, -parser_ag_ptr->constants.mMred * sizeof (struct P_reduction));

    if (parser_ag_ptr->constants.Mtstate != 0)
	WRITE (F_pt, parser_ag_ptr->t_bases + 1, parser_ag_ptr->constants.Mtstate * sizeof (struct P_base));

    if (parser_ag_ptr->constants.Mntstate != 0)
	WRITE (F_pt, parser_ag_ptr->nt_bases + 1, parser_ag_ptr->constants.Mntstate * sizeof (struct P_base));

    if (parser_ag_ptr->constants.Mvec != 0)
	WRITE (F_pt, parser_ag_ptr->vector + 1, parser_ag_ptr->constants.Mvec * sizeof (struct P_item));

    if (parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred != 0)    
	WRITE (F_pt, parser_ag_ptr->prdcts + 1, (parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred) * sizeof (struct P_prdct));

    if (parser_ag_ptr->germe_to_gr_act != NULL)
	WRITE (F_pt, parser_ag_ptr->germe_to_gr_act, (parser_ag_ptr->constants.Mgerme - parser_ag_ptr->constants.mgerme)  * sizeof (SXINT));

    if (parser_ag_ptr->constants.Mprdct_list != 0)
	WRITE (F_pt, parser_ag_ptr->prdct_list + 1, parser_ag_ptr->constants.Mprdct_list * sizeof (struct P_prdct_list));

    close (F_pt);
    return true;

write_error:
    fprintf (sxstderr, "%s: write error on ", ME);
file_error:
    sxperror (parser_tables_name);
    return false;
}
