/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/









/* Manipulation de tableaux de bits pour SYNTAX */




#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_OPS2[] = "@(#)SYNTAX - $Id: sxba_ops2.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static SXBA_INDEX	LASTBIT [] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};


SXBA
sxba_copy (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_copy" copies into its first argument its second arguments.
 * It returns its (modified) first argument.
 */
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXBA_ELT	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (SXBASIZE (lhs_bits_array) == SXBASIZE (rhs_bits_array), "copy (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "copy");
    sxbassert ((*(rhs_bits_array+SXNBLONGS (SXBASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "copy");

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
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXBA_ELT	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (SXBASIZE (lhs_bits_array) == SXBASIZE (rhs_bits_array), "minus (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "minus");
    sxbassert ((*(rhs_bits_array+SXNBLONGS (SXBASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "minus");

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- &= ~*rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA_INDEX_OR_ERROR
sxba_1_rlscan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit)
/*
 * "sxba_1_rlscan" returns the index of the first non-null bit preceding
 * "from_bit", or -1 if the head of the array is all zeroes.  If
 * "from_bit" is greater than SXBASIZE, the scan starts at the last bit.
 */
{
    SXBA_INDEX	bit;
    SXBA	bits_ptr;
    SXBA_ELT	elt;
    SXBA_ELT	mask, cur_mask;

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "1_rlscan");

    if (from_bit <= 0)
	return -1;

    if (from_bit < (bit = sxba_cast (SXBASIZE (bits_array))))
	bit = from_bit;

    bits_ptr = bits_array + SXNBLONGS (bit);
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




SXBA_INDEX_OR_ERROR
sxba_0_lrscan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit)
/*
 * "sxba_0_lrscan" returns the index of the first null bit following
 * "from_bit", or -1 if the remainder of the array is all ones.  If
 * "from_bit" is negative, the scan starts at the first bit.
 */
{
    SXBA_INDEX bit = from_bit < 0 ? 0 : (from_bit + 1);
    SXBA	bits_ptr;
    SXBA	last_bits_ptr;
    SXBA_ELT	elt;
    SXBA_INDEX	size;
    SXBA_ELT	mask, cur_mask;
    SXBA_ELT		last_mask;
    static SXBA_INDEX   FIRSTBIT [] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "0_lrscan");

    if (bit >= (size = sxba_cast (SXBASIZE (bits_array))))
	return -1;

    bits_ptr = bits_array + SXNBLONGS (bit + 1);
    last_bits_ptr = bits_array + SXNBLONGS (size);
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


SXBA_INDEX_OR_ERROR
sxba_0_rlscan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit)
/*
 * "sxba_0_rlscan" returns the index of the first null bit preceding
 * "from_bit", or -1 if the head of the array is all zeroes.  If
 * "from_bit" is greater than SXBASIZE, the scan starts at the last bit.
 */
{
    SXBA_INDEX	bit;
    SXBA	bits_ptr;
    SXBA_ELT	elt;
    SXBA_ELT	mask, cur_mask;

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "0_rlscan");

    if (from_bit <= 0)
	return -1;

    if (from_bit < (bit = sxba_cast (SXBASIZE (bits_array))))
	bit = from_bit;

    bits_ptr = bits_array + SXNBLONGS (bit);
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
