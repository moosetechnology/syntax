/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.        *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030520 14:37 (phd):	Modifs pour calmer le compilo SGI	*/
/************************************************************************/
/* 25-05-87 12:40 (phd):	Ajout d'un message d'erreur	   	*/
/************************************************************************/
/* 16-03-87 09:18 (pb):	Ajout du "copyright"			*/
/************************************************************************/
/* 16-03-87 09:18 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
char WHAT_TDEFSACT[] = "@(#)SYNTAX - $Id: tdef_sact.c 870 2007-11-07 15:21:10Z rlacroix $" WHAT_DEBUG;


SXINT
tdef_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    case OPEN:
    case INIT:
    case FINAL:
    case CLOSE:
	break;

    case ACTION:
	switch (act_no) {
	case 1:
	    /* \nnn => char */
	    {
		char    val;
		char	c, *s, *t;

		t = s = sxsvar.lv_s.token_string + sxsvar.lv.mark.index;

		for (val = *s++ - '0'; (c = *s++) != NUL; ) {
		    val = (val << 3) + c - '0';
		}

		*t = val;
		sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + 1;
		sxsvar.lv.mark.index = -1;
	    }

	    return 0;

	default:
	    break;
	}

    default:
	fputs ("The function \"tdef_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }

    return 0;
}
