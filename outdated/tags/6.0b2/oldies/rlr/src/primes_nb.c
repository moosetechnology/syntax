/* ********************************************************
   *							  *
   *							  *
   * Copyright (©) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'équipe Langages et Traducteurs.  (pb)  *
   *							  *
   ******************************************************** */





/* Calcul de nombres premiers pour SYNTAX® */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20010327 08:53 (phd):	Accélération du crible			*/
/************************************************************************/
/* 20010327 08:33 (phd):	Création de cette rubrique.		*/
/************************************************************************/


#ifndef lint
#define WHAT	"@(#)primes_nb.c	- SYNTAX [unix] - 27 mars 2001"
static struct what {
  struct what	*whatp;
  char		what [sizeof (WHAT)];
} what = {&what, WHAT};

#endif



static char	ME [] = "primes_nb";

#include "sxunix.h"

FILE	*sxstderr = {NULL};
FILE	*sxtty;

BOOLEAN	sxverbosep;

static int	sroot, max_nb;
static SXBA	sieve;

static char	Usage [] = "\
Usage:\t%s N\n\
\tComputes all prime numbers less than N*N\n";

#define is_digit(c)	((c >= '0') && (c <= '9'))

static int	str_to_nb (arg)
    char	*arg;
{
    register int	value = 0;
    register char	c;

    while ((c = *arg++) && is_digit (c)) {
	value = 10 * value + c - '0';
    }

    return value;
}

main (argc, argv)
    int		argc;
    char	*argv [];
{
    register int i, j, x;

    if (sxstderr == NULL) {
	sxstderr = stderr;
    }

#ifdef BUG
    /* Suppress bufferisation, in order to have proper	 */
    /* messages when something goes wrong...		 */

    setbuf (stdout, NULL);

#endif

    sxopentty ();
    if (argc <= 1 || argc >= 3) {
	fprintf (sxstderr, Usage, ME);
	exit (1);
    }

    sroot = str_to_nb (argv [1]);
    max_nb = sroot * sroot;

    sieve = sxba_calloc (max_nb + 1);
    i = 1;

    while (((i = sxba_0_lrscan (sieve, i)) > 0) && (i <= sroot)) {
	j = i * i;

	do {
	  sxba_1_bit (sieve, j);
	} while ((j += i) <= max_nb);
    }

    i = 1;

    while ((i = sxba_0_lrscan (sieve, i)) > 0) {
	printf ("%d\n", i);
    }

    sxfree (sieve);
    SXEXIT (0);
}
