/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1985 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *                                                      *
   ******************************************************** */




/* Sauvegarde des chaines de caracteres */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20000622 14:39 (phd) :	Vérification de SXHASH_LENGTH et	*/
/*				Utilisation de SXINLINE			*/
/************************************************************************/
/* 01-04-94 10:14 (pb) :	Utilisation de sxscrmbl,		*/
/*				sxstrretrieve ds le meme module	et 	*/
/*				on ajoute un sxstr2retrieve		*/
/************************************************************************/
/* 28-03-94 18:45 (phd&pb) :	Correction pour chaines vides sans NUL	*/
/************************************************************************/
/* 09-04-90 11:15 (phd) :	Meilleur mode "mise au point"		*/
/************************************************************************/
/* 06-04-90 13:00 (phd) :	Mode "mise au point" (EBUG_ALLOC)	*/
/************************************************************************/
/* 05-01-88 17:13 (phd) :	Bogue lors de la reallocation de chaine	*/
/************************************************************************/
/* 09-12-87 16:28 (phd) :	Bogue lors de la reallocation de table	*/
/************************************************************************/
/* 26-11-87 17:32 (phd) :	On n'alloue des zones de taille		*/
/*				croissante que si RESIZE est definie	*/
/************************************************************************/
/* 20-11-87 14:12 (phd) :	Utilisation de la macro SXRESIZE	*/
/************************************************************************/
/* 18-11-87 17:27 (phd):	Revue des (re)allocations		*/
/************************************************************************/
/* 17-11-87 10:08 (phd):	Ajout de la procedure sxstr2add.	*/
/************************************************************************/
/* 16-11-87 11:52 (phd):	Correction des acces a la longueur.	*/
/************************************************************************/
/* 13-11-87 15:02 (phd):	Mise dans le tas de sxstrmngr.head	*/
/************************************************************************/
/* 13-11-87 09:50 (phd):	Modification du calcul du hash-code et	*/
/*				suppression des reallocations de chaines*/
/************************************************************************/
/* 07-04-87 12:19 (phd):	sxstr2save n'impose plus de NUL en fin	*/
/************************************************************************/
/* 03-04-87 12:18 (phd):	Modifications pour accepter		*/
/*				correctement les NULs dans les chaines.	*/
/************************************************************************/
/* 30-01-87 14:07 (phd):	Ajout de sxstr2save.			*/
/*				Suppression de sxstrget			*/
/*				Suppression des parametres "size" de	*/
/*				sxstr_mngr.				*/
/*				Utilisation de ERROR_STE et EMPTY_STE	*/
/************************************************************************/
/* 18-12-86 13:32 (phd):	Separation de "sxstrretrieve"		*/
/************************************************************************/
/* 22-05-86 11:30 (phd):	Ajout du parametre "strinitsize" a	*/
/*				l'entree BEGIN de "sxstr_mngr".		*/
/*				Ajout de "nextstrincr".			*/
/************************************************************************/
/* 15-05-86 12:56 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 15-05-86 12:55 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include "sxunix.h"

char WHAT_SXSTR_MNGR[] = "@(#)SYNTAX - $Id: sxstr_mngr.c 1046 2008-02-15 08:41:05Z rlacroix $" WHAT_DEBUG;


#include "sxscrmbl.h"

#if (SXHASH_LENGTH % 2) == 0 || (SXHASH_LENGTH % 3) == 0 || (SXHASH_LENGTH % 5) == 0 || (SXHASH_LENGTH % 7) == 0 || (SXHASH_LENGTH % 11) == 0 || (SXHASH_LENGTH % 13) == 0
#error SXHASH_LENGTH should not be divisible by a small prime.  You might use one of {257,521,1031,2053,4099,8209,16411,32771,65539,131101,262217,524309,1048583,2097169} instead
#endif


#if EBUG_ALLOC
# define BELL	((char) 7)

SXVOID INCR_SXIN_STRMACRO (void) 
{
  SXIN_STRMACRO++; 
}

/* Get a pointer to a string from its unique identifier */

char	*
SXSTRGET (SXUINT ste)
{
    SXIN_STRMACRO--;
    return sxstrmngr.table [ste].pointer;
}




