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

static char	ME [] = "tables_c";

#include "sxversion.h"
#include "sxunix.h"
#include "tables.h"
#include "sem_by.h"

char WHAT_TABLESINITTBL[] = "@(#)SYNTAX - $Id: init_tbl.c 3147 2023-05-02 13:42:48Z garavel $" WHAT_DEBUG;

#define nbt 4

SXBOOLEAN		init_tables (char *name)
{
    static char		discrepancy_message [] =
	 "%s: Be careful: %s.%s and %s.%s do not come from the same grammar of %s.\n";
    static char		bad_followers_message [] =
	 "%s: Implementation restriction: Followers_Number value was too large (%ld) and has been replaced by %ld.\n";
    SXINT	j, i;
    static char		*t_name [nbt] = {"bt", "st", "rt",};
    SXINT	bnf_time [nbt];

    if (sxverbosep)
	fputs ("\tCompatibility Tests\n", sxtty);

/* verification de la date des fichiers */

    bnf_time [0] = P.bnf_modif_time;
    bnf_time [1] = S.bnf_modif_time;
    bnf_time [2] = R.bnf_modif_time;

    switch (PC.sem_kind) {
    case sem_by_abstract_tree:
	bnf_time [3] = T.bnf_modif_time;
	t_name [3] = "att";
	break;

    case sem_by_paradis:
	bnf_time [3] = PP.bnf_modif_time;
	t_name [3] = "ppt";
	break;

    default:
	bnf_time [3] = 0 /* temps vide */ ;
	t_name [3] = NULL /* Inutile! */ ;
	break;
    }

    for (i = 0; i < nbt - 1; i++) {
	if (bnf_time [i] != 0) {
	    for (j = i + 1; j < nbt; j++) {
		if (bnf_time [i] != bnf_time [j] && bnf_time [j] != 0)
		    fprintf (sxstderr, discrepancy_message, ME, name, t_name [i], name, t_name [j], name);
	    }
	}
    }

    if (RC.P_followers_number > 5) {
	fprintf (sxstderr, bad_followers_message, ME, RC.P_followers_number, (SXINT) 5);
	RC.P_followers_number = 5;
    }

    return SXTRUE;
}
