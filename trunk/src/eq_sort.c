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
/*   E Q U A L I T Y _ S O R T   */








#include "sxversion.h"
#include "sxunix.h"

char WHAT_EQ_SORT[] = "@(#)SYNTAX - $Id: eq_sort.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;


static SXINT *T;
static bool (*LESS_EQUAL) (SXINT, SXINT);


static bool le (SXINT i, SXINT j)
{
return (*LESS_EQUAL) (T[i],T[j]);
}

void equality_sort (SXINT *t, SXINT bi, SXINT bs, bool (*less_equal) (SXINT, SXINT), bool (*equal) (SXINT, SXINT))
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
    bool	is_equality;
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
    is_equality = false;

    for (i = bi + 1; i <= bs; i++) {
	if ((*equal) (min, xi = t [sorted [i]])) {
	    is_equality = true;
	    min = (min <= xi) ? min : xi;
	}
	else {
	    if (is_equality) {
		is_equality = false;

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
