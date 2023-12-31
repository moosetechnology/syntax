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





/* Manipulation de tableaux de bits pour SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* Jeu 19 Avr 2001 14:48(pb):	Ajout de sxba_is_subset ()		*/
/************************************************************************/
/* 23-09-96 15:31 (pb):		Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* 15-02-94 18:14 (pb):		Ne depend plus de "sxunix.h"		*/
/************************************************************************/
/* 29-03-90 15:45 (phd&pb):	Correction des decalages gauches de	*/
/*				SXBITS_PER_LONG				*/
/************************************************************************/
/* 28-12-87 17:18 (phd):	Documentation				*/
/************************************************************************/
/* 23-12-87 12:53 (phd):	Creation				*/
/************************************************************************/


#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_OPS[] = "@(#)SYNTAX - $Id: sxba_ops.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

SXBA
sxba_and (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_and" puts into its first argument the result of the bitwise
 * "AND" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (BASIZE (lhs_bits_array) == BASIZE (rhs_bits_array), "and (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "and");
    sxbassert ((*(rhs_bits_array+NBLONGS (BASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "and");

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- &= *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}

SXBA
sxba_or (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_or" puts into its first argument the result of the bitwise
 * "OR" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (BASIZE (lhs_bits_array) == BASIZE (rhs_bits_array), "or (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "or");
    sxbassert ((*(rhs_bits_array+NBLONGS (BASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "or");

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- |= *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA
sxba_xor (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_xor" puts into its first argument the result of the bitwise
 * "XOR" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (BASIZE (lhs_bits_array) == BASIZE (rhs_bits_array), "xor (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "xor");
    sxbassert ((*(rhs_bits_array+NBLONGS (BASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "xor");

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- ^= *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA
sxba_not (SXBA bits_array)
/*
 * "sxba_not" inverts all significant bits of its argument and returns
 * it, modified.
 */
{
    register SXBA	bits_ptr;
    register int	slices_number;
    register int	size;

    bits_ptr = bits_array + (slices_number = NBLONGS (size = BASIZE (bits_array)));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "not");

    if ((size = MOD (size)) != 0) {
	*bits_ptr-- ^= ~((~((SXBA_ELT)0)) << size), slices_number--;
    }

    while (slices_number-- > 0) {
	*bits_ptr-- ^= ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    }

    return bits_array;
}



BOOLEAN
sxba_is_empty (SXBA bits_array)
/*
 * "sxba_is_empty" returns "FALSE" if any bit is set in its argument,
 * "TRUE" otherwise.
 */
{
    register SXBA	bits_ptr = bits_array + NBLONGS (BASIZE (bits_array));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "is_empty");

    while (bits_ptr > bits_array) {
	if (*bits_ptr-- != ((SXBA_ELT)0)) {
	    return FALSE;
	}
    }

    return TRUE;
}



BOOLEAN
sxba_is_full (SXBA bits_array)
/*
 * "sxba_is_full" returns "FALSE" if any bit is reset in its argument,
 * "TRUE" otherwise.
 */
{
    register SXBA	bits_ptr;
    register int	slices_number;
    register int	size;

    bits_ptr = bits_array + (slices_number = NBLONGS (size = BASIZE (bits_array)));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "is_full");

    if ((size = MOD (size)) != 0) {
	if (*bits_ptr-- != ~((~((SXBA_ELT)0)) << size)) {
	    return FALSE;
	}

	slices_number--;
    }

    while (slices_number-- > 0) {
	if (*bits_ptr-- != ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1))) {
	    return FALSE;
	}
    }

    return TRUE;
}



int
sxba_first_difference (SXBA bits_array_1, SXBA bits_array_2)
/*
 * "sxba_first_difference" returns the index of the first bit which is
 * set in one of its arguments and reset in the other, or -1 if its
 * arguments hold the same bits.
 */
{
    register int	slices_number;
    register SXBA	start_1;

    slices_number = NBLONGS (BASIZE (start_1 = bits_array_1));

    sxbassert (BASIZE (bits_array_1) == BASIZE (bits_array_2), "first_difference (unequal sizes)");
    sxbassert ((*(bits_array_1+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array_1) - 1))) == 0, "first_difference");
    sxbassert ((*(bits_array_2+NBLONGS (BASIZE (bits_array_2))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array_2) - 1))) == 0, "first_difference");

    while (slices_number-- > 0) {
	if (*++bits_array_1 != *++bits_array_2) {
	    SXBA_ELT	xor [2];

	    xor [0] = SXBITS_PER_LONG /* no need to be finer */ ;
	    xor [1] = *bits_array_1 ^ *bits_array_2;
	    return MUL (bits_array_1 - start_1 - 1) + sxba_scan (xor, -1);
	}
    }

    return -1;
}


BOOLEAN
sxba_is_subset (SXBA bits_array_1, SXBA bits_array_2)
/*
 * "sxba_is_subset" returns TRUE iff bits_array_1 is a subset of bits_array_2.
 */
{
    int	slices_number = NBLONGS (BASIZE (bits_array_1));

    sxbassert (BASIZE (bits_array_1) == BASIZE (bits_array_2), "is_subset (unequal sizes)");
    sxbassert ((*(bits_array_1+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array_1) - 1))) == 0, "is_subset");
    sxbassert ((*(bits_array_2+NBLONGS (BASIZE (bits_array_2))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array_2) - 1))) == 0, "is_subset");

    bits_array_1 += slices_number;
    bits_array_2 += slices_number;

    while (slices_number-- > 0) {
	if ((*bits_array_1-- & ~(*bits_array_2--)) != 0)
	    return FALSE;  
    }

    return TRUE;
}
