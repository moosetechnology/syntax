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

static char	ME [] = "Watson_Crick.c";

#include "sxunix.h"
#include "rcg_glbl.h"

char WHAT_RCGTESTWATSON[] = "@(#)SYNTAX - $Id: Watson_Crick.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

static SXINT Watson_Crick [5][5]= {
    /*	0	A	C	G	U	*/	         
/* 0 */{0,	0,	0,	0,	0},
/* A */{0,	0,	0,	0,	2},
/* C */{0,	0,	0,	4,	0},
/* G */{0,	0,	5,	0,	1},
/* U */{0,	3,	0,	0,	0}
};


bool
_wc (rho0, ilb, iub)
    SXINT *rho0, *ilb, *iub;
{
    return Watson_Crick [SOURCE(iub[0])][SOURCE(iub[1])];
}

