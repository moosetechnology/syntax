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
/* 20030512 10:59 (phd):	Adaptation à SGI (64 bits)		*/
/************************************************************************/
/* 02-06-94 11:37 (pb):		Ajout de sxword_[array_|header_]to_c et	*/
/*				sxword_reuse				*/
/************************************************************************/
/* 01-06-94 14:00 (pb):		Ajout de sxword_write et sxword_read	*/
/************************************************************************/
/* 24-03-94 10:02 (pb):		Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxunix.h"
#include "sxword.h"
#include "sxscrmbl.h"
#if 0
#include <memory.h>
#endif /* 0 */ /* remplacé par: */
#include <string.h> /* Pour memcpy, strlen, ... */
#include <unistd.h>

char WHAT_SXWORD_MNGR[] = "@(#)SYNTAX - $Id: sxword_mngr.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


#if EBUG_ALLOC
# define BELL		((char) 7)
static void
sxbadnews ()
{
    fprintf (sxstderr, "\tInternal error: possible reallocation in words manager...%c\n", BELL);
}
#endif

#ifndef CHAR_TO_STRING
#define CHAR_TO_STRING(c)	((sxc_to_str+128)[c])
  extern char *sxc_to_str [];
#endif


/* Get a pointer to a string from its unique identifier */
char*
sxword_get (sxword_header *header, unsigned int ste)
{
#if EBUG_ALLOC
    header->MACRO--;
#endif

    return header->table [ste].pointer;
}




/* Get the length of the string associated with a unique identifier */
unsigned long int
sxword_len (sxword_header *header, unsigned int ste)
{
#if EBUG_ALLOC
    header->MACRO--;
#endif

    return header->table [ste].length;
}


