#include "sxunix.h"
#include "lecl_td.h"
char WHAT_LECLPACT[] = "@(#)SYNTAX - $Id: lecl_pact.c 582 2007-05-25 08:25:51Z rlacroix $" WHAT_DEBUG;

static int	NOT_ste, KEYWORD_ste;

int	lecl_pars_act (entry, action_no)
    int		entry, action_no;
{
    switch (entry) {
    case OPEN:
    case CLOSE:
    case FINAL:
	return 0;

    case INIT:
	/* The keywords "NOT" and "KEYWORD" are not reserved. */
	NOT_ste = sxstrsave ("NOT");
	KEYWORD_ste = sxstrsave ("KEYWORD");
	return 0;

    case PREDICATE:
	switch (action_no) {
	case 1:
	    return sxget_token (sxplocals.ptok_no)->string_table_entry == (unsigned)NOT_ste &&
		sxget_token (sxplocals.ptok_no + 1)->string_table_entry == (unsigned)KEYWORD_ste;

	case 2:
	    return sxget_token (sxplocals.ptok_no)->string_table_entry == (unsigned)KEYWORD_ste &&
		sxget_token (sxplocals.ptok_no + 1)->lahead != EQUAL_code;

	default:
	    break;
	}

	break;

    default:
	break;
    }

    fputs ("The function \"lecl_pars_act\" is out of date \
with respect to its specification.\n", sxstderr);
    abort ();
}
