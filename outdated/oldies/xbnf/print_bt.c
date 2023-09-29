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

#define SX_DFN_EXT_VAR_XBNF

#include "sxunix.h"
#include "B_tables.h"
#include "xbnf_vars.h"
char WHAT_XBNFPRINTBT[] = "@(#)SYNTAX - $Id: print_bt.c 3498 2023-08-20 18:14:09Z garavel $" WHAT_DEBUG;

extern SXVOID xbnf_dump (void);

/************************************************************************/
/* main function */
/************************************************************************/
int main (int argc, char *argv[])
{
  char	*name;

  sxopentty ();

  sxuse(argc); /* pour faire taire gcc -Wunused */

  name = argv [1];
  printf ("GRAMMAIRE:%s\n", name);

  if (bnf_read (&bnf_ag, name)) {
    xbnf_dump ();
    bnf_free (&bnf_ag);
  }

  return EXIT_SUCCESS;
}
