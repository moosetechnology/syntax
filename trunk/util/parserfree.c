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

char WHAT_PARSERFREE[] = "@(#)SYNTAX - $Id: parserfree.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;


void	parser_free (struct parser_ag_item *parser_ag_ptr)
{
    if (parser_ag_ptr->prdct_list != NULL)
	sxfree (parser_ag_ptr->prdct_list);

    if (parser_ag_ptr->germe_to_gr_act != NULL)
	sxfree (parser_ag_ptr->germe_to_gr_act);

    if (parser_ag_ptr->prdcts != NULL)
	sxfree (parser_ag_ptr->prdcts);

    if (parser_ag_ptr->reductions != NULL)
	sxfree (parser_ag_ptr->reductions);

    if (parser_ag_ptr->vector != NULL)
	sxfree (parser_ag_ptr->vector);

    if (parser_ag_ptr->nt_bases != NULL)
	sxfree (parser_ag_ptr->nt_bases);

    if (parser_ag_ptr->t_bases != NULL)
	sxfree (parser_ag_ptr->t_bases);
}

