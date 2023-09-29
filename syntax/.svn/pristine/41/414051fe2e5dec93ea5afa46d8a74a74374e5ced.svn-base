typedef struct {
  char		**hd, *cur_top, *cur_bot, *pool_lim;
  int		hd_size, hd_top, pool_size, elem_size;
  int		used_size, total_size; /* #if EBUG */
} pool_header;

#define pool_next(b,t)	(t)((((b).cur_top+=(b).elem_size)>=(b).pool_lim)?pool_oflw(&b):(b).cur_top)

#define pool_close(b,t,bot,top) 		\
                        bot = (t)(b).cur_bot,	\
                        top = (t)(b)->cur_top,	\
			(b).cur_bot = (b)->cur_top + (b).elem_size


#include <memory.h>


static void
pool_alloc (ppool, pool_size, elem_size)
    pool_header	*ppool;
    int 	pool_size, elem_size;
{
    ppool->hd_top = 0;
    ppool->hd_size = 1;
    ppool->elem_size = sizeof (char) * elem_size;
    ppool->pool_size = pool_size * ppool->elem_size;
    ppool->hd = (char**) sxalloc (ppool->hd_size, sizeof (char*));
    ppool->pool_lim = ppool->cur_top = ppool->cur_bot = ppool->hd [ppool->hd_top] =
	    (char*) sxalloc (ppool->pool_size, ppool->elem_size);
    ppool->cur_bot += ppool->elem_size;
    ppool->pool_lim += ppool->pool_size;

#if EBUG
    ppool->total_size = ppool->pool_size;
#endif
}



static char*
pool_oflw (ppool)
    pool_header	*ppool;
{
    char	*ptr = ppool->cur_bot;
    int 	n = ppool->cur_lim - ppool->cur_bot;

    if (++ppool->hd_top >= ppool->hd_size)
	ppool->hd = (char**) sxrealloc (ppool->hd, ppool->hd_size *= 2, sizeof (char*));

    ppool->pool_size *= 2;

#if EBUG
    printf ("New pool of size %i is created.\n", ppool->pool_size);
    ppool->used_size = ppool->total_size;
    ppool->total_size += ppool->pool_size;
#endif

    ppool->pool_lim = ppool->cur_bot = ppool->hd [ppool->hd_top] =
	(char*) sxalloc (ppool->pool_size, sizeof (char));

    ppool->cur_bot += ppool->elem_size;
    ppool->pool_lim += ppool->pool_size;

    memcpy (ppool->cur_bot, ptr, n);

    return ppool->cur_top = ppool->cur_bot + n;
}



static void
pool_free (ppool)
    pool_header	*ppool;
{
#if EBUG
    printf ("Pool: used_size = %i bytes, total_size = %i bytes\n",
	    (ppool->used_size + (ppool->cur_bot - ppool->hd [ppool->hd_top]))
	     + (ppool->hd_top + 1) * sizeof (int),
    	    ppool->total_size + ppool->hd_size * sizeof (int));
#endif

    while (--ppool->hd_top >= 0)
	sxfree (ppool->hd [ppool->hd_top]);

    sxfree (ppool->hd);
}
