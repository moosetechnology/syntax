/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (phd) *
   *							  *
   ******************************************************** */





/* Definitions communes aux modules "bitsarray" pour SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20061002 16:50 (pb):	Correction des macros et DEBUG		*/
/************************************************************************/
/* 20050119 13:57 (phd):	Ajout de sxba_2op, sxba_incr, sxba_decr	*/
/************************************************************************/
/* 20041209 11:53 (phd):	Adaptation à Linux 64 bits		*/
/************************************************************************/
/* 20030527 13:31 (phd):	Utilisation de SX{ante,post}.h		*/
/************************************************************************/
/* 20030430 14:42 (phd):	Adaptation au MIPS 64 bits		*/
/************************************************************************/
/* 20030428 14:41 (pb&phd):	Inclusion de <stdio.h> pour LONG_BIT	*/
/************************************************************************/
/* 20030311 13:12 (phd):	Adaptation à l'Alpha 64 bits		*/
/************************************************************************/
/* 20030116 18:12 (phd):	Utilisation de possibilités de GNU CC	*/
/************************************************************************/
/* Jeu 19 Avr 2001 14:48(pb):	Ajout de sxba_is_subset ()		*/
/************************************************************************/
/* Jeu  2 Dec 1999 18:23(pb):	On utilise la version de la 5.0		*/
/************************************************************************/
/* 23-09-96 15:56 (phd&pb):	Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* 27-07-92 10:30 (pb):		Macro "COMB" for sxba_clear 		*/
/************************************************************************/
/* 28-12-87 16:02 (phd):	Correction de la macro BASIZE		*/
/************************************************************************/
/* 23-12-87 11:37 (phd):	Creation a partir de "sxbitsarray.c"	*/
/************************************************************************/


#ifndef sxba_h
#define sxba_h
#include "SXante.h"

/******************************************
 * SXBA arrays implement efficiently bit arrays.
 * "sxba" is a package that handles sets of integer numbers between 0 and m-1.
 * A set is a string of bits. For a given set, the maximum length m of its
 * string of bits is given at allocation time. An element i is in a set if the
 * bit at position i in the string of bits is set. It can contains elements i
 * s.t. 0 <= i < m and thus, its cardinality cannot exceed m.
 *
 *
 * Interesting values and types:
 *
 *   SXBA_ELT is the type of the items of an SXBA array.
 *   SXBA_ELT should be the integer type with the same width as the registers
 *    i.e.   sizeof(SXBA_ELT) == __WORDSIZE / BITS_PER_BYTE    (with BITS_PER_BYTE==8)
 *
 *   SXBITS_PER_LONG is the number of bits of an SXBA_ELT item.
 *
 * Interesting macros :
 *
 *   NBLONGS(nb_bits) returns the number of SXBA_ELT items necessary to store 
 *     nb_bits bits of data
 *
 *   BASIZE(bits_array) returns the maximum length m of bits_array
 *     this value is stored in bits_array [0]
 *     this macro returns an SXBA_ELT, thus it may need to be cast.
 *
 * The string of bits starts at bits_array [1]
 * and ends at bits_array [NBLONGS (BASIZE (bits_array))]
 *
 *
 ******************************************/
#include "sxcommon.h"

#if 0 /* tentative de PhD pour détecter les situations G5 + Darwin avec entiers 64 bits possibles mais compilo plutôt 32 bits */
# if not defined(SXBITS_PER_LONG) && defined(__LONG_MAX__) && defined(__LONG_LONG_MAX__) && (__LONG_LONG_MAX__ > __LONG_MAX__) && (__LONG_LONG_MAX__ == 9223372036854775807LL)
    /* Darwin/G5 ? */
#  undef  SXBA_ELT
#  define SXBA_ELT	SXUINT long int
#  define SXBITS_PER_LONG	64
# endif
#endif /* 0 */


#include	<stdio.h>
#include "sxalloc.h"
#include <unistd.h>   /* for ssize_t */
#if 0
SX_GLOBAL_VAR	SXINT	_SXBA_TARGET_SIZE; /* Must be set when sxba_to_c () is used for other machines. */
#endif

