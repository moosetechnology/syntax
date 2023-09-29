/* Compare l'efficacite en temps de differentes versions de sxindex" */

#include "sxindex.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>

#include "sxcell.h"


extern long random ();

static int	*seekstack, *erasestack;

static sxindex_header index_hd;
static sxcell_header	cell_hd;

static struct rusage prev_usage, next_usage;

FILE	*sxstderr = {stderr}, *sxtty = {stdout};

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


static void bug ()

{
    printf ("bug ds XH...\n");
    sxexit (2);
}


void main (argc, argv)
    int		argc;
    char	*argv [];
{
    int 	columnnb, actionnb, erasenb, seeknb, xerase, xseek, x;
    long	t1;

    columnnb = str_to_nb (argv [1]);
    actionnb = str_to_nb (argv [2]);
    erasenb = actionnb/3;
    seeknb = erasenb * 2;

    seekstack = (int*) sxalloc (seeknb + 1, sizeof (int));
    erasestack = (int*) sxalloc (erasenb + 1, sizeof (int));
    xerase = 1;

    for (xseek = 1; xseek <= seeknb; xseek++) {
	seekstack [xseek++] = 1; /* not erased */
	seekstack [xseek] = 1; /* not erased */

	while (seekstack [x = (random () % xseek) + 1] == 0);

	seekstack [x] = 0;
	erasestack [xerase++] = -x; /* to be erased */

    }


    getrusage (RUSAGE_SELF, &prev_usage);
    sxindex_alloc (&index_hd, 0, columnnb, NULL);

    xerase = 1;

    for (xseek = 1; xseek <= seeknb; xseek++) {
	seekstack [xseek++] = sxindex_seek (&index_hd);
	seekstack [xseek] = sxindex_seek (&index_hd);
	
	x = -erasestack [xerase++];

	sxindex_release (index_hd, seekstack [x]);
    }

    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf (" time (index) = %d\n\n", t1);


    getrusage (RUSAGE_SELF, &prev_usage);
    sxcell_alloc (cell_hd, 1, columnnb, NULL /* oflw */);

    xerase = 1;

    for (xseek = 1; xseek <= seeknb; xseek++) {
	sxcell_seek (cell_hd, x);
	seekstack [xseek++] = x;
	sxcell_seek (cell_hd,x);
	seekstack [xseek] = x;
	x = -erasestack [xerase++];

	x = seekstack [x];
	sxcell_release (cell_hd, x);
    }

    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf (" time (cell) = %d\n\n", t1);

    sxcell_free (cell_hd);
    sxindex_free (index_hd);
    sxfree (seekstack);
    sxfree (erasestack);
    sxexit (0);
}
