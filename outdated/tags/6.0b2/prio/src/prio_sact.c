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
/* 20030515 14:43 (phd):	Adaptation � SGI (64 bits)		*/
/************************************************************************/
/* 25-05-87 12:40 (phd):	Ajout d'un message d'erreur	   	*/
/************************************************************************/
/* 16-03-87 09:18 (pb):	Ajout du "copyright"				*/
/************************************************************************/
/* 16-03-87 09:18 (pb):	Ajout de cette rubrique "modifications"		*/
/************************************************************************/

#include "sxunix.h"
char WHAT_PRIOSACT[] = "@(#)SYNTAX - $Id: prio_sact.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


SXVOID
prio_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
	return;

    case ACTION:
	switch (act_no) {
	case 1:
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
	    break;
	}

    default:
	fputs ("The function \"prio_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
