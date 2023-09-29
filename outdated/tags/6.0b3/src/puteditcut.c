/* ********************************************************
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   ******************************************************** */



/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */


#include "sxunix.h"
#include "put_edit.h"

char WHAT_PUTEDITCUT[] = "@(#)SYNTAX - $Id: puteditcut.c 938 2008-01-17 15:00:37Z rlacroix $" WHAT_DEBUG;

VOID	put_edit_cut_point (char *string, char *cut_chars, size_t max_cut_point, size_t *xcut, BOOLEAN *is_ok)
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
    *is_ok = TRUE;
    s = sxindex (string, NEWLINE);

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
	    *is_ok = FALSE;
	}
    }
    else
	--*xcut;
}
