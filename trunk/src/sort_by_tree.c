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





#include "sxversion.h"
#include "sxunix.h"

char WHAT_SORT_BY_TREE[] = "@(#)SYNTAX - $Id: sort_by_tree.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

static SXINT	*val;



static bool	is_less_or_equal (SXINT	left, SXINT right, bool (*less_equal) (SXINT, SXINT))
{
    SXINT	gauche, droit;

    if ((gauche = val [left]) == SXINT_MAX)
	return false;

    if ((droit = val [right]) == SXINT_MAX)
	return true;

    return (*less_equal) (gauche, droit);
}



void sort_by_tree (
		     SXINT *t,
		     SXINT bi,
		     SXINT bs,
		     bool (*less_equal) (SXINT, SXINT))

/*
   Procedure triant par ordre croissant au sens large (la nature du tri
   depend de la fonction "less_equal" passee en parametre) les elements
   du tableau "t" entre les bornes "bi" et "bs".
   Le resultat du tri est dans "t" entre "bi" et "bs".
  
   Methode utilisee: tri par arborescence binaire.
   Date: 11 Mai 1982.
   Auteur: Pierre Boullier.
   Complexite: temps n*logn.
	       place 2*n-1.
  
*/


{

/*   L O C A L	 V A R I A B L E S   */

    SXINT	x, n;
    SXINT	*aval;
    SXINT	left, right;
    SXINT	*avalr;
    SXINT	*VAL;
    SXINT	k, j, size, valr, r;
    SXINT	*avalleft, *avalright;
    SXINT	*ati;


/*   I N I T I A L I S A T I O N S   */

    if ((size = bs - bi + 1) <= 1)
	return;


/* le nombre d'elements a trier doit etre pair */

    n = (size & 1) ? size + 1 : size;
    r = 2 * n - 1;
    val = VAL = (SXINT*) sxalloc (1 + r, sizeof (SXINT));
    avalr = &(VAL [r]);


/* CONSTRUCTION DE L'ARBRE */

    /* creation du premier niveau */

    for (x = bi, aval = &(VAL [1]); x <= bs; x++)
	*aval++ = t [x];


/* si size impair on ajoute un element bidon */

    if (n > size)
	VAL [n] = SXINT_MAX;


/* creation du second niveau */

    aval = &(VAL [n + 1]);

    for (right = 2, left = 1; right <= n; right += 2, left += 2) {
	*aval++ = (is_less_or_equal (left, right, less_equal)) ? left : right;
    }

    x = (n >> 1) - 1;
    avalleft = &(VAL [n + 1]);
    avalright = avalleft + 1;


/* creation des autres niveaux */

    while (x-- > 0) {
	*aval++ = (is_less_or_equal (left = *avalleft, right = *avalright, less_equal)) ? left : right;
	avalleft += 2;
	avalright += 2;
    }


/* TRI PROPREMENT DIT */

    ati = &(t [bi]);

    for (j = n - 1; j > 0; j--) {
	/* sortie du minimum et remplacement par un nombre superieur a toutes
	   les valeurs a trier */
	*ati++ = VAL [valr = *avalr];
	VAL [valr] = SXINT_MAX;

/* transformation de la branche de bas en haut : initialisation */

	x = (valr + 1) >> 1;

/* on doit distinguer le deuxieme niveau */

	right = 2 * x;
	left = right - 1;
	VAL [n + x] = (is_less_or_equal (left, right, less_equal)) ? left : right;


/* autres niveaux */

	while ((x = (n + x + 1) >> 1) < n) {
	    right = VAL [k = 2 * x];
	    left = VAL [k - 1];
	    VAL [n + x] = (is_less_or_equal (left, right, less_equal)) ? left : right;
	}
    }

    if (n == size)
	t [bs] = VAL [*avalr];

    sxfree (val);
}
