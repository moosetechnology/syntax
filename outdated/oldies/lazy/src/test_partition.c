
#include "sxpart.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

static unsigned long int **T;

static sxpart_header partition;
static SXBA	output;
FILE	*sxstdout = {stdout}, *sxstderr = {stderr};

extern long random ();

static struct rusage prev_usage, next_usage;

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


void main (argc, argv)
    int		argc;
    char	*argv [];
{
    int n, subset_nb, i, j;
    SXBA set;
    long	t1;

    subset_nb = str_to_nb (argv [1]);

    T = (unsigned long int **) sxalloc (subset_nb, sizeof (unsigned long int *));

    for (j = 0; j < subset_nb; j++)
    {
	T [j] = (unsigned long int *) sxalloc (8, sizeof (unsigned long int));

	for (i = 0; i < 8; i++)
	    T [j][i] = (unsigned long int) random ();
    }

    sxpart_alloc (&partition, 2*subset_nb, 256);

    sxpart_clear (partition);

    sxba_empty (sxpart_to_input (partition));	

    for (j = 0; j < subset_nb; j++)
    {
	n = sxpart_seek (partition);
	set = sxpart_to_subset (partition, n);

	for (i = 0; i < 8; i++)
	    set [i+1] = T [j][i];

	SXBA_1_bit (sxpart_to_input (partition), n);
    }

    getrusage (RUSAGE_SELF, &prev_usage);
    output = sxpart_doit (&partition);
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("By elem = %dms, cardinal = %d\n\n", t1, sxba_cardinal (output));

    sxpart_free (&partition);

    sxexit (0);
}

