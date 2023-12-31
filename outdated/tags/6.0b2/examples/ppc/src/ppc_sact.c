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
/* 07-12-87 13:30 (phd):	Adaptation pour autres styles		*/
/************************************************************************/
/* 17-02-87 17:35 (phd):	Ajout de ppc_args_scan_act		*/
/************************************************************************/
/* 13-01-87 14:00 (phd):	Creation				*/
/************************************************************************/

#include "sxunix.h"
char WHAT_PPCSACT[] = "@(#)SYNTAX - $Id: ppc_sact.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;


extern struct sxtables	ppc_args_tables, *ppc_tables;
extern int	ppc_TYPE;


static VOID	gripe (sxtables, act_no)
    struct sxtables	*sxtables;
    int		act_no;
{
    sxput_error (sxsvar.lv.terminal_token.source_index,
	 "%sInternal inconsistency: Action %d not yet implemented.\nExecution aborted.", sxtables->err_titles [2]);
    SXEXIT (3);
}



static BOOLEAN	*user_types;
static int	typ_lgth;


SXVOID
ppc_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    default:
	gripe (ppc_tables, code);

    case BEGIN:
	user_types = (BOOLEAN*) sxcalloc (typ_lgth = sxstrmngr.tablength, sizeof (BOOLEAN));
	return;

    case END:
	sxfree (user_types);
	return;

    case OPEN:
    case CLOSE:
	return;

    case INIT:
    case FINAL:
	return;

    case ACTION:
	if (typ_lgth < sxstrmngr.top) {
	    register int	i = typ_lgth;

	    user_types = (BOOLEAN*) sxrealloc (user_types, typ_lgth = sxstrmngr.tablength, sizeof (BOOLEAN));

	    while (i < typ_lgth) {
		user_types [i++] = FALSE;
	    }
	}

	switch (act_no) {
	default:
	    gripe (ppc_tables, act_no);

	case 0:
	    /* -TYPE */
	    user_types [STACKtoken (STACKtop ()).string_table_entry] = TRUE;
	    break;

	case 1:
	    /* {lower}+ */
	    if (sxkeywordp (ppc_tables, sxsvar.lv.terminal_token.lahead)) {
		return;
	    }

	case 2:
	    /* %NAME */
	    if (user_types [sxsvar.lv.terminal_token.string_table_entry]) {
		sxsvar.lv.terminal_token.lahead = ppc_TYPE;
	    }
	}

	return;
    }
}


SXVOID
ppc_args_scan_act (code, act_no)
    int		code;
    int		act_no;
{
    switch (code) {
    default:
	gripe (&ppc_args_tables, code);

    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
	return;

    case ACTION:
	switch (act_no) {
	    register int	length;
	    register char	*mark, *value;
	    extern char		*getenv ();

	default:
	    gripe (&ppc_args_tables, act_no);

	case 3:
	    if ((value = getenv (mark = sxsvar.lv_s.token_string + sxsvar.lv.mark.index)) == NULL) {
		sxput_error (sxsvar.lv.mark.source_coord, "%s%s is not in the environment.\n", ppc_args_tables.err_titles
		     [2], mark);
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
