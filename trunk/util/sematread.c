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
#include "T_tables.h"

char WHAT_SEMATREAD[] = "@(#)SYNTAX - $Id: sematread.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


bool		semat_read (struct T_ag_item *T_ag, char *langname)
{
    SXINT	bytes;
    sxfiledesc_t	/* file descriptor */ F_att;
    SXINT		att_version;
    char	ent_name [128];
    static char		ME [] = "semat_read";


/* allocation et lecture des tables de semat */

    if ((F_att = open (strcat (strcpy (ent_name, langname), ".att"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_att, &(att_version), sizeof (SXINT));

    if (att_version != att_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new semat\n");
	return false;
    }

    READ (F_att, &T_ag->bnf_modif_time, sizeof (SXINT));
    READ (F_att, &T_ag->T_constants, sizeof (struct T_constants));
    READ (F_att, T_ag->SXT_node_info = (struct SXT_node_info*) sxalloc (T_ag->T_constants.T_nbpro + 2, sizeof (struct
	 SXT_node_info)), (T_ag->T_constants.T_nbpro + 2) * sizeof (struct SXT_node_info));
    READ (F_att, T_ag->T_stack_schema = (SXINT*) sxalloc (T_ag->T_constants.T_stack_schema_size + 1, sizeof (SXINT)), (T_ag->
	 T_constants.T_stack_schema_size + 1) * sizeof (SXINT));
    READ (F_att, T_ag->T_nns_indx = (SXINT*) sxalloc (T_ag->T_constants.T_nbnod + 2, sizeof (SXINT)), (T_ag->T_constants.
	 T_nbnod + 2) * sizeof (SXINT));
    READ (F_att, T_ag->T_ter_to_node_name = (SXINT*) sxalloc (T_ag->T_constants.T_ter_to_node_name_size + 1, sizeof (SXINT)),
	 (T_ag->T_constants.T_ter_to_node_name_size + 1) * sizeof (SXINT));
    READ (F_att, T_ag->T_node_name_string = sxalloc (T_ag->T_constants.T_node_name_string_lgth, sizeof (char)), (T_ag->
	 T_constants.T_node_name_string_lgth) * sizeof (char));
    close (F_att);
    return true;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (ent_name);
    return false;
}
