/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
#include "sxunix.h"

#include "PP_tables.h"
char WHAT_PARADISREAD[] = "@(#)SYNTAX - $Id: paradisread.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


#define READ(f,p,l)		\
	if (bytes = (l), (read (f, p, bytes) != bytes))		\
		goto read_error


SXBOOLEAN		paradis_read (struct PP_ag_item *PP_ag, char *langname)
{
    SXINT	bytes;
    sxfiledesc_t	/* file descriptor */ F_ppt;
    SXINT		ppt_version;
    char	ent_name [128];
    static char		ME [] = "paradis_read";


/* allocation et lecture des tables de paradis */

    if ((F_ppt = open (strcat (strcpy (ent_name, langname), ".ppt"), 0+O_BINARY)) < 0) {
	fprintf (sxstderr, "%s: cannot open (read) ", ME);
	goto file_error;
    }

    READ (F_ppt, &(ppt_version), sizeof (SXINT));

    if (ppt_version != ppt_num_version) {
	fprintf (sxstderr, "tables format has changed : please use the new paradis\n");
	return (SXFALSE);
    }

    READ (F_ppt, &PP_ag->bnf_modif_time, sizeof (long));
    READ (F_ppt, &PP_ag->PP_constants, sizeof (struct PP_constants));
    READ (F_ppt, PP_ag->PP_indx = (SXINT*) sxalloc (PP_ag->PP_constants.PP_indx_lgth + 1, sizeof (SXINT)), (PP_ag->
	 PP_constants.PP_indx_lgth + 1) * sizeof (SXINT));
    READ (F_ppt, PP_ag->SXPP_schema = (struct SXPP_schema*) sxalloc (PP_ag->PP_constants.PP_schema_lgth + 1, sizeof (struct
	 SXPP_schema)), (PP_ag->PP_constants.PP_schema_lgth + 1) * sizeof (struct SXPP_schema));
    close (F_ppt);
    return SXTRUE;

read_error:
    fprintf (sxstderr, "%s: read error on ", ME);
file_error:
    sxperror (ent_name);
    return SXFALSE;
}