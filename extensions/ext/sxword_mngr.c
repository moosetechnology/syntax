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

/* Manipulation de chaines de caracteres (a la str_mngr) dans des espaces disjoints. */

#include "sxversion.h"
#include "sxunix.h"
#include "sxword.h"
#include "sxscrmbl.h"
#include <string.h> /* Pour memcpy, strlen, ... */
#include <unistd.h>

char WHAT_SXWORD_MNGR[] = "@(#)SYNTAX - $Id: sxword_mngr.c 3914 2024-04-22 14:38:32Z garavel $" WHAT_DEBUG;


#if EBUG_ALLOC
static void
sxbadnews (void)
{
    fprintf (sxstderr, "\tInternal error: possible reallocation in words manager...%c\n", sxbell);
}
#endif

#ifndef SXCHAR_TO_STRING
#define SXCHAR_TO_STRING(c)	((sxc_to_str+128)[c])
  extern char *sxc_to_str [];
#endif


/* Get a pointer to a string from its unique identifier */
char*
sxword_get (sxword_header *header, SXINT ste)
{
#if EBUG_ALLOC
    header->MACRO--;
#endif

    return header->table [ste].pointer;
}




/* Get the length of the string associated with a unique identifier */
SXUINT
sxword_len (sxword_header *header, SXINT ste)
{
#if EBUG_ALLOC
    header->MACRO--;
#endif

    return header->table [ste].length;
}


