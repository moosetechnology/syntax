

#include "sxunix.h"
#include "SS.h"

static int	*stack;
static int bot;

/* Doit etre fait par des tdef... */
#define	a	2
#define	b	3

bool wcw_pars_act (entry, action_no)
    int		entry, action_no;
{
    switch (entry) {
    case SXOPEN:
    case SXCLOSE:
	return SXANY_BOOL;

    case SXINIT:
	stack = SS_alloc (100);
	SS_push (stack, 0);
	bot = SS_bot (stack) + 1;
	return SXANY_BOOL;

    case SXFINAL:
	SS_free (stack);
	return SXANY_BOOL;

    case SXPREDICATE:
	switch (action_no) {
	case 1:
	    return SS_get (stack, bot) == a;
	case 2:
	    return SS_get (stack, bot) == b;

	default:
	    break;
	}

	break;

    case SXACTION:
	switch (action_no) {
	case 1:
	    bot++;
	    return SXANY_BOOL;
	case 2:
	    SS_push (stack, a);
	    return SXANY_BOOL;
	case 3:
	    SS_push (stack, b);
	    return SXANY_BOOL;

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
