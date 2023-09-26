#include "sxunix.h"
char WHAT_PPRECORSACT[] = "@(#)SYNTAX - $Id: pprecor_sact.c 870 2007-11-07 15:21:10Z rlacroix $" WHAT_DEBUG;

VOID	pp_recor_scan_act (SXINT what, SXINT act_no)
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
	    if (sxsrcmngr.current_char != sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth - 1])
		sxput_error (sxsrcmngr.source_coord, "%sA dark symbol must be built up with the same character.", sxsvar.
		     sxtables->err_titles [1]);

	    return;

	case 2:
	    do {
		switch (sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth - 1]) {
		case ' ':
		case '\t':
		case '\n':
		    sxsvar.lv.ts_lgth--;
		    continue;

		default:
		    sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth++] = '\n';
		    break;
		}

		break;
	    } while (sxsvar.lv.ts_lgth > 0);

	    return;

	default:
	    break;
	}

    default:
	fputs ("The function \"pp_recor_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
