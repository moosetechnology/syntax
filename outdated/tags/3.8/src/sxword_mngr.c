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
   *  Produit de l'equipe ChLoE (PB)			  *
   *                                                      *
   ******************************************************** */




/* Manipulation de chaines de caracteres (a la str_mngr) dans des espaces disjoints. */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 24-03-94 10:02 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#ifndef lint
char	what_sxword_mngr [] = "@(#)sxword_mngr.c\t- SYNTAX [unix] - Mer 30 Mar 1994 11:01:21";
#endif

#include "sxunix.h"
#include "sxword.h"
#include "sxscrmbl.h"


/* Quelques petites choses bien utiles */

#ifndef BOOLEAN
#define BOOLEAN		char
#define FALSE	        (BOOLEAN) 0
#define TRUE	        (BOOLEAN) 1	/* (!FALSE) */
#endif

#ifndef NULL
#define NULL		0
#endif

#ifndef NUL
#define NUL		'\0'
#endif

#ifdef EBUG_ALLOC
# define BELL		((char) 7)

static void
sxbadnews ()
{
    fprintf (stdout, "\tInternal error: possible reallocation in words manager...%c\n", BELL);
}
#endif


/* Get a pointer to a string from its unique identifier */
char*
sxword_get (header, ste)
    sxword_header	*header;
    unsigned int	ste;
{
#ifdef EBUG_ALLOC
    header->MACRO--;
#endif

    return header->table [ste].pointer;
}




/* Get the length of the string associated with a unique identifier */
unsigned long int
sxword_len (header, ste)
    sxword_header	*header;
    unsigned int	ste;
{
#ifdef EBUG_ALLOC
    header->MACRO--;
#endif

    return header->table [ste].length;
}


/* Save a string of known length and return a unique identifier for it */
/* The string may be not terminated by a NUL and may contain NULs */
unsigned int
sxword_2save (header, string, strlen)
    sxword_header	*header;
    char		*string;
    unsigned long	strlen;
{
    struct sxword_table	*ptable;
    unsigned int	ste, *pste, old_tablength;
    unsigned long	strlength;

#ifdef EBUG_ALLOC
    if (header->MACRO) {
	sxbadnews ();
    }
#endif

    pste = header->head + sxscrmbl (string, strlen) % header->hashsiz;

    for (ste = *pste;
	 ste != SXERROR_STE;
	 ste = ptable->collision_link)
    {
	ptable = &(header->table [ste]);

	if (ptable->length == strlen &&
	    (strlen == 0 ||
	     *string == *(ptable->pointer) &&
	     memcmp (string + 1, ptable->pointer + 1, strlen - 1) == 0)) {
	    /* string already exists */

	    return ste;
	}
    }

    /* Brand new string */
    /* Room always exists for its header. */
    ptable = &(header->table [ste = header->top++]);

    ptable->length = strlen;
    ptable->collision_link = *pste;
    *pste = ste;

    if (header->strnext + (strlength = strlen + 1) >= header->strlength) {
	struct sxword_string	*old = header->string;

	if (header->resize != NULL)
	{
	    (*header->resize) (old, sizeof (struct sxword_string*) +
			       (header->strnext - 1) * sizeof (char)); 
	}

	if (strlength > header->strlength)
	    header->strlength = strlength;

	header->strnext = 0;
	header->string = (struct sxword_string*)
	    (*header->malloc) (sizeof (struct sxword_string*) + header->strlength * sizeof (char));
	header->string->next = old;
    }

    memcpy (ptable->pointer = header->string->chars + header->strnext, string, strlen);
    *(ptable->pointer + strlen) = NUL;
    header->strnext += strlength;


    if (header->top >= (old_tablength = header->tablength))
    {
	/* Area is full, reallocation for the next time. */
	unsigned int 		*p, i, hash;
	struct sxword_table	*pti;

	header->hashsiz = (unsigned int) sxnext_prime ((int) header->hashsiz);

	header->head = (unsigned int*)
	    (*header->realloc) (header->head, header->hashsiz * sizeof (unsigned int));

	header->table = (struct sxword_table*)
	    (*header->realloc) (header->table,
				(header->tablength *= 2) * sizeof (struct sxword_table));

	p = header->head + header->hashsiz;

	while (--p >= header->head)
	    *p = 0;

	for (i = 1; i <= ste; i++)
	{
	    pti = &(header->table [i]);
	    pste = header->head + sxscrmbl (pti->pointer, pti->length) % header->hashsiz;
	    pti->collision_link = *pste;
	    *pste = i;
	}

	/* User is warned and may reallocate concurrent structures. */
	if (header->oflw != NULL)
	    (*header->oflw) (old_tablength, header->tablength);
    }

    return ste;
}


/* Return the unique identifier of a string or SXERROR_STE if it does not exist */
unsigned int
sxword_2retrieve (header, string, strlength)
    sxword_header	*header;
    char		*string;
    unsigned long	strlength;
{
    struct sxword_table	*ptable;
    unsigned int	ste;

#ifdef EBUG_ALLOC
    if (header->MACRO) {
	sxbadnews ();
    }
#endif

    for (ste = header->head [sxscrmbl (string, strlength) % header->hashsiz];
	 ste != SXERROR_STE;
	 ste = ptable->collision_link)
    {
	ptable = &(header->table [ste]);

	if (ptable->length == strlength && memcmp (string, ptable->pointer, strlength) == 0)
	    /* string already exists */
	    break;
    }

    return ste;
}


