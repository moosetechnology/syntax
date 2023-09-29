/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1998 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe ATOLL.		          *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Jeu 26 Fev 1998 10:56 (pb):	Creation				*/
/************************************************************************/

#include "sxunix.h"
#include "varstr.h"
#include <ctype.h>
char WHAT_RCGSACT[] = "@(#)SYNTAX - $Id: rcg_sact.c 1112 2008-02-26 12:54:01Z rlacroix $" WHAT_DEBUG;

extern struct sxtables	rcg_tables;

static VARSTR	vstr;


static VOID	gripe (void)
{
    fputs ("\nThe function \"rcg_scan_act\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

/* ATTENTION : rcg_scan_act peut etre appele au cours de la passe rcg puis au cours de la passe drcg.
   Dans ce cas on a la sequence
   rcg_scan_act (OPEN) rcg ... rcg_scan_act (ACTION) rcg ... rcg_scan_act (OPEN) drcg ...
   rcg_scan_act (ACTION) drcg ... rcg_scan_act (CLOSE) drcg ... rcg_scan_act (CLOSE) rcg ...
   et donc vstr est alloue 2 fois (pas trop grave) mais surtout on le libere 2 fois de suite...
   D'ou les tests a l'alloc et a la liberation */
SXINT rcg_scan_act (SXINT code, SXINT act_no)
{
    SXINT 	x, kw;
    char	c;

    switch (code) {
    default:
	gripe ();

    case OPEN:
	if (vstr == NULL)
	    vstr = varstr_alloc (32);
	break;

    case CLOSE:
	if (vstr)
	    varstr_free (vstr), vstr = NULL;
	break;

    case BEGIN:
    case END:
    case INIT:
    case FINAL:
	break;

    case ACTION:
	switch (act_no) {
	default:
	    gripe ();

	case 1:
	    /* On regarde si la version minuscule de token_string est un predicat predefini */
	    if (!sxkeywordp (&rcg_tables, sxsvar.sxlv.terminal_token.lahead)) {
		/* Ce n'est pas un predicat predefini ecrit en minuscule */
		varstr_raz (vstr);

		for (x = 0; x < sxsvar.sxlv.ts_lgth; x++) {
		    c = sxsvar.sxlv_s.token_string [x];

		    if (isupper (c)) {
			c = tolower (c);
		    }

		    varstr_catchar (vstr, c);
		}

		varstr_complete (vstr);

		if ((kw = (*sxsvar.SXS_tables.check_keyword) (varstr_tostr (vstr), sxsvar.sxlv.ts_lgth)) > 0) {
		    /* C'est un predicat predefini */
		    sxsvar.sxlv.terminal_token.lahead = kw;
		    sxsvar.sxlv.terminal_token.string_table_entry = EMPTY_STE;
		    sxsvar.sxlv.ts_lgth = 0;
		}
	    }
	}

	break;
    }

    return 0;
}

