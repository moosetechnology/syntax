/* Compare l'efficacite en temps et place des XH, XT, sxlist, sxsubset, sxSL et sxset. */

#include "sxunix.h"
#include "XxY.h"
#include "X.h"
#include "XH.h"
#include "XT.h"
#include "SS.h"

typedef int sxSL_elems;
#include "sxlist.h"
#include "sxSL.h"
#include "sxsubset.h"
#include "sxset.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/resource.h>


extern long random ();

static struct list_head {
    int lgth, *elems;
} *list_head;



static XH_header XH_work;
static XT_header XT_work;
static sxlist_header sxlist_work;
static sxsubset_header sxsubset_work;
static sxSL_header sxSL_work;
static sxset_header sxset_work;
static XxY_header Xset_work;

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


static void sxSL_move (header, x, y, lgth)
    sxSL_header	*header;
    int			x, y, lgth;
{
    /* La liste list est reimplantee en y */
    /* Aucun debordement, on peut utiliser des pointeurs. */
    int	d;
    sxSL_elems	*p, *q;

    p = &(sxSL_elem (*header, x));
    q = &(sxSL_elem (*header, y));

    for (d = 0; d < lgth; d++)
	*q++ = *p++;
}


    void main (argc, argv)
	int		argc;
    char	*argv [];
{
    int 	listnb, erasenb, listsize, elem_val, i, j, k, l, top, lim, result, elem,
    scrmbl, *stack, *p;
    int		subset_id;
    SXBA	subset;
    long	t1;

    listnb = str_to_nb (argv [1]);
    listsize = str_to_nb (argv [2]);
    elem_val = str_to_nb (argv [3]); /* les elements sont ds [0 .. elem_val[ */
    erasenb = random () % listnb;

    stack = SS_alloc (listsize);

    getrusage (RUSAGE_SELF, &prev_usage);
    list_head = (struct list_head*) sxalloc (listnb, sizeof (struct list_head));

    for (j = 0; j < listnb; j++) {
	list_head [j].lgth = lim = random () % listsize;
	list_head [j].elems = p = (int*) sxalloc (lim, sizeof (int));

	for (k = 0; k < lim; k++) {
	    *p++ = random () % elem_val;
	}
    }
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("list generation time = %d\n", t1);

    getrusage (RUSAGE_SELF, &prev_usage);
    XH_alloc (&XH_work, "XH_work", listnb, 1, listsize / 2, NULL, stdout);
    
    for (j = 0; j < listnb; j++) {
	lim = list_head [j].lgth;
	p = list_head [j].elems;

	for (k = 0; k < lim; k++) {
	    XH_push (XH_work, *p);
	    p++;
	}
	
	XH_set (&XH_work, &k);
    }
    
    top = XH_top (XH_work) - 1;
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nXH_time (put) = %d\n", t1);

    XH_stat (stdout, &XH_work);
    getrusage (RUSAGE_SELF, &prev_usage);


    for (j = 1; j <= erasenb; j++) {
	XH_erase (XH_work, (random () % top) + 1);
    }
    
    XH_pack (&XH_work, 1, NULL);
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("XH_time (erase) = %d\n", t1);
    XH_free (&XH_work);


    getrusage (RUSAGE_SELF, &prev_usage);
    XT_alloc (&XT_work, "XT_work", listnb, 1, listsize / 2, NULL, stdout);
    
    for (j = 0; j < listnb; j++) {
	lim = list_head [j].lgth;
	p = list_head [j].elems;
	result = -1;

	for (k = 0; k < lim; k++) {
	    XT_push (XT_work, result, *p, &result);
	    p++;
	}

	XT_set (&XT_work, result, &result);
    }
    
    top = XT_top (XT_work) - 1;
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nXT_time (put) = %d\n", t1);

    XT_stat (stdout, &XT_work);

    getrusage (RUSAGE_SELF, &prev_usage);

    for (j = 1; j <= erasenb; j++) {
	XT_erase (XT_work, (random () % top) + 1);
    }
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("XT_time (erase) = %d\n", t1);
    XT_free (&XT_work);
    

    getrusage (RUSAGE_SELF, &prev_usage);
    sxlist_alloc (&sxlist_work, "sxlist_work", listnb, 1, listsize / 2, NULL, NULL, stdout);
    
    top = 0;

    for (j = 0; j < listnb; j++) {
	lim = list_head [j].lgth;
	p = list_head [j].elems;
	scrmbl = 0;

	for (k = 0; k < lim; k++) {
	    SS_push (stack, *p);
	    scrmbl += *p;
	    p++;
	}
	
	if (sxlist_set (&sxlist_work, scrmbl, stack, &result))
	    top = result;
    }
    
    top++;
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nsxlist_time (put) = %d\n", t1);

    sxlist_stat (stdout, &sxlist_work);

    getrusage (RUSAGE_SELF, &prev_usage);

    for (j = 1; j <= erasenb; j++) {
	sxlist_erase (&sxlist_work, (random () % top));
    }
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("sxlist_time (erase) = %d\n", t1);
    sxlist_free (&sxlist_work);
    

    getrusage (RUSAGE_SELF, &prev_usage);
    sxSL_alloc (sxSL_work, listnb, listsize / 2, NULL /* list_oflw */, sxSL_move);
    
    top = 0;

    for (j = 0; j < listnb; j++) {
/*
 	(((result = (sxSL_work).display_top) < (sxSL_work).display_size				 
 	  ? (sxSL_work).display_top++ 							 
	  : ((result = (sxSL_work).free) != -1						 
            ? (sxSL_work).free = (sxSL_work).display [result].X					 
	    : (sxSL_GC (&(sxSL_work))						 
	      ? NULL								 
              : ((sxSL_work).display = (struct sxSL_display*)			 
				 sxrealloc ((sxSL_work).display,			 
					    (sxSL_work).display_size *= 2,		 
					    sizeof (struct sxSL_display)),	 
	       (((sxSL_work).oflw != NULL)						 
	         ? (*(sxSL_work).oflw) ((sxSL_work).display_size)				 
                 : NULL),							 
	      result = (sxSL_work).display_top++)))),					 	 
         (sxSL_work).display [(sxSL_work).sorted_top++].sorted = result,				 
	 (sxSL_work).display [result].X = (sxSL_work).elems_top,					 
	 (sxSL_work).display [result].lgth = 0);
*/

	sxSL_seek (sxSL_work, result);

	lim = list_head [j].lgth;
	p = list_head [j].elems;

	for (k = 0; k < lim; k++) {
	    sxSL_put (sxSL_work, result, *p);
	    p++;
	}
	
	top = result;
    }
    
    top++;
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nsxSL_time (put) = %d\n", t1);

    sxSL_stat (&sxSL_work, stdout, sizeof (sxSL_elems));

    getrusage (RUSAGE_SELF, &prev_usage);

    for (j = 1; j <= erasenb; j++) {
	l = (random () % top);

	if (!sxSL_is_erased (sxSL_work, l))
	    sxSL_erase (sxSL_work, l);
    }
    
    sxSL_GC (&sxSL_work);

    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("sxSL_time (erase) = %d\n", t1);

    sxSL_stat (&sxSL_work, stdout, sizeof (sxSL_elems));

    sxSL_free (sxSL_work);
    

    getrusage (RUSAGE_SELF, &prev_usage);
    sxsubset_alloc (&sxsubset_work, listnb, elem_val, NULL, NULL, stdout);
    
    top = 0;
    subset_id = -1;

    for (j = 0; j < listnb; j++) {
	if (subset_id == -1)
	    subset_id = sxsubset_seek (&sxsubset_work);

	subset = sxsubset_to_sxba (sxsubset_work, subset_id);
	sxba_empty (subset);

	lim = list_head [j].lgth;
	p = list_head [j].elems;

	for (k = 0; k < lim; k++) {
	    elem = *p;
	    SXBA_1_bit (subset, elem);
	    p++;
	}
	
	if (sxsubset_set (&sxsubset_work, subset_id) == subset_id) {
	    top = subset_id;
	    subset_id = -1;
	}
    }
    
    top++;
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nsxsubset_time (put) = %d\n", t1);

    sxsubset_stat (stdout, &sxsubset_work);

    /* sxsubset_to_c (&sxsubset_work, stdout, "sxsubset_work", true); */
    getrusage (RUSAGE_SELF, &prev_usage);

    for (j = 1; j <= erasenb; j++) {
	i = random () % top;
	
	if (!sxsubset_is_released (sxsubset_work, i))
	{
	    if (sxsubset_is_set (sxsubset_work, i))
		sxsubset_unset (sxsubset_work, i);

	    sxsubset_release (sxsubset_work, i);
	}
    }
    
    /* sxsubset_to_c (&sxsubset_work, stdout, "sxsubset_work", true); */

    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("sxsubset_time (delete) = %d\n", t1);
    sxsubset_free (&sxsubset_work);
    
{
    unsigned long set_id, elem;

    getrusage (RUSAGE_SELF, &prev_usage);
    sxset_alloc (&sxset_work, "sxset_work", elem_val, listnb, listsize / 2, NULL, NULL, stdout);
    
    for (j = 0; j < listnb; j++) {
	set_id = sxset_new_set_id (&sxset_work); /* set_id == j */

	lim = list_head [j].lgth;
	p = list_head [j].elems;

	for (k = 0; k < lim; k++) {
	    elem = (unsigned long) *p;
	    sxset_add_member (&sxset_work, set_id, elem);
	    p++;
	}
    }
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nsxset_time (put) = %d\n", t1);

    sxset_stat (&sxset_work, stdout);

    getrusage (RUSAGE_SELF, &prev_usage);

    for (j = 1; j <= erasenb; j++) {
	i = random () % sxset_sets_top (sxset_work);
	
	if (!sxset_is_set_deleted (sxset_work, i))
	{
	    sxset_delete_set (sxset_work, i);
	}
    }
    
    sxset_pack_elems (&sxset_work);

    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("sxset_time (delete) = %d\n", t1);
    sxset_free (&sxset_work);
}
    

 {
    int set_id = 0, x;
    long	t2;

    getrusage (RUSAGE_SELF, &prev_usage);
    XxY_alloc (&Xset_work, "Xset_work", listnb * listsize / 2, 1, 1, 0, NULL, stdout);
    
    for (j = 0; j < listnb; j++) {
	set_id++;

	lim = list_head [j].lgth;
	p = list_head [j].elems;

	for (k = 0; k < lim; k++) {
	    XxY_set (&Xset_work, set_id, *p, &x);
	    p++;
	}
    }
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("\nXset_time (put) = %d\n", t1);

    XxY_stat (stdout, &Xset_work);

    getrusage (RUSAGE_SELF, &prev_usage);

    for (j = 1; j <= erasenb; j++) {
	i = (random () % set_id) + 1;
	
	XxY_Xforeach (Xset_work, i, x)
	{
	    XxY_erase (Xset_work, x);
	}
    }
    
    getrusage (RUSAGE_SELF, &next_usage);
    
    t1 = (long) ((1000000 * next_usage.ru_utime.tv_sec + next_usage.ru_utime.tv_usec - (1000000 * prev_usage.ru_utime.tv_sec + prev_usage.ru_utime.tv_usec)) / 1000);
    
    printf ("Xset_time (delete) = %d\n", t1);
    
    t1 = (long) XxY_total_sizeof (Xset_work);
    t2 = (long) XxY_used_sizeof (Xset_work);

    printf ("Total_sizeof (byte) = %d (%d%%)\n", t1, (100*t2)/t1);

    XxY_free (&Xset_work);
}

   SS_free (stack);

    sxexit (0);
}

