/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
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
/* 03-03-87 11:01 (phd):	Ajout de ppada_args_scan_act		*/
/************************************************************************/
/* 02-03-87 13:35 (phd):	Creation				*/
/************************************************************************/

#include "sxunix.h"
#include "ppada_td.h"
#include <ctype.h>
char WHAT_PPADASACT[] = "@(#)SYNTAX - $Id: ppada_sact.c 1177 2008-03-10 12:00:54Z rlacroix $" WHAT_DEBUG;


extern struct sxtables	ppada_args_tables, ppada_tables;



static VOID	gripe (struct sxtables *sxtables, SXINT act_no)
{
    sxuse (act_no);
    sxput_error (sxsvar.lv.terminal_token.source_index,
	 "%sInternal inconsistency: Action %d not yet implemented.\nExecution aborted.", sxtables->err_titles [2]);
    SXEXIT (3);
}


BOOLEAN
ppada_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe (&ppada_tables, code);
	    /*NOTREACHED*/

    case BEGIN:
    case END:
	break;

    case OPEN:
    case CLOSE:
	break;

    case INIT:
    case FINAL:
	break;

    case ACTION:
	switch (act_no) {
	default:
	    gripe (&ppada_tables, act_no);
		/*NOTREACHED*/

	case 1:
	    /* {lower} UPPER {LETTER} */
	    switch (sxsvar.lv.ts_lgth) {
		static char	string [10];

	    case 2:
	    case 3:
	    case 4:
	    case 5:
	    case 6:
	    case 7:
	    case 8:
	    case 9:
		{
		    char	c, *str1, *str2;

		    str1 = sxsvar.lv_s.token_string, str2 = string;

		    for (; ; ) {
			if (isupper (c = *str1++)) {
			    *str2++ = c - 'A' + 'a';
			    continue;
			}

			if ((*str2++ = c) == NUL)
			    break;
		    }
		}

		if ((*sxsvar.SXS_tables.check_keyword) (string, sxsvar.lv.ts_lgth)) {
		    strcpy (sxsvar.lv_s.token_string, string);
		}

	    default:
		break;
	    }

	    break;

	case 4:
	    /* comments: rtrim (ESPACE, TAB, EOL) */
	    for (; ; ) {
		switch (sxsvar.lv_s.token_string [--sxsvar.lv.ts_lgth]) {
		case '\n':
		case '\t':
		case ' ':
		    if (sxsvar.lv.ts_lgth == 0) {
			sxsvar.lv_s.token_string [0] = NUL;
			break;
		    }

		    continue;

		default:
		    sxsvar.lv_s.token_string [++sxsvar.lv.ts_lgth] = '\n';
		    sxsvar.lv_s.token_string [++sxsvar.lv.ts_lgth] = NUL;
		    break;
		}

		break;
	    }

	    break;
	}

	break;

    case PREDICATE:
	switch (act_no) {
	default:
	    gripe (&ppada_tables, act_no);
		/*NOTREACHED*/

	case 1:
	    switch (sxsvar.lv.terminal_token.lahead) {
	    default:
		return FALSE;

	    case IDENTIFIER:
	    case RP:
		return TRUE;
	    }
	}
    }

    return TRUE;
}


SXVOID
ppada_args_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe (&ppada_args_tables, code);

    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
	break;

    case ACTION:
	switch (act_no) {
	    SXINT	length;
	    char	*mark, *value;

	default:
	    gripe (&ppada_args_tables, act_no);
		/*NOTREACHED*/

	case 3:
	    if ((value = getenv (mark = sxsvar.lv_s.token_string + sxsvar.lv.mark.index)) == NULL) {
		sxput_error (sxsvar.lv.mark.source_coord, "%s%s is not in the environment.\n", ppada_args_tables.
		     err_titles [2], mark);
		length = 0;
	    }
	    else {
		if (sxsvar.lv_s.ts_lgth_use <= sxsvar.lv.mark.index + (length = strlen (value))) {
		    sxsvar.lv_s.token_string = sxrealloc (sxsvar.lv_s.token_string, sxsvar.lv_s.ts_lgth_use += length + 1
			 , sizeof (char));
		}

		strcpy (mark, value);
	    }

	    sxsvar.lv.ts_lgth = sxsvar.lv.mark.index + length;
	    break;
	}

	break;
    }
}
