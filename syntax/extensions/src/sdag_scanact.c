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
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 



/* Actions du scanner communes a la construction des dicos et a la lecture des
   formes flechies decorees */




static char	ME [] = "sdag_scanact.c";

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SDAG_SCANACT[] = "@(#)SYNTAX - $Id: sdag_scanact.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;


static SXVOID	gripe (void)
{
    fputs ("\nA function of \"sdag\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

SXINT
sdag_scanact (SXINT entry, SXINT act_no)
{
  switch (entry) {
  case SXOPEN:
  case SXCLOSE:
  case SXINIT:
  case SXFINAL:
    return 0;


  case SXACTION:
    switch (act_no) {
    case 2:
      /* %SEMLEX = -"[" -"|" {ANY | "\"&SXTRUE ANY"}* "|" -"]"&SXTRUE ; @2 ; */
      /* enleve le "|" final (avec des @Mark et @Release on a 2 car de look-ahead) puis @1 */
      sxsvar.sxlv_s.token_string [--sxsvar.sxlv.ts_lgth] = SXNUL;

    case 1:
#if is_parse_forest
      sxsvar.sxlv_s.token_string [sxsvar.sxlv.ts_lgth] = SXNUL;

      if (sxsvar.sxlv.terminal_token.source_index.column == 1)
	fputs ("%% ", stdout);

      printf ("%s", sxsvar.sxlv_s.token_string);
#endif
      return 0;

    default:
      break;
    }

  case SXPREDICATE:
    switch (act_no) {
    case 1:
      return sxsvar.sxlv_s.counters [0] == 2;

    default:
      break;
    }
  default:
    gripe ();
  }
}
