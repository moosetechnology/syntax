/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1989 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ChLoE.     		  *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 07-04-94 15:31 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/



/* 			CALCUL DE LA PARTITION MINIMALE				*/

/* C'est l'ensemble des mintermes non vides a l'exclusion du minterme nul. 	*/

#include        "sxcommon.h"
#include "sxpart.h"

char WHAT_SXPART_MNGR[] = "@(#)SYNTAX - $Id: sxpart_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

#define sxpart_tseek(h)		((h).tx == (h).tl ? sxpart_tline_oflw (&(h)) : (h).tx++)
#define SET(i)			h->subsets [i]
#define TRANS(i)		h->trans [i]


/* Utilise' dans sxpart_seek */
int
sxpart_line_oflw (h)
    sxpart_header	*h;
{
    int old_line_nb = h->sl;

    h->subsets = sxbm_resize (h->subsets, old_line_nb, h->sl *= 2, h->sc);
    h->subsets_to_trans = (int*) sxrealloc (h->subsets_to_trans, h->sl, sizeof (int));
    h->input = sxba_resize (h->input, h->sl);
    h->output = sxba_resize (h->output, h->sl);

    return h->sx++;
}

static int
sxpart_tline_oflw (h)
    sxpart_header	*h;
{
    int old_line_nb = h->tl;

    h->subsets = sxbm_resize (h->subsets, old_line_nb, h->tl *= 2, h->tc);
    h->trans_to_subsets = (int*) sxrealloc (h->trans_to_subsets, h->tl, sizeof (int));

    return h->tx++;
}

/* Appelee directement par l'utilisateur sur debordement du re'fe'rentiel */
void
sxpart_ref_oflw (h, new_size)
    sxpart_header	*h;
    int			new_size;
{
    h->subsets = sxbm_resize (h->subsets, h->tl, h->tl, h->tc = new_size);

    /* On n'augmente pas "trans", elle le sera (eventuellement) de facon lazy lors
       de son utilisation. */
}

/* Initialise une zone de travail */
void
sxpart_alloc (h, line_nb, column_nb)
    sxpart_header	*h;
    int			line_nb, column_nb;
{
    h->subsets = sxbm_calloc (h->sl = line_nb, h->sc = column_nb);
    h->subsets_to_trans = (int*) sxalloc (line_nb, sizeof (int));
    h->input = sxba_calloc (line_nb);
    h->output = sxba_calloc (line_nb);

    h->trans = sxbm_calloc (h->tl = column_nb, h->tc = line_nb);
    h->trans_to_subsets = (int*) sxalloc (column_nb, sizeof (int));
    h->sx = h->tx = 0;
}

/* Libere une zone de travail */
void
sxpart_free (header)
    sxpart_header	*header;
{
    sxbm_free (header->subsets);
    sxfree (header->subsets_to_trans);
    sxfree (header->input);
    sxfree (header->output);

    sxbm_free (header->trans);
    sxfree (header->trans_to_subsets);
}



/* Calcule la partition: les ensembles de de'part doivent etre initialise's
   (Chaque appel a sxpart_seek(h) retourne un nouvel identifiant i qui est transforme'
    en SXBA par sxpart_to_subset(h,i)). Attention, a cause des reallocations eventuelles
   un SXBA peut-e^tre invalide' par un appel de sxpart_seek, en revanche, l'identifiant
   retourne' par sxpart_seek est toujours valide (jusqu'au prochain sxpart_clear).
   Les identifiants de ces ensembles de de'part doivent figurer dans le SXBA input
   obtenu par sxpart_to_input.
   Retourne un SXBA qui contient les identifiants des e'le'ments de la partition re'sultat.
   De plus, pour chacun de ces e'le'ments, il est possible de connai^tre (via sxpart_to_orig)
   le minterme associe' (sous-ensemble de input). */
SXBA
sxpart_doit (h)
    sxpart_header	*h;
{
    int		or_nb, in, last, elem, x, subset, minterme;

    sxba_empty (h->output);

    if ((last = sxba_1_rlscan (h->input, h->sl)) >= 0)
    {
	if (last > h->tc)
	    h->trans = sxbm_resize (h->trans, h->tl, h->tl, h->tc = last);

	h->tx = 0;

	minterme = sxpart_tseek (*h);
	sxba_empty (TRANS (minterme));

	if ((in = sxba_1_rlscan (h->input, last)) == -1)
	{
	    /* Un seul ensemble */
	    SXBA_1_bit (h->output, last);
	    SXBA_1_bit (TRANS (minterme), last);
	    h->subsets_to_trans [last] = minterme;
	    return h->output;
	}

	or_nb = sxpart_seek (*h);
	sxba_copy (SET (or_nb), SET (last));
	sxba_or (SET (or_nb), SET (in));

	while ((in = sxba_1_rlscan (h->input, in)) >= 0)
	    sxba_or (SET (or_nb), SET (in));
    
	elem = -1;

	while ((elem = sxba_scan (SET (or_nb), elem)) >= 0)
	{
	    in = -1;

	    while ((in = sxba_scan (h->input, in)) >= 0)
	    {
		if (SXBA_bit_is_set (SET (in), elem))
		    SXBA_1_bit (TRANS (minterme), in);
	    }

	    for (x = 0; x < minterme; x++)
	    {
		if (sxba_first_difference (TRANS (x), TRANS (minterme)) == -1)
		    break;
	    }

	    if (x < minterme)
	    {
		/* existe deja en x */
		subset = h->trans_to_subsets [x];
		SXBA_1_bit (SET (subset), elem);
	    }
	    else
	    {
		/* Nouveau */
		subset = sxpart_seek (*h);
		SXBA_1_bit (h->output, subset);
		sxba_empty (SET (subset));
		SXBA_1_bit (SET (subset), elem);
		h->subsets_to_trans [subset] = minterme;
		h->trans_to_subsets [minterme] = subset;
		minterme = sxpart_tseek (*h);
	    }

	    sxba_empty (TRANS (minterme));
	}
    }

    return h->output;
}