/* la macro SXBITS_PER_LONG contient la taille en bits d'un SXBA_ELT. Un meilleur
   nom pourrait etre SXBITS_PER_BA_ELT.
   Une definition intuitite pourrait etre : #define SXBITS_PER_LONG (8 * sizeof SXBA_ELT)
   mais ceci n'est pas possible car on a besoin de cette macro dans le préprocesseur alors
   que le preprocesseur ne sait pas evaluer sizeof() : sizeof() est évalué par le compilateur.
   C'est pourquoi on définit SXBITS_PER_LONG a l'aide de limits.h
*/

#include <limits.h>

#ifndef SXBITS_PER_LONG
#if LONG_MAX == 2147483647
/* on est en 32 bits */
#define SXBITS_PER_LONG 32
#else
/* on est en 64 bits */
#define SXBITS_PER_LONG 64
#endif
#endif

#if SXBITS_PER_LONG==(1<<7)
# define SXSHIFT_AMOUNT 7
#else
#if SXBITS_PER_LONG==(1<<6)
# define SXSHIFT_AMOUNT 6
#else
#if SXBITS_PER_LONG==(1<<5)
# define SXSHIFT_AMOUNT 5
#else
#if SXBITS_PER_LONG==(1<<4)
# define SXSHIFT_AMOUNT 4
#endif
#endif
#endif
#endif

#ifdef SXSHIFT_AMOUNT
# define MUL(n)	((n) << SXSHIFT_AMOUNT)
# define DIV(n)	((n) >> SXSHIFT_AMOUNT)
# define MOD(n)	((n) & (SXBITS_PER_LONG-1))
#else
# define MUL(n)	((n) * SXBITS_PER_LONG)
# define DIV(n)	((n) / SXBITS_PER_LONG)
# define MOD(n)	((n) % SXBITS_PER_LONG)
#endif


#define NBLONGS(nb_bits)	DIV((nb_bits)+(SXBITS_PER_LONG-1))

#define BASIZE(bits_array)	((bits_array)[0])



/* 
   SXBA_INDEX denotes a (positive or null) bit index. 
   SXBA_INDEX_OR_ERROR denotes either an SXBA_INDEX value or -1,
   which is used, for instance, to indicate an error

   Types SXBA_INDEX and SXBA_INDEX_OR_ERROR must be identical.

   Assertion : the following assertion must always be true
   sizeof (SXBA_INDEX) <= sizeof (SXBA_ELT)
*/ 

#ifdef SX_32BIT_ORIGINAL
/*   Obsolete : the type SXBA_INDEX used to be 'int'. To use
     the old type, compile with the following flag defined :
     -DSX_32BIT_ORIGINAL  */
#define SXBA_INDEX          int
#define SXBA_INDEX_OR_ERROR int
#else
/** Since July, 2007, the new type for SXBA_INDEX is 'ssize_t'
   This type must be signed, because some functions such as
     - sxba_scan()
     - sxba_scan_reset()
     - sxba_1_rlscan()
     - sxba_0_rlscan()
     - sxba_0_lrscan()
   may return the negative value (-1) of type SXBA_INDEX
   in case of error, as does read(2).
**/
#define SXBA_INDEX          ssize_t
#define SXBA_INDEX_OR_ERROR ssize_t
#endif /* SX_32BIT_ORIGINAL */

/* Assume i+l <= SXBITS_PER_LONG */
/* Creates a "comb" of "l" teeth at index "i" */
#define COMB(i,l)	((~((~(((SXBA_ELT)0)))<<(l)))<<(i))


#if EBUG
extern void sxbatrap(char *caller);
# define	sxbassert(a,w)	if (a) ; else sxbatrap (w)
#else
# define	sxbassert(a,w)
#endif

#ifndef __GNUC__
# define SXBA_0_bit(ba,b)		sxba_0_bit (ba,b)
# define SXBA_1_bit(ba,b)		sxba_1_bit (ba,b)
# define SXBA_bit_is_set(ba,b)		sxba_bit_is_set (ba,b)
# define SXBA_bit_is_reset_set(ba,b)	sxba_bit_is_reset_set (ba,b)
# define SXBA_bit_is_set_reset(ba,b)	sxba_bit_is_set_reset (ba,b)
#else

