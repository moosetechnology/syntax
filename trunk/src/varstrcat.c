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
#include "varstr.h"
#include "sxunix.h"
#include <string.h>

char WHAT_VARSTRCAT[] = "@(#)SYNTAX - $Id: varstrcat.c 3275 2023-05-20 05:49:04Z garavel $" WHAT_DEBUG;

void vncpy (char *s1, char *s2, SXINT l2)
{
    /* Copie la chaine s2 de longueur l2 dans la chaine s1.
       La taille de s1 doit etre suffisante.
       Marche meme si s1 et s2 se chevauchent.
       Les NULs ne sont pas traites, ca permet de copier des
       sous-chaines. */
#define min_slice	4
    SXINT	delta;


    if (s1 == s2 || l2 == 0)
	return;

    if (s1+l2 <= s2 || s2+l2 <= s1) {
	strncpy (s1, s2, (size_t)l2);
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
	    strncpy (s1, s2, (size_t)delta);

	    if (s1 > s2)
		s1 -= delta, s2 -= delta;
	    else
		s1 += delta, s2 += delta;
	} while ((l2 -= delta) >= delta);

	if (l2 > 0) {
	    if (s1 > s2)
		s1 += delta - l2, s2 += delta - l2;

	    strncpy (s1, s2, (size_t)l2);
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



VARSTR varstr_modify (VARSTR varstr_ptr, SXINT i, SXINT l, char *s, SXINT sl)
{
    /* Remplace ds varstr_ptr la sous chaine d'indice i et de longueur l
       par la chaine s de longueur sl. */
    /* Suppose que s n'est pas une sous-chaine de varstr_ptr. */
    SXINT delta;

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
	    (void) varstr_complete (varstr_ptr);
	}
    }

    /* On copie s */
    if (sl != 0)
	strncpy (varstr_tostr (varstr_ptr) + i, s, (size_t)sl);

    return varstr_ptr;
}



VARSTR	varstr_realloc (VARSTR varstr_ptr)
{
    SXINT	old_size, new_size, old_current;

    old_size = varstr_ptr->last - varstr_ptr->first;
    old_current = varstr_ptr->current - varstr_ptr->first;
    varstr_ptr->first = sxrealloc (varstr_ptr->first, new_size = (old_size << 1), sizeof (char));
    varstr_ptr->current = varstr_ptr->first + old_current;
    varstr_ptr->last = varstr_ptr->first + new_size;
    return varstr_ptr;
}



VARSTR	varstr_lcatenate (VARSTR varstr_ptr, char *s, SXUINT l)
/* Concatene s sur une longueur l a varstr_ptr */
{
    if (l != 0) {
	while (l >= (SXUINT)(varstr_ptr->last - varstr_ptr->current))
	    varstr_ptr = varstr_realloc (varstr_ptr);

	strncpy (varstr_ptr->current, s, (size_t)l);
	*(varstr_ptr->current += l) = SXNUL;
    }

    return varstr_ptr;
}

VARSTR	varstr_catenate (VARSTR varstr_ptr, char *s)
{
     return varstr_lcatenate (varstr_ptr, s, strlen (s));
}

/* concatene le reverse de la chaine s de longueur l (s peut ne pas etre termine' par SXNUL) */
VARSTR	varstr_rcatenate (VARSTR varstr_ptr, char *s, SXINT l)
    /* Si l est nul, on ne touche a rien */
    /* On suppose qu'il y a toujours au moins un element de libre. Le premier
       caractere de s ecrase le SXNUL eventuel de fin de varstr, un SXNUL est ajoute' en fin et pointe par current */
{
  char *sptr;

  if (l != 0) {
    while (l >= varstr_ptr->last - varstr_ptr->current)
      varstr_ptr = varstr_realloc (varstr_ptr);

    sptr = s+l;

    while (--sptr >= s)
      *(varstr_ptr->current++) = *sptr;

    varstr_ptr = varstr_complete (varstr_ptr); /* On met le SXNUL */
  }

  return varstr_ptr;
}




