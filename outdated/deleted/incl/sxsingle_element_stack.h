
/* this (commented out) code fragment was originally in sxnd.h */

#if 0

/* S I N G L E - E L E M E N T - S T A C K  */

struct sxses {
    SXBA	set;
    SXINT		*stack;
};

#define sxses_set(ses)		ses.set

#define sxses_alloc(ses,size)	ses.set = sxba_calloc (size + 1),			\
                                ses.stack = (SXINT *) sxalloc (size + 1, sizeof (SXINT)),	\
                                ses.stack [0] = 0

#define sxses_resize(ses,size)	ses.set = sxba_resize (ses.set, size + 1),		\
                                ses.stack = (SXINT *) sxrealloc (ses.stack, size + 1, sizeof (SXINT))

#define sxses_is_free(ses)	(ses.set == NULL)

#define sxses_clean(ses)	ses.set = NULL,						\
                                ses.stack = NULL

#define sxses_free(ses)		sxfree (ses.set),					\
                                sxfree (ses.stack),					\
                                sxses_clean (ses)

#define sxses_size(ses)		(SXBASIZE(ses.set) - 1)

#define sxses_top(ses)		ses.stack [0]

#define sxses_is_empty(ses)	(sxses_top(ses) == 0)

#define sxses_raz(ses)		sxba_empty (ses.set), ses.stack [0] = 0

#define sxses_get(ses,i)	ses.stack [i]

#define sxses_spush(ses,x)	ses.stack [++*ses.stack] = x

#define sxses_push(ses,x)	(SXBA_bit_is_reset_set (ses.set, x)			\
                                    ? ses.stack [++*ses.stack] = x			\
                                    : 0)

#define sxses_fpush(ses,x)	SXBA_1_bit (ses.set, x),				\
                                ses.stack [++*ses.stack] = x

#define sxses_pop(ses)		ses.stack [(*ses.stack)--]


/* E N D   of  S I N G L E - E L E M E N T - S T A C K  */