# define SXBA_0_bit(ba,b)		({									\
					  SXBA	bits_array = ba;						\
					  SXBA_INDEX	bit = b;							\
					  sxbassert (bit >= 0 && bit < sxba_cast (BASIZE (bits_array)), "0_bit");		\
					  bits_array [DIV (bit) + 1] &= ~(((SXBA_ELT)1) << MOD (bit));		\
					  bits_array;								\
					 })
# define SXBA_1_bit(ba,b)		({									\
					  SXBA	bits_array = ba;						\
					  SXBA_INDEX	bit = b;							\
					  sxbassert (bit >= 0 && bit < sxba_cast (BASIZE (bits_array)), "1_bit");		\
					  bits_array [DIV (bit) + 1] |= (((SXBA_ELT)1) << MOD (bit));		\
					  bits_array;								\
					 })
# define SXBA_bit_is_set(ba,b)		({									\
					  SXBA	bits_array = ba;						\
					  SXBA_INDEX	bit = b;							\
					  sxbassert (bit >= 0 && bit < sxba_cast (BASIZE (bits_array)), "bit_is_set");	\
					  (bits_array [DIV (bit) + 1] & (((SXBA_ELT)1) << MOD (bit))) != 0;	\
					 })
# define SXBA_bit_is_reset_set(ba,b)	({									\
					  SXBA	bits_array = ba;						\
					  SXBA_INDEX	bit = b;							\
					  SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;				\
					  SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);			\
					  sxbassert (bit >= 0 && bit < sxba_cast (BASIZE (bits_array)), "bit_is_reset_set");\
					  (*bits_ptr & elt) ? 0 : (*bits_ptr |= elt, 1);			\
					 })
# define SXBA_bit_is_set_reset(ba,b)	({									\
					  SXBA	bits_array = ba;						\
					  SXBA_INDEX	bit = b;							\
					  SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;				\
					  SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);			\
					  sxbassert (bit >= 0 && bit < sxba_cast (BASIZE (bits_array)), "bit_is_set_reset");\
					  (*bits_ptr & elt) ? (*bits_ptr &= ~elt, 1) : 0;			\
					 })
#endif /*__GNUC__ */

#define SXBA_total_sizeof(ba)		(sizeof(SXBA_ELT)*(NBLONGS(BASIZE(ba))+1))
#define SXBA_used_sizeof(ba)		SXBA_total_sizeof(ba)

#define SXBA_OP_NULL  0
#define SXBA_OP_AND   1
#define SXBA_OP_OR    2
#define SXBA_OP_XOR   3
#define SXBA_OP_MINUS 4
#define SXBA_OP_NOT   5
#define SXBA_OP_COPY  6
#define SXBA_OP_IS    7

/* alloc/free */
extern SXBA    sxba_calloc (SXBA_INDEX bits_number);
extern SXBA    sxba_resize (SXBA bits_array, SXBA_INDEX new_bits_number);
extern SXBA    *sxbm_calloc (SXBA_INDEX lines_number, SXBA_INDEX bits_number);
extern SXBA    *sxbm_resize (SXBA *bits_matrix, SXBA_INDEX old_lines_number, SXBA_INDEX new_lines_number, SXBA_INDEX new_bits_number);
extern void    sxbm_free (SXBA *bits_matrix);

/* Manipulations discretes */
extern SXBA    sxba_0_bit (SXBA bits_array, SXBA_INDEX bit);
extern SXBA    sxba_1_bit (SXBA bits_array, SXBA_INDEX bit);
extern SXBA    sxba_empty (SXBA bits_array);
extern SXBA    sxba_fill (SXBA bits_array);
#define sxba_clear(b,i,l) sxba_clear_range (b, i, l)
/* extern SXBA    sxba_clear (SXBA bits_array, SXINT i, SXINT l); */
extern SXBA    sxba_range (SXBA bits_array, SXBA_INDEX i, SXBA_INDEX l);
extern SXBA    sxba_clear_range (SXBA bits_array, SXBA_INDEX i, SXBA_INDEX l);
extern void    sxbm_empty (SXBA *bits_matrix, SXBA_INDEX line_nb);

