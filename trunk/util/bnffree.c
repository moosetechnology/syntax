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
#include "B_tables.h"

char WHAT_BNFFREE[] = "@(#)SYNTAX - $Id: bnffree.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

void	bnf_free (B)
    struct bnf_ag_item		*B;
{
    if (B->NT_STRING != NULL)
	sxfree (B->NT_STRING);

    if (B->T_STRING != NULL)
	sxfree (B->T_STRING);

    if (B->FOLLOW != NULL)
	sxbm_free (B->FOLLOW);

    if (B->FIRST != NULL)
	sxbm_free (B->FIRST);

    if (B->NULLABLE != NULL)
	sxfree (B->NULLABLE);

    if (B->BVIDE != NULL)
	sxfree (B->BVIDE);

    if (B->IS_A_GENERIC_TERMINAL != NULL)
	sxfree (B->IS_A_GENERIC_TERMINAL);

    if (B->RULE_TO_REDUCE != NULL)
	sxfree (B->RULE_TO_REDUCE);

    if (B->TNUMPD != NULL)
	sxfree (B->TNUMPD);

    if (B->NUMPD != NULL)
	sxfree (B->NUMPD);

    if (B->TPD != NULL)
	sxfree (B->TPD - B->WS_TBL_SIZE.xtmax - 1);

    if (B->ADR != NULL)
	sxfree (B->ADR - B->WS_TBL_SIZE.xtmax - 1);

    if (B->WS_NTMAX != NULL)
	sxfree (B->WS_NTMAX);

    if (B->XNT_TO_NT_PRDCT != NULL)
	sxfree (B->XNT_TO_NT_PRDCT);

    if (B->XT_TO_T_PRDCT != NULL)
	sxfree (B->XT_TO_T_PRDCT);

    if (B->WS_INDPRO != NULL)
	sxfree (B->WS_INDPRO);

    if (B->WS_NBPRO != NULL)
	sxfree (B->WS_NBPRO);
}
