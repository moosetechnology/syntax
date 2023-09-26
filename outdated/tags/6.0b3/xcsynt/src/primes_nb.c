static char	ME [] = "primes_nb";

#define SX_DFN_EXT_VAR_XCSYNT 

#include "sxunix.h"
char WHAT_XCSYNTPRIMESNB[] = "@(#)SYNTAX - $Id: primes_nb.c 1093 2008-02-19 15:00:34Z rlacroix $" WHAT_DEBUG;

#ifndef SX_GLOBAL_VAR
/* declarations indispensables pour Syntax V3.8 et interdites pour Syntax V6 */ 
FILE *sxstdout, *sxstderr, *sxtty;
BOOLEAN sxverbosep;
#endif

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

  sxstdout = stdout;
  sxstderr = stderr;

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
    printf ("%d\n", i);
  }

  sxfree (primes);
  sxexit (0);

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
