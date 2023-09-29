#include "sxunix.h"
#include "lecl_td.h"

static SXINT	NOT_ste, KEYWORD_ste;

SXINT	lecl_pars_act (SXINT entry, SXINT action_no)
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

    case PREDICATE: {
	switch (action_no) {
	default:
	    break;
	case 1:
	    return sxget_token (sxplocals.ptok_no)->string_table_entry == (SXUINT)NOT_ste &&
		sxget_token (sxplocals.ptok_no + 1)->string_table_entry == (SXUINT)KEYWORD_ste;

	case 2:
	    return sxget_token (sxplocals.ptok_no)->string_table_entry == (SXUINT)KEYWORD_ste &&
		sxget_token (sxplocals.ptok_no + 1)->lahead != EQUAL_code;

	}
	break;
    }
    default:
	break;
    }

    fputs ("The function \"lecl_pars_act\" is out of date \
with respect to its specification.\n", sxstderr);
    abort ();
}
