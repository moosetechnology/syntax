/* ********************************************************
   *							  *
   * Copyright (c) 1985 by Institut National de Recherche *
   *			en Informatique et en Automatique *
   *			and Universite d'Orleans	  *
   *							  *
   ******************************************************** */


/* ********************************************************
   *     Produit de l'equipe Langages et Traducteurs.     *
   ******************************************************** */


/* ********************************************************
   *							  *
   * Allocation controlee pour la version UN*X de SYNTAX. *
   * Ecrit par JJL.  Modifications de PhD.		  *
   *							  *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20040211 13:41 (phd):	Modif pour calmer le compilo HPUX	*/
/************************************************************************/
/* 20040120 10:17 (phd&pb):	Prototypes et SXUINT		*/
/************************************************************************/
/* 20030516 18:37 (phd):	Utilisation de "arena memory allocator"	*/
/************************************************************************/
/* 20030506 10:21 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 20010404 12:11 (pb&phd)	MALLOC_CHECK_ pour Linux		*/
/************************************************************************/
/* Mer  5 Jul 2000 11:23 (pb)	Ajout de sxcont_recalloc ()		*/
/************************************************************************/
/* 24-11-87 17:35 (phd):	Appel de sxgc en cas de probleme	*/
/************************************************************************/
/* 27-08-87 11:09 (phd):	Ajout des appels a "malloc_debug".	*/
/************************************************************************/
/* 17-08-87 09:37 (phd):	Utilisation de "free" plutot que de	*/
/*				"cfree", qui n'est pas documentee sur	*/
/*				tous les systemes.			*/
/************************************************************************/
/* 16-12-86 10:30 (phd):	Correction de la modification de pb...	*/
/************************************************************************/
/* 08-10-86 15:10 (pb):		sxtmsg remplace par fprintf		*/
/************************************************************************/
/* 04-06-86 14:52 (phd):	Passage de NULL a sxtmsg		*/
/************************************************************************/
/* 22-05-86 17:05 (phd):	Message sur "sxcont_free (NULL)"	*/
/************************************************************************/
/* 16-05-86 10:40 (phd):	Remplacement de "BUGALLOC" par "BUG"	*/
/************************************************************************/
/* 16-05-86 10:36 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

#include	"sxunix.h"

char WHAT_SXMEM_MNGR[] = "@(#)SYNTAX - $Id: sxmem_mngr.c 1213 2008-03-13 12:49:08Z rlacroix $" WHAT_DEBUG;


#define BELL	((char) 7)


#if	EBUG_ALLOC
static SXBOOLEAN debug_initiated = {FALSE};
# if defined ( linux ) || defined ( __sun  )
#  define MALLOC_DEBUG()	(debug_initiated || (setenv ("MALLOC_CHECK_", "2", 1), debug_initiated = TRUE))
# else
#  ifdef __alpha
SXUINT __sbrk_override;
SXUINT __taso_mode;
#   define MALLOC_DEBUG()	(debug_initiated || (__sbrk_override = 1, __taso_mode = 0, debug_initiated = TRUE))
#  else
#   ifdef __sgi
#    include <ulocks.h>
#    include <malloc.h>
#    define MALLOC_DEBUG()	(debug_initiated || (usconfig (CONF_ARENATYPE, US_SHAREDONLY), usmallopt (M_DEBUG, 1, manche = usinit ("/tmp/sxphdmalX")), debug_initiated = TRUE))
static usptr_t *manche;
#    define malloc(size)			usmalloc (size, manche)
#    define free(ptr)				usfree (ptr, manche)
#    define realloc(ptr, size)			usrealloc (ptr, size, manche)
#    define calloc(nelem, elsize)		uscalloc (nelem, elsize, manche)
#    define recalloc(ptr, nelem, elsize)	usrecalloc (ptr, nelem, elsize, manche)
#   else
#    ifdef WIN32
#     define  MALLOC_DEBUG()  ( sxuse(debug_initiated) )
#    else
extern SXINT 	malloc_debug ();
#     define MALLOC_DEBUG()	(debug_initiated || (malloc_debug (2), debug_initiated = TRUE))
#    endif
#   endif
#  endif
# endif
#else
# define MALLOC_DEBUG()
#endif


#if 0 /* (commentaire donné par Vasy) En effet, d'une part ces fonctions n'ont pas de prototype pour
	 leurs arguments, et d'autre part, sur solaris, l'inclusion de stdlib.h declare bien la
	 fonction malloc(), mais ne declare pas de macro malloc. Donc ce test #ifndef malloc n'est
	 pas portable. */
#ifndef malloc
extern void	*malloc (), *calloc (), *realloc ();
extern VOID	free ();
#endif
#endif /* 0 */


extern VOID	sxgc (void) /* exit when no more allocation is possible */ ;



void	*sxcont_malloc (size_t size)

/* Allocate a block of size size or exit */

{
    void	*result;

    MALLOC_DEBUG ();

# if BUG
    fprintf (sxstdout, "mallocating %lu:", (unsigned long)size);
# endif

    while ((result = malloc (size)) == NULL)
	sxgc ();

# if BUG
    fprintf (sxstdout, "\t%lX allocated\n", (SXUINT) result);
# endif

    return result;
}



void	*sxcont_alloc (size_t n, size_t size)

/* Allocate n blocks of size size or exit */

{
    void	*result;

    MALLOC_DEBUG ();

# if BUG
    fprintf (sxstdout, "allocating %lu (%lu):", (unsigned long)n, (unsigned long)size);
# endif

    while ((result = calloc (n, size)) == NULL)
	sxgc ();

# if BUG
    fprintf (sxstdout, "\t%lX allocated\n", (SXUINT) result);
# endif

    return result;
}



void	*sxcont_realloc (void *table, size_t size)

/* Reallocate table with new size or exit */

{
    void	*result;

    MALLOC_DEBUG ();

# if BUG
    fprintf (sxstdout, "\nreallocating %lX (%lu):", (SXUINT) table, (SXUINT) size);
# endif

    while ((result = realloc (table, size)) == NULL)
	sxgc ();

# if BUG
    fprintf (sxstdout, "\t%lX reallocated\n", (SXUINT) result);
# endif

    return result;
}



void	*sxcont_recalloc (void *table, size_t old_size, size_t new_size)

/* Reallocate table with new_size and RAZ new elements or exit */

{
    void	*result;

    MALLOC_DEBUG ();

# if BUG
    fprintf (sxstdout, "\nrecallocating %lX (%lu):", (SXUINT) table, (SXUINT) new_size);
# endif

# ifdef recalloc
    while ((result = recalloc (table, 1, new_size)) == NULL)
	sxgc ();
# else
    while ((result = realloc (table, new_size)) == NULL)
	sxgc ();

    memset (((char*) result) + old_size, 0, new_size-old_size);
# endif

# if BUG
    fprintf (sxstdout, "\t%lX recallocated\n", (SXUINT) result);
# endif

    return result;
}



VOID	sxcont_free (void *zone)

/* Frees the area "zone" */

{
    MALLOC_DEBUG ();

# if	BUG
    if (zone == NULL) {
	fprintf (sxstderr, "\tInternal error: freeing NULL...%c\n", BELL);
	SXEXIT (SEVERITIES);
    }
    else {
	fprintf (sxstdout, "freeing %lX:", (SXUINT) zone);
	free (zone);
	fprintf (sxstdout, "\tfreed\n");
    }
# else
    if (zone == NULL)
	fprintf (sxstderr, "\tInternal error (bypassed): freeing NULL...%c\n", BELL);
    else
	free (zone);
# endif
}