/* Get the length of the string associated with a unique identifier */

SXUINT
SXSTRLEN (SXUINT ste)
{
    SXIN_STRMACRO--;
    return sxstrmngr.table [ste].length;
}



static VOID
sxbadnews (void)
{
    fprintf (sxstderr, "\tInternal error: possible reallocation in strings manager...%c\n", BELL);
}
#endif


/* Increment for the string reallocation */

#ifndef	BUFSIZ
#	define	BUFSIZ	512
#endif	/* BUFSIZ */



/* Compare for equality strings s1 and s2 on their first n characters. */
/* The strings may contain NULs and may not be terminated by a NUL. */

static SXINLINE BOOLEAN
sxstrequal (char *s1, char *s2, SXUINT n)
{
    do
	if (n-- == 0)
	    return TRUE;
    while (*s1++ == *s2++);

    return FALSE;
}




/* Put in s1 first (n-1) chars of s2 and terminate by a NUL*/

static SXINLINE VOID
sxstrput (char *s1, char *s2, SXUINT n)
{
    while (--n != 0)
	*s1++ = *s2++;

    *s1 = NUL;
}




/* Enter into strings table a string of length strlen for entry top++ */

SXUINT
sxstr2add (char *string, SXUINT sxstr2add_strlen)
{
    SXUINT	strlength = sxstr2add_strlen;
    SXUINT	ste;

#if EBUG_ALLOC
    if (SXIN_STRMACRO) {
	sxbadnews ();
    }
#endif

    if (sxstrmngr.strnext + ++strlength >= sxstrmngr.strlength) {
	struct sxstrstring	*old;

	old = sxstrmngr.string;

#ifdef SXRESIZE
	SXRESIZE (old, 1, sizeof (struct sxstrstring*) + (sxstrmngr.strnext - 1) * sizeof (char));
	sxstrmngr.strlength += (strlength & -sizeof (char*)) + BUFSIZ;
#else
	/* This might be a buddy system allocator; find a power of 2 */

	if (sxstrmngr.strlength < strlength) {
	    SXUINT	min = strlength + sizeof (struct sxstrstring*) / sizeof (char);

	    sxstrmngr.strlength += sizeof (struct sxstrstring*) / sizeof (char);

	    do {
	    } while ((sxstrmngr.strlength *= 2) < min);

	    sxstrmngr.strlength -= sizeof (struct sxstrstring*) / sizeof (char);
	}
#endif	/* SXRESIZE */

	sxstrmngr.strnext = 0;
	sxstrmngr.string = (struct sxstrstring*) sxalloc (1, sizeof (struct sxstrstring*) + sxstrmngr.strlength * sizeof (char));
	sxstrmngr.string->next = old;
    }

    if ((ste = sxstrmngr.top++) >= sxstrmngr.tablength)
	sxstrmngr.table = (struct sxstrtable*) sxrealloc (sxstrmngr.table, sxstrmngr.tablength += SXHASH_LENGTH, sizeof (struct sxstrtable));

    {
	struct sxstrtable	*ptable = &(sxstrmngr.table [ste]);

	sxstrput (ptable->pointer = sxstrmngr.string->chars + sxstrmngr.strnext, string, strlength);
	sxstrmngr.strnext += strlength;
	ptable->length = sxstr2add_strlen;
	ptable->collision_link = ERROR_STE;
    }

    return ste;
}





/* Save a string of known length and return a unique identifier for it */
/* The string may be not terminated by a NUL and may contain NULs */

