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
#include "varstr.h"
#include "lecl_ag.h"

char WHAT_LECLSATURE[] = "@(#)SYNTAX - $Id: lecl_sature.c 3631 2023-12-20 17:16:41Z garavel $" WHAT_DEBUG;

void	lecl_sature (SXINT nbt)
{
    SXINT	old_size, new_size;

    sxinitialise(old_size);sxinitialise(new_size); /* pour faire taire gcc -Wuninitialized */
    if (nbt > 0) {
	lgt [nbt] = new_size = 2 * (old_size = lgt [nbt]);

	if (sxverbosep) {
	    if (!sxttycol1p) {
		fputc ('\n', sxtty);
		sxttycol1p = true;
	    }

	    fprintf (sxtty, "Array %ld of size %ld overflowed: reallocation with size %ld.\n", (SXINT) nbt, (SXINT) old_size, (SXINT) lgt [nbt]);
	}
    }

    switch (nbt) {
    case 0:
	substate = (struct lr_marker*) sxalloc (lgt [1] + 1, sizeof (struct lr_marker));
	state = (struct lr_marker*) sxalloc (lgt [1] + 1, sizeof (struct lr_marker));
	fsa = (struct fsa_item*) sxalloc (lgt [2] + 1, sizeof (struct fsa_item));
	item = (struct lr_marker*) sxalloc (lgt [3] + 1, sizeof (struct lr_marker));
	/* compound_classes alloue manu dans lecl_fsa... */
	fsa_trans = (struct fsa_trans_item*) sxalloc (lgt [4] + 1, sizeof (struct fsa_trans_item));
	break;

    case 1:
	substate = (struct lr_marker*) sxrealloc (substate, new_size + 1, sizeof (struct lr_marker));
	state = (struct lr_marker*) sxrealloc (state, new_size + 1, sizeof (struct lr_marker));
	break;

    case 2:
	fsa = (struct fsa_item*) sxrealloc (fsa, new_size + 1, sizeof (struct fsa_item));
	break;

    case 3:
	item = (struct lr_marker*) sxrealloc (item, new_size + 1, sizeof (struct lr_marker));
	break;

    case 4:
	{
	    SXINT	i;
	    SXBA	*old1_ptr = compound_classes;

	    fsa_trans = (struct fsa_trans_item*) sxrealloc (fsa_trans, new_size + 1, sizeof (struct fsa_trans_item));
	    compound_classes = sxbm_calloc (new_size + 1, smax + 1);

	    for (i = 1; i <= old_size; i++)
		sxba_copy (compound_classes [i], old1_ptr [i]);

	    sxbm_free (old1_ptr);
	    break;
	}
    default:
#if EBUG
      sxtrap("lecl_sature","unknown switch case #1");
#endif
      break;
    }
}
