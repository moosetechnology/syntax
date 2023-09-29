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
/* 23-09-96 15:56 (phd&pb):	Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* 29-03-90 15:45 (phd&pb):	Correction des decalages gauches de	*/
/*				SXBITS_PER_LONG				*/
/************************************************************************/
/* 28-12-87 17:18 (phd):	Documentation				*/
/************************************************************************/
/* 23-12-87 12:53 (phd):	Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxba_ops [] = "@(#)sxba_ops.c\t- SYNTAX [unix] - Lun 23 Sep 1996 15:56:30";
#endif


#include "sxunix.h"

#define SXBA_IMPLEMENTATION
#include "sxba.h"

SXBA	sxba_and (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * "sxba_and" puts into its first argument the result of the bitwise
 * "AND" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- &= *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA	sxba_or (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * "sxba_or" puts into its first argument the result of the bitwise
 * "OR" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- |= *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA	sxba_xor (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * "sxba_xor" puts into its first argument the result of the bitwise
 * "XOR" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- ^= *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA	sxba_not (bits_array)
    SXBA	bits_array;
/*
 * "sxba_not" inverts all significant bits of its argument and returns
 * it, modified.
 */
{
    register SXBA	bits_ptr;
    register int	slices_number;
    register int	size;

    bits_ptr = bits_array + (slices_number = NBLONGS (size = BASIZE (bits_array)));

    if ((size = MOD (size)) != 0) {
	*bits_ptr-- ^= ~((~((SXBA_ELT)0)) << size), slices_number--;
    }

    while (slices_number-- > 0) {
	*bits_ptr-- ^= ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    }

    return bits_array;
}

/*---------------------------------------------------------------------------*/
#ifdef sun
/*
 * pour que strip_names ne signale pas de probleme avec ce module (dont
 * aucune fonction n'appelle une autre fonction), on declare une fonction
 * fictive, qui ne sera jamais appelee, mais qui en appelle une autre
 */
static int strip_name_function ()
{
     exit (0);
}
#endif
/*---------------------------------------------------------------------------*/

BOOLEAN		sxba_is_empty (bits_array)
    register SXBA	bits_array;
/*
 * "sxba_is_empty" returns "FALSE" if any bit is set in its argument,
 * "TRUE" otherwise.
 */
{
    register SXBA	bits_ptr = bits_array + NBLONGS (BASIZE (bits_array));

    while (bits_ptr > bits_array) {
	if (*bits_ptr-- != ((SXBA_ELT)0)) {
	    return FALSE;
	}
    }

    return TRUE;
}



BOOLEAN		sxba_is_full (bits_array)
    SXBA	bits_array;
/*
 * "sxba_is_full" returns "FALSE" if any bit is reset in its argument,
 * "TRUE" otherwise.
 */
{
    register SXBA	bits_ptr;
    register int	slices_number;
    register int	size;

    bits_ptr = bits_array + (slices_number = NBLONGS (size = BASIZE (bits_array)));

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



int	sxba_first_difference (bits_array_1, bits_array_2)
    register SXBA	bits_array_1, bits_array_2;
/*
 * "sxba_first_difference" returns the index of the first bit which is
 * set in one of its arguments and reset in the other, or -1 if its
 * arguments hold the same bits.
 */
{
    register int	slices_number;
    register SXBA	start_1;

    slices_number = NBLONGS (BASIZE (start_1 = bits_array_1));

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
