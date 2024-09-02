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

#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"
char WHAT_XBNFSMP[] = "@(#)SYNTAX - $Id: xbnf_smp.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;


extern	void symbol_table_processing (SXNODE *);
extern	void property_test (void);
extern	void follow_construction (void);


void
sxxbnfsmp (SXINT what, SXTABLES *sxtables_ptr)
{
    switch (what) {
    case SXOPEN:
	sxatcvar.atc_lv.node_size = sizeof (SXNODE);
	sxtab_ptr = sxtables_ptr;
	break;

    case SXCLOSE:
	break;

    case SXACTION:

/*-----------------*/
/* initializations */
/*-----------------*/

	ws_nbpro = (struct ws_nbpro_s*) NULL;
	ws_indpro = (struct ws_indpro_s*) NULL;
	ws_ntmax = (struct ws_ntmax_s*) NULL;
	adr = tpd = numpd = tnumpd = (SXINT*) NULL;
	t_string = nt_string = (char*) NULL;
	bvide = (SXBA) NULL;
	follow = (SXBM) NULL;

	if (IS_ERROR) {
	    return;
	}


/* ws_tbl_size(W) initialize */

	W.sem_kind = 0;
	WN = (struct ws_nbpro_s*) NULL;
	EOF_ste = sxstrsave ("END OF FILE");


/*---------*/
/* actions */
/*---------*/

	if (sxverbosep)
	    fputs ("   Symbol Table Processing\n", sxtty);

	symbol_table_processing (sxatcvar.atc_lv.abstract_tree_root);

	if (sxverbosep)
	    fputs ("   Property Test\n", sxtty);

	property_test ();

	if (sxverbosep)
	    fputs ("   Follow Construction\n", sxtty);

	follow_construction ();
	break;

    default:
	fputs ("The function \"sxxbnfsmp\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
