/* Teste le "sxword_mngr" */

#include "word.h"

#include <sys/time.h>
#include <sys/resource.h>

extern char *malloc (), *calloc (), *realloc ();
extern void free ();

static word_header	area;

extern long random ();

static struct rusage prev_usage, next_usage;

static int	old, new;

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

void main (argc, argv)
    int		argc;
    char	*argv [];
{
    int 		c, max, loop, tries;
    char		*str, *last, *hd;
    unsigned int	ste, wrd, top, x1;
    unsigned long	l, l2;
    long		t1;

#define vstr_length (1024*1024*sizeof (char))

    max = str_to_nb (argv [1]); /* taille max d'une chaine */
    loop = str_to_nb (argv [2]); /* nb de boucles */

    last = hd + read (0, hd = malloc (vstr_length), vstr_length);

    word_alloc (&area, "test_area", 1000 /* loop * ((last - hd) / (max/2)) */, 1, max / 2,
		malloc, calloc, realloc, NULL, free, NULL, stdout);

    getrusage (RUSAGE_SELF, &prev_usage);

    *last = '\0';
    tries = top = 0;

    while (loop-- > 0)
    {
	str = hd;

	do
	{
	    tries++;
	    l = random () % max;

	    if (str + l >= last)
	    {
		if (SXWORD_test_and_save (area, x1, wrd, str))
		    new++;
		else
		    old++;

		/* wrd = word_save (&area, str); */
	    }
	    else
	    {
		if (SXWORD_test_and_2save (area, x1, wrd, str, l))
		    new++;
		else
		    old++;

		/* wrd = word_2save (&area, str, l); */
	    }

	    if (wrd > top)
		top = wrd;

	    str += l;
	} while (str < last);
    }

    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nWord time = %dms for %d tries, (old = %i, new = %i)\n\n", t1, tries, old, new);

    for (ste = 1; ste <= top; ste++)
    {
	l2 = WORD_len (area, ste);
	last = WORD_get (area, ste);
    }

    word_free (&area);
    sxexit (0);
}

