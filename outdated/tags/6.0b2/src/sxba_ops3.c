/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1989 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *							  *
   ******************************************************** */





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20070206 17:15 (pb):	        Suppression de sxba_clear qui est equiv */
/*                              a sxba_clear_range et ajout d'assert sur*/
/*                              sxba_range, sxba_clear_range et         */
/*                              sxba_substr                             */
/************************************************************************/
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 07-05-02 15:10 (pb):		Ajout de sxba_clear_range		*/
/************************************************************************/
/* Jeu  2 Dec 1999 18:27(pb):	Ne depend plus de "sxunix.h"		*/
/************************************************************************/
/* 23-09-96 15:31 (pb&phd):	Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* 14-04-94 14:57 (pb):		Ajout de sxba_range			*/
/************************************************************************/
/* 29-05-89 16:23 (pb):		Creation				*/
/************************************************************************/


#include "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_OPS3[] = "@(#)SYNTAX - $Id: sxba_ops3.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;


int
sxba_scan_reset (SXBA bits_array, int from_bit)
/*
 * "sxba_scan_reset" returns the index of the first non-null bit following
 * "from_bit" and reset it, or -1 if the remainder of the array is all zeroes.  If
 * "from_bit" is negative, the scan starts at the first bit.
 */
{
  register int	bit = from_bit < 0 ? 0 : (from_bit + 1);
  register SXBA	bits_ptr;
  register SXBA_ELT	elt;
  register int	size;
  static int	FIRSTBIT [] = {0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0};

  if (bit >= (size = BASIZE (bits_array)))
    return -1;

  bits_ptr = bits_array + NBLONGS (bit + 1);

  if ((elt = *bits_ptr & ((~((SXBA_ELT)0)) << MOD (bit))) == 0) {
    register SXBA	last_bits_ptr = bits_array + NBLONGS (size);

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


#if 0
/* c'est sxba_clear_range () */
SXBA
sxba_clear (SXBA bits_array, int i, int l)
/*
 * "sxba_clear" resets the l bits in bits_array from index i
 */
{
  register SXBA	from_bits_ptr, to_bits_ptr;
  register int	size, j;
    
  sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "clear");

  if (l > 0 && i < (size = BASIZE (bits_array))) {
    if ((j = i + l) > size) {
      to_bits_ptr = bits_array + (l = NBLONGS (size));
      l = l * SXBITS_PER_LONG - i;
    }
    else
      to_bits_ptr = bits_array + (NBLONGS (j));
	
    from_bits_ptr = bits_array + (NBLONGS (i + 1));
	
    i = MOD (i);
	
    if (from_bits_ptr < to_bits_ptr) {
      if (i == 0)
	*from_bits_ptr = ((SXBA_ELT)0), l -= SXBITS_PER_LONG;
      else
	*from_bits_ptr &= ~COMB (i, j = SXBITS_PER_LONG - i), l -= j;
	    
      while (++from_bits_ptr <  to_bits_ptr)
	*from_bits_ptr = ((SXBA_ELT)0), l -= SXBITS_PER_LONG;
	    
      i = 0;
    }
	
    if (l == SXBITS_PER_LONG)
      *from_bits_ptr = ((SXBA_ELT)0);
    else
      *from_bits_ptr &= ~COMB (i, l);
	    
  }

  return bits_array;
}
#endif /* 0 */





SXBA
sxba_range (SXBA bits_array, int i, int l)
/*
 * "sxba_range" sets the l bits in bits_array from index i
 */
{
  SXBA	from_bits_ptr, to_bits_ptr;
  int	        size = BASIZE (bits_array), j;
    
  sxbassert (i >= 0 && l >= 0 && i < size, "range");
  sxbassert ((*(bits_array+NBLONGS (size)) & ((~((SXBA_ELT)0)) << 1 << MOD (size - 1))) == 0, "range");

  if (l > 0) {
    if ((j = i + l) > size) {
      to_bits_ptr = bits_array + (l = NBLONGS (size));
      l = l * SXBITS_PER_LONG - i;
    }
    else
      to_bits_ptr = bits_array + (NBLONGS (j));
	
    from_bits_ptr = bits_array + (NBLONGS (i + 1));
	
    i = MOD (i);
	
    if (from_bits_ptr < to_bits_ptr) {
      if (i == 0)
	*from_bits_ptr = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1)), l -= SXBITS_PER_LONG;
      else
	*from_bits_ptr |= COMB (i, j = SXBITS_PER_LONG - i), l -= j;
	    
      while (++from_bits_ptr <  to_bits_ptr)
	*from_bits_ptr = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1)), l -= SXBITS_PER_LONG;
	    
      i = 0;
    }
	
    if (l == SXBITS_PER_LONG)
      *from_bits_ptr = ~((~((SXBA_ELT)0)) << 1 << (SXBITS_PER_LONG - 1));
    else
      *from_bits_ptr |= COMB (i, l);
	    
  }

  return bits_array;
}


