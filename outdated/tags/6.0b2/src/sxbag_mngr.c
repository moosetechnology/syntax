/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1999 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL (PB).		  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* Lun 20 Dec 1999 16:07 (PB)	Ajout de cette rubrique "modifications"	*/
/************************************************************************/




#include "sxcommon.h"
#include "bag.h"

char WHAT_SXBAG_MNGR[] = "@(#)SYNTAX - $Id: sxbag_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

void
bag_alloc (pbag, name, size)
    bag_header	*pbag;
    char	*name;
    int 	size;
{
    pbag->name = name;
    pbag->hd_top = 0;
    pbag->hd_high = 0;
    pbag->hd_size = 1;
    pbag->pool_size = size;
    pbag->hd = (struct bag_disp_hd*) sxalloc (pbag->hd_size, sizeof (struct bag_disp_hd));
    pbag->pool_top = pbag->hd [0].set = (SXBA_ELT*) sxcalloc ((unsigned long int)pbag->pool_size + 2, sizeof (SXBA_ELT));
    pbag->hd [0].size = pbag->pool_size;
    pbag->room = pbag->pool_size;

#if EBUG
    pbag->total_size = pbag->pool_size + 2;
    pbag->used_size = pbag->prev_used_size = 0;
#endif
}




SXBA
bag_get (pbag, size)
    bag_header	*pbag;
    int		size;
{
    int 	slice_nb = NBLONGS (size) + 1;
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
	    printf ("Bag %s: New bag of size %i is created.\n", pbag->name, pbag->pool_size);
	    pbag->total_size += pbag->pool_size + 2;
#endif

	    pbag->hd_high = pbag->hd_top;
	    pbag->hd [pbag->hd_top].set = (SXBA) sxcalloc ((unsigned long int)pbag->pool_size + 2, sizeof (SXBA_ELT));
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
bag_free (pbag)
    bag_header	*pbag;
{
#if EBUG
    printf ("Bag %s: used_size = %u bytes, total_size = %u bytes\n",
	    pbag->name,
	    (pbag->used_size > pbag->prev_used_size ? pbag->used_size : pbag->prev_used_size) * sizeof (SXBA_ELT) + (pbag->hd_high + 1) * sizeof (struct bag_disp_hd),
    	    pbag->total_size * sizeof (SXBA_ELT) + pbag->hd_size * sizeof (struct bag_disp_hd));
#endif

    do {
	sxfree (pbag->hd [pbag->hd_high].set);
    } while (--pbag->hd_high >= 0);

    sxfree (pbag->hd);
}
    

void
bag_clear (pbag)
    bag_header	*pbag;
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
    
