#ifndef SS_h
#define SS_h

#include "sxalloc.h"

extern int		*SS_alloc (/* int size */);
extern int		SS_sature (/* int **SS_ptr_ptr */);
#define SS_free(t)	sxfree((t)-1)
#define SS_get(t,x)	((t)[x])
#define SS_top(t)	(*(t))
#define SS_bot(t)	((t)[1])
#define SS_get_top(t)	((t)[SS_top(t)-1])
#define SS_get_bot(t)	((t)[SS_bot(t)])
#define SS_size(t)	((t)[-1])
#define SS_decr(t)	(--SS_top(t))
/* ATTENTION : utilisation de &(t) ... */
#define SS_push(t,e)	((t)[SS_top(t)++]=(e),SS_top(t)>SS_size(t)?SS_sature(&(t)):0)
#define SS_pop(t)	(SS_get(t,SS_decr(t)))
#define SS_open(t)	(SS_push(t,SS_bot(t)),SS_bot(t)=SS_top(t))
#define SS_close(t)	(SS_bot(t)=SS_get(t,SS_top(t)=SS_bot(t)-1))
#define SS_is_empty(t)	(SS_bot(t)==SS_top(t))
#define SS_is_ss_empty(t)	(SS_is_empty(t)&&SS_bot(t)==2)
#define SS_clear(t)	SS_top(t)=SS_bot(t)
#define SS_clear_ss(t)	SS_clear(t)=2

#endif
