#include "sxunix.h"
char WHAT_LECLSACT[] = "@(#)SYNTAX - $Id: lecl_sact.c 870 2007-11-07 15:21:10Z rlacroix $" WHAT_DEBUG;

VOID	lecl_scan_act (SXINT what, SXINT act_no)
{
    switch (what) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
	return;

    case ACTION:
	switch (act_no) {
	case 1:
	    /* Dark Letter Check */
	    if (sxsrcmngr.current_char != sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth - 1])
		sxput_error (sxsrcmngr.source_coord, "%sA dark symbol must be built up with the same character.", sxsvar.
		     sxtables->err_titles [1] /* Warning */ );

	    return;

	case 2:
	    /* \nnn => char */
	    {
		char	val;
		char	c, *s, *t;

		t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != NUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
		sxsvar.lv.mark.index = -1;
	    }

	    return;

	default:
	    break;
	}

    default:
	fputs ("The function \"lecl_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