SXBA
sxba_clear_range (SXBA bits_array, int i, int l)
/*
 * "sxba_range" resets the l bits in bits_array from index i
 */
{
  SXBA	from_bits_ptr, to_bits_ptr;
  int	        size = BASIZE (bits_array), j;
    
  sxbassert (i >= 0 && l >= 0 && i < size, "clear_range");
  sxbassert ((*(bits_array+NBLONGS (size)) & ((~((SXBA_ELT)0)) << 1 << MOD (size - 1))) == 0, "clear_range");

  if (l > 0) {
    if ((j = i + l) > size) {
      to_bits_ptr = bits_array + (l = NBLONGS (size));
      l = l * SXBITS_PER_LONG - i;
    }
    else
      to_bits_ptr = bits_array + (NBLONGS (j));
	
    from_bits_ptr = bits_array + (NBLONGS (i + 1));
	
    i = MOD (i);
	
    if (from_bits_ptr < to_bits_ptr) {
      if (i == 0)
	*from_bits_ptr = (SXBA_ELT)0L, l -= SXBITS_PER_LONG;
      else
	*from_bits_ptr &= ~COMB (i, j = SXBITS_PER_LONG - i), l -= j;
	    
      while (++from_bits_ptr <  to_bits_ptr)
	*from_bits_ptr = (SXBA_ELT)0L, l -= SXBITS_PER_LONG;
	    
      i = 0;
    }
	
    if (l == SXBITS_PER_LONG)
      *from_bits_ptr = (SXBA_ELT)0L;
    else
      *from_bits_ptr &= ~COMB (i, l);
	    
  }

  return bits_array;
}


SXBA
sxba_substr (SXBA bits_array1, SXBA bits_array2, int i1, int i2, int l)
/*
 * "sxba_substr" copies the l bits from bits_array2 starting at index i2
 * in bits_array1 at index i1
 */
{
  SXBA	from_bits_ptr1, from_bits_ptr2;
  int		size1 = BASIZE (bits_array1), size2 = BASIZE (bits_array2), j;
    
  sxbassert (size1 == size2, "substr (unequal sizes)");
  sxbassert (i1 >= 0 && i2 >= 0 && l >= 0 && i1 < size1 && i2 < size2, "substr");
  sxbassert ((*(bits_array1+NBLONGS (size1)) & ((~((SXBA_ELT)0)) << 1 << MOD (size1 - 1))) == 0, "substr");
  sxbassert ((*(bits_array2+NBLONGS (size2)) & ((~((SXBA_ELT)0)) << 1 << MOD (size2 - 1))) == 0, "substr");

  if (l > 0) {
    sxba_clear_range (bits_array1, i1, l);

    if (i2 + l > size2)
      l = size2 - i2;

    if (i1 + l > size1)
      l = size1 - i1;
	    
    from_bits_ptr1 = bits_array1 + (NBLONGS (i1 + 1));	    
    i1 = MOD (i1);
    from_bits_ptr2 = bits_array2 + (NBLONGS (i2 + 1));	    
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