/* Save a string of known length and return a unique identifier for it */
/* The string may be not terminated by a NUL and may contain NULs */
unsigned int
sxword_2save (sxword_header *header, char *string, unsigned long sxword_2save_strlen)
{
    struct sxword_table	*ptable;
    unsigned int	ste, *pste, old_tablength;
    unsigned long	strlength;

#if EBUG_ALLOC
    if (header->MACRO) {
	sxbadnews ();
    }
#endif

    pste = header->head + sxscrmbl (string, sxword_2save_strlen) % header->hashsiz;

    for (ste = *pste;
	 ste != SXERROR_STE;
	 ste = ptable->collision_link)
    {
	ptable = &(header->table [ste]);

	if (ptable->length == sxword_2save_strlen &&
	    (sxword_2save_strlen == 0 ||
	     ((*string == *(ptable->pointer)) &&
	     (memcmp (string + 1, ptable->pointer + 1, sxword_2save_strlen - 1) == 0)))) {
	    /* string already exists */

	    return ste;
	}
    }

    /* Brand new string */
    /* Room always exists for its header. */
    ptable = &(header->table [ste = header->top++]);

    ptable->length = sxword_2save_strlen;
    ptable->collision_link = *pste;
    *pste = ste;

    if (header->strnext + (strlength = sxword_2save_strlen + 1) >= header->strlength) {
	struct sxword_string	*old = header->string;

	if (header->resize != NULL && (old->next != NULL || !header->is_string_static))
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

    memcpy (ptable->pointer = header->string->chars + header->strnext, string, sxword_2save_strlen);
    *(ptable->pointer + sxword_2save_strlen) = NUL;
    header->strnext += strlength;


    if (header->top >= (old_tablength = header->tablength))
    {
	/* Area is full, reallocation for the next time. */
	unsigned int 		*p, i;
	struct sxword_table	*pti;

	header->hashsiz = (unsigned int) sxnext_prime ((int) header->hashsiz);

	if (header->is_head_table_static)
	{ 
	    unsigned int	local_old_tablength = header->tablength;
	    struct sxword_table	*old_table = header->table;

	    header->head = (unsigned int*)
		(*header->calloc) (header->head, header->hashsiz * sizeof (unsigned int));

	    header->table = (struct sxword_table*)
		(*header->malloc) (header->table,
				   (header->tablength *= 2) * sizeof (struct sxword_table));

	    memcpy ((char*) header->table,
		    (char*) old_table,
		    sizeof (struct sxword_table) * local_old_tablength);
	}
	else
	{
	    header->head = (unsigned int*)
		(*header->realloc) (header->head, header->hashsiz * sizeof (unsigned int));

	    header->table = (struct sxword_table*)
		(*header->realloc) (header->table,
				    (header->tablength *= 2) * sizeof (struct sxword_table));

	    p = header->head + header->hashsiz;

	    while (--p >= header->head)
		*p = 0;
	}

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

	/* Used by (*header->oflw) () */
	header->is_head_table_static = FALSE;
    }

    return ste;
}


/* Return the unique identifier of a string or SXERROR_STE if it does not exist */
unsigned int
sxword_2retrieve (sxword_header *header, char *string, unsigned long strlength)
{
    struct sxword_table	*ptable;
    unsigned int	ste;

#if EBUG_ALLOC
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
sxword_retrieve (sxword_header *header, char *string)
{
    return sxword_2retrieve (header, string, (unsigned long) strlen (string));
}

/* Save a (C) string of unknown length and return a unique identifier for it */
unsigned int
sxword_save (sxword_header *header, char *string)
{
    return sxword_2save (header, string, (unsigned long) strlen (string));
}



/* Output on stat_file some statistics about the current state of header. */
void
sxword_stat (sxword_header *header, FILE *stat_file)
{
    unsigned int ste;
    int	i, buckets_nb = 0, strings_lgth = 0, l, minl = 0x7FFFFFFF, maxl = 0,
    mincl = header->top, maxcl = 0, cl, maxste = 0, maxhash = 0;

    for (i = 0; (unsigned int)i < header->hashsiz; i++)
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
static struct sxword_string*
free_strings (string, sxword_string_free)
    struct sxword_string	*string;
    void			(*sxword_string_free) ();
{
    /* string est non NULL */
    struct sxword_string	*next;

    while ((next = string->next) != NULL)
    {
	next = string->next;
	(*sxword_string_free) (string);
	string = next;
    }

    return string;
}



/* Empty header (without changing its current size). */
void
sxword_clear (sxword_header *header)
{
    unsigned int	*phead;

#if EBUG_ALLOC
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

    header->string = free_strings (header->string, header->free);

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
sxword_alloc (sxword_header *header, char *area_name, unsigned int init_words_nb, unsigned int average_word_nb_per_bucket, unsigned int average_word_lgth,
	      void *(*sxword_alloc_malloc) (), void *(*sxword_alloc_calloc) (), void *(*sxword_alloc_realloc) (), void *(*resize) (), void (*sxword_alloc_free) (), void (*oflw) (), FILE *stat_file)
{
#if EBUG_ALLOC
    header->MACRO = 0;
#endif

    init_words_nb += 2; /* Au moins 2 (Error et Empty) */
    average_word_lgth += 1; /* Au moins 1 */

    if (average_word_nb_per_bucket == 0)
      average_word_nb_per_bucket = 1;  /* Au moins 1 */
    
    header->hashsiz = (unsigned int)
	sxnext_prime ((int) (init_words_nb / average_word_nb_per_bucket));
    header->tablength = init_words_nb;
    header->strlength = (unsigned long int) (init_words_nb * average_word_lgth);

    header->head = (unsigned int*) (*sxword_alloc_calloc) (header->hashsiz, sizeof (unsigned int));
    header->table = (struct sxword_table*)
	(*sxword_alloc_malloc) (header->tablength * sizeof (struct sxword_table));
    header->string = (struct sxword_string*)
	(*sxword_alloc_malloc) (sizeof (struct sxword_string*) + header->strlength * sizeof (char));

    header->string->next = NULL;

    sxword_clear (header);

    header->area_name = area_name;
    header->malloc = sxword_alloc_malloc;
    header->calloc = sxword_alloc_calloc;
    header->realloc = sxword_alloc_realloc;
    header->resize = resize;
    header->free = sxword_alloc_free;
    header->oflw = oflw;
    header->stat_file = stat_file;

    header->is_string_static = FALSE;
    header->is_head_table_static = FALSE;
}


void
sxword_free (sxword_header *header)
{
#if EBUG_ALLOC
    if (header->MACRO) {
	sxbadnews ();
    }

    if (header->stat_file != NULL)
	sxword_stat (header, header->stat_file);
#endif

    header->string = free_strings (header->string, header->free);

    if (!header->is_string_static)
	(*header->free) (header->string);

    header->string = NULL;

    if (!header->is_head_table_static)
    {
	(*header->free) (header->table);
	(*header->free) (header->head);
    }

    header->table = NULL;
    header->head = NULL;
}

#define WRITE(p,l)	((bytes=(l))>0&&(write(file_descr, p, (size_t)bytes) == bytes))
#define READ(p,l)	((bytes=(l))>0&&(read (file_descr, p, (size_t)bytes) == bytes))

static BOOLEAN
write_table (header, file_descr)
    sxword_header 	*header;
    int			file_descr;
{
    int	bytes; 
    unsigned int	i;
    struct sxword_table	*p;

    for (i = 0; i < header->top; i++)
    {
	p = header->table + i;

	if (!(WRITE (&(p->length), sizeof (unsigned long int))
	    && WRITE (&(p->collision_link), sizeof (unsigned int))
	    && WRITE (p->pointer, sizeof (char) * (p->length + 1))
	    ))
	    return FALSE;
    }

    return TRUE;
}



static BOOLEAN
read_table (sxword_header *header, int file_descr)
{
    int	bytes; 
    unsigned int	i;
    struct sxword_table	*p;

    header->table = (struct sxword_table*) (*header->malloc)
	(header->tablength * sizeof (struct sxword_table));

    for (i = 0; i < header->top; i++)
    {
	p = header->table + i;

	if (!(READ (&(p->length), sizeof (unsigned long int))
	    && READ (&(p->collision_link), sizeof (unsigned int))
	    && ((void)READ (p->pointer = &(header->string->chars [header->strnext]),
		            sizeof (char) * (p->length + 1)),
		header->strnext += p->length + 1)
	    ))
	    return FALSE;
    }

    return TRUE;
}

BOOLEAN
sxword_write (sxword_header *header, int file_descr)
{
    int	bytes;  
    unsigned int	i;
    unsigned long int	total_lgth = 0;

    for (i = 0; i < header->top; i++)
	total_lgth += header->table [i].length + 1;

    return (WRITE (&header->hashsiz, sizeof (unsigned int))
	    && WRITE (&header->tablength, sizeof (unsigned int))
	    && WRITE (&header->top, sizeof (unsigned int))
	    && WRITE (&total_lgth /* strlength */, sizeof (unsigned long int))
	    && WRITE (header->head, sizeof (unsigned int) * header->hashsiz)
	    && write_table (header, file_descr)
	    );
}

BOOLEAN
sxword_read (sxword_header *header, int file_descr, char *area_name,
	     void *(*sxword_read_malloc) (), void *(*sxword_read_calloc) (), void *(*sxword_alloc_realloc) (), void *(*resize) (), void (*sxword_alloc_free) (), void (*oflw) (), FILE *stat_file)
{
  int	        bytes;

#if EBUG_ALLOC
    header->MACRO = 0;
#endif

    header->area_name = area_name;
    header->malloc = sxword_read_malloc;
    header->calloc = sxword_read_calloc;
    header->realloc = sxword_alloc_realloc;
    header->resize = resize;
    header->free = sxword_alloc_free;
    header->oflw = oflw;
    header->stat_file = stat_file;

    return (READ (&header->hashsiz, sizeof (unsigned int))
	    && READ (&header->tablength, sizeof (unsigned int))
	    && READ (&header->top, sizeof (unsigned int))
	    && (header->strnext = 0, READ (&header->strlength, sizeof (unsigned long int)))
	    && READ (header->head =
		     (unsigned int*) (*sxword_read_malloc) (header->hashsiz * sizeof (unsigned int)),
		     sizeof (unsigned int) * header->hashsiz)
	    && (header->string = (struct sxword_string*)
		(*sxword_read_malloc) (sizeof (struct sxword_string*) + header->strlength * sizeof (char)))
	    && (header->string->next = NULL, read_table (header, file_descr))
	    );
}


/* Nelle version le 22/11/06 */
unsigned int
sxword_array_to_c (sxword_header *header, FILE *file, char *name)
{
    unsigned long int	i, total_lgth;
    unsigned int	l, lgth;
    unsigned long int	*mot2start;
    char                *mot;

    mot2start = (unsigned long int *) (*header->malloc) (header->top * sizeof (unsigned long int));
    mot2start [0] = 0;
    total_lgth = 0;

    for (i = 1; i < header->top; i++) {
      mot2start [i] = total_lgth;
      total_lgth += SXWORD_len (*header, i)+1;
    }

    fprintf (file, "\n\nstatic struct {\n\
\tstruct sxword_string	*next;\n\
\tchar	chars [%lu];\n\
} %s_string = {\n\
NULL,\n\
\"", total_lgth, name);
    total_lgth = 0;

    for (i = 1; i < header->top; i++) {
      mot = SXWORD_get (*header, i);
      lgth = SXWORD_len (*header, i);

      for (l = 0; l <= lgth; l++) {
	/* on prend le NUL */
	fprintf (file, "%s", CHAR_TO_STRING (*mot));
	mot++;
	total_lgth++;
	
	if ((total_lgth & 077) == 077)
	    fprintf (file, "\\\n");
      }
    }
    fprintf (file, "\"} /* End %s_string */;\n", name);

    fprintf (file, "\n\nstatic unsigned int %s_head [] = {", name);
    for (i = 0; i < header->hashsiz; i++)
    {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %lu */ ", i);

	fprintf (file, "%i, ", header->head [i]);
    } 
    fprintf (file, "\n} /* End %s_head */;\n", name);

    fprintf (file, "\n\nstatic struct sxword_table %s_table [%i] = {\n", name, header->tablength);
    for (i = 0; i < header->top; i++)
    {
	fprintf (file, "/* %lu */ {&(%s_string.chars [%li]), %lu, %i},\n",
		 i,
		 name,
		 mot2start [i],
		 header->table [i].length,
		 header->table [i].collision_link);
    }
    fprintf (file, "} /* End %s_table */;\n", name);

    (*header->free) (mot2start);

    return total_lgth;
}


#if 0
/* bugee si plus d'un bloc !! */
void
sxword_array_to_c (sxword_header *header, FILE *file, char *name)
{
    unsigned long int	i;

    fprintf (file, "\n\nstatic struct {\n\
\tstruct sxword_string	*next;\n\
\tchar	chars [%lu];\n\
} %s_string = {\n\
NULL,\n\
\"", header->strlength, name);
    for (i = 0; i < header->strnext; i++)
    {
	fprintf (file, "%s", CHAR_TO_STRING (header->string->chars [i]));
	
	if ((i & 077) == 077)
	    fprintf (file, "\\\n");
    }
    fprintf (file, "\"} /* End %s_string */;\n", name);

    fprintf (file, "\n\nstatic unsigned int %s_head [] = {", name);
    for (i = 0; i < header->hashsiz; i++)
    {
	if ((i & 7) == 0)
	    fprintf (file, "\n/* %lu */ ", i);

	fprintf (file, "%i, ", header->head [i]);
    } 
    fprintf (file, "\n} /* End %s_head */;\n", name);

    fprintf (file, "\n\nstatic struct sxword_table %s_table [%i] = {\n", name, header->tablength);
    for (i = 0; i < header->top; i++)
    {
	fprintf (file, "/* %lu */ {&(%s_string.chars [%li]), %lu, %i},\n", i,
		 name, header->table [i].pointer - header->string->chars,
		 header->table [i].length, header->table [i].collision_link);
    }
    fprintf (file, "} /* End %s_table */;\n", name);
}
#endif /* 0 */


void
sxword_header_to_c (sxword_header *header, FILE *file, char *name, unsigned int total_lgth)
{
    fprintf (file, "{\n");

    fprintf (file, "%s_table,\n", name);
    fprintf (file, "(struct sxword_string*) &%s_string,\n", name);
    fprintf (file, "%s_head,\n", name);
    fprintf (file, "%i /* tablength */,\n", header->tablength);
    fprintf (file, "%i /* top */,\n", header->top);
    fprintf (file, "%i /* hashsiz */,\n", header->hashsiz);
    fprintf (file, "%lu /* strlength */,\n", header->strlength);
    fprintf (file, "%lu /* strnext */,\n", header->strnext);

    fprintf (file, "NULL /* area_name */,\n");

    fprintf (file, "0L /* MACRO */,\n");

    fprintf (file, "NULL /* (*malloc) () */,\n");
    fprintf (file, "NULL /* (*calloc) () */,\n");
    fprintf (file, "NULL /* (*realloc) () */,\n");
    fprintf (file, "NULL /* (*resize) () */,\n");
    fprintf (file, "NULL /* (*free) () */,\n");
    fprintf (file, "NULL /* (*oflw) () */,\n");
    fprintf (file, "NULL /* *stat_file */,\n");

    fprintf (file, "TRUE /* is_string_static */,\n");
    fprintf (file, "TRUE /* is_head_table_static */\n");

    fprintf (file, "}");
}

void
sxword_to_c (sxword_header *header, FILE *file, char *name, BOOLEAN is_static)
{
  unsigned int	total_lgth;

  total_lgth = sxword_array_to_c (header, file, name);
  fprintf (file, "\n\n%ssxword_header %s =\n", is_static ? "static " : "", name);
  sxword_header_to_c (header, file, name, total_lgth);
  fprintf (file, ";\n");
}



void
sxword_reuse (sxword_header *header, char *area_name,
	      void *(*sxword_reuse_malloc) (), void *(*sxword_reuse_calloc) (), void *(*sxword_reuse_realloc) (), void *(*resize) (), void (*sxword_reuse_free) (), void (*oflw) (), FILE *stat_file)
{
#if EBUG_ALLOC
    header->MACRO = 0;
#endif

    header->area_name = area_name;
    header->malloc = sxword_reuse_malloc;
    header->calloc = sxword_reuse_calloc;
    header->realloc = sxword_reuse_realloc;
    header->resize = resize;
    header->free = sxword_reuse_free;
    header->oflw = oflw;
    header->stat_file = stat_file;
}
