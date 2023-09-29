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


#include "sxversion.h"
#include "sxcommon.h"

char WHAT_SXBA_OPS4[] = "@(#)SYNTAX - $Id: sxba_ops4.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;

#include "sxba.h"


SXBOOLEAN
sxba_bit_is_reset_set (SXBA bits_array, SXBA_INDEX bit)
/*
 * "sxba_bit_is_reset_set" returns "SXFALSE" if the bit numbered "bit" is set
 * in "bits_array", sets it and returns "SXTRUE" otherwise.
 */
{
    SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;
    SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "bit_is_reset_set");
    sxbassert (bit >= 0 && (unsigned)bit < SXBASIZE (bits_array), "bit_is_reset_set");

    return (*bits_ptr & elt) ? 0 : (*bits_ptr |= elt, 1);
}



SXBOOLEAN
sxba_bit_is_set_reset (SXBA bits_array, SXBA_INDEX bit)
/*
 * "sxba_bit_is_set_reset" returns "SXFALSE" if the bit numbered "bit" is reset
 * in "bits_array", resets it and returns "SXTRUE" otherwise.
 */
{
    SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;
    SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);

    sxbassert ((*(bits_array+SXNBLONGS (SXBASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (SXBASIZE (bits_array) - 1))) == 0, "bit_is_set_reset");
    sxbassert (bit >= 0 && (unsigned)bit < SXBASIZE (bits_array), "bit_is_set_reset");

    return (*bits_ptr & elt) ? (*bits_ptr &= ~elt, 1) : 0;
}
