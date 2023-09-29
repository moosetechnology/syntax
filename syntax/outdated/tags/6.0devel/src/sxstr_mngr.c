/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/

/* Sauvegarde des chaines de caracteres */

#include "sxversion.h"
#include "sxunix.h"
#include <stdarg.h>

char WHAT_SXSTR_MNGR[] = "@(#)SYNTAX - $Id: sxstr_mngr.c 2326 2021-06-07 13:24:09Z garavel $" WHAT_DEBUG;

/*---------------------------------------------------------------------------*/

#include "sxscrmbl.h"

#if (SXHASH_LENGTH % 2) == 0 || (SXHASH_LENGTH % 3) == 0 || (SXHASH_LENGTH % 5) == 0 || (SXHASH_LENGTH % 7) == 0 || (SXHASH_LENGTH % 11) == 0 || (SXHASH_LENGTH % 13) == 0
#error SXHASH_LENGTH should not be divisible by a small prime.  You might use one of {257,521,1031,2053,4099,8209,16411,32771,65539,131101,262217,524309,1048583,2097169} instead
#endif

/*---------------------------------------------------------------------------*/

/* Increment for the string reallocation */

#ifndef	BUFSIZ
#define	BUFSIZ	512
#endif	/* BUFSIZ */

/*---------------------------------------------------------------------------*/

/* Compare for equality strings s1 and s2 on their first n characters. */
/* The strings may contain '\0's and may not be terminated by a '\0'. */

static SXINLINE SXBOOLEAN sxstrequal (char *s1, char *s2, SXUINT n)
{
    do
	if (n-- == 0)
	    return SXTRUE;
    while (*s1++ == *s2++);

    return SXFALSE;
}

/*---------------------------------------------------------------------------*/

/* Put in s1 first (n-1) chars of s2 and terminate by a '\0' */

static SXINLINE SXVOID sxstrput (char *s1, char *s2, SXUINT n)
{
    while (--n != 0)
	*s1++ = *s2++;

    *s1 = SXNUL;
}

/*---------------------------------------------------------------------------*/

/* Enter into strtable a string of length strlength for entry top++ */

static SXINT sxstr_add (sxstrmngr_t *strtable, char *string, SXUINT strlength)
{
    SXUINT	length = strlength;
    SXINT	ste;

    if (strtable->strnext + ++length >= strtable->strlength) {
	sxstrstring_t	*old;

	old = strtable->string;

#ifdef SXRESIZE
	SXRESIZE (old, 1, sizeof (sxstrstring_t*) + (strtable->strnext - 1) * sizeof (char));
	strtable->strlength += (length & -sizeof (char*)) + BUFSIZ;
#else
	/* This might be a buddy system allocator; find a power of 2 */

	if (strtable->strlength < length) {
	    SXUINT	min = length + sizeof (sxstrstring_t*) / sizeof (char);

	    strtable->strlength += sizeof (sxstrstring_t*) / sizeof (char);

	    do {
	    } while ((strtable->strlength *= 2) < min);

	    strtable->strlength -= sizeof (sxstrstring_t*) / sizeof (char);
	}
#endif	/* SXRESIZE */

	strtable->strnext = 0;
	strtable->string = (sxstrstring_t*) sxalloc (1, sizeof (sxstrstring_t*) + strtable->strlength * sizeof (char));
	strtable->string->next = old;
    }

    if ((ste = strtable->top++) >= (SXINT)strtable->tablength)
	strtable->table = (sxstrtable_t*) sxrealloc (strtable->table, strtable->tablength += SXHASH_LENGTH, sizeof (sxstrtable_t));

    {
	sxstrtable_t	*ptable = &(strtable->table [ste]);

	sxstrput (ptable->pointer = strtable->string->chars + strtable->strnext, string, length);
	strtable->strnext += length;
	ptable->length = strlength;
	ptable->collision_link = SXERROR_STE;
    }

    return ste;
}

/*---------------------------------------------------------------------------*/

/* Save a string of known length in strtable and return a unique identifier for it */
/* The string may be not terminated by a SXNUL and may contain SXNULs */

