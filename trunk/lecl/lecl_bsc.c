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

/*   B U I L D _ S I M P L E _ C L A S S E S   */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"

char WHAT_LECLBSC[] = "@(#)SYNTAX - $Id: lecl_bsc.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

extern SXINT	equality_sort (SXINT *t, SXINT bi, SXINT bs, bool (*less_equal) (SXINT, SXINT), bool (*equal) (SXINT, SXINT));

SXBA	/* cmax */ *char_to_class_set /* 1:char_max */ ;
static SXINT	CMAX;

static bool	less_equal (SXINT i, SXINT j)
{
    SXBA	bsi, bsj;
    SXINT		x, vi, vj;

    bsi = char_to_class_set [i];
    bsj = char_to_class_set [j];

    for (x = 1; x <= CMAX; x++) {
	vi = sxba_bit_is_set (bsi, x);
	vj = sxba_bit_is_set (bsj, x);

	if (vi < vj)
	    return true;

	if (vi > vj)
	    return false;
    }

    return true;
}



static bool	equal (SXINT i, SXINT j)
{
    return sxba_first_difference (char_to_class_set [i], char_to_class_set [j]) == -1;
}


void lecl_bsc (SXBA *class_to_char_set, 
	       SXBA is_class_used_in_re, 
	       SXINT char_max, 
	       SXINT cmax, 
	       SXINT *smax, 
	       SXINT *simple_class /* out */ )
{
    SXBA	/* char_max */ is_legal, char_set;
    SXINT		*to_be_sorted /* 1:char_max */ , *sc_val /* 1:char_max */ ;
    SXINT		i, j, x, max, codofcar;

    CMAX = cmax;
    char_to_class_set = sxbm_calloc (char_max + 1, cmax + 1);
    is_legal = sxba_calloc (char_max + 1);
    to_be_sorted = (SXINT*) sxalloc ((SXINT) (char_max + 1), sizeof (SXINT));
    sc_val = (SXINT*) sxalloc ((SXINT) (char_max + 1), sizeof (SXINT));

    i = 0;
    while ((i = sxba_scan (is_class_used_in_re, i)) > 0) {
	char_set = class_to_char_set [i];

	j = 0;
	while ((j = sxba_scan (char_set, j)) > 0) {
	    SXBA_1_bit (char_to_class_set [j], i);
	    SXBA_1_bit (is_legal, j);
	}
    }

    for (i = 1; i <= char_max; i++) {
	to_be_sorted [i] = i;
    }

    *smax = 2;
    /* illegal chars => 1
        eof           => 2 */
    equality_sort (to_be_sorted, 1, char_max, less_equal, equal);
    max = 0;

    for (i = 1; i <= char_max; i++) {
	codofcar = to_be_sorted [i];

	if (sxba_bit_is_set (is_legal, codofcar))
	    if (codofcar > max) {
		x = sc_val [codofcar] = ++*smax;
		max = codofcar;
	    }
	    else
		x = sc_val [codofcar];
	else
	    x = 1;


/* illegal char */

	simple_class [i - 1] = x;
    }

    sxfree (sc_val);
    sxfree (to_be_sorted);
    sxfree (is_legal);
    sxbm_free (char_to_class_set);
}
