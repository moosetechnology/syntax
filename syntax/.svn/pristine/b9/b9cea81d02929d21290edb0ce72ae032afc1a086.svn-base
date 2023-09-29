/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 2000 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   * Produit de l'équipe ATOLL (Langages et Traducteurs). *
   *							  *
   ******************************************************** */




/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030311 17:56 (phd):	TRUE et FALSE pour OSF1...		*/
/************************************************************************/
/* 20030311 17:55 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



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



/* Quelques petites choses bien utiles */
#include "SXante.h"


#ifndef BOOLEAN
#define BOOLEAN		char
#undef	TRUE
#define TRUE	(BOOLEAN) 1
#undef	FALSE
#define FALSE	(BOOLEAN) 0
#endif /* BOOLEAN */

#ifndef NULL
#define NULL		0
#endif /* NULL */

#ifndef NUL
#define NUL		'\0'
#endif /* NUL */


/* Constantes de la table des chaines */
#ifndef SXERROR_STE
#define SXERROR_STE		0	/* Chaine vide pour les erreurs */
#define SXEMPTY_STE		1	/* Veritable chaine vide */
#endif /* SXERROR_STE */

/* A word table element */
struct sxword_table {
    char		*pointer;
    unsigned long int	length;
    unsigned int	collision_link;
};


/* A word table buffer */
struct sxword_string {
    struct sxword_string	*next;
    char			chars [sizeof (char*)];
};

typedef struct {
    struct sxword_table		*table;
    struct sxword_string	*string;
    unsigned int		*head;
    unsigned int		tablength;
    unsigned int		top;
    unsigned int		hashsiz;
    unsigned long int		strlength;
    unsigned long int		strnext;
    char			*area_name;
    int				MACRO;
    void			*(*malloc) ();
    void			*(*calloc) ();
    void			*(*realloc) ();
    void			*(*resize) ();
    void			(*free) ();
    void			(*oflw) ();
    FILE			*stat_file;
    BOOLEAN			is_string_static;
    BOOLEAN			is_head_table_static;
} sxword_header;

extern void		sxword_alloc (sxword_header *header, char *area_name, unsigned int init_words_nb, unsigned int average_word_nb_per_bucket, unsigned int average_word_lgth,
				      void *(*sxword_alloc_malloc) (), void *(*sxword_alloc_calloc) (), void *(*sxword_alloc_realloc) (), void *(*resize) (), void (*sxword_alloc_free) (), void (*oflw) (), FILE *stat_file);
extern void		sxword_free (sxword_header *header);
extern void		sxword_clear (sxword_header *header);
extern unsigned int	sxword_save (sxword_header *header, char *string);
extern unsigned int	sxword_2save (sxword_header *header, char *string, unsigned long sxword_2save_strlen);
extern unsigned int	sxword_retrieve (sxword_header *header, char *string);
extern unsigned int	sxword_2retrieve (sxword_header *header, char *string, unsigned long strlength);
extern void		sxword_stat (sxword_header *header, FILE *stat_file);
extern BOOLEAN		sxword_write (sxword_header *header, int file_descr);
extern BOOLEAN		sxword_read (sxword_header *header, int file_descr, char *area_name,
				     void *(*sxword_read_malloc) (), void *(*sxword_read_calloc) (), void *(*sxword_read_realloc) (), void *(*resize) (), void (*sxword_read_free) (), void (*oflw) (), FILE *stat_file);
extern unsigned int	sxword_array_to_c (sxword_header *header, FILE *file, char *name);
extern void		sxword_header_to_c (sxword_header *header, FILE  *file, char *name, unsigned int total_lgth);
extern void		sxword_to_c (sxword_header *header, FILE *file, char *name, BOOLEAN is_static);
extern void		sxword_reuse (sxword_header *header, char *area_name,
				      void *(*sxword_reuse_malloc) (), void *(*sxword_reuse_calloc) (), void *(*sxword_reuse_realloc) (), void *(*resize) (), void (*sxword_reuse_free) (), void (*oflw) (), FILE *stat_file);

#if !EBUG_ALLOC

/* Get a pointer to a string from its unique identifier */
#define SXWORD_get(h,ste)		((h).table [ste].pointer)

/* Get the length of the string associated with a unique identifier */
#define SXWORD_len(h,ste)		((h).table [ste].length)

#else

/* Catch possible bugs in the use of the words manager macros. */
#define SXWORD_get(h,ste)		((h).MACRO++, sxword_get (&(h), ste))
#define SXWORD_len(h,ste)		((h).MACRO++, sxword_len (&(h), ste))

extern char*		sxword_get (sxword_header *header, unsigned int ste);
extern unsigned long	sxword_len (sxword_header *header, unsigned int ste);

#endif /* EBUG_ALLOC */

/* Logical size of the word_manager data */
#define SXWORD_top(h)			(h).top

/* Physical size of the word_manager data */
#define SXWORD_size(h)			(h).tablength

/* Save a NUL terminated string of unknown length */
#define SXWORD_save(h,s)		sxword_2save (&(h),s,strlen(s))

/* Retrieve a NUL terminated string of unknown length */
#define SXWORD_retrieve(h,s)		sxword_2retrieve (&(h),s,strlen(s))

/* Save "s", a NUL terminated string of unknown length in table "h" and sets "ste" to its
   unique identifier. Moreover this macro returns TRUE iff "s" is brand new in "h".
   The unsigned int cell "x" is used as a temporary. */
#define SXWORD_test_and_save(h,x,ste,s) (x=SXWORD_top(h),(ste=sxword_save(&(h),s))==x)

/* Save "s", a string of length "l" in table "h" and sets "ste" to its
   unique identifier. Moreover this macro returns TRUE iff "s" is brand new in "h".
   The unsigned int cell "x" is used as a temporary. */
#define SXWORD_test_and_2save(h,x,ste,s,l) (x=SXWORD_top(h),(ste=sxword_2save(&(h),s,l))==x)

/* Allows the (*oflw)() user's function to process its [re]allocation */
#define SXWORD_is_static(h)		(h).is_head_table_static


#endif /* sxword_h */