SXINT sxstr_savelen (sxstrmngr_t *strtable, char *string, SXUINT strlength)
{
    sxstrtable_t	*ptable, *qtable;
    SXINT	ste;
    SXUINT	hash;

    if ((ste = strtable->head [hash = sxscrmbl (string, strlength) % SXHASH_LENGTH]) == SXERROR_STE)
	/* Brand new string */
	return strtable->head [hash] = sxstr_add (strtable, string, strlength);

    qtable = NULL;

    do {
	ptable = &(strtable->table [ste]);

	if (ptable->length == strlength &&
	    (strlength == 0 || (*string == *(ptable->pointer) &&
				sxstrequal (string, ptable->pointer, strlength)))) {
	    /* string already exists */
	    if (qtable != NULL) {
		/* Arrange to find it faster next time */
		qtable->collision_link = ptable->collision_link;
		ptable->collision_link = strtable->head [hash];
		strtable->head [hash] = ste;
	    }

	    return ste;
	}
    } while ((ste = (qtable = ptable)->collision_link) != SXERROR_STE);

    /* We must do this in two steps, as sxstr_add may reallocate table */

    ptable->collision_link = strtable->top;
    return sxstr_add (strtable, string, strlength);
}

/*---------------------------------------------------------------------------*/

/* Searches for the existence of string in strtable; return the unique identifier of string or 0 if it does not exist */

SXINT sxstr_retrievelen (sxstrmngr_t *strtable, char *string, SXUINT strlength)
{
    sxstrtable_t	*ptable;
    SXINT	ste;

    if ((ste = strtable->head [sxscrmbl (string, strlength) % SXHASH_LENGTH]) != SXERROR_STE)
	do {
	    ptable = &(strtable->table [ste]);

	    if (ptable->length == strlength && sxstrequal (string, ptable->pointer, strlength)) {
		/* string already exists */
		break;
	    }
	} while ((ste = ptable->collision_link) != SXERROR_STE);

    return ste;
}

/*---------------------------------------------------------------------------*/

static SXVOID free_strings (sxstrstring_t *string)
{
    while (string != NULL) {
	sxstrstring_t	*next = string->next;

	sxfree (string);
	string = next;
    }
}

/*---------------------------------------------------------------------------*/

static SXVOID sxstr_create (sxstrmngr_t *strtable)
{
    if (strtable->head == NULL) {
	/* Allocate */
	strtable->head = (SXINT*) sxcalloc (SXHASH_LENGTH, sizeof (SXINT));
	strtable->table = (sxstrtable_t*) sxalloc (strtable->tablength = BUFSIZ, sizeof (sxstrtable_t));
	strtable->string = (sxstrstring_t*) sxalloc (1, sizeof (sxstrstring_t*) + (strtable->strlength = BUFSIZ*4 - sizeof (sxstrstring_t*)) * sizeof (char));
    }
    else
	free_strings (strtable->string->next);
    
    strtable->string->next = NULL;
    
    if (SXERROR_STE != 0 || strtable->top != 0) {
	/* Initialise */
	SXINT	*phead;
	
	for (phead = &(strtable->head [SXHASH_LENGTH - 1]); phead >= strtable->head; *phead-- = SXERROR_STE) {
	    /* null; */
	}
    }

    /* On range des chaines vides pour "SXERROR_STE" et "SXEMPTY_STE".  */

    *(strtable->table [SXERROR_STE].pointer = strtable->string->chars) = SXNUL;
    strtable->table [SXERROR_STE].length = 0;
    strtable->table [SXERROR_STE].collision_link = SXERROR_STE;
    strtable->table [SXEMPTY_STE] = strtable->table [SXERROR_STE];
    strtable->head [0] = SXEMPTY_STE;
    strtable->top = SXEMPTY_STE + 1;
    strtable->strnext = 1;
}

/*---------------------------------------------------------------------------*/

static SXVOID sxstr_delete (sxstrmngr_t *strtable)
{
    free_strings (strtable->string), strtable->string = NULL;
    sxfree (strtable->table), strtable->table = NULL;
    sxfree (strtable->head), strtable->head = NULL;
}

/*---------------------------------------------------------------------------*/

