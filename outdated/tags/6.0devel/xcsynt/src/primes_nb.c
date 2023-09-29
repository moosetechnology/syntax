/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 
static char	ME [] = "primes_nb";

#define SX_DFN_EXT_VAR_XCSYNT 

#include "sxunix.h"
char WHAT_XCSYNTPRIMESNB[] = "@(#)SYNTAX - $Id: primes_nb.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static SXINT	sroot, max_nb;
static SXBA	primes;

static char	Usage [] = "\
Computes all primes number less than n*n\n\
Usage:\t%s n\n";

#define is_digit(c)	c >= '0' && c <= '9'

static SXINT	str_to_nb (arg)
    char	*arg;
{
    SXINT	value = 0;
    char	c;

    c = *arg++;

    while (is_digit (c)) {
	value *= 10;
	value += c - '0';
	c = *arg++;
    }

    return value;
}

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  SXINT i, j, x;

  sxopentty ();

  if (argc <= 1 || argc >= 3) {
    fprintf (sxstderr, Usage, ME);
    sxexit (1);
  }

  sroot = str_to_nb (argv [1]);
  max_nb = sroot * sroot;

  primes = sxba_calloc (max_nb + 1);
  i = 1;

  while ((i = sxba_0_lrscan (primes, i)) > 0) {
    if (i > sroot)
      break;

    j = i;

    while ((x = i * j) <= max_nb) {
      sxba_1_bit (primes, x);
      j++;
    }
  }

  i = 1;

  while ((i = sxba_0_lrscan (primes, i)) > 0) {
    printf ("%ld\n", (long)i);
  }

  sxfree (primes);
  sxexit (0);

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