/* Tests */
extern BOOLEAN sxba_bit_is_set (SXBA bits_array, SXBA_INDEX bit);
extern BOOLEAN sxba_bit_is_reset_set (SXBA bits_array, SXBA_INDEX bit);
extern BOOLEAN sxba_bit_is_set_reset (SXBA bits_array, SXBA_INDEX bit);
extern BOOLEAN sxba_is_empty (SXBA bits_array);
extern BOOLEAN sxba_is_full (SXBA bits_array);
extern BOOLEAN sxba_is_subset (SXBA bits_array_1, SXBA bits_array_2);
extern SXBA_INDEX_OR_ERROR sxba_first_difference (SXBA bits_array_1, SXBA bits_array_2);

/* boucles */
extern SXBA_INDEX_OR_ERROR     sxba_scan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit);
extern SXBA_INDEX_OR_ERROR     sxba_scan_reset (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit);
extern SXBA_INDEX_OR_ERROR     sxba_1_rlscan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit);
extern SXBA_INDEX_OR_ERROR     sxba_0_lrscan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit);
extern SXBA_INDEX_OR_ERROR     sxba_0_rlscan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit);
#define sxba_1_lrscan sxba_scan

/* Operations ensemblistes */
/* generiques */
/* lhs op1= rhs1 op2 rhs2, return TRUE <=> ensemble resultat non vide */
/* rhs2 != NULL && op2 != SXBA_OP_NULL */
extern BOOLEAN sxba_2op (SXBA lhs, SXINT op1, SXBA rhs1, SXINT op2, SXBA rhs2);

/* Binaires */
extern SXBA    sxba_and (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_or (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_xor (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_minus (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_substr (SXBA bits_array1, SXBA bits_array2, SXBA_INDEX i1, SXBA_INDEX i2, SXBA_INDEX l);
extern SXBA    sxba_copy (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern BOOLEAN sxba_incr (SXBA lhs, SXBA rhs, SXBA_INDEX incr); /* lhs = {i+incr | i \in rhs} */
extern BOOLEAN sxba_decr (SXBA lhs, SXBA rhs, SXBA_INDEX decr); /* lhs = {i-decr | i \in rhs} */
/* Binaires + return (la lhs a-t-elle change') */
extern BOOLEAN sxba_or_with_test (SXBA lhs_bits_array, SXBA rhs_bits_array);

/* unaires */
extern SXBA    sxba_not (SXBA bits_array);
extern SXBA_INDEX     sxba_cardinal (SXBA bits_array);

/* I/O */
extern BOOLEAN sxba_read (filedesc_t file, SXBA bits_array);
extern BOOLEAN sxba_write (filedesc_t file, SXBA bits_array);
extern BOOLEAN sxbm_read (filedesc_t file, SXBA *bits_matrix, SXBA_INDEX line_nb);
extern BOOLEAN sxbm_write (filedesc_t file, SXBA *bits_matrix, SXBA_INDEX line_nb);
extern void    sxba_to_c (SXBA set, FILE *file_descr, char *name1, char *name2, BOOLEAN is_static);
extern void    sxba_print (SXBA set);

/* ... en C */
extern void    sxba32_to_c (FILE *file, SXBA set);
extern void    sxba64_to_c (FILE *file, SXBA set);
extern void    sxbm_to_c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, BOOLEAN is_static);
extern void    sxbm2_to_c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, BOOLEAN is_static);
extern void    sxbm3_to_c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, BOOLEAN is_static);
#ifdef varstr_h
     /* L'appelant ne peut les utiliser que s'il a inclus "varstr.h" */
extern void    sxba2c (SXBA set, FILE *file_descr, char *name1, char *name2, BOOLEAN is_static, VARSTR vstr);
extern void    sxbm2c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, BOOLEAN is_static, VARSTR vstr)
     ;
#endif /* varstr_h */

/* Explicit conversion functions, with non-overflow checking */

#if defined (lint) || EBUG
extern SXBA_INDEX sxba_cast (SXBA_ELT x);
#else
#define sxba_cast(x)  ((SXBA_INDEX)(x))
#endif

#include "SXpost.h"
#endif /* sxba_h */
