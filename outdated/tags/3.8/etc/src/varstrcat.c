#include "varstr.h"
#include "sxunix.h"


void vncpy (s1, s2, l2)
    register char	*s1, *s2;
    register int	l2;
{
    /* Copie la chaine s2 de longueur l2 dans la chaine s1.
       La taille de s1 doit etre suffisante.
       Marche meme si s1 et s2 se chevauchent.
       Les NULs ne sont pas traites, ca permet de copier des
       sous-chaines. */
#define min_slice	4
    register int	delta;


    if (s1 == s2 || l2 == 0)
	return;

    if (s1+l2 <= s2 || s2+l2 <= s1) {
	strncpy (s1, s2, l2);
	return;
    }

    /* taille du non recouvrement */
    delta = s1 < s2 ? s2 - s1 : s1 - s2;

    if (delta > min_slice) {
	/* On copie par tranche */
	if (s1 > s2) {
	  /* On copie depuis la fin */ 
	    s1 += l2 - delta;
	    s2 += l2 - delta;
	}

	do {
	    strncpy (s1, s2, delta);

	    if (s1 > s2)
		s1 -= delta, s2 -= delta;
	    else
		s1 += delta, s2 += delta;
	} while ((l2 -= delta) >= delta);

	if (l2 > 0) {
	    if (s1 > s2)
		s1 += delta - l2, s2 += delta - l2;

	    strncpy (s1, s2, l2);
	}
    }
    else {
	/* On copie caractere par caractere */
	if (s1 > s2) {
	  /* On copie depuis la fin */ 
	    s1 += l2-1;
	    s2 += l2-1;
	}

	do {
	    *s1 = *s2;

	    if (s1 > s2)
		s1--, s2--;
	    else
		s1++, s2++;
	} while (--l2 > 0);
    }
}



VARSTR varstr_modify (varstr_ptr, i, l, s, sl)
    register VARSTR	varstr_ptr;
    int			i, l, sl;
    char		*s;
{
    /* Remplace ds varstr_ptr la sous chaine d'indice i et de longueur l
       par la chaine s de longueur sl. */
    /* Suppose que s n'est pas une sous-chaine de varstr_ptr. */
    register int delta;

    if (l != sl) {
	/* On definit le suffixe */
	if ((delta = varstr_length (varstr_ptr) - i - l) > 0) {
	    /* Non vide */
	    if (sl > l) {
		/* la taille augmente, on s'arrange pour que ca tienne. */
		while (varstr_maxlength (varstr_ptr) <= i + sl + delta)
		   varstr_realloc (varstr_ptr); 
	    }

	    /* On bouge le suffixe */
	    vncpy (varstr_tostr (varstr_ptr) + i + sl, varstr_tostr (varstr_ptr) + i + l, delta);
	    varstr_complete (varstr_ptr);
	}
    }

    /* On copie s */
    if (sl != 0)
	strncpy (varstr_tostr (varstr_ptr) + i, s, sl);

    return varstr_ptr;
}



VARSTR	varstr_realloc (varstr_ptr)
    register VARSTR	varstr_ptr;
{
    register int	old_size, new_size, old_current;

    old_size = varstr_ptr->last - varstr_ptr->first;
    old_current = varstr_ptr->current - varstr_ptr->first;
    varstr_ptr->first = sxrealloc (varstr_ptr->first, new_size = (old_size << 1), sizeof (char));
    varstr_ptr->current = varstr_ptr->first + old_current;
    varstr_ptr->last = varstr_ptr->first + new_size;
    return varstr_ptr;
}



VARSTR	varstr_lcatenate (varstr_ptr, s, l)
    register VARSTR	varstr_ptr;
    char		*s;
    register int	l;

/* Concatene s sur une longueur l a varstr_ptr */
{
    if (l != 0) {
	while (l >= varstr_ptr->last - varstr_ptr->current)
	    varstr_ptr = varstr_realloc (varstr_ptr);

	strncpy (varstr_ptr->current, s, l);
	*(varstr_ptr->current += l) = NUL;
    }

    return varstr_ptr;
}




VARSTR	varstr_catenate (varstr_ptr, s)
    register VARSTR	varstr_ptr;
    char		*s;

/* On suppose qu'il y a toujours au moins un element de libre. Le premier
    caractere de s ecrase le NUL de fin de varstr, le NUL terminant s est
    recopie et pointe par current */

{
    register int l = strlen (s);

    if (l != 0) {
	while (l >= varstr_ptr->last - varstr_ptr->current)
	    varstr_ptr = varstr_realloc (varstr_ptr);

	strcpy (varstr_ptr->current, s);
	varstr_ptr->current += l;
    }

    return varstr_ptr;
}




VARSTR	varstr_quote (varstr_ptr, s)
    register VARSTR	varstr_ptr;
    register char	*s;
{
    register char	c, *s1;

    varstr_catchar (varstr_ptr, '\"');

    while (c = *s++) {
	s1 = CHAR_TO_STRING (c);

	while (c = *s1++)
	    varstr_catchar (varstr_ptr, c);
    }

    varstr_catchar (varstr_ptr, '\"');
    *(varstr_ptr->current) = NUL;
    return varstr_ptr;
}



VARSTR	varstr_lcat_cstring (varstr_ptr, s, l)
    register VARSTR	varstr_ptr;
    register char	*s;
    register int	l;

{
    register char	c, *s1;

    if (l == 0)
	return varstr_ptr;

    if (*s != '\"' || l <= 2 || s [l - 1] != '\"') {
	do {
	    varstr_catchar (varstr_ptr, *s++);
	} while (--l > 0);
    }
    else {
	varstr_catchar (varstr_ptr, '\"');
	l -= 2;

	do {
	    s1 = CHAR_TO_STRING (*++s);

	    while (c = *s1++)
		varstr_catchar (varstr_ptr, c);
	} while (--l > 0);

	varstr_catchar (varstr_ptr, '\"');
    }

    *(varstr_ptr->current) = NUL;
    return varstr_ptr;
}

VARSTR	varstr_cat_cstring (varstr_ptr, s)
    register VARSTR	varstr_ptr;
    register char	*s;
{
    return varstr_lcat_cstring (varstr_ptr, s, strlen (s));
}

