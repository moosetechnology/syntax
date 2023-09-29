

#include "sxunix.h"
#include "SS.h"

static int	*stack;
static int bot;

/* Doit etre fait par des tdef... */
#define	a	2
#define	b	3

int	wcw_pars_act (entry, action_no)
    int		entry, action_no;
{
    switch (entry) {
    case OPEN:
    case CLOSE:
	return;

    case INIT:
	stack = SS_alloc (100);
	SS_push (stack, 0);
	bot = SS_bot (stack) + 1;
	return;

    case FINAL:
	SS_free (stack);
	return;

    case PREDICATE:
	switch (action_no) {
	case 1:
	    return SS_get (stack, bot) == a;
	case 2:
	    return SS_get (stack, bot) == b;

	default:
	    break;
	}

	break;

    case ACTION:
	switch (action_no) {
	case 1:
	    bot++;
	    return;
	case 2:
	    SS_push (stack, a);
	    return;
	case 3:
	    SS_push (stack, b);
	    return;

	default:
	    break;
	}

	break;

    default:
	break;
    }

    fputs ("The function \"wcw_pars_act\" is out of date \
with respect to its specification.\n", sxstderr);
    abort ();
}
