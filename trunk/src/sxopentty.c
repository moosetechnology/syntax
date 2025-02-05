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

/* Ouverture du fichier "sxtty" pour le mode "verbose" */

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXOPENTTY[] = "@(#)SYNTAX - $Id: sxopentty.c 3632 2023-12-20 17:58:08Z garavel $" WHAT_DEBUG;

void sxopentty (void)
{
  if (sxstdout == NULL) {
    /* variable non encore initialisee par l'utilisateur */
    sxstdout = stdout;
  }

  if (sxstderr == NULL) {
    /* variable non encore initialisee par l'utilisateur */
    sxstderr = stderr;
  }

  if (sxtty == NULL) {
    if ((sxtty = fopen (SX_DEV_TTY, "a")) == NULL || !isatty (fileno (sxtty))) {
      sxtty = fopen (SX_DEV_NUL, "w") ; 
      /* if (sxtty==NULL) sxverbosep=false; */
    } else {
#if O_TEXT
      setmode (fileno (sxtty), O_TEXT);
#endif
      setbuf (sxtty, NULL);
    }
  }

#if BUG
    /* Suppress bufferisation, in order to have proper   */
    /* messages when something goes wrong...             */
    setbuf (stdout, NULL);
#endif
}
