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
#include "put_edit.h"

char WHAT_PUTEDITCUT[] = "@(#)SYNTAX - $Id: puteditcut.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

void	put_edit_cut_point (char *string, char *cut_chars, size_t max_cut_point, size_t *xcut, bool *is_ok)
{
    char	*s;
    static size_t	l;


/*
Cette procedure recherche dans la chaine substr(string,1,max_cut_point)
   - s'il existe un NL, => xcut=index du 1er NL - 1; is_ok=true
   - sinsi length(string) < max_cut_point => xcut=length(string); is_ok=true
   - sinsi (xcut=index du caractere precedant le dernier caractere de la
	         chaine appartenant a cut_chars); is_ok=true
   - sinon xcut=max_cut_point;is_ok=false
*/

    l = strlen (string);
    *is_ok = true;
    s = sxindex (string, SXNEWLINE);

    if (s == NULL || s >= (string + max_cut_point))
	*xcut = 0;
    else  /* LINTED s et string sont deux pointeurs qui reperent des elements du meme tableau : string[] */      
        *xcut = s - string + 1;

    if (*xcut == 0) {
	if (max_cut_point >= l)
	    *xcut = l;
	else {
	    for (s = string + max_cut_point - 1; s != string; s--) {
		if (sxindex (cut_chars, *s) != NULL) {  
		    /* LINTED s et string sont deux pointeurs qui reperent des elements du meme tableau : string[] */
		    *xcut = s - string;
		    return;
		}
	    }

	    *xcut = max_cut_point;
	    *is_ok = false;
	}
    }
    else
	--*xcut;
}