SXVOID sxstr_mngr (SXINT sxstr_mngr_what, ...)
/* si sxstr_mngr_what vaut SXOPEN ou SXCLOSE, on a un second parametre 
 * strtable de type (sxstrmngr_t *) */
{
    va_list ap;
    sxstrmngr_t *strtable;

    switch (sxstr_mngr_what) {
    case SXBEGIN:
	/* creation de la table par defaut */
    case SXCLEAR:
	/* remise a zero de la table par defaut */
	sxstr_create (&sxstrmngr);
	break;

    case SXEND:
	/* destruction de la table par defaut */
	sxstr_delete (&sxstrmngr);
	break;

    case SXOPEN:
	/* creation d'une table supplementaire voulue par l'utilisateur */
	va_start (ap, sxstr_mngr_what);
	strtable = va_arg (ap, sxstrmngr_t*);
	sxstr_create (strtable);
	va_end (ap);
	break;

    case SXCLOSE:
	/* destruction d'une table supplementaire voulue par l'utilisateur */
	va_start (ap, sxstr_mngr_what);
	strtable = va_arg (ap, sxstrmngr_t*);
	sxstr_delete (strtable);
	va_end (ap);
	break;

    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxstr_mngr","unknown switch case #1");
#endif
      break;
    }
}

/*---------------------------------------------------------------------------*/

SXVOID sxstr_dump (FILE *f, sxstrmngr_t *strtable)
{
    SXINT n, i;

    n = strtable->top;
    fprintf (f, "contents of string table at 0x%08lx (%ld elements)\n", 
        (long) strtable, n);
    for (i = 0 ; i < n; i++) {
        fprintf (f, "\tentry %ld -> \"%s\" (length = %lu)\n", 
	    i, sxstr_get (strtable, i), sxstr_length (strtable, i));
    }
}

/*---------------------------------------------------------------------------*/

SXVOID sxstr_save_keywords (sxstrmngr_t *strtable, char *lang)
{
    if (strcmp (lang, "C") == 0) {
	sxstr_save (strtable, "asm");
	sxstr_save (strtable, "auto");
	sxstr_save (strtable, "break");
	sxstr_save (strtable, "case");
	sxstr_save (strtable, "char");
	sxstr_save (strtable, "const");
	sxstr_save (strtable, "continue");
	sxstr_save (strtable, "default");
	sxstr_save (strtable, "do");
	sxstr_save (strtable, "double");
	sxstr_save (strtable, "else");
	sxstr_save (strtable, "enum");
	sxstr_save (strtable, "extern");
	sxstr_save (strtable, "float");
	sxstr_save (strtable, "for");
	sxstr_save (strtable, "fortran");
	sxstr_save (strtable, "goto");
	sxstr_save (strtable, "if");
	sxstr_save (strtable, "inline");
	sxstr_save (strtable, "int");
	sxstr_save (strtable, "long");
	sxstr_save (strtable, "main"); /* not a keyword, strictly speaking */
	sxstr_save (strtable, "register");
	sxstr_save (strtable, "restrict");
	sxstr_save (strtable, "return");
	sxstr_save (strtable, "short");
	sxstr_save (strtable, "signed");
	sxstr_save (strtable, "sizeof");
	sxstr_save (strtable, "static");
	sxstr_save (strtable, "struct");
	sxstr_save (strtable, "switch");
	sxstr_save (strtable, "typedef");
	sxstr_save (strtable, "union");
	sxstr_save (strtable, "unsigned");
	sxstr_save (strtable, "void");
	sxstr_save (strtable, "volatile");
	sxstr_save (strtable, "while");
	sxstr_save (strtable, "_Alignas");
	sxstr_save (strtable, "_Alignof");
	sxstr_save (strtable, "_Atomic");
	sxstr_save (strtable, "_Bool");
	sxstr_save (strtable, "_Complex");
	sxstr_save (strtable, "_Generic");
	sxstr_save (strtable, "_Imaginary");
	sxstr_save (strtable, "_Noreturn");
	sxstr_save (strtable, "_Pragma");
	sxstr_save (strtable, "_Static_assert");
	sxstr_save (strtable, "_Thread_local");
    } else {
	 /* unsupported language : do nothing */
    }
}

/*---------------------------------------------------------------------------*/

