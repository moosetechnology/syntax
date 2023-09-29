#include "sxunix.h"

int	lecl_scan_act (lecl_scan_act_what, act_no)
    int		lecl_scan_act_what, act_no;
{
    switch (lecl_scan_act_what) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
	return 1;

    case ACTION:
	switch (act_no) {
	case 1:
	    /* Dark Letter Check */
	    if (sxsrcmngr.current_char != sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth - 1])
		sxput_error (sxsrcmngr.source_coord, "%sA dark symbol must be built up with the same character.", sxsvar.
		     sxtables->err_titles [1] /* Warning */ );

	    return 1;

	case 2:
	    /* \nnn => char */
	    {
		register int	val;
		register char	c, *s, *t;

		t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != NUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
		sxsvar.lv.mark.index = -1;
	    }

	    return 1;

	default:
	    break;
	}

    default:
	fputs ("The function \"lecl_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
