/*   B U I L D _ S I M P L E _ C L A S S E S   */

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
/*  This program has been translated from lecl_bsc.pl1       */
/*  on Tuesday the twenty-fifth of March, 1986, at 15:07:40, */
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
#include "sxba.h"
char WHAT_LECLBSC[] = "@(#)SYNTAX - $Id: lecl_bsc.c 1114 2008-02-27 15:13:51Z rlacroix $" WHAT_DEBUG;

extern SXINT	equality_sort (SXINT *t, SXINT bi, SXINT bs, BOOLEAN (*less_equal) (SXINT, SXINT), BOOLEAN (*equal) (SXINT, SXINT));
SXBA	/* cmax */ *char_to_class_set /* 1:char_max */ ;
static SXINT	CMAX;



static BOOLEAN	less_equal (SXINT i, SXINT j)
{
    SXBA	bsi, bsj;
    SXINT		x, vi, vj;

    bsi = char_to_class_set [i];
    bsj = char_to_class_set [j];

    for (x = 1; x <= CMAX; x++) {
	vi = sxba_bit_is_set (bsi, x);
	vj = sxba_bit_is_set (bsj, x);

	if (vi < vj)
	    return TRUE;

	if (vi > vj)
	    return FALSE;
    }

    return TRUE;
}



static BOOLEAN	equal (SXINT i, SXINT j)
{
    return sxba_first_difference (char_to_class_set [i], char_to_class_set [j]) == -1;
}


VOID lecl_bsc (SXBA *class_to_char_set, 
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
