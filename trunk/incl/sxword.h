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

/* 			WORDS MANAGER		*/

/* 
   Have similar functionalities with "sxstr_mngr" but:
   - Allows several word areas to coexist
   - Macro names are prefixed by SXWORD
   - Subroutine names are prefixed by sxword
   - The type name of a word area is sxword_header
   - The first parameter of macros is a word area (object whose type is sxword_header)
   - The first parameter of subroutines is a pointer to a word area.
   - The include file "word.h" allows not to use (when unambiguous) the prefixes
     "SX" or "sx" in the various names
*/


#ifndef sxword_h
#define sxword_h

#include <stdio.h>

#include "SXante.h"
#include "sxcommon.h"

/* A word table element */
struct sxword_table {
    char		*pointer;
    SXUINT	length;
    SXINT	collision_link;
};


/* A word table buffer */
struct sxword_string {
    struct sxword_string	*next;
    char			chars [sizeof (char*)];
};

typedef struct {
    struct sxword_table		*table;
    struct sxword_string	*string;
    SXINT		*head;
    SXUINT		tablength;
    SXINT		top;
    SXUINT		hashsiz;
    SXUINT		strlength;
    SXUINT		strnext;
    char			*area_name;
    SXINT				MACRO;
    void			*(*malloc) (size_t);
    void			*(*calloc) (size_t, size_t);
    void			*(*realloc) (void *, size_t);
    void			*(*resize) (void*, size_t);
    void			(*free) (void *);
    void			(*oflw) (SXINT, SXINT);
    FILE			*stat_file;
    bool			is_string_static;
    bool			is_head_table_static;
} sxword_header;

extern void		sxword_alloc (sxword_header *header,
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
				      FILE *stat_file);
extern void		sxword_free (sxword_header *header);
extern void		sxword_clear (sxword_header *header);
extern SXINT	sxword_save (sxword_header *header, char *string);
extern SXINT	sxword_2save (sxword_header *header, char *string, SXUINT sxword_2save_strlen);
extern SXINT	sxword_retrieve (sxword_header *header, char *string);
extern SXINT	sxword_2retrieve (sxword_header *header, char *string, SXUINT strlength);
extern void		sxword_stat (sxword_header *header, FILE *stat_file);
extern bool		sxword_write (sxword_header *header, sxfiledesc_t file_descr);
extern bool		sxword_read (sxword_header *header,
				     sxfiledesc_t file_descr,
				     char *area_name,
				     void *(*sxword_read_malloc) (size_t),
				     void *(*sxword_read_calloc) (size_t, size_t),
				     void *(*sxword_read_realloc) (void*, size_t),
				     void *(*resize) (void*, size_t),
				     void (*sxword_read_free) (void*),
				     void (*oflw) (SXINT, SXINT),
				     FILE *stat_file);
extern SXUINT	sxword_array_to_c (sxword_header *header, FILE *file, char *name);
extern void		sxword_header_to_c (sxword_header *header, FILE  *file, char *name, SXUINT total_lgth);
extern void		sxword_to_c (sxword_header *header, FILE *file, char *name, bool is_static);
extern void		sxword_reuse (sxword_header *header,
				      char *area_name,
				      void *(*sxword_reuse_malloc) (size_t),
				      void *(*sxword_reuse_calloc) (size_t, size_t),
				      void *(*sxword_reuse_realloc) (void*, size_t),
				      void *(*resize) (void*, size_t),
				      void (*sxword_reuse_free) (void*),
				      void (*oflw) (SXINT, SXINT),
				      FILE *stat_file);

#if defined(SXVERSION) && EBUG_ALLOC

/* Catch possible bugs in the use of the words manager macros. */
#define SXWORD_get(h,ste)	(++((h).MACRO), sxword_get (&(h), ste))
#define SXWORD_len(h,ste)	(++((h).MACRO), sxword_len (&(h), ste))
extern char*		sxword_get (sxword_header *header, SXINT ste);
extern SXUINT	sxword_len (sxword_header *header, SXINT ste);

#else

/* Get a pointer to a string from its unique identifier */
#define SXWORD_get(h,ste)		((h).table [ste].pointer)
/* Get the length of the string associated with a unique identifier */
#define SXWORD_len(h,ste)		((h).table [ste].length)

#endif /* EBUG_ALLOC */

/* Logical size of the word_manager data */
#define SXWORD_top(h)			(h).top

/* Physical size of the word_manager data */
#define SXWORD_size(h)			(h).tablength

/* Save a SXNUL terminated string of unknown length */
#define SXWORD_save(h,s)		sxword_2save (&(h),s,strlen(s))

/* Retrieve a SXNUL terminated string of unknown length */
#define SXWORD_retrieve(h,s)		sxword_2retrieve (&(h),s,strlen(s))

/* Save "s", a SXNUL terminated string of unknown length in table "h" and sets "ste" to its
   unique identifier. Moreover this macro returns true iff "s" is brand new in "h".
   The SXUINT cell "x" is used as a temporary. */
#define SXWORD_test_and_save(h,x,ste,s) (x=SXWORD_top(h),(ste=sxword_save(&(h),s))==x)

/* Save "s", a string of length "l" in table "h" and sets "ste" to its
   unique identifier. Moreover this macro returns true iff "s" is brand new in "h".
   The SXUINT cell "x" is used as a temporary. */
#define SXWORD_test_and_2save(h,x,ste,s,l) (x=SXWORD_top(h),(ste=sxword_2save(&(h),s,l))==x)

/* Allows the (*oflw)() user's function to process its [re]allocation */
#define SXWORD_is_static(h)		(h).is_head_table_static


#endif /* sxword_h */
