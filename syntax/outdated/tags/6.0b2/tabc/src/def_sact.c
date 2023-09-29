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
/* 10-09-87 18:18 (pb&phd):	Action 2 				*/
/************************************************************************/
/* 18-03-87 10:32 (phd&pb):	Mise en commun des actions de tabc et	*/
/*				tabact.					*/
/************************************************************************/
/* 15-03-87 17:04 (pb&phd):	Actions 0 et 3				*/
/************************************************************************/
/* 16-03-87 09:18 (pb):		Ajout du "copyright"			*/
/************************************************************************/
/* 16-03-87 09:18 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"
char WHAT_TABCDEFSACT[] = "@(#)SYNTAX - $Id: def_sact.c 564 2007-05-15 15:19:42Z rlacroix $" WHAT_DEBUG;


extern SXVOID    bnf_skip_rule ();
extern SXVOID    bnf_found_bad_beginning_of_rule ();



int def_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    case INIT:
    case FINAL:
    case OPEN:
    case CLOSE:
	return 0;

    case ACTION:
	switch (act_no) {
	case 0:
	    bnf_found_bad_beginning_of_rule ();
	    return 0;

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

	    return 0;

	case 2:
	    sxput_error (sxsrcmngr.source_coord,
			"%s\"%s)\" is forced before End Of Line.",
			sxsvar.sxtables->err_titles [1], CHAR_TO_STRING (sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth - 1]));
	    return 0;

	case 3:
	    bnf_skip_rule ();
	    return 0;
	default:
#if EBUG
	  sxtrap("def_sact","unknown switch case #1");
#endif
	  break;
	}

    default:
	fputs ("\nThe function \"def_scan_act\" is out of date with respect to its specification.\n", sxstderr);
	abort ();
    }
}
