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

char WHAT_TABLESOUTLIGTBL[] = "@(#)SYNTAX - $Id: out_lig_tbl.c 3147 2023-05-02 13:42:48Z garavel $" WHAT_DEBUG;


/* This is a restricted version in which out_lig_tables() does nothing. The 
 * original version of out_lig_tbl.c can be found in extensions/tables; it is 
 * only useful for processing LIG grammars
 */ 

bool out_lig_tables (char *ln)
{
    sxuse (ln);
    return 0;
}
