/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2006 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe ATOLL.  (pb)                    *
   *							  *
   ******************************************************** */

/* 
   Gestionnaire de "reservoir" de SXBA 
*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 03-10-06 13:30 (pb):		Creation				*/
/************************************************************************/

#if 0
static char	ME [] = "sxba_bag_mngr";
#endif

#include "sxcommon.h"
#include "sxba_bag.h"

char WHAT_SXBA_BAG_MNGR[] = "@(#)SYNTAX - $Id: sxba_bag_mngr.c 654 2007-06-26 10:42:18Z rlacroix $" WHAT_DEBUG;

/* BAG_mngr */
void
sxba_bag_alloc (bag_header *pbag, char *name, int size, FILE *file_name)
{

  pbag->name = name;
  pbag->hd_top = 0;
  pbag->hd_high = 0;
  pbag->hd_size = 1;
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
sxba_bag_get (bag_header *pbag, int bits_number)
{
  int 	slice_nb = NBLONGS (bits_number) + 1;
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
	fprintf (pbag->file_name, "Bag %s: New bag of size %i is created.\n", pbag->name, pbag->pool_size *= 2);
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
	     (long)((pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) *
	     sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd)),
	     (long)(pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd)));
  }

  do {
    sxfree (pbag->hd [pbag->hd_high].set);
  } while (--pbag->hd_high >= 0);

  sxfree (pbag->hd);
}
    

void
sxba_bag_clear (bag_header *pbag)
{
  int top;

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
 
