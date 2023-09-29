#include "sxunix.h"
char WHAT_PPLECLSACT[] = "@(#)SYNTAX - $Id: pplecl_sact.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

VOID	pp_lecl_scan_act (what, act_no)
    int		what, act_no;
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
	fputs ("The function \"pp_lecl_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
