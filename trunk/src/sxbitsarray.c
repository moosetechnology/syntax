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

char WHAT_SXBITSARRAY[] = "@(#)SYNTAX - $Id: sxbitsarray.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#include "sxba.h"


SXBA
sxba_calloc (SXBA_INDEX bits_number)
/*
 * "sxba_calloc" allocates a memory zone suitable for holding
 * "bits_number" bits, which are all initialized to zero.  It returns
 * a pointer to that zone.
 */
{
    SXBA	bits_array;

    bits_array = (SXBA) sxcalloc ((SXUINT) SXNBLONGS (bits_number) + 1, sizeof (SXBA_ELT));
    SXBASIZE (bits_array) = bits_number;
    return bits_array;
}



SXBA
sxba_resize (SXBA bits_array, SXBA_INDEX new_bits_number)
/*
 * "sxba_resize" reallocates the existing "bits_array", so that it may
 * afterwards hold "new_bits_number" bits.  If the new number of bits
 * is greater than the old one, the bits that become allocated are
 * reset.  The bits belonging to both the old and the new arrays are
 * not changed.
 */
{
    SXBA_INDEX	old_bits_number   = sxba_cast (SXBASIZE (bits_array));
    SXBA_INDEX	old_slices_number = SXNBLONGS (old_bits_number);
    SXBA_INDEX	new_slices_number = SXNBLONGS (new_bits_number);

    sxbassert ((*(bits_array+old_slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (old_bits_number - 1))) == 0, "resize");

    SXBASIZE (bits_array) = new_bits_number;

    if (new_slices_number != old_slices_number) {
	bits_array = (SXBA) sxrealloc (bits_array, new_slices_number + 1, sizeof (SXBA_ELT));

	if (new_slices_number > old_slices_number) {
	    do {
		bits_array [++old_slices_number] = ((SXBA_ELT)0);
	    } while (old_slices_number < new_slices_number);

	    return bits_array;
	}
    }
    else if (old_bits_number <= new_bits_number) {
	return bits_array;
    }


/* Must pad last slice with zeroes */

    if (MOD (new_bits_number) != 0) {
	bits_array [new_slices_number] &= ~((~((SXBA_ELT)0)) << MOD (new_bits_number));
    }

    return bits_array;
}



SXBA
sxba_empty (SXBA bits_array)
/*
 * sxba_empty resets all bits of "bits_array".
 */
{
    SXBA	bits_ptr = bits_array + SXNBLONGS (SXBASIZE (bits_array));


    while (bits_ptr > bits_array) {
	*bits_ptr-- = ((SXBA_ELT)0);
    }

    return bits_array;
}



SXBA
sxba_fill (SXBA bits_array)
/*
 * sxba_fill sets all bits of "bits_array".
 */
{
    SXBA	bits_ptr;
    SXBA_INDEX	size = sxba_cast (SXBASIZE (bits_array));

    bits_ptr = bits_array + SXNBLONGS (size);


    if ((size = MOD (size)) != 0) {
	*bits_ptr-- = ~((~((SXBA_ELT)0)) << size);
    }

    while (bits_ptr > bits_array) {
	*bits_ptr-- = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    }

    return bits_array;
}



SXBA
sxba_0_bit (SXBA bits_array, SXBA_INDEX bit)
/*
 * "sxba_0_bit" resets the bit numbered "bit" in "bits_array".
 */
{
    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "0_bit");
    sxbassert (bit >= 0 && (unsigned)bit < SXBASIZE (bits_array), "0_bit");

    bits_array [DIV (bit) + 1] &= ~(((SXBA_ELT)1) << MOD (bit));
    return bits_array;
}



SXBA
sxba_1_bit (SXBA bits_array, SXBA_INDEX
 bit)
/*
 * "sxba_1_bit" sets the bit numbered "bit" in "bits_array".
 */
{
    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "1_bit");
    sxbassert (bit >= 0 && (unsigned)bit < SXBASIZE (bits_array), "1_bit");

    bits_array [DIV (bit) + 1] |= (((SXBA_ELT)1) << MOD (bit));
    return bits_array;
}



bool
sxba_bit_is_set (SXBA bits_array, SXBA_INDEX bit)
/*
 * "sxba_bit_is_set" returns "true" if the bit numbered "bit" is set
 * in "bits_array", "false" otherwise.
 */
{
    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "bit_is_set");
    sxbassert (bit >= 0 && (unsigned)bit < SXBASIZE (bits_array), "bit_is_set");

    return (bits_array [DIV (bit) + 1] & (((SXBA_ELT)1) << MOD (bit))) != 0;
}



SXBA_INDEX
sxba_cardinal (SXBA bits_array)
/*
 * "sxba_cardinal" returns the number of bits which are set in
 * "bits_array".
 */
{
    SXBA_ELT	elt;
    SXBA_INDEX	result = 0;
    SXBA	bits_ptr = bits_array + SXNBLONGS (SXBASIZE (bits_array));

    sxbassert ((*bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "cardinal");

    while (bits_ptr > bits_array) {
	if ((elt = *bits_ptr--) != 0) {
	    do {
		static SXINT	NBBITS [256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2
		     , 3, 3, 4, 3, 4, 4, 5, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4,
		     4, 5, 4, 5, 5, 6, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5
		     , 4, 5, 5, 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5,
		     6, 6, 7, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5
		     , 6, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		     2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 3, 4
		     , 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8};

		if ((elt & 0xFFFF) == 0) {
		    elt >>= 16;
		}

		result += NBBITS [elt & 0xFF];
	    } while ((elt >>= 8) != 0);
	}
    }

    return result;
}



SXBA_INDEX_OR_ERROR
sxba_scan (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit)
/*
 * "sxba_scan" returns the index of the first non-null bit following
 * "from_bit", or -1 if the remainder of the array is all zeroes.  If
 * "from_bit" is negative, the scan starts at the first bit.
 */
{
    SXBA_INDEX	bit = from_bit < 0 ? 0 : (from_bit + 1);
    SXBA	bits_ptr;
    SXBA_ELT	elt;
    SXBA_INDEX	size;
    static   SXBA_INDEX	FIRSTBIT [] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "scan");

    if (bit >= (size = sxba_cast (SXBASIZE (bits_array))))
	return -1;

    bits_ptr = bits_array + SXNBLONGS (bit + 1);

    if ((elt = *bits_ptr & ((~((SXBA_ELT)0)) << MOD (bit))) == 0) { /* s'il n'y a pas de 1 dans le mot courant au del� de l� o� on cherche */
	SXBA	last_bits_ptr = bits_array + SXNBLONGS (size);

	do { /* on va voir les mots suivants */
	    if (bits_ptr == last_bits_ptr) {
	      return -1; /* si on arrive au bout sans trouver de 1, on rend -1 */
	    }
	} while (*++bits_ptr == 0);

	elt = *bits_ptr; /* si on trouve un 1 dans un des mots suivants, elt est ce mot */
    }
    /* elt est le premier mot contenant un bit non nul, sachant que les zones en de�a de l� o� on cherche ont �t� z�rois�es */
    bit = MUL (bits_ptr - bits_array - 1);
    /* on cherche par "dichotomie" qui est le premier bit concern� dans le mot trouv� */
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
