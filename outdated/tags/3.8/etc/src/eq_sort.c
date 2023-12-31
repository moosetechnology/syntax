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

static char	what [] = "@(#)eq_sort.c\t- SYNTAX [unix] - 7 Decembre 1988";

#include "sxunix.h"

extern VOID	sort_by_tree ();

static int *T;
static BOOLEAN (*LESS_EQUAL) ();


static BOOLEAN le (i, j)
int i,j;
{
return (*LESS_EQUAL) (T[i],T[j]);
}


SXVOID equality_sort (t, bi, bs, less_equal, equal)
    /*   P A R A M E T E R S   */
    int		*t, bi, bs;
    BOOLEAN	(*less_equal) ();
    BOOLEAN	(*equal) ();
{
    /* Procedure modifie le tableau t entre bi et bs de facon telle que: 
       si (*equal) (t [i], t [j]) alors 
       t [t [i] > t [j] ? i : j] = min (t [i], t [j])

       Elle utilise sort_by_tree. 
      
       Auteur: Pierre Boullier. Complexite: temps n*logn. place 2*n-1. 
      
    */

    /* L O C A L   V A R I A B L E S   */
    register int	i, xmin, min;
    register int	*sorted;
    register BOOLEAN	is_equality;
    register int	xi;


/* I N I T I A L I S A T I O N S   */

    if (bs - bi <= 0) {
	return;
    }

    sorted = (int*) sxalloc (bs + 1, sizeof (int));

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