/* Save a string of known length and return a unique identifier for it */
/* The string may be not terminated by a SXNUL and may contain NULs */
SXINT
sxword_2save (sxword_header *header, char *string, SXUINT sxword_2save_strlen)
{
    struct sxword_table	*ptable;
    SXINT	ste, *pste;
    SXUINT	strlength, old_tablength;

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
    *(ptable->pointer + sxword_2save_strlen) = SXNUL;
    header->strnext += strlength;


    if (header->top >= (SXINT)(old_tablength = header->tablength))
    {
	/* Area is full, reallocation for the next time. */
	SXINT 		*p, i;
	struct sxword_table	*pti;

	header->hashsiz = (SXUINT) sxnext_prime ((SXINT) header->hashsiz);

	if (header->is_head_table_static)
	{ 
	    SXUINT	local_old_tablength = header->tablength;
	    struct sxword_table	*old_table = header->table;

	    header->head = (SXINT*)
                (*header->calloc) (1, header->hashsiz * sizeof (SXINT));

	    header->table = (struct sxword_table*)
		(*header->malloc) ((header->tablength *= 2) * sizeof (struct sxword_table));

	    memcpy ((char*) header->table,
		    (char*) old_table,
		    sizeof (struct sxword_table) * local_old_tablength);
	}
	else
	{
	    header->head = (SXINT*)
		(*header->realloc) (header->head, header->hashsiz * sizeof (SXINT));

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
	header->is_head_table_static = false;
    }

    return ste;
}


/* Return the unique identifier of a string or SXERROR_STE if it does not exist */
SXINT
sxword_2retrieve (sxword_header *header, char *string, SXUINT strlength)
{
    struct sxword_table	*ptable;
    SXINT	ste;

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
SXINT
sxword_retrieve (sxword_header *header, char *string)
{
    return sxword_2retrieve (header, string, (SXUINT) strlen (string));
}

/* Save a (C) string of unknown length and return a unique identifier for it */
SXINT
sxword_save (sxword_header *header, char *string)
{
    return sxword_2save (header, string, (SXUINT) strlen (string));
}



/* Output on stat_file some statistics about the current state of header. */
void
sxword_stat (sxword_header *header, FILE *stat_file)
{
    SXINT ste;
    SXINT	i, buckets_nb = 0, strings_lgth = 0, l, minl = 0x7FFFFFFF, maxl = 0,
    mincl = header->top, maxcl = 0, cl, maxste = 0, maxhash = 0;

    for (i = 0; (SXUINT)i < header->hashsiz; i++)
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

    fprintf (stat_file, "%s: hash_length (%lu/%ld) = %lu%%, \
strings_nb (%lu/%lu) = %lu%%,\n\
strings_lgth (min, mean, max(ste), total) = (%ld, %lu, %ld(%ld), %ld),\n\
collisions (min, mean, max(hash)) = (%ld, %lu, %ld(%ld))\n\n",
	     header->area_name,
	     header->hashsiz, buckets_nb, (100*buckets_nb)/header->hashsiz,
	     header->tablength, header->top, (100*header->top)/header->tablength,
	     minl, strings_lgth /header->top , maxl, maxste, strings_lgth,
	     mincl, header->top /buckets_nb , maxcl, maxhash
	     );
}



/* Local procedure called from "sxword_free" or "sxword_clear". */
static struct sxword_string*
free_strings (struct sxword_string *string, void (*sxword_string_free) (void*))
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
    SXINT	*phead;

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

    *(header->table [SXERROR_STE].pointer = header->string->chars) = SXNUL;
    header->table [SXERROR_STE].length = 0;
    header->table [SXERROR_STE].collision_link = SXERROR_STE;

    header->table [SXEMPTY_STE] = header->table [SXERROR_STE];
    header->head [0] = SXEMPTY_STE;
    header->top = SXEMPTY_STE + 1;
    header->strnext = 1;
}


void
sxword_alloc (sxword_header *header,
	      char *area_name,
	      SXUINT init_words_nb,
	      SXUINT average_word_nb_per_bucket,
	      SXUINT average_word_lgth,
	      void *(*sxword_alloc_malloc) (size_t),
	      void *(*sxword_alloc_calloc) (size_t, size_t),
	      void *(*sxword_alloc_realloc) (void *, size_t),
	      void *(*resize) (void*, size_t),
	      void (*sxword_alloc_free) (void *),
	      void (*oflw) (SXINT, SXINT),
	      FILE *stat_file)
{
#if EBUG_ALLOC
    header->MACRO = 0;
#endif

    init_words_nb += 2; /* Au moins 2 (Error et Empty) */
    average_word_lgth += 1; /* Au moins 1 */

    if (average_word_nb_per_bucket == 0)
      average_word_nb_per_bucket = 1;  /* Au moins 1 */
    
    header->hashsiz = (SXUINT)
	sxnext_prime ((SXINT) (init_words_nb / average_word_nb_per_bucket));
    header->tablength = init_words_nb;
    header->strlength = (SXUINT) (init_words_nb * average_word_lgth);

    header->head = (SXINT*) (*sxword_alloc_calloc) (header->hashsiz, sizeof (SXINT));
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

    header->is_string_static = false;
    header->is_head_table_static = false;
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

static bool
write_table (sxword_header *header, sxfiledesc_t file_descr)
{
    SXINT	bytes; 
    SXINT	i;
    struct sxword_table	*p;

    for (i = 0; i < header->top; i++)
    {
	p = header->table + i;

	if (!(WRITE (&(p->length), sizeof (SXUINT))
	    && WRITE (&(p->collision_link), sizeof (SXINT))
	    && WRITE (p->pointer, sizeof (char) * (p->length + 1))
	    ))
	    return false;
    }

    return true;
}



static bool
read_table (sxword_header *header, sxfiledesc_t file_descr)
{
    SXINT	bytes; 
    SXINT	i;
    struct sxword_table	*p;

    header->table = (struct sxword_table*) (*header->malloc)
	(header->tablength * sizeof (struct sxword_table));

    for (i = 0; i < header->top; i++)
    {
	p = header->table + i;

	if (!(READ (&(p->length), sizeof (SXUINT))
	    && READ (&(p->collision_link), sizeof (SXINT))
	    && ((void)READ (p->pointer = &(header->string->chars [header->strnext]),
		            sizeof (char) * (p->length + 1)),
		header->strnext += p->length + 1)
	    ))
	    return false;
    }

    return true;
}

bool
sxword_write (sxword_header *header, sxfiledesc_t file_descr)
{
    SXINT	bytes;  
    SXINT	i;
    SXUINT	total_lgth = 0;

    for (i = 0; i < header->top; i++)
	total_lgth += header->table [i].length + 1;

    return (WRITE (&header->hashsiz, sizeof (SXUINT))
	    && WRITE (&header->tablength, sizeof (SXUINT))
	    && WRITE (&header->top, sizeof (SXINT))
	    && WRITE (&total_lgth /* strlength */, sizeof (SXUINT))
	    && WRITE (header->head, sizeof (SXINT) * header->hashsiz)
	    && write_table (header, file_descr)
	    );
}

bool
sxword_read (sxword_header *header,
	     sxfiledesc_t file_descr,
	     char *area_name,
	     void *(*sxword_read_malloc) (size_t),
	     void *(*sxword_read_calloc) (size_t, size_t),
	     void *(*sxword_read_realloc) (void*, size_t),
	     void *(*resize) (void*, size_t),
	     void (*sxword_read_free) (void*),
	     void (*oflw) (SXINT, SXINT),
	     FILE *stat_file)
{
  SXINT	        bytes;

#if EBUG_ALLOC
    header->MACRO = 0;
#endif

    header->area_name = area_name;
    header->malloc = sxword_read_malloc;
    header->calloc = sxword_read_calloc;
    header->realloc = sxword_read_realloc;
    header->resize = resize;
    header->free = sxword_read_free;
    header->oflw = oflw;
    header->stat_file = stat_file;

    return (READ (&header->hashsiz, sizeof (SXUINT))
	    && READ (&header->tablength, sizeof (SXUINT))
	    && READ (&header->top, sizeof (SXINT))
	    && (header->strnext = 0, READ (&header->strlength, sizeof (SXUINT)))
	    && READ (header->head =
		     (SXINT*) (*sxword_read_malloc) (header->hashsiz * sizeof (SXINT)),
		     sizeof (SXUINT) * header->hashsiz)
	    && (header->string = (struct sxword_string*)
		(*sxword_read_malloc) (sizeof (struct sxword_string*) + header->strlength * sizeof (char)))
	    && (header->string->next = NULL, read_table (header, file_descr))
	    );
}


/* Nelle version le 22/11/06 */
SXUINT
sxword_array_to_c (sxword_header *header, FILE *file, char *name)
{
    SXINT	i;
    SXUINT	j, l, lgth, total_lgth;
    SXUINT	*mot2start;
    char                *mot;

    mot2start = (SXUINT *) (*header->malloc) (header->top * sizeof (SXUINT));
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
	/* on prend le SXNUL */
	fprintf (file, "%s", SXCHAR_TO_STRING (*mot));
	mot++;
	total_lgth++;
	
	if ((total_lgth & 077) == 077)
	    fprintf (file, "\\\n");
      }
    }
    fprintf (file, "\"} /* End %s_string */;\n", name);

    fprintf (file, "\n\nstatic SXINT %s_head [] = {", name);
    for (j = 0; j < header->hashsiz; j++)
    {
	if ((j & 7) == 0)
	    fprintf (file, "\n/* %lu */ ", j);

	fprintf (file, "%ld, ", (SXINT) header->head [j]);
    } 
    fprintf (file, "\n} /* End %s_head */;\n", name);

    fprintf (file, "\n\nstatic struct sxword_table %s_table [%ld] = {\n", name, (SXINT) header->tablength);
    for (i = 0; i < header->top; i++)
    {
	fprintf (file, "/* %lu */ {&(%s_string.chars [%li]), %lu, %ld},\n",
		 i,
		 name,
		 mot2start [i],
		 header->table [i].length,
		 (SXINT) header->table [i].collision_link);
    }
    fprintf (file, "} /* End %s_table */;\n", name);

    (*header->free) (mot2start);

    return total_lgth;
}




