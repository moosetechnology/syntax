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
char WHAT_SRCGSACT[] = "@(#)SYNTAX - $Id: srcg_sact.c 1112 2008-02-26 12:54:01Z rlacroix $" WHAT_DEBUG;

extern struct sxtables	srcg_tables;

static VARSTR	vstr;


static SXVOID
gripe (void)
{
    fputs ("\nThe function \"srcg_scan_act\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

/* ATTENTION : srcg_scan_act peut etre appele au cours de la passe srcg puis au cours de la passe dsrcg.
   Dans ce cas on a la sequence
   srcg_scan_act (OPEN) srcg ... srcg_scan_act (ACTION) srcg ... srcg_scan_act (OPEN) dsrcg ...
   srcg_scan_act (ACTION) dsrcg ... srcg_scan_act (CLOSE) dsrcg ... srcg_scan_act (CLOSE) srcg ...
   et donc vstr est alloue 2 fois (pas trop grave) mais surtout on le libere 2 fois de suite...
   D'ou les tests a l'alloc et a la liberation */
SXINT
srcg_scan_act (SXINT code, SXINT act_no)
{
    SXINT 	x, kw;
    char	c;

    switch (code) {
    default:
	gripe ();

    case SXOPEN:
	if (vstr == NULL)
	    vstr = varstr_alloc (32);
	break;

    case SXCLOSE:
	if (vstr)
	    varstr_free (vstr), vstr = NULL;
	break;

    case SXBEGIN:
    case SXEND:
    case SXINIT:
    case SXFINAL:
	break;

    case SXACTION:
	switch (act_no) {
	default:
	    gripe ();

	case 1:
	    /* On regarde si la version minuscule de token_string est un predicat predefini */
	    if (!sxkeywordp (&srcg_tables, sxsvar.sxlv.terminal_token.lahead)) {
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
		    sxsvar.sxlv.terminal_token.string_table_entry = SXEMPTY_STE;
		    sxsvar.sxlv.ts_lgth = 0;
		}
	    }
	}

	break;
    }

    return 0;
}