/* Return the unique identifier of a (C) string of unknown length or
   SXERROR_STE if it does not exist */
unsigned int
sxword_retrieve (header, string)
    sxword_header	*header;
    char		*string;
{
    return sxword_2retrieve (header, string, strlen (string));
}

/* Save a (C) string of unknown length and return a unique identifier for it */
unsigned int
sxword_save (header, string)
    sxword_header 	*header;
    char		*string;
{
    return sxword_2save (header, string, strlen (string));
}



/* Output on stat_file some statistics about the current state of header. */
void
sxword_stat (header, stat_file)
    sxword_header	*header;
    FILE		*stat_file;
{
    unsigned int ste;
    int	i, buckets_nb = 0, strings_lgth = 0, l, minl = 0x7FFFFFFF, maxl = 0,
    mincl = header->top, maxcl = 0, cl, maxste, maxhash;

    for (i = 0; i < header->hashsiz; i++)
    {

	if ((ste = header->head [i]) != SXERROR_STE)
	{
	    buckets_nb++;
	    cl = 0;

	    do {
		cl++;
		strings_lgth += (l = SXWORD_len (*header, ste));

		if (l > maxl)
		    maxl = l, maxste = ste;

		if (l < minl)
		    minl = l;
	    } while ((ste = header->table [ste].collision_link) != SXERROR_STE);

	    if (cl > maxcl)
		maxcl = cl, maxhash = i;

	    if (cl < mincl)
		mincl = cl;
	}
    }

    fprintf (stat_file, "%s: hash_length (%d/%d) = %d%%, \
strings_nb (%d/%d) = %d%%,\n\
strings_lgth (min, mean, max(ste), total) = (%d, %d, %d(%d), %d),\n\
collisions (min, mean, max(hash)) = (%d, %d, %d(%d))\n\n",
	     header->area_name,
	     header->hashsiz, buckets_nb, (100*buckets_nb)/header->hashsiz,
	     header->tablength, header->top, (100*header->top)/header->tablength,
	     minl, strings_lgth /header->top , maxl, maxste, strings_lgth,
	     mincl, header->top /buckets_nb , maxcl, maxhash
	     );
}



/* Local procedure called from "sxword_free" or "sxword_clear". */
static void
free_strings (string, free)
    struct sxword_string	*string;
    void			(*free) ();
{
    struct sxword_string	*next;

    while (string != NULL)
    {
	next = string->next;
	(*free) (string);
	string = next;
    }
}



/* Empty header (without changing its current size). */
void
sxword_clear (header)
    sxword_header 	*header;
{
    unsigned int	*phead;

#ifdef EBUG_ALLOC
    if (header->MACRO) {
	sxbadnews ();
    }
#endif

    if (header->top != 0) {
	/* Initialise */
	phead = header->head + header->hashsiz;

	while (--phead >= header->head)
	    *phead = 0;
    }

    if (header->string->next != NULL)
    {
	free_strings (header->string->next, header->free);
	header->string->next = NULL;
    }

    /* On range des chaines vides pour "SXERROR_STE" et "SXEMPTY_STE".  */

    *(header->table [SXERROR_STE].pointer = header->string->chars) = NUL;
    header->table [SXERROR_STE].length = 0;
    header->table [SXERROR_STE].collision_link = SXERROR_STE;

    header->table [SXEMPTY_STE] = header->table [SXERROR_STE];
    header->head [0] = SXEMPTY_STE;
    header->top = SXEMPTY_STE + 1;
    header->strnext = 1;
}


void
sxword_alloc (header, area_name, init_words_nb, average_word_nb_per_bucket, average_word_lgth,
	      malloc, calloc, realloc, resize, free, oflw, stat_file)
    sxword_header 	*header;
    char		*area_name;
    unsigned int	init_words_nb, average_word_nb_per_bucket, average_word_lgth;
    char		*(*malloc) (), *(*calloc) (), *(*realloc) (), *(*resize) ();
    void		(*free) ();
    int			(*oflw) ();
    FILE		*stat_file;
{
#ifdef EBUG_ALLOC
    header->MACRO = 0;
#endif

    header->hashsiz = (unsigned int)
	sxnext_prime ((int) (init_words_nb / average_word_nb_per_bucket));
    header->tablength = init_words_nb;
    header->strlength = (unsigned long int) (init_words_nb * average_word_lgth);

    header->head = (unsigned int*) (*calloc) (header->hashsiz, sizeof (unsigned int));
    header->table = (struct sxword_table*)
	(*malloc) (header->tablength * sizeof (struct sxword_table));
    header->string = (struct sxword_string*)
	(*malloc) (sizeof (struct sxword_string*) + header->strlength * sizeof (char));

    header->string->next = NULL;

    sxword_clear (header);

    header->area_name = area_name;
    header->malloc = malloc;
    header->calloc = calloc;
    header->realloc = realloc;
    header->resize = resize;
    header->free = free;
    header->oflw = oflw;
    header->stat_file = stat_file;
}


void
sxword_free (header)
    sxword_header 	*header;
{
#ifdef EBUG_ALLOC
    if (header->MACRO) {
	sxbadnews ();
    }

    if (header->stat_file != NULL)
	sxword_stat (header, header->stat_file);
#endif

    free_strings (header->string, header->free), header->string = NULL;
    (*header->free) (header->table), header->table = NULL;
    (*header->free) (header->head), header->head = NULL;
}



