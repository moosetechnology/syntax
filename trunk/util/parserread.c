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

char WHAT_PARSERREAD[] = "@(#)SYNTAX - $Id: parserread.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#define READ(f,p,l)						\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


bool		parser_read (struct parser_ag_item *parser_ag_ptr, char *langname)
{
    SXINT	bytes, n;
    sxfiledesc_t	/* file descriptor */ F_pt;
    SXINT		pt_version;
    char	parser_tables_name [128];
    static char		ME [] = "parser_read";


/* allocation et lecture des tables du parser */

    if ((F_pt = open (strcat (strcpy (parser_tables_name, langname), ".pt"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_pt, &(pt_version), sizeof (SXINT));

    if (pt_version != pt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new csynt\n");
	return (false);
    }

    READ (F_pt, &(parser_ag_ptr->bnf_modif_time), sizeof (SXINT));
    READ (F_pt, &(parser_ag_ptr->constants), sizeof (struct P_constants));

    if ((n = -parser_ag_ptr->constants.mMred) != 0) {
	parser_ag_ptr->reductions = (struct P_reduction*) sxalloc (n + 1, sizeof (struct P_reduction));
	READ (F_pt, parser_ag_ptr->reductions + 1, n * sizeof (struct P_reduction));
    }
    else
	parser_ag_ptr->reductions = NULL;

    if ((n = parser_ag_ptr->constants.Mtstate) != 0) {
	parser_ag_ptr->t_bases = (struct P_base*) sxalloc (n + 1, sizeof (struct P_base));
	READ (F_pt, parser_ag_ptr->t_bases + 1, n * sizeof (struct P_base));
    }
    else
	parser_ag_ptr->t_bases = NULL;

    if ((n = parser_ag_ptr->constants.Mntstate) != 0) {
	parser_ag_ptr->nt_bases = (struct P_base*) sxalloc (n + 1, sizeof (struct P_base));
	READ (F_pt, parser_ag_ptr->nt_bases + 1, n * sizeof (struct P_base));
    }
    else
	parser_ag_ptr->nt_bases = NULL;

    if ((n = parser_ag_ptr->constants.Mvec) != 0) {
	parser_ag_ptr->vector = (struct P_item*) sxalloc (n + 1, sizeof (struct P_item));
	READ (F_pt, parser_ag_ptr->vector + 1, n * sizeof (struct P_item));
    }
    else
	parser_ag_ptr->vector = NULL;

    if ((n = parser_ag_ptr->constants.Mprdct + parser_ag_ptr->constants.mMred) != 0)     {
	parser_ag_ptr->prdcts = (struct P_prdct*) sxalloc (n + 1, sizeof (struct P_prdct));
	READ (F_pt, parser_ag_ptr->prdcts + 1, n * sizeof (struct P_prdct));
    }
    else
	parser_ag_ptr->prdcts = NULL;

    if ((n = parser_ag_ptr->constants.Mgerme - parser_ag_ptr->constants.mgerme) != 0)     {
	parser_ag_ptr->germe_to_gr_act = (SXINT*) sxalloc (n, sizeof (SXINT));
	READ (F_pt, parser_ag_ptr->germe_to_gr_act, n * sizeof (SXINT));
    }
    else
	parser_ag_ptr->germe_to_gr_act = NULL;

    if ((n = parser_ag_ptr->constants.Mprdct_list) != 0) {
	parser_ag_ptr->prdct_list = (struct P_prdct_list*)
	    sxalloc (n + 1, sizeof (struct P_prdct_list));
	READ (F_pt, parser_ag_ptr->prdct_list + 1, n * sizeof (struct P_prdct_list));
    }
    else
	parser_ag_ptr->prdct_list = NULL;

    close (F_pt);
    return true;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (parser_tables_name);
    return false;
}
