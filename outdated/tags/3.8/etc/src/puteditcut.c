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


VOID	put_edit_cut_point (string, cut_chars, max_cut_point, xcut, is_ok)
    char	*string, *cut_chars;
    int		max_cut_point, *xcut;
    BOOLEAN	*is_ok;
{
    register char	*s;
    static int	l;


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
    else
	*xcut = s - string + 1;

    if (*xcut == 0) {
	if (max_cut_point >= l)
	    *xcut = l;
	else {
	    for (s = string + max_cut_point - 1; s != string; s--) {
		if (sxindex (cut_chars, *s) != NULL) {
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