SXUINT
sxstr2save (char *string, SXUINT sxstr2save_strlen)
{
    struct sxstrtable	*ptable, *qtable;
    SXUINT	ste;
    SXUINT	strlength = sxstr2save_strlen;
    SXUINT	hash;

#if EBUG_ALLOC
    if (SXIN_STRMACRO) {
	sxbadnews ();
    }
#endif

    if ((ste = sxstrmngr.head [hash = sxscrmbl (string, strlength) % SXHASH_LENGTH]) == ERROR_STE)
	/* Brand new string */
	return sxstrmngr.head [hash] = sxstr2add (string, strlength);

    qtable = NULL;

    do {
	ptable = &(sxstrmngr.table [ste]);

	if (ptable->length == strlength &&
	    (strlength == 0 || (*string == *(ptable->pointer) &&
				sxstrequal (string, ptable->pointer, strlength)))) {
	    /* string already exists */
	    if (qtable != NULL) {
		/* Arrange to find it faster next time */
		qtable->collision_link = ptable->collision_link;
		ptable->collision_link = sxstrmngr.head [hash];
		sxstrmngr.head [hash] = ste;
	    }

	    return ste;
	}
    } while ((ste = (qtable = ptable)->collision_link) != ERROR_STE);

    /* We must do this in two steps, as sxstr2add may reallocate table */

    ptable->collision_link = sxstrmngr.top;
    return sxstr2add (string, strlength);
}



/* Save a string of unknown length and return a unique identifier for it */
SXUINT
sxstrsave (char *string)
{
    return sxstr2save (string, strlen (string));
}



/* Return the unique identifier of a string or 0 if it does not exist */
SXUINT
sxstr2retrieve (char *string, SXUINT strlength)
{
    struct sxstrtable	*ptable;
    SXUINT	ate;

    if ((ate = sxstrmngr.head [sxscrmbl (string, strlength) % SXHASH_LENGTH]) != ERROR_STE)
	do {
	    ptable = &(sxstrmngr.table [ate]);

	    if (ptable->length == strlength && sxstrequal (string, ptable->pointer, strlength)) {
		/* string already exists */
		break;
	    }
	} while ((ate = ptable->collision_link) != ERROR_STE);

    return ate;
}



/* Return the unique identifier of a C string of unknown length or 0 if it does not exist */
SXUINT
sxstrretrieve (char *string)
{
    return sxstr2retrieve (string, strlen (string));
}



static SXVOID
free_strings (struct sxstrstring *string)
{
    while (string != NULL) {
	struct sxstrstring	*next = string->next;

	sxfree (string);
	string = next;
    }
}



SXVOID
sxstr_mngr (SXINT sxstr_mngr_what)
{
#if EBUG_ALLOC
    if (SXIN_STRMACRO) {
	sxbadnews ();
    }
#endif

    switch (sxstr_mngr_what) {
    case BEGIN:
	if (sxstrmngr.head == NULL) {
	    /* Allocate */
	    sxstrmngr.head = (SXUINT*) sxcalloc (SXHASH_LENGTH, sizeof (SXUINT));
	    sxstrmngr.table = (struct sxstrtable*) sxalloc (sxstrmngr.tablength = BUFSIZ, sizeof (struct sxstrtable));
	    sxstrmngr.string = (struct sxstrstring*) sxalloc (1, sizeof (struct sxstrstring*) + (sxstrmngr.strlength = BUFSIZ*4 - sizeof (struct sxstrstring*)) * sizeof (char));
	}
	else
	    free_strings (sxstrmngr.string->next);

	sxstrmngr.string->next = NULL;

	if (ERROR_STE != 0 || sxstrmngr.top != 0) {
	    /* Initialise */
	    SXUINT	*phead;

	    for (phead = &(sxstrmngr.head [SXHASH_LENGTH - 1]); phead >= sxstrmngr.head; *phead-- = ERROR_STE) {
		/* null; */
	    }
	}


/* On range des chaines vides pour "ERROR_STE" et "EMPTY_STE".  */

	*(sxstrmngr.table [ERROR_STE].pointer = sxstrmngr.string->chars) = NUL;
	sxstrmngr.table [ERROR_STE].length = 0;
	sxstrmngr.table [ERROR_STE].collision_link = ERROR_STE;
	sxstrmngr.table [EMPTY_STE] = sxstrmngr.table [ERROR_STE];
	sxstrmngr.head [0] = EMPTY_STE;
	sxstrmngr.top = EMPTY_STE + 1;
	sxstrmngr.strnext = 1;
	break;

    case END:
	free_strings (sxstrmngr.string), sxstrmngr.string = NULL;
	sxfree (sxstrmngr.table), sxstrmngr.table = NULL;
	sxfree (sxstrmngr.head), sxstrmngr.head = NULL;
	break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxstr_mngr","unknown switch case #1");
#endif
      break;
    }
}
