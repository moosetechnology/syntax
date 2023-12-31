/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */

/* Actions du scanner communes a la construction des dicos et a la lecture des
   formes flechies decorees */

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* Jeu 15 Jun 2000 16:09(PB)	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



static char	ME [] = "sdag_scanact.c";

#include "sxunix.h"

char WHAT_SDAG_SCANACT[] = "@(#)SYNTAX - $Id: sdag_scanact.c 1006 2008-02-12 09:08:05Z rlacroix $" WHAT_DEBUG;


static VOID	gripe (void)
{
    fputs ("\nA function of \"sdag\" is out of date with respect to its specification.\n", sxstderr);
    sxexit(1);
}

SXINT
sdag_scanact (SXINT entry, SXINT act_no)
{
  switch (entry) {
  case OPEN:
  case CLOSE:
  case INIT:
  case FINAL:
    return 0;


  case ACTION:
    switch (act_no) {
    case 2:
      /* %SEMLEX = -"[" -"|" {ANY | "\"&TRUE ANY"}* "|" -"]"&TRUE ; @2 ; */
      /* enleve le "|" final (avec des @Mark et @Release on a 2 car de look-ahead) puis @1 */
      sxsvar.lv_s.token_string [--sxsvar.lv.ts_lgth] = NUL;

    case 1:
#if is_parse_forest
      sxsvar.lv_s.token_string [sxsvar.lv.ts_lgth] = NUL;

      if (sxsvar.lv.terminal_token.source_index.column == 1)
	fputs ("%% ", stdout);

      printf ("%s", sxsvar.lv_s.token_string);
#endif
      return 0;

    default:
      break;
    }

  case PREDICATE:
    switch (act_no) {
    case 1:
      return sxsvar.lv_s.counters [0] == 2;

    default:
      break;
    }
  default:
    gripe ();
  }
}
