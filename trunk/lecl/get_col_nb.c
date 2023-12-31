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

char WHAT_LECLGETCOLNB[] = "@(#)SYNTAX - $Id: get_col_nb.c 3603 2023-09-23 20:02:36Z garavel $" WHAT_DEBUG;

SXINT	get_column_number (char *string, SXINT index)
{
    /* Cette procedure retourne la colonne d'affichage du caractere d'index 
   "index" de la chaine "string". On suppose que les positions de tabulation
   sont tous les "tabs" (1, tabs+1, 2*tabs+1, ...) et que le debut de la chaine
   s'affiche en colonne 1 */

#define tabs 8
#define BS 8
#define HT 9
    char	*s, *x;
    SXINT		col;

    x = string + (index - 1);
    col = 0;

    for (s = string; s < x; s++)
	if (*s == HT)
	    col += tabs - (col % tabs);
	else if (*s == BS)
	    col--;
	else
	    col++;

    return col;
}
