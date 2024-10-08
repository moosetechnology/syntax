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

#define SX_DFN_EXT_VAR_BNF	/* Pour definir les variables globales du module BNF */

#include "sxversion.h"
#include "sxunix.h"
#include "B_tables.h"
#include "bnf_vars.h"

char WHAT_PRINTBT[] = "@(#)SYNTAX - $Id: print_bt.c 3621 2023-12-17 11:11:31Z garavel $" WHAT_DEBUG;

extern void bnf_dump (void);

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  char	*name;

  sxuse(argc); /* pour faire taire gcc -Wunused */
  sxopentty ();

  name = argv [1];
  printf ("GRAMMAIRE:%s\n", name);

  if (bnf_read (&bnf_ag, name)) {
    bnf_dump ();
    bnf_free (&bnf_ag);
  }

  return EXIT_SUCCESS;
}
