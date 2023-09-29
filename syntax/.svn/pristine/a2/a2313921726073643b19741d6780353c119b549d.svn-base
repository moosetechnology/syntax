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

/* Constantes de la table des chaines */
#ifndef SXERROR_STE
#define SXERROR_STE		0	/* Chaine vide pour les erreurs */
#define SXEMPTY_STE		1	/* Veritable chaine vide */
#endif


extern void		sxword_alloc ( /* 	sxword_header	*header_ptr,
						char		*area_name,
						unsigned int	init_words_nb,
						unsigned int	average_word_nb_per_bucket,
						unsigned int	average_word_lgth,
						char		*(*malloc) (),
						char		*(*calloc) (),
						char		*(*realloc) (),
						char		*(*resize) (),
						void		(*free) (),
						int		(*oflw) (),
						FILE		*stat_file	*/);
extern void		sxword_free ( /* 	sxword_header	*header_ptr	*/);
extern void		sxword_clear ( /* 	sxword_header	*header_ptr	*/);
extern unsigned int	sxword_save ( /* 	sxword_header	*header_ptr,
						char		*string		*/);
extern unsigned int	sxword_2save ( /* 	sxword_header	*header_ptr,
						char		*string,
						unsigned long	strlen		*/);
extern unsigned int	sxword_retrieve ( /* 	sxword_header	*header_ptr,
						char		*string		*/);
extern unsigned int	sxword_2retrieve ( /* 	sxword_header	*header_ptr,
						char		*string,
						unsigned long	strlen		*/);
extern char*		sxword_get ( /* 	sxword_header	*header_ptr,
						unsigned int	ste		*/);
extern unsigned long	sxword_len ( /* 	sxword_header	*header_ptr,
						unsigned int	ste		*/);
extern void		sxword_stat ( /* 	sxword_header	*header_ptr,
				                FILE		*stat_file	*/);

#ifndef EBUG_ALLOC

/* Get a pointer to a string from its unique identifier */
#define SXWORD_get(h,ste)		((h).table [ste].pointer)

/* Get the length of the string associated with a unique identifier */
#define SXWORD_len(h,ste)		((h).table [ste].length)

#else

/* Catch possible bugs in the use of the words manager macros. */
#define SXWORD_get(h,ste)		((h).MACRO++, sxword_get (&(h), ste))
#define SXWORD_len(h,ste)		((h).MACRO++, sxword_len (&(h), ste))

#endif

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
    char			*(*malloc) ();
    char			*(*calloc) ();
    char			*(*realloc) ();
    char			*(*resize) ();
    void			(*free) ();
    int				(*oflw) ();
    FILE			*stat_file;
} sxword_header;

#endif
