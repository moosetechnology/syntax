/*   E Q U A L I T Y _ S O R T   */

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



#include "sxunix.h"

char WHAT_EQ_SORT[] = "@(#)SYNTAX - $Id: eq_sort.c 995 2008-02-11 15:23:54Z rlacroix $" WHAT_DEBUG;


static SXINT *T;
static BOOLEAN (*LESS_EQUAL) (SXINT, SXINT);


static BOOLEAN le (SXINT i, SXINT j)
{
return (*LESS_EQUAL) (T[i],T[j]);
}


SXVOID equality_sort (SXINT *t, SXINT bi, SXINT bs, BOOLEAN (*less_equal) (SXINT, SXINT), BOOLEAN (*equal) (SXINT, SXINT))
{
    /* Procedure modifie le tableau t entre bi et bs de facon telle que: 
       si (*equal) (t [i], t [j]) alors 
       t [t [i] > t [j] ? i : j] = min (t [i], t [j])

       Elle utilise sort_by_tree. 
      
       Auteur: Pierre Boullier. Complexite: temps n*logn. place 2*n-1. 
      
    */

    /* L O C A L   V A R I A B L E S   */
    SXINT	i, xmin, min;
    SXINT	*sorted;
    BOOLEAN	is_equality;
    SXINT	xi;


/* I N I T I A L I S A T I O N S   */

    if (bs - bi <= 0) {
	return;
    }

    sorted = (SXINT*) sxalloc (bs + 1, sizeof (SXINT));

    for (i = bi; i <= bs; i++) {
	sorted [i] = i;
    }

    T = t;
    LESS_EQUAL = less_equal;
    sort_by_tree (sorted, bi, bs, le);

/* test des egalites dans le tableau trie */

    min = t [sorted [xmin = bi]];
    is_equality = FALSE;

    for (i = bi + 1; i <= bs; i++) {
	if ((*equal) (min, xi = t [sorted [i]])) {
	    is_equality = TRUE;
	    min = (min <= xi) ? min : xi;
	}
	else {
	    if (is_equality) {
		is_equality = FALSE;

		for (; xmin < i; xmin++) {
		    t [sorted [xmin]] = min;
		}
	    }

	    xmin = i;
	    min = xi;
	}
    }

    if (is_equality) {
	for (; xmin <= bs; xmin++) {
	    t [sorted [xmin]] = min;
	}
    }

    sxfree (sorted);
    /* end equality_sort */
}
