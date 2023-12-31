/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2003 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */

/* Actions du scanner associe a l'analyseur de grammaires LFG definissant
   la partie lexicale et qui permet le traitement des includes */


/* ********************************************************
   *                                                      *
   *          Produit de l'equipe ATOLL.                  *
   *                                                      *
   ******************************************************** */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 03-11-03 12:45 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#if 0
static char	ME [] = "lfg_lex_scanact";
#endif

#include "sxunix.h"
char WHAT_LFGLEXSCANACT[] = "@(#)SYNTAX - $Id: lfg_lex_scanact.c 579 2007-05-24 11:30:48Z rlacroix $" WHAT_DEBUG;

void
lfg_lex_scanact (entry, act_no)
     int entry, act_no;
{
  switch (entry) {
    case OPEN:
    case CLOSE:
    case INIT:
    case FINAL:
      sxincl_mngr (entry);
      return;

    case ACTION:
      switch (act_no) {
      case 1:
	/* The pathname of the include file is in token_string */
	if (sxpush_incl (sxsvar.sxlv_s.token_string))
	  /* Now, the source text comes from the include file ... */
	  return;

	/* something failed (unable to open, recursive call, ...) */
	/* Error message */
	sxput_error (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the include command */,
		     "%sUnable to process the include file \"%s\".",
		     sxsvar.sxtables->err_titles [2] /* Severity level: Error */,
		     sxsvar.sxlv_s.token_string /* Include file name */
		     );
	/* However, scanning of the current files goes on ... */
	return;

      case 2:
	/* End of include processing */
	if (sxpop_incl ())
	  return;

	/* Something really goes wrong ... */
	/* Error message */
	fputs ("Sorry, the include processing garbled, nevertheless hope to see you again soon.\n", sxstderr);
	sxexit (3) /* panic */;

      case 3:
	/* "|" = "|" | -"/" @3 ; -- @3 => warning, "old style" */
	if (sxsvar.sxlv.ts_lgth == 0) {
	  /* cas "/" */
	  sxput_error (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the "|" operator */,
		       "%sOld style, use \"|\" instead.",
		       sxsvar.sxtables->err_titles [1] /* Severity level: Warning */
		       );
	}
	
	return;

      case 4:
	/* CONTRAINT	= -"=" -"c"&true (-BLANK&true | ANY @4 @Release) ; --	@4 => Error constraint assumed */
	/* Error message */
	sxput_error (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the constraint operator */,
		     "%sA blank character is mandatory after the \"=c\" operator.",
		     sxsvar.sxtables->err_titles [2] /* Severity level: Error */
		     );
	/* However, scanning will restart on the non blank character ... */
	
	return;

      default:
	break;
      }

    default :
      fputs ("The function \"lfg_lex_scanact\" is out of date w.r.t. its specification.\n", sxstderr);
      sxexit (3) /* panic */;
  }
}
