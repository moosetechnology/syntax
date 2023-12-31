/* ********************************************************
   *							  *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *			and Universite d'Orleans	  *
   *							  *
   ******************************************************** */


/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */


/***************************************************************/
/*							       */
/*							       */
/*  This program has been translated from sort_by_tree.pl1     */
/*  on Friday the twenty-third of November, 1984, at 14:18:33, */
/*  on the Multics system at INRIA,			       */
/*  by the release 3.0 PL1_C translator of INRIA,	       */
/*	   developped by the "Langages et Traducteurs" team,   */
/*	   using the SYNTAX (*), FNC and Paradis systems.      */
/*							       */
/*							       */
/***************************************************************/
/* (*) SYNTAX is a trademark of INRIA.			       */
/***************************************************************/



#include "sxunix.h"

char WHAT_SORT_BY_TREE[] = "@(#)SYNTAX - $Id: sort_by_tree.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

/* Le Ven 1 Oct 1999 16:12:17, changement de 077777 en (((unsigned int)~0)>>1) */
#define max_bin_15 (((unsigned int)~0)>>1)

static int	*val;



static BOOLEAN	is_less_or_equal (left, right, less_equal)
    int		left, right;
    BOOLEAN	(*less_equal) ();
{
    register int	gauche, droit;

    if ((gauche = val [left]) == max_bin_15)
	return FALSE;

    if ((droit = val [right]) == max_bin_15)
	return TRUE;

    return (*less_equal) (gauche, droit);
}



SXVOID sort_by_tree (t, bi, bs, less_equal)
    int		*t, bi, bs;
    BOOLEAN	(*less_equal) ();

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

    register int	x, n;
    register int	*aval;
    register int	left, right;
    register int	*avalr;
    register int	*VAL;
    register int	k, j, size, valr, r;
    register int	*avalleft, *avalright;
    register int	*ati;


/*   I N I T I A L I S A T I O N S   */

    if ((size = bs - bi + 1) <= 1)
	return;


/* le nombre d'elements a trier doit etre pair */

    n = (size & 1) ? size + 1 : size;
    r = 2 * n - 1;
    val = VAL = (int*) sxalloc (1 + r, sizeof (int));
    avalr = &(VAL [r]);


/* CONSTRUCTION DE L'ARBRE */

    /* creation du premier niveau */

    for (x = bi, aval = &(VAL [1]); x <= bs; x++)
	*aval++ = t [x];


/* si size impair on ajoute un element bidon */

    if (n > size)
	VAL [n] = max_bin_15;


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
	VAL [valr] = max_bin_15;

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
