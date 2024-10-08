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
char WHAT_XBNF2LEXPRDCTSMP[] = "@(#)SYNTAX - $Id: xbnf2lex_prdct_smp.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;


extern	void xbnf2lex_prdct (SXNODE *adam);

SXTABLES *sxtab_ptr;


void
xbnf2lexprdct_smp (SXINT what, SXTABLES *sxtables_ptr)
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
	if (sxerrmngr.nbmess[2]>0) {
	    return;
	}


/*---------*/
/* actions */
/*---------*/

	if (sxverbosep)
	    fputs ("   xbnf2lex_prdct\n", sxtty);

	xbnf2lex_prdct (sxatcvar.atc_lv.abstract_tree_root);
	break;

    default:
	fputs ("The function \"xbnf2lex_prdct_smp\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
