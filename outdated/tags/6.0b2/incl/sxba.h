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

#ifndef SXBA_ELT
#define SXBA_ELT	unsigned long int

#if 0 /* tentative de PhD pour détecter les situations G5 + Darwin avec entiers 64 bits possibles mais compilo plutôt 32 bits */
# if not defined(SXBITS_PER_LONG) && defined(__LONG_MAX__) && defined(__LONG_LONG_MAX__) && (__LONG_LONG_MAX__ > __LONG_MAX__) && (__LONG_LONG_MAX__ == 9223372036854775807LL)
    /* Darwin/G5 ? */
#  undef  SXBA_ELT
#  define SXBA_ELT	unsigned long long int
#  define SXBITS_PER_LONG	64
# endif
#endif /* 0 */

#endif

typedef SXBA_ELT	*SXBA;

#include	<stdio.h>
#include "sxalloc.h"
#include "sxcommon.h"
#if 0
SX_GLOBAL_VAR	int	_SXBA_TARGET_SIZE; /* Must be set when sxba_to_c () is used for other machines. */
#endif

#ifndef SXBITS_PER_LONG
# ifdef LONG_BIT
   /* OSF1/alpha */
#  define SXBITS_PER_LONG LONG_BIT
# else
#  ifdef _MIPS_SZLONG
    /* IRIX/mips */
#   define SXBITS_PER_LONG _MIPS_SZLONG
#  else
#   ifdef __WORDSIZE
     /* Linux ? */
#    define SXBITS_PER_LONG __WORDSIZE
#   else
     /* Use something reasonable */
#    define SXBITS_PER_LONG 32
#   endif
#  endif
# endif
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
					  int	bit = b;							\
					  sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "0_bit");		\
					  bits_array [DIV (bit) + 1] &= ~(((SXBA_ELT)1) << MOD (bit));		\
					  bits_array;								\
					 })
# define SXBA_1_bit(ba,b)		({									\
					  SXBA	bits_array = ba;						\
					  int	bit = b;							\
					  sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "1_bit");		\
					  bits_array [DIV (bit) + 1] |= (((SXBA_ELT)1) << MOD (bit));		\
					  bits_array;								\
					 })
# define SXBA_bit_is_set(ba,b)		({									\
					  SXBA	bits_array = ba;						\
					  int	bit = b;							\
					  sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "bit_is_set");	\
					  (bits_array [DIV (bit) + 1] & (((SXBA_ELT)1) << MOD (bit))) != 0;	\
					 })
# define SXBA_bit_is_reset_set(ba,b)	({									\
					  SXBA	bits_array = ba;						\
					  int	bit = b;							\
					  SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;				\
					  SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);			\
					  sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "bit_is_reset_set");\
					  (*bits_ptr & elt) ? 0 : (*bits_ptr |= elt, 1);			\
					 })
# define SXBA_bit_is_set_reset(ba,b)	({									\
					  SXBA	bits_array = ba;						\
					  int	bit = b;							\
					  SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;				\
					  SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);			\
					  sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "bit_is_set_reset");\
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
extern SXBA    sxba_calloc (int bits_number);
extern SXBA    sxba_resize (SXBA bits_array, int new_bits_number);
extern SXBA    *sxbm_calloc (int lines_number, int bits_number);
extern SXBA    *sxbm_resize (SXBA *bits_matrix, int old_lines_number, int new_lines_number, int new_bits_number);
extern void    sxbm_free (SXBA *bits_matrix);

/* Manipulations discretes */
extern SXBA    sxba_0_bit (SXBA bits_array, int bit);
extern SXBA    sxba_1_bit (SXBA bits_array, int bit);
extern SXBA    sxba_empty (SXBA bits_array);
extern SXBA    sxba_fill (SXBA bits_array);
#define sxba_clear(b,i,l) sxba_clear_range (b, i, l)
/* extern SXBA    sxba_clear (SXBA bits_array, int i, int l); */
extern SXBA    sxba_range (SXBA bits_array, int i, int l);
extern SXBA    sxba_clear_range (SXBA bits_array, int i, int l);

/* Tests */
extern BOOLEAN sxba_bit_is_set (SXBA bits_array, int bit);
extern BOOLEAN sxba_bit_is_reset_set (SXBA bits_array, int bit);
extern BOOLEAN sxba_bit_is_set_reset (SXBA bits_array, int bit);
extern BOOLEAN sxba_is_empty (SXBA bits_array);
extern BOOLEAN sxba_is_full (SXBA bits_array);
extern BOOLEAN sxba_is_subset (SXBA bits_array_1, SXBA bits_array_2);
extern int     sxba_first_difference (SXBA bits_array_1, SXBA bits_array_2);

/* boucles */
extern int     sxba_scan (SXBA bits_array, int from_bit);
extern int     sxba_scan_reset (SXBA bits_array, int from_bit);
extern int     sxba_1_rlscan (SXBA bits_array, int from_bit);
extern int     sxba_0_lrscan (SXBA bits_array, int from_bit);
extern int     sxba_0_rlscan (SXBA bits_array, int from_bit);
#define sxba_1_lrscan sxba_scan

/* Operations ensemblistes */
/* generiques */
/* lhs op1= rhs1 op2 rhs2, return TRUE <=> ensemble resultat non vide */
/* rhs2 != NULL && op2 != SXBA_OP_NULL */
extern BOOLEAN sxba_2op (SXBA lhs, int op1, SXBA rhs1, int op2, SXBA rhs2);

/* Binaires */
extern SXBA    sxba_and (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_or (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_xor (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_minus (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern SXBA    sxba_substr (SXBA bits_array1, SXBA bits_array2, int i1, int i2, int l);
extern SXBA    sxba_copy (SXBA lhs_bits_array, SXBA rhs_bits_array);
extern BOOLEAN sxba_incr (SXBA lhs, SXBA rhs, int incr); /* lhs = {i+incr | i \in rhs} */
extern BOOLEAN sxba_decr (SXBA lhs, SXBA rhs, int decr); /* lhs = {i-decr | i \in rhs} */
/* Binaires + return (la lhs a-t-elle change') */
extern BOOLEAN sxba_or_with_test (SXBA lhs_bits_array, SXBA rhs_bits_array);

/* unaires */
extern SXBA    sxba_not (SXBA bits_array);
extern int     sxba_cardinal (SXBA bits_array);

/* I/O */
extern BOOLEAN sxba_read (int file, SXBA bits_array);
extern BOOLEAN sxba_write (int file, SXBA bits_array);
extern BOOLEAN sxbm_read (int file, SXBA *bits_matrix, int line_nb);
extern BOOLEAN sxbm_write (int file, SXBA *bits_matrix, int line_nb);
extern void    sxba_to_c (SXBA set, FILE *file_descr, char *name1, char *name2, BOOLEAN is_static);
extern void    sxba_print (SXBA set);

/* ... en C */
extern void    sxba32_to_c (FILE *file, SXBA set);
extern void    sxba64_to_c (FILE *file, SXBA set);
extern void    sxbm_to_c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static);
extern void    sxbm2_to_c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static);
extern void    sxbm3_to_c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static);
#ifdef varstr_h
     /* L'appelant ne peut les utiliser que s'il a inclus "varstr.h" */
extern void    sxba2c (SXBA set, FILE *file_descr, char *name1, char *name2, BOOLEAN is_static, VARSTR vstr);
extern void    sxbm2c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static, VARSTR vstr)
     ;
#endif /* varstr_h */

#include "SXpost.h"
#endif /* sxba_h */
