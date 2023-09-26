/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from lecl_sature.pl1    */
/*  on Wednesday the twelfth of March, 1986, at 10:16:39,    */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
char WHAT_LECLSATURE[] = "@(#)SYNTAX - $Id: lecl_sature.c 1114 2008-02-27 15:13:51Z rlacroix $" WHAT_DEBUG;



VOID	lecl_sature (SXINT nbt)
{
    long	old_size, new_size;

    sxinitialise(old_size);sxinitialise(new_size); /* pour faire taire gcc -Wuninitialized */
    if (nbt > 0) {
	lgt [nbt] = new_size = 2 * (old_size = lgt [nbt]);

	if (sxverbosep) {
	    if (!sxttycol1p) {
		fputc ('\n', sxtty);
		sxttycol1p = TRUE;
	    }

	    fprintf (sxtty, "Array %ld of size %ld overflowed: reallocation with size %ld.\n", (long)nbt, (long)old_size, (long)lgt [nbt]);
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
