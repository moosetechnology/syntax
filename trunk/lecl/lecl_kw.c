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

/* ********************************************************************
   *  This program has been gracefully generated  for		      *
   *                        Michel LOYER			      *
   *  from lecl.lecl						      *
   *  on Mon May 18 13:47:52 1987				      *
   *  by the SYNTAX (*) lexical constructor LECL                      *
   ********************************************************************
   *  (*) SYNTAX is a trademark of INRIA.                             *
   ******************************************************************** */

#include "sxcommon.h"
char WHAT_LECLKW[] = "@(#)SYNTAX - $Id: lecl_kw.c 3603 2023-09-23 20:02:36Z garavel $" WHAT_DEBUG;

static SXINT hash_to_code [/* 50 */] = {
12, 47, 22, 49, 0, 40, 43, 64, 18, 29, 
66, 48, 41, 0, 31, 42, 51, 62, 18, 33, 
20, 21, 0, 68, 0, 30, 63, 19, 67, 50, 
25, 61, 0, 52, 45, 32, 53, 44, 27, 54, 
59, 60, 58, 0, 0, 65, 56, 57, 1, 16, 
};

static SXINT lecl_keyword (string, length)
char *string;
SXINT length;
{
SXUINT scramble;

#include "sxscrmbl_1.h"

switch (scramble & (SXINT) 31) {
case 2:
return (((SXINT) 2146372470 / scramble) & 1) == 0 ? 12 : 47;
case 4:
return (((SXINT) 2143833440 / scramble) & 1) == 0 ? 22 : 49;
case 5:
return (hash_to_code + 4) [((SXINT) 2145649221 / scramble) & 3];
case 6:
return 18;
case 8:
return 29;
case 9:
return (((SXINT) 2144710567 / scramble) & 1) == 0 ? 66 : 48;
case 12:
return (hash_to_code + 12) [((SXINT) 2135406627 / scramble) & 3];
case 13:
return 51;
case 14:
return (hash_to_code + 17) [((SXINT) 2132045837 / scramble) & 7];
case 15:
return (hash_to_code + 25) [((SXINT) 2131590429 / scramble) & 3];
case 16:
return 50;
case 18:
return (hash_to_code + 30) [((SXINT) 2146487112 / scramble) & 3];
case 19:
return 45;
case 20:
return (((SXINT) 2141988780 / scramble) & 1) == 0 ? 32 : 53;
case 21:
return (((SXINT) 2146612779 / scramble) & 1) == 0 ? 44 : 27;
case 22:
return 54;
case 25:
return (hash_to_code + 40) [((SXINT) 2106348248 / scramble) & 7];
case 28:
return 1;
case 31:
return 16;
default :
return 0;
}
