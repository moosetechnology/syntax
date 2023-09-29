/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs. (pb)*
   *                                                      *
   ******************************************************** */

#include "sxunix.h"
char WHAT_RECORSACT[] = "@(#)SYNTAX - $Id: recor_sact.c 564 2007-05-15 15:19:42Z rlacroix $" WHAT_DEBUG;

static VOID	gripe ()
{
    fputs ("\nThe function \"recor_scan_act\" is out of date with respect to its specification.\n", sxstderr);
    abort ();
}



VOID	recor_scan_act (what, act_no)
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

	    return;
	default:
#if EBUG
	  sxtrap("recor_sact","unknown switch case #1");
#endif
	  break;
	}

    default:
	gripe ();
    }
}
