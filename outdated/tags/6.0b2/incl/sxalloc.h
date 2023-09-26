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
/* 20040120 10:17 (phd&pb):	Prototypes et unsigned long int		*/
/************************************************************************/
/* 20030527 13:31 (phd):	Utilisation de SX{ante,post}.h		*/
/************************************************************************/
/* 20030311 17:56 (phd):	TRUE et FALSE pour OSF1...		*/
/************************************************************************/
/* 20001002 15:48 (phd):	Modifications diverses suite à la	*/
/*				demande d'utilisateurs (surtout VASY)	*/
/************************************************************************/
/* 20001002 15:48 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/



#ifndef sxalloc_h
#define sxalloc_h
#include "SXante.h"

/* Quelques petites choses bien utiles */

#ifndef NULL
#define NULL	0
#endif

#ifndef BOOLEAN
#define BOOLEAN SXBOOLEAN
#undef	FALSE
#define FALSE   SXFALSE
#undef	TRUE
#define TRUE    SXTRUE
#endif

#ifndef SXBOOLEAN
#define SXBOOLEAN	char
#define SXFALSE	        (SXBOOLEAN) 0
#define SXTRUE	        (SXBOOLEAN) 1	/* (!SXFALSE) */
#endif

#ifdef lint


#if 0 /* Let us hope that the system's include files do the right thing */
extern char *malloc (), *calloc (), *realloc ();
extern void free ();
#endif

#  define sxalloc(NB, SZ)		malloc ((unsigned)((NB)*(SZ)))
#  define sxcalloc(NB, SZ)		calloc ((unsigned)(NB), (unsigned)(SZ))
#  define sxrealloc(PTR, NB, SZ)	realloc ((char*)(PTR), (unsigned)((NB)*(SZ)))
#  define sxrecalloc(PTR, NB1, NB2, SZ)	realloc ((char*)(PTR), (unsigned)((NB1)*(SZ)+(NB2)*(SZ)))
#  define sxfree(PTR)			free ((char*)(PTR))

#else

extern void *sxcont_malloc (unsigned long int size), *sxcont_alloc (unsigned long int n, unsigned long int size), *sxcont_realloc (void *table, unsigned long int size), *sxcont_recalloc (void *table, unsigned long int old_size, unsigned long int new_size);
extern void sxcont_free (void *zone);

#  define sxalloc(NB, SZ)		sxcont_malloc ((NB)*(SZ))
#  define sxcalloc(NB, SZ)		sxcont_alloc (NB, SZ)
#  define sxrealloc(PTR, NB, SZ)	sxcont_realloc (PTR, (NB)*(SZ))
#  define sxrecalloc(PTR, NB1, NB2, SZ)	sxcont_recalloc (PTR, (NB1)*(SZ), (NB2)*(SZ))
#  define sxfree(PTR)			sxcont_free (PTR)

#endif /* lint */

#include "SXpost.h"
#endif /* sxalloc_h */

/*
 * Local Variables:
 * mode: C
 * version-control: yes
 * End:
 */