void
sxword_header_to_c (sxword_header *header, FILE *file, char *name, SXUINT total_lgth)
{
    sxuse(total_lgth); /* pour faire taire gcc -Wunused */
    fprintf (file, "{\n");

    fprintf (file, "%s_table,\n", name);
    fprintf (file, "(struct sxword_string*) &%s_string,\n", name);
    fprintf (file, "%s_head,\n", name);
    fprintf (file, "%ld /* tablength */,\n", (SXINT) header->tablength);
    fprintf (file, "%ld /* top */,\n", (SXINT) header->top);
    fprintf (file, "%ld /* hashsiz */,\n", (SXINT) header->hashsiz);
    fprintf (file, "%lu /* strlength */,\n", header->strlength);
    fprintf (file, "%lu /* strnext */,\n", header->strnext);

    fprintf (file, "NULL /* area_name */,\n");

    fprintf (file, "(SXINT) 0 /* MACRO */,\n");

    fprintf (file, "NULL /* (*malloc) () */,\n");
    fprintf (file, "NULL /* (*calloc) () */,\n");
    fprintf (file, "NULL /* (*realloc) () */,\n");
    fprintf (file, "NULL /* (*resize) () */,\n");
    fprintf (file, "NULL /* (*free) () */,\n");
    fprintf (file, "NULL /* (*oflw) () */,\n");
    fprintf (file, "NULL /* *stat_file */,\n");

    fprintf (file, "true /* is_string_static */,\n");
    fprintf (file, "true /* is_head_table_static */\n");

    fprintf (file, "}");
}

void
sxword_to_c (sxword_header *header, FILE *file, char *name, bool is_static)
{
  SXUINT	total_lgth;

  total_lgth = sxword_array_to_c (header, file, name);
  fprintf (file, "\n\n%ssxword_header %s =\n", is_static ? "static " : "", name);
  sxword_header_to_c (header, file, name, total_lgth);
  fprintf (file, ";\n");
}



void
sxword_reuse (sxword_header *header,
	      char *area_name,
	      void *(*sxword_reuse_malloc) (size_t),
	      void *(*sxword_reuse_calloc) (size_t, size_t),
	      void *(*sxword_reuse_realloc) (void*, size_t),
	      void *(*resize) (void*, size_t),
	      void (*sxword_reuse_free) (void*),
	      void (*oflw) (SXINT, SXINT),
	      FILE *stat_file)
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
