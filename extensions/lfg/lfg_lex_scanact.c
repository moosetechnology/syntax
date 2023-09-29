/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/


/* Actions du scanner associe a l'analyseur de grammaires LFG definissant
   la partie lexicale et qui permet le traitement des includes */


#if 0
static char	ME [] = "lfg_lex_scanact";
#endif

#include "sxunix.h"
char WHAT_LFGLEXSCANACT[] = "@(#)SYNTAX - $Id: lfg_lex_scanact.c 3352 2023-06-12 11:52:07Z garavel $" WHAT_DEBUG;

void
lfg_lex_scanact (SXINT entry, SXINT act_no)
{
  switch (entry) {
    case SXOPEN:
    case SXCLOSE:
    case SXINIT:
    case SXFINAL:
      sxincl_mngr (entry);
      return;

    case SXACTION:
      switch (act_no) {
      case 1:
	/* The pathname of the include file is in token_string */
	if (sxpush_incl (sxsvar.sxlv_s.token_string))
	  /* Now, the source text comes from the include file ... */
	  return;

	/* something failed (unable to open, recursive call, ...) */
	/* Error message */
	sxerror (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the include command */,
		 sxsvar.sxtables->err_titles [2][0] /* Severity level: Error */,
		 "%sUnable to process the include file \"%s\".",
		 sxsvar.sxtables->err_titles [2]+1 /* Severity level: Error */,
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
	  sxerror (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the "|" operator */,
		   sxsvar.sxtables->err_titles [1][0] /* Severity level: Warning */,
		   "%sOld style, use \"|\" instead.",
		   sxsvar.sxtables->err_titles [1]+1 /* Severity level: Warning */
		   );
	}
	
	return;

      case 4:
	/* CONTRAINT	= -"=" -"c"&true (-BLANK&true | ANY @4 @Release) ; --	@4 => Error constraint assumed */
	/* Error message */
	sxerror (sxsvar.sxlv.terminal_token.source_index /* Source coordinates of the constraint operator */,
		 sxsvar.sxtables->err_titles [2][0] /* Severity level: Error */,
		 "%sA blank character is mandatory after the \"=c\" operator.",
		 sxsvar.sxtables->err_titles [2]+1 /* Severity level: Error */
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
