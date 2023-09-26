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
#include "sxcommon.h"
#include "bag.h"

char WHAT_SXBAG_MNGR[] = "@(#)SYNTAX - $Id: sxbag_mngr.c 2955 2023-03-30 14:20:45Z garavel $" WHAT_DEBUG;

void
bag_alloc (bag_header *pbag, char *name, SXINT size)
{
    pbag->name = name;
    pbag->hd_top = 0;
    pbag->hd_high = 0;
    pbag->hd_size = 1;
    pbag->pool_size = size;
    pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
    pbag->pool_top = pbag->hd [0].set = (SXBA_ELT*) sxcalloc ((SXUINT)pbag->pool_size + 2, sizeof (SXBA_ELT));
    pbag->hd [0].size = pbag->pool_size;
    pbag->room = pbag->pool_size;

#if EBUG
    pbag->total_size = pbag->pool_size + 2;
    pbag->used_size = pbag->prev_used_size = 0;
#endif
}




SXBA
bag_get (bag_header *pbag, ssize_t size)
{
    SXBA_INDEX 	slice_nb = SXNBLONGS (size) + 1;
    SXBA	set;

    if (slice_nb > pbag->room)
    {
	if (++pbag->hd_top >= pbag->hd_size)
	    pbag->hd = (struct bag_disp_hd*) sxrealloc (pbag->hd,
							pbag->hd_size *= 2,
							sizeof (struct bag_disp_hd));

	if (pbag->hd_top <= pbag->hd_high) {
	    pbag->pool_size = pbag->hd [pbag->hd_top].size;

	    while (slice_nb > (pbag->room = pbag->pool_size)) {
		if (++pbag->hd_top >  pbag->hd_high)
		    break;

		pbag->pool_size = pbag->hd [pbag->hd_top].size;
	    }
	}

	if (pbag->hd_top > pbag->hd_high) {	    
	    while (slice_nb > (pbag->room = pbag->pool_size))
		pbag->pool_size *= 2;

#if EBUG
	    printf ("Bag %s: New bag of size %li is created.\n", pbag->name, (SXINT) pbag->pool_size);
	    pbag->total_size += pbag->pool_size + 2;
#endif

	    pbag->hd_high = pbag->hd_top;
	    pbag->hd [pbag->hd_top].set = (SXBA) sxcalloc ((SXUINT)pbag->pool_size + 2, sizeof (SXBA_ELT));
	    pbag->hd [pbag->hd_top].size = pbag->pool_size;
	}

	pbag->pool_top = pbag->hd [pbag->hd_top].set;
    }

    *(set = pbag->pool_top) = size;
    pbag->pool_top += slice_nb;
    pbag->room -= slice_nb;

#if EBUG
    pbag->used_size += slice_nb;
#endif

    return set;
}

void
bag_free (bag_header *pbag)
{
#if EBUG
    printf ("Bag %s: used_size = %lu bytes, total_size = %lu bytes\n",
	    pbag->name,
	    (SXUINT)((pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) * sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd)),
    	    (SXUINT)(pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd)));
#endif

    do {
	sxfree (pbag->hd [pbag->hd_high].set);
    } while (--pbag->hd_high >= 0);

    sxfree (pbag->hd);
}
    

void
bag_clear (bag_header *pbag)
{
    /* On suppose que les SXBA sont empty. */
    pbag->hd_top = 0;
    pbag->pool_top = pbag->hd [0].set;
    pbag->pool_size = pbag->hd [0].size;
    pbag->room = pbag->pool_size;

#if EBUG
    if (pbag->prev_used_size < pbag->used_size)
	pbag->prev_used_size = pbag->used_size;

    pbag->used_size = 0;
#endif
}
    
