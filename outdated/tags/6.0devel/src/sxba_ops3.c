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











#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_OPS3[] = "@(#)SYNTAX - $Id: sxba_ops3.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;


SXBA_INDEX_OR_ERROR
sxba_scan_reset (SXBA bits_array, SXBA_INDEX_OR_ERROR from_bit)
/*
 * "sxba_scan_reset" returns the index of the first non-null bit following
 * "from_bit" and reset it, or -1 if the remainder of the array is all zeroes.  If
 * "from_bit" is negative, the scan starts at the first bit.
 */
{
  SXBA_INDEX	bit = from_bit < 0 ? 0 : (from_bit + 1);
  SXBA	bits_ptr;
  SXBA_ELT	elt;
  SXBA_INDEX   size;
  static   SXBA_INDEX	FIRSTBIT [] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

  if (bit >= (size = sxba_cast (SXBASIZE (bits_array))))
    return -1;

  bits_ptr = bits_array + SXNBLONGS (bit + 1);

  if ((elt = *bits_ptr & ((~((SXBA_ELT)0)) << MOD (bit))) == 0) {
    SXBA	last_bits_ptr = bits_array + SXNBLONGS (size);

    do {
      if (bits_ptr == last_bits_ptr) {
	return -1;
      }
    } while (*++bits_ptr == 0);

    elt = *bits_ptr;
  }

  bit = 0;

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

  bit += FIRSTBIT [elt & 0xF];
  *bits_ptr &= ~(((SXBA_ELT)1) << bit);
  bit += MUL (bits_ptr - bits_array - 1);

  sxbassert (bit >= 0 && bit < size, "scan_reset");

  return bit;
}







SXBA
sxba_range (SXBA bits_array, SXBA_INDEX i, SXBA_INDEX l)
/*
 * "sxba_range" sets the l bits in bits_array from index i
 */
{
  SXBA	from_bits_ptr, to_bits_ptr;
  SXBA_INDEX	        size = sxba_cast (SXBASIZE (bits_array)), j;
    
  sxbassert (i >= 0 && l >= 0 && i < size, "range");
  sxbassert ((*(bits_array+SXNBLONGS (size)) & ((~((SXBA_ELT)0)) << 1 << MOD (size - 1))) == 0, "range");

  if (l > 0) {
    if ((j = i + l) > size) {
      to_bits_ptr = bits_array + (l = SXNBLONGS (size));
      l = l * SXBITS_PER_LONG - i;
    }
    else
      to_bits_ptr = bits_array + (SXNBLONGS (j));
	
    from_bits_ptr = bits_array + (SXNBLONGS (i + 1));
	
    i = MOD (i);
	
    if (from_bits_ptr < to_bits_ptr) {
      if (i == 0)
	*from_bits_ptr = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1)), l -= SXBITS_PER_LONG;
      else
	*from_bits_ptr |= SXCOMB (i, j = SXBITS_PER_LONG - i), l -= j;
	    
      while (++from_bits_ptr <  to_bits_ptr)
	*from_bits_ptr = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1)), l -= SXBITS_PER_LONG;
	    
      i = 0;
    }
	
    if (l == SXBITS_PER_LONG)
      *from_bits_ptr = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    else
      *from_bits_ptr |= SXCOMB (i, l);
	    
  }

  return bits_array;
}


SXBA
sxba_clear_range (SXBA bits_array, SXBA_INDEX i, SXBA_INDEX l)
/*
 * "sxba_range" resets the l bits in bits_array from index i
 */
{
  SXBA	from_bits_ptr, to_bits_ptr;
  SXBA_INDEX	        size = sxba_cast (SXBASIZE (bits_array)), j;
    
  sxbassert (i >= 0 && l >= 0 && i < size, "clear_range");
  sxbassert ((*(bits_array+SXNBLONGS (size)) & ((~((SXBA_ELT)0)) << 1 << MOD (size - 1))) == 0, "clear_range");

  if (l > 0) {
    if ((j = i + l) > size) {
      to_bits_ptr = bits_array + (l = SXNBLONGS (size));
      l = l * SXBITS_PER_LONG - i;
    }
    else
      to_bits_ptr = bits_array + (SXNBLONGS (j));
	
    from_bits_ptr = bits_array + (SXNBLONGS (i + 1));
	
    i = MOD (i);
	
    if (from_bits_ptr < to_bits_ptr) {
      if (i == 0)
	*from_bits_ptr = (SXBA_ELT)0L, l -= SXBITS_PER_LONG;
      else
	*from_bits_ptr &= ~SXCOMB (i, j = SXBITS_PER_LONG - i), l -= j;
	    
      while (++from_bits_ptr <  to_bits_ptr)
	*from_bits_ptr = (SXBA_ELT)0L, l -= SXBITS_PER_LONG;
	    
      i = 0;
    }
	
    if (l == SXBITS_PER_LONG)
      *from_bits_ptr = (SXBA_ELT)0L;
    else
      *from_bits_ptr &= ~SXCOMB (i, l);
	    
  }

  return bits_array;
}


SXBA
sxba_substr (SXBA bits_array1, SXBA bits_array2, SXBA_INDEX i1, SXBA_INDEX i2, SXBA_INDEX l)
/*
 * "sxba_substr" copies the l bits from bits_array2 starting at index i2
 * in bits_array1 at index i1
 */
{
  SXBA	from_bits_ptr1, from_bits_ptr2;
  SXBA_INDEX  size1 = sxba_cast (SXBASIZE (bits_array1));
  SXBA_INDEX  size2 = sxba_cast (SXBASIZE (bits_array2));
  SXBA_INDEX  j;
    
  sxbassert (size1 == size2, "substr (unequal sizes)");
  sxbassert (i1 >= 0 && i2 >= 0 && l >= 0 && i1 < size1 && i2 < size2, "substr");
  sxbassert ((*(bits_array1+SXNBLONGS (size1)) & ((~((SXBA_ELT)0)) << 1 << MOD (size1 - 1))) == 0, "substr");
  sxbassert ((*(bits_array2+SXNBLONGS (size2)) & ((~((SXBA_ELT)0)) << 1 << MOD (size2 - 1))) == 0, "substr");

  if (l > 0) {
    sxba_clear_range (bits_array1, i1, l);

    if (i2 + l > size2)
      l = size2 - i2;

    if (i1 + l > size1)
      l = size1 - i1;
	    
    from_bits_ptr1 = bits_array1 + (SXNBLONGS (i1 + 1));	    
    i1 = MOD (i1);
    from_bits_ptr2 = bits_array2 + (SXNBLONGS (i2 + 1));	    
    i2 = MOD (i2);

    while (l > 0) {
      if ((j = SXBITS_PER_LONG - i2 - l) < 0)
	j = 0;

      *from_bits_ptr1 |= (((*from_bits_ptr2 << j) >> (j + i2)) << i1);

      if (i2 == i1) {
	from_bits_ptr1++;
	from_bits_ptr2++;
	l -= SXBITS_PER_LONG - i1;
	i1 = 0;
	i2 = 0;
      }
      else
	if (i2 < i1) {
	  i2 += SXBITS_PER_LONG - i1;
	  l -= SXBITS_PER_LONG - i1;
	  from_bits_ptr1++;
	  i1 = 0;
	}
	else {
	  i1 += SXBITS_PER_LONG - i2;
	  l -= SXBITS_PER_LONG - i2;
	  from_bits_ptr2++;
	  i2 = 0;
	}
    }
  }

  return bits_array1;
}
