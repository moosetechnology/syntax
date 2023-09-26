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
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                (PB)				  *
   *                                                      *
   ******************************************************** */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 11-05-93 15:54 (pb):		changement de char_to_string [] ?? en	*/
/*				sxc_to_str []				*/
/************************************************************************/
/* 08-04-92 15:58 (pb):		Utilisation de sxsvar.sxlv.mode		*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 05-05-87 17:52 (pb&phd) :	On ne sort plus de message en look-ahead*/
/************************************************************************/
/* 26-03-87 11:02 (pb) :	Ajout du parametre what	        	*/
/************************************************************************/

#include "sxunix.h"

#ifndef VARIANT_32
char WHAT_SXS_SRECOVERY[] = "@(#)SYNTAX - $Id: sxs_srcvr.c 1040 2008-02-15 08:40:05Z rlacroix $" WHAT_DEBUG;
#else
char WHAT_SXS_SRECOVERY[] = "@(#)SYNTAX - $Id: sxs_srcvr.c 1040 2008-02-15 08:40:05Z rlacroix $ SXS_SRCVR_32" WHAT_DEBUG;
#endif
#define char_to_class(c) (sxsvar.SXS_tables.S_char_to_simple_class[c])




static BOOLEAN	recovery (SXINT state_no, unsigned char *class)
{
    BOOLEAN	is_error_in_la;

    if (!(is_error_in_la = sxsvar.SXS_tables.S_transition_matrix [state_no] [1]) &&
	!sxsvar.sxlv.mode.is_silent) {
	char	*s;

	switch (sxsrcmngr.current_char) {
	case EOF:
	    s = sxsvar.SXS_tables.S_global_mess [4];
	    break;

	case NEWLINE:
	    s = sxsvar.SXS_tables.S_global_mess [3];
	    break;

	default:
	    s = sxc_to_str [sxsrcmngr.current_char];
	    break;
	}

	sxput_error (sxsrcmngr.source_coord, sxsvar.SXS_tables.S_global_mess [1], sxsvar.sxtables->err_titles [2], s);
    }

    if (sxsrcmngr.current_char == EOF) {
	sxsvar.lv.terminal_token.lahead = sxsvar.SXS_tables.S_termax /* End Of File */ ;
	*class = 2;
	return FALSE;
    }
    else {
	*class = char_to_class (is_error_in_la ? sxlanext_char () : sxnext_char ());
	return TRUE;
    }
}



BOOLEAN		sxssrecovery (SXINT sxssrecovery_what, SXINT state_no, unsigned char *class)
{
    switch (sxssrecovery_what) {
    case ACTION:
	return recovery (state_no, class);

    case CLOSE:
	break;

    default:
	fprintf (sxstderr, "The function \"sxssrecovery\" is out of date with respect to its specification.\n");
	sxexit(1);
    }

    return TRUE;
}
