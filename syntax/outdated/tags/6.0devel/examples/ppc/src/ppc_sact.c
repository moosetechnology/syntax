/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */





#include "sxunix.h"
char WHAT_PPCSACT[] = "@(#)SYNTAX - $Id: ppc_sact.c 1558 2008-09-04 11:55:31Z rlacroix $";


extern struct sxtables	ppc_args_tables, *ppc_tables;
extern SXINT	ppc_TYPE;


static SXVOID	gripe (struct sxtables *sxtables, SXINT act_no)
{
    sxuse (act_no);
    sxerror (sxsvar.sxlv.terminal_token.source_index,
	     sxtables->err_titles [2][0],
	     "%sInternal inconsistency: Action %d not yet implemented.\nExecution aborted.",
	     sxtables->err_titles [2]+1);
    SXEXIT (3);
}



static SXBOOLEAN	*user_types;
static SXINT	typ_lgth;


SXVOID
ppc_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe (ppc_tables, code);

    case SXBEGIN:
	user_types = (SXBOOLEAN*) sxcalloc (typ_lgth = sxstrmngr.tablength, sizeof (SXBOOLEAN));
	return;

    case SXEND:
	sxfree (user_types);
	return;

    case SXOPEN:
    case SXCLOSE:
	return;

    case SXINIT:
    case SXFINAL:
	return;

    case SXACTION:
	if (typ_lgth < sxstrmngr.top) {
	    SXINT	i = typ_lgth;

	    user_types = (SXBOOLEAN*) sxrealloc (user_types, typ_lgth = sxstrmngr.tablength, sizeof (SXBOOLEAN));

	    while (i < typ_lgth) {
		user_types [i++] = SXFALSE;
	    }
	}

	switch (act_no) {
	default:
	    gripe (ppc_tables, act_no);

	case 0:
	    /* -TYPE */
	    user_types [SXSTACKtoken (SXSTACKtop ()).string_table_entry] = SXTRUE;
	    break;

	case 1:
	    /* {lower}+ */
	    if (sxkeywordp (ppc_tables, sxsvar.sxlv.terminal_token.lahead)) {
		return;
	    }

	case 2:
	    /* %NAME */
	    if (user_types [sxsvar.sxlv.terminal_token.string_table_entry]) {
		sxsvar.sxlv.terminal_token.lahead = ppc_TYPE;
	    }
	}

	return;
    }
}


SXVOID
ppc_args_scan_act (SXINT code, SXINT act_no)
{
    switch (code) {
    default:
	gripe (&ppc_args_tables, code);

    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
	return;

    case SXACTION:
	switch (act_no) {
	    SXINT	length;
	    char	*mark, *value;

	default:
	    gripe (&ppc_args_tables, act_no);

	case 3:
	    if ((value = getenv (mark = sxsvar.sxlv_s.token_string + sxsvar.sxlv.mark.index)) == NULL) {
		sxerror (sxsvar.sxlv.mark.source_coord,
			 ppc_args_tables.err_titles[2][0],
			 "%s%s is not in the environment.\n",
			 ppc_args_tables.err_titles[2]+1,
			 mark);
		length = 0;
	    }
	    else {
		if (sxsvar.sxlv_s.ts_lgth_use <= sxsvar.sxlv.mark.index + (length = strlen (value))) {
		    sxsvar.sxlv_s.token_string = sxrealloc (sxsvar.sxlv_s.token_string, sxsvar.sxlv_s.ts_lgth_use += length + 1
			 , sizeof (char));
		}

		strcpy (mark, value);
	    }

	    sxsvar.sxlv.ts_lgth = sxsvar.sxlv.mark.index + length;
	    break;
	}

	break;
    }
}
