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

/* Gestionnaire de "reservoir" de SXBA */

#include "sxversion.h"
#include "sxcommon.h"
#include "sxba_bag.h"

char WHAT_SXBA_BAG_MNGR[] = "@(#)SYNTAX - $Id: sxba_bag_mngr.c 2955 2023-03-30 14:20:45Z garavel $" WHAT_DEBUG;

/* BAG_mngr */
void
sxba_bag_alloc (bag_header *pbag, char *name, SXBA_INDEX size, FILE *file_name)
{

  pbag->name = name;
  pbag->hd_top = 0;
  pbag->hd_high = 0;
  pbag->hd_size = 1;

  if (size > MAX_SXBA_BAG_SIZE) {
#if EBUG
    fprintf (stderr, "### WARNING: sxba_bag_alloc refuses to allocate sxba_bags greater than %ld. Your sxba_bag of size %ld has been reduced to this size.\n", MAX_SXBA_BAG_SIZE, size);
#endif /* EBUG */
    size = MAX_SXBA_BAG_SIZE;
  }

  pbag->pool_size = size;
  pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
  /* Pour pouvoir faire sxba_empty sur bag_clear */
  pbag->pool_top = (pbag->hd [0].set = sxba_calloc ((pbag->pool_size + 3)<<SXSHIFT_AMOUNT))+1;
  pbag->hd [0].size = pbag->pool_size;
  pbag->room = pbag->pool_size;

  if ((pbag->file_name = file_name)) {
    pbag->total_size = pbag->pool_size + 3;
    pbag->used_size = pbag->prev_used_size = 0;
  }
}



SXBA
sxba_bag_get (bag_header *pbag, SXBA_INDEX bits_number)
{
  SXBA_INDEX	slice_nb = SXNBLONGS (bits_number) + 1;
  SXBA	set;

  if (slice_nb > pbag->room) {
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

      if (pbag->file_name) {
	fprintf (pbag->file_name, "Bag %s: New bag of size %li is created.\n", pbag->name, (SXINT) (pbag->pool_size *= 2));
	pbag->total_size += pbag->pool_size + 3;
      }

      pbag->hd_high = pbag->hd_top;
  /* Pour pouvoir faire sxba_empty sur bag_clear */
      pbag->hd [pbag->hd_top].set = sxba_calloc ((pbag->pool_size + 3)<<SXSHIFT_AMOUNT);
      pbag->hd [pbag->hd_top].size = pbag->pool_size;
    }

    pbag->pool_top = pbag->hd [pbag->hd_top].set+1;
  }

  *(set = pbag->pool_top) = bits_number;
  pbag->pool_top += slice_nb;
  pbag->room -= slice_nb;

  if (pbag->file_name) {
    pbag->used_size += slice_nb;
  }

  return set;
}
    

void
sxba_bag_free (bag_header *pbag)
{
  if (pbag->file_name) {
    fprintf (pbag->file_name, "Bag %s: used_size = %lu bytes, total_size = %lu bytes\n",
	     pbag->name,
	     (SXUINT)((pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) *
	     sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd)),
	     (SXUINT)(pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd)));
  }

  do {
    sxfree (pbag->hd [pbag->hd_high].set);
  } while (--pbag->hd_high >= 0);

  sxfree (pbag->hd);
}
    

void
sxba_bag_clear (bag_header *pbag)
{
  SXBA_INDEX top;

  top = pbag->hd_high;

  do {
    sxba_empty (pbag->hd [top].set);
  } while (--top >= 0);

  pbag->hd_top = 0;
  pbag->pool_top = pbag->hd [0].set+1;
  pbag->pool_size = pbag->hd [0].size;
  pbag->room = pbag->pool_size;

  if (pbag->file_name) {
    if (pbag->prev_used_size < pbag->used_size)
      pbag->prev_used_size = pbag->used_size;

    pbag->used_size = 0;
  }
}
/* END BAG_mngr */
 
