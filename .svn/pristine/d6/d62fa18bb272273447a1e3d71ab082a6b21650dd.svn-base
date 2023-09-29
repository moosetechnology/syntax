#include "sxunix.h"

static int primes [] = {1, 2, 5, 11, 17, 37, 67, 131, 257, 521, 1031, 2053, 4099, 8209, 16411, 32771, 65539, 131101, 262217, 524309, 1048583, 2097169};
#define primes_size	(sizeof (primes) / sizeof (int))


static int	LASTBIT [] = {0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4};



int	sxlast_bit (nb)
    int nb;
{
    /* retourne :
       si nb est nul 0
       sinon k tel que :
        k-1          k
       2    <= nb < 2
    */
    register int	bit = 0;

    while ((nb & ~0xFFFF) != 0) {
	nb >>= 16;
	bit += 16;
    }

    if ((nb & 0xFF00) != 0) {
	nb >>= 8;
	bit += 8;
    }

    if ((nb & 0xF0) != 0) {
	nb >>= 4;
	bit += 4;
    }

    return bit + LASTBIT [nb & 0xF];
}


int	sxnext_prime (germe)
    int germe;
{
    /* Soit germe un int qcq strictement positif.
       Cette fonction retourne (si possible), un nombre premier
       prime tel que :
        k-1             k             k+1
       2    < germe <= 2  <= prime < 2
       sinon, elle retourne 2*germe+1. */

    register int	bit;

    if (germe >= primes [primes_size - 1])
	return 2*germe + 1;
    
    bit = sxlast_bit (germe);
    /* bit > 0 car germe est par hypothese > 0 */

    if (germe == (1 << (bit - 1)))
	bit--;

    return bit >= primes_size ? germe : primes [bit];
}
