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
/* 23-09-96 15:56 (phd&pb):	Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivenr etre typees	*/
/************************************************************************/
/* 22-02-94 12:08 (phd):	Correction d'un decalage dans 0_lrscan	*/
/************************************************************************/
/* 25-06-90 18:10 (pb):		Bugs ds les ...scan			*/
/************************************************************************/
/* 29-03-90 15:45 (phd&pb):	Correction des decalages gauches de	*/
/*				SXBITS_PER_LONG				*/
/************************************************************************/
/* 31-05-88 13:24 (pb):		Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxba_ops2 [] = "@(#)sxba_ops2.c\t- SYNTAX [unix] - Lun 23 Sep 1996 15:56:30";
#endif

#include "sxunix.h"
#include "sxba.h"

static int	LASTBIT [] = {0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3};


SXBA	sxba_copy (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * "sxba_copy" copies into its first argument its second arguments.
 * It returns its (modified) first argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- = *rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



SXBA	sxba_minus (lhs_bits_array, rhs_bits_array)
    SXBA	lhs_bits_array, rhs_bits_array;
/*
 * "sxba_and" puts into its first argument the result of the bitwise
 * "MINUS" of its two arguments.  It returns its (modified) first
 * argument.
 */
{
    register SXBA	lhs_bits_ptr, rhs_bits_ptr;
    register int	slices_number = NBLONGS (BASIZE (lhs_bits_array));

    lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

    while (slices_number-- > 0) {
	*lhs_bits_ptr-- &= ~*rhs_bits_ptr--;
    }

    return lhs_bits_array;
}



int	sxba_1_rlscan (bits_array, from_bit)
    SXBA	bits_array;
    int		from_bit;
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

    while ((elt & ~0xFFFF) != 0) {
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



int	sxba_0_lrscan (bits_array, from_bit)
    SXBA	bits_array;
    int		from_bit;
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


int	sxba_0_rlscan (bits_array, from_bit)
    SXBA	bits_array;
    int		from_bit;
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

    while ((elt & ~0xFFFF) != 0) {
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