VARSTR	varstr_quote (VARSTR varstr_ptr, char *s)
{
    char	c, *s1;

    varstr_catchar (varstr_ptr, '\"');

    while ((c = *s++)) {
	s1 = SXCHAR_TO_STRING (c);

	while ((c = *s1++))
	    varstr_catchar (varstr_ptr, c);
    }

    varstr_catchar (varstr_ptr, '\"');
    *(varstr_ptr->current) = SXNUL;
    return varstr_ptr;
}



VARSTR	varstr_lcat_cstring (VARSTR varstr_ptr, char *s, SXUINT l)
{
    char	c, *s1;

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
	    s1 = SXCHAR_TO_STRING (*++s);

	    while ((c = *s1++))
		varstr_catchar (varstr_ptr, c);
	} while (--l > 0);

	varstr_catchar (varstr_ptr, '\"');
    }

    *(varstr_ptr->current) = SXNUL;
    return varstr_ptr;
}

VARSTR	varstr_cat_cstring (VARSTR varstr_ptr, char *s)
{
    return varstr_lcat_cstring (varstr_ptr, s, strlen (s));
}

/* Les caracteres de escape_string sont echappes par un \ */
VARSTR
varstr_lcatenate_with_escape (VARSTR varstr_ptr, char *string, SXUINT string_lgth, char *escape_string)
{
  SXUINT l;
  SXINT  offset;
  char   cur_char, *cur_str;

  /*
    SYNOPSIS
    #include <string.h>
    size_t strcspn(const char *s, const char *reject);

    DESCRIPTION
    The strcspn() function calculates the length of the initial segment  of
    s which consists entirely of characters not in reject.

    RETURN VALUE
    The strcspn() function returns the number of characters in the  initial
    segment of s which are not in the string reject.
  */

  offset = varstr_length (varstr_ptr);
  (void) offset; /* pour faire taire gcc -Wall */

  cur_str = varstr_ptr->current;

  while (string_lgth > 0) {
    varstr_ptr = varstr_lcatenate (varstr_ptr, string, string_lgth);
    cur_str = varstr_ptr->current - string_lgth;

    if ((l = strcspn (cur_str, escape_string))) {
      cur_str += l;
      string += l;
      string_lgth -= l;
    }

    if (string_lgth > 0) {
      cur_char = *cur_str;
      varstr_ptr->current = cur_str;
      varstr_ptr = varstr_catchar (varstr_ptr, '\\');
      varstr_ptr = varstr_catchar (varstr_ptr, cur_char);      
      (void) varstr_complete (varstr_ptr);
      string++;
      string_lgth --;
      cur_str += 2;
    }
  }

  return varstr_complete (varstr_ptr); /* au cas ou */
}

VARSTR
varstr_lcatenate_with_escape_except_for_comments (VARSTR varstr_ptr, char *string, SXUINT string_lgth, char *escape_string)
{
  SXUINT l;
  SXINT  offset;
  char   cur_char, *cur_str;

  offset = varstr_length (varstr_ptr);
  (void) offset; /* pour faire taire gcc -Wall */

  cur_str = varstr_ptr->current;

  varstr_ptr = varstr_lcatenate (varstr_ptr, string, string_lgth);
  cur_str = varstr_ptr->current - string_lgth;

  if ((l = strcspn (cur_str, "{"))) {
    cur_str += l;
    string += l;
    string_lgth -= l;
  }
  
  varstr_ptr->current = cur_str;
  
  if ((l = strcspn (cur_str, "}"))) {
    cur_str += l;
    string += l;
    string_lgth -= l;
  }

  varstr_ptr->current = cur_str;

  while (string_lgth > 0) {
    varstr_ptr = varstr_lcatenate (varstr_ptr, string, string_lgth);
    cur_str = varstr_ptr->current - string_lgth;

    if ((l = strcspn (cur_str, escape_string))) {
      cur_str += l;
      string += l;
      string_lgth -= l;
    }

    if (string_lgth > 0) {
      cur_char = *cur_str;
      varstr_ptr->current = cur_str;
      varstr_ptr = varstr_catchar (varstr_ptr, '\\');
      varstr_ptr = varstr_catchar (varstr_ptr, cur_char);      
      (void) varstr_complete (varstr_ptr);
      string++;
      string_lgth --;
      cur_str += 2;
    }
  }

  return varstr_complete (varstr_ptr); /* au cas ou */
}
