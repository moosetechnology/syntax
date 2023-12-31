static char	ME [] = "primes_nb";

#include "sxunix.h"
char WHAT_XCSYNTPRIMESNB[] = "@(#)SYNTAX - $Id: primes_nb.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

/* On lit a priori sur stdin, et cetera */

FILE	*sxstdout = {NULL}, *sxstderr = {NULL};
FILE	*sxtty;

BOOLEAN	sxverbosep;

static int	sroot, max_nb;
static SXBA	primes;

static char	Usage [] = "\
Computes all primes number less than n*n\n\
Usage:\t%s n\n";

#define is_digit(c)	c >= '0' && c <= '9'

static int	str_to_nb (arg)
    char	*arg;
{
    register int	value = 0;
    register char	c;

    c = *arg++;

    while (is_digit (c)) {
	value *= 10;
	value += c - '0';
	c = *arg++;
    }

    return value;
}

/************************************************************************/
/* main function
/************************************************************************/
int main (int argc, char *argv[])
{
  int i, j, x;

  if (sxstdout == NULL) {
    sxstdout = stdout;
  }
  if (sxstderr == NULL) {
    sxstderr = stderr;
  }

  if (argc <= 1 || argc >= 3) {
    fprintf (sxstderr, Usage, ME);
    exit (1);
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
  exit (0);

  return EXIT_SUCCESS; /* Jamais atteint !! pour les compilo susceptibles ... */
}
