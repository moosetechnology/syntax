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

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"

char WHAT_BNFTERMINAL[] = "@(#)SYNTAX - $Id: bnf_terminal.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

static void
output_header (time_t bnf_modif_time, char *langname)
{
  time_t cur_time;

  cur_time = time (0);  /* now */

  printf ("\
/* ***************************************************************************************\n\
\tThis terminal symbol list has been extracted on %s\
\tby the SYNTAX(*) BNF processor running with the \"-terminal\" option\n\
\tfrom the CFG \"%s\" in BNF form last modified on %s\
******************************************************************************************\n\
\t(*) SYNTAX is a trademark of INRIA.\n\
****************************************************************************************** */\n\n\n",
	  ctime (&cur_time),
	  (langname == NULL) ? "stdin" : langname,
	  ctime (&bnf_modif_time));
}



static void
escape_printf (char *str)
{
  while (*str != SXNUL) {
    if (*str == '\\' || *str == '"')
      printf ("\\%c",*str);
    else
      printf ("%c",*str);

    str++;
  }
}


#define get_t_string(t)		(bnf_ag.T_STRING + bnf_ag.ADR [t])

void
bnf_terminal (struct bnf_ag_item *B, char *langname)
{
  SXINT    t;

  if (sxverbosep)
    fputs ("   Terminal Symbol List Output\n", sxtty);

  output_header (B->bnf_modif_time, langname);

  for (t = 1; t < -W.tmax; t++) {
    if (!is_col_1)
      /* On ne genere pas le terminal en colonne 1 */
      fputs ("\t", stdout);
      
    fputs ("\"", stdout);
    escape_printf (get_t_string (-t));
    printf ("\"\t/* %ld */\n", t);
  }
}
