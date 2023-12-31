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
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *							  *
   ******************************************************** */





/* Manipulation de tableaux de bits pour SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 23-09-96 15:31 (pb):		Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* 22-02-94 12:08 (phd):	Correction d'un decalage dans 0_lrscan	*/
/************************************************************************/
/* 15-02-94 18:15 (pb):		Ne depend plus de "sxunix.h"		*/
/************************************************************************/
/* 25-06-90 18:10 (pb):		Bugs ds les ...scan			*/
/************************************************************************/
/* 29-03-90 15:45 (phd&pb):	Correction des decalages gauches de	*/
/*				SXBITS_PER_LONG				*/
/************************************************************************/
/* 31-05-88 13:24 (pb):		Creation				*/
/************************************************************************/


#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_OPS2[] = "@(#)SYNTAX - $Id: sxba_ops2.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

static int	LASTBIT [] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};


SXBA
sxba_copy (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_copy" copies into its first argument its second arguments.
 * It returns its (modified) first argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (BASIZE (lhs_bits_array) == BASIZE (rhs_bits_array), "copy (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "copy");
    sxbassert ((*(rhs_bits_array+NBLONGS (BASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "copy");

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- = *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA
sxba_minus (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_minus" puts into its first argument the result of the bitwise
 * "MINUS" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (BASIZE (lhs_bits_array) == BASIZE (rhs_bits_array), "minus (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "minus");
    sxbassert ((*(rhs_bits_array+NBLONGS (BASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "minus");

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- &= ~*rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



int
sxba_1_rlscan (SXBA bits_array, int from_bit)
/*
 * "sxba_1_rlscan" returns the index of the first non-null bit preceding
 * "from_bit", or -1 if the head of the array is all zeroes.  If
 * "from_bit" is greater than BASIZE, the scan starts at the last bit.
 */
{
    register int	bit;
    register SXBA	bits_ptr;
    register SXBA_ELT	elt;
    register long	mask, cur_mask;

    sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "1_rlscan");

    if (from_bit <= 0)
	return -1;

    if (from_bit < (bit = BASIZE (bits_array)))
	bit = from_bit;

    bits_ptr = bits_array + NBLONGS (bit);
    cur_mask = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    mask = ~((~((SXBA_ELT)0)) << 1 << MOD (bit - 1));
    
    while ((elt = *bits_ptr & mask) == 0) {
	if (--bits_ptr == bits_array)
	    return -1;

	mask = cur_mask;
    }

    bit = MUL (bits_ptr - bits_array - 1);

    while ((elt & ~((SXBA_ELT)0xFFFF)) != 0) {
	elt >>= 16;
	bit += 16;
    }

    if ((elt & 0xFF00) != 0) {
	elt >>= 8;
	bit += 8;
    }

    if ((elt & 0xF0) != 0) {
	elt >>= 4;
	bit += 4;
    }

    return bit + LASTBIT [elt & 0xF];
}




int
sxba_0_lrscan (SXBA bits_array, int from_bit)
/*
 * "sxba_0_lrscan" returns the index of the first null bit following
 * "from_bit", or -1 if the remainder of the array is all ones.  If
 * "from_bit" is negative, the scan starts at the first bit.
 */
{
    register int	bit = from_bit < 0 ? 0 : (from_bit + 1);
    register SXBA	bits_ptr;
    register SXBA	last_bits_ptr;
    register SXBA_ELT	elt;
    register int	size;
    register long	mask, cur_mask;
    long		last_mask;
    static int	FIRSTBIT [] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

    sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "0_lrscan");

    if (bit >= (size = BASIZE (bits_array)))
	return -1;

    bits_ptr = bits_array + NBLONGS (bit + 1);
    last_bits_ptr = bits_array + NBLONGS (size);
    last_mask = ~((~((SXBA_ELT)0)) << 1 << MOD (size - 1));
    cur_mask = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    mask = ((~((SXBA_ELT)0)) << MOD (bit)) & ((bits_ptr == last_bits_ptr) ? last_mask : cur_mask);

    while ((elt = (*bits_ptr & mask) ^ mask) == 0) {
	if (bits_ptr == last_bits_ptr)
	    return -1;

	mask = ++bits_ptr == last_bits_ptr ? last_mask : cur_mask;
    }

    bit = MUL (bits_ptr - bits_array - 1);

    while ((elt & 0xFFFF) == 0) {
	elt >>= 16;
	bit += 16;
    }

    if ((elt & 0xFF) == 0) {
	elt >>= 8;
	bit += 8;
    }

    if ((elt & 0xF) == 0) {
	elt >>= 4;
	bit += 4;
    }

    return bit + FIRSTBIT [elt & 0xF];
}


int
sxba_0_rlscan (SXBA bits_array, int from_bit)
/*
 * "sxba_0_rlscan" returns the index of the first null bit preceding
 * "from_bit", or -1 if the head of the array is all zeroes.  If
 * "from_bit" is greater than BASIZE, the scan starts at the last bit.
 */
{
    register int	bit;
    register SXBA	bits_ptr;
    register SXBA_ELT	elt;
    register long	mask, cur_mask;

    sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "0_rlscan");

    if (from_bit <= 0)
	return -1;

    if (from_bit < (bit = BASIZE (bits_array)))
	bit = from_bit;

    bits_ptr = bits_array + NBLONGS (bit);
    cur_mask = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    mask = ~((~((SXBA_ELT)0)) << 1 << MOD (bit - 1));

    while ((elt = (*bits_ptr & mask) ^ mask) == 0) {
	if (--bits_ptr == bits_array)
	    return -1;

	mask = cur_mask;
    }

    bit = MUL (bits_ptr - bits_array - 1);

    while ((elt & ~((SXBA_ELT)0xFFFF)) != 0) {
	elt >>= 16;
	bit += 16;
    }

    if ((elt & 0xFF00) != 0) {
	elt >>= 8;
	bit += 8;
    }

    if ((elt & 0xF0) != 0) {
	elt >>= 4;
	bit += 4;
    }

    return bit + LASTBIT [elt & 0xF];
}
