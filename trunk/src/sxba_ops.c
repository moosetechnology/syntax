/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2023 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://sourcesup.renater.fr/projects/syntax
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.info
 *****************************************************************************/









/* Manipulation de tableaux de bits pour SYNTAX */




#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_OPS[] = "@(#)SYNTAX - $Id: sxba_ops.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

SXBA
sxba_and (SXBA lhs_bits_array, SXBA rhs_bits_array)
/*
 * "sxba_and" puts into its first argument the result of the bitwise
 * "AND" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXBA_ELT	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (SXBASIZE (lhs_bits_array) == SXBASIZE (rhs_bits_array), "and (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "and");
    sxbassert ((*(rhs_bits_array+SXNBLONGS (SXBASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "and");

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
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXBA_ELT	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (SXBASIZE (lhs_bits_array) == SXBASIZE (rhs_bits_array), "or (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "or");
    sxbassert ((*(rhs_bits_array+SXNBLONGS (SXBASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "or");

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
    SXBA	lhs_bits_ptr, rhs_bits_ptr;
    SXBA_ELT	slices_number = SXNBLONGS (SXBASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    sxbassert (SXBASIZE (lhs_bits_array) == SXBASIZE (rhs_bits_array), "xor (unequal sizes)");
    sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (lhs_bits_array) - 1))) == 0, "xor");
    sxbassert ((*(rhs_bits_array+SXNBLONGS (SXBASIZE (rhs_bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (rhs_bits_array) - 1))) == 0, "xor");

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
    SXBA	bits_ptr;
    SXBA_ELT	slices_number;
    SXBA_ELT	size;

    bits_ptr = bits_array + (slices_number = SXNBLONGS (size = SXBASIZE (bits_array)));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "not");

    if ((size = MOD (size)) != 0) {
	*bits_ptr-- ^= ~((~((SXBA_ELT)0)) << size), slices_number--;
    }

    while (slices_number-- > 0) {
	*bits_ptr-- ^= ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    }

    return bits_array;
}



bool
sxba_is_empty (SXBA bits_array)
/*
 * "sxba_is_empty" returns "false" if any bit is set in its argument,
 * "true" otherwise.
 */
{
    SXBA	bits_ptr = bits_array + SXNBLONGS (SXBASIZE (bits_array));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "is_empty");

    while (bits_ptr > bits_array) {
	if (*bits_ptr-- != ((SXBA_ELT)0)) {
	    return false;
	}
    }

    return true;
}



bool
sxba_is_full (SXBA bits_array)
/*
 * "sxba_is_full" returns "false" if any bit is reset in its argument,
 * "true" otherwise.
 */
{
    SXBA	bits_ptr;
    SXBA_ELT	slices_number;
    SXBA_ELT	size;

    bits_ptr = bits_array + (slices_number = SXNBLONGS (size = SXBASIZE (bits_array)));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "is_full");

    if ((size = MOD (size)) != 0) {
	if (*bits_ptr-- != ~((~((SXBA_ELT)0)) << size)) {
	    return false;
	}

	slices_number--;
    }

    while (slices_number-- > 0) {
	if (*bits_ptr-- != ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1))) {
	    return false;
	}
    }

    return true;
}



SXBA_INDEX_OR_ERROR
sxba_first_difference (SXBA bits_array_1, SXBA bits_array_2)
/*
 * "sxba_first_difference" returns the index of the first bit which is
 * set in one of its arguments and reset in the other, or -1 if its
 * arguments hold the same bits.
 */
{
    SXBA_ELT	slices_number;
    SXBA	start_1;

    slices_number = SXNBLONGS (SXBASIZE (start_1 = bits_array_1));

    sxbassert (SXBASIZE (bits_array_1) == SXBASIZE (bits_array_2), "first_difference (unequal sizes)");
    sxbassert ((*(bits_array_1+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array_1) - 1))) == 0, "first_difference");
    sxbassert ((*(bits_array_2+SXNBLONGS (SXBASIZE (bits_array_2))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array_2) - 1))) == 0, "first_difference");

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


bool
sxba_is_subset (SXBA bits_array_1, SXBA bits_array_2)
/*
 * "sxba_is_subset" returns true iff bits_array_1 is a subset of bits_array_2.
 */
{
    SXBA_ELT slices_number = SXNBLONGS (SXBASIZE (bits_array_1));

    sxbassert (SXBASIZE (bits_array_1) == SXBASIZE (bits_array_2), "is_subset (unequal sizes)");
    sxbassert ((*(bits_array_1+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array_1) - 1))) == 0, "is_subset");
    sxbassert ((*(bits_array_2+SXNBLONGS (SXBASIZE (bits_array_2))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array_2) - 1))) == 0, "is_subset");

    bits_array_1 += slices_number;
    bits_array_2 += slices_number;

    while (slices_number-- > 0) {
	if ((*bits_array_1-- & ~(*bits_array_2--)) != 0)
	    return false;  
    }

    return true;
}

/* function sxba_cast() is always exported by the library even if the lint
   preprocessor macro and the EBUG preprocessor macro are not defined 
   (this is needed for applications compiled with EBUG) */

#if defined (lint) || EBUG
/* nothing to do */
#else
#undef sxba_cast
#endif

SXBA_INDEX sxba_cast (SXBA_ELT x)
{
  /* no sanity check because the 2 types have the same bit-width */
  /* LINTED */
  return (SXBA_INDEX)(x);
}


