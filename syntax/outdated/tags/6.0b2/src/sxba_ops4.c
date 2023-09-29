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





/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20050119 16:16 (phd):	Création d'après pb			*/
/************************************************************************/

#include "sxcommon.h"

char WHAT_SXBA_OPS4[] = "@(#)SYNTAX - $Id: sxba_ops4.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

#include "sxba.h"


BOOLEAN
sxba_bit_is_reset_set (SXBA bits_array, int bit)
/*
 * "sxba_bit_is_reset_set" returns "FALSE" if the bit numbered "bit" is set
 * in "bits_array", sets it and returns "TRUE" otherwise.
 */
{
    SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;
    SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);

    sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "bit_is_reset_set");
    sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "bit_is_reset_set");

    return (*bits_ptr & elt) ? 0 : (*bits_ptr |= elt, 1);
}



BOOLEAN
sxba_bit_is_set_reset (SXBA bits_array, int bit)
/*
 * "sxba_bit_is_set_reset" returns "FALSE" if the bit numbered "bit" is reset
 * in "bits_array", resets it and returns "TRUE" otherwise.
 */
{
    SXBA	bits_ptr = bits_array + DIV (bit) + 1 ;
    SXBA_ELT	elt = ((SXBA_ELT)1) << MOD (bit);

    sxbassert ((*(bits_array+NBLONGS (BASIZE (bits_array))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (bits_array) - 1))) == 0, "bit_is_set_reset");
    sxbassert (bit >= 0 && (unsigned)bit < BASIZE (bits_array), "bit_is_set_reset");

    return (*bits_ptr & elt) ? (*bits_ptr &= ~elt, 1) : 0;
}
