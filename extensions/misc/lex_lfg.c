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

/* Actions semantiques construisant la f_structure d'une analyse LFG */
/* L'analyseur est de type Earley */
/* Le source est sous forme de DAG */


static char	ME [] = "lex_lfg";

#include "sxversion.h"
#include "sxcommon.h"
#include lex_lfg_h

char WHAT_LEX_LFG[] = "@(#)SYNTAX - $Id: lex_lfg.c 3946 2024-05-02 07:47:07Z garavel $" WHAT_DEBUG;
