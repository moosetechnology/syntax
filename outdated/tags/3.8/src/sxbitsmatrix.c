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





/* Manipulation de matrices de bits pour SYNTAX */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 23-09-96 15:56 (phd&pb):	Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* 29-03-90 15:45 (phd&pb):	Remplacement de (-1) par (~0)		*/
/************************************************************************/
/* 09-05-88 11:54 (pb):		Creation				*/
/************************************************************************/

#ifndef lint
char	what_sxbitsmatrix [] = "@(#)sxbitsmatrix.c\t- SYNTAX [unix] - Lun 23 Sep 1996 15:56:30";
#endif


#include "sxunix.h"
#include "sxba.h"


SXBA	*sxbm_calloc (lines_number, bits_number)
    register int	lines_number;
    int		bits_number;
/*
 * "sxbm_calloc" allocates two memory zone. 
 * The first holding "lines_number" pointers to bitsarrays of size
 * "bits_number" bits, which are all initialized to zero and contained in the second zone.
 *  It returns a pointer to the first zone.
 */
{
    register SXBA	*bits_matrix;
    register SXBA	bits_array;
    register long	elem_index, line_size;

    bits_matrix = (SXBA*) sxcalloc (lines_number, sizeof (SXBA));
    bits_array = (SXBA) sxcalloc ((elem_index = (lines_number * (line_size = NBLONGS (bits_number) + 1))), sizeof (
	 SXBA_ELT));

    while (--lines_number >= 0) {
	BASIZE (bits_matrix [lines_number] = &bits_array [elem_index -= line_size]) = bits_number;
    }

    return bits_matrix;
}



SXBA	*sxbm_resize (bits_matrix, old_lines_number, new_lines_number, new_bits_number)
    SXBA	*bits_matrix;
    int		old_lines_number, new_lines_number, new_bits_number;
/*
 * "sxbm_resize" reallocates the existing "bits_matrix", so that it may
 * afterwards hold "new_lines_number" of "new_bits_number" bits each.
 *  If the new number of bits or the new number of lines
 * are greater than the old one, the bits that become allocated are
 * reset.  The bits belonging to both the old and the new arrays are
 * not changed.
 */
{
    register int	old_bits_number = BASIZE (bits_matrix [0]);
    register int	new_slices_number = NBLONGS (new_bits_number);
    register int	line, slice;
    register int	old_slices_number = NBLONGS (old_bits_number);
    register BOOLEAN	has_changed;
    register int	min_slice_number;
    register SXBA	old_bits_array, new_bits_array, bits_array, old_ba;

    if ((has_changed = new_lines_number != old_lines_number)) {
	bits_matrix = (SXBA*) sxrealloc (bits_matrix, new_lines_number, sizeof (SXBA));
    }

    if ((has_changed = has_changed || new_slices_number != old_slices_number)) {
	old_ba = bits_matrix [0];
	bits_array = (SXBA) sxcalloc (new_lines_number * (new_slices_number + 1), sizeof (SXBA_ELT));
	min_slice_number = (new_slices_number < old_slices_number) ? new_slices_number : old_slices_number;
    }
    else
	bits_array = bits_matrix [0];

    for (line = 0, new_bits_array = bits_array; line < new_lines_number; line++, new_bits_array += new_slices_number + 1)
	 {
	BASIZE (new_bits_array) = new_bits_number;

	if (line < old_lines_number) {
	    if (has_changed) {
		old_bits_array = bits_matrix [line];

		for (slice = min_slice_number; slice > 0; slice--) {
		    new_bits_array [slice] = old_bits_array [slice];
		}

		bits_matrix [line] = new_bits_array;
	    }

	    if (old_bits_number > new_bits_number) {
		/* Must pad last slice with zeroes */

		if (MOD (new_bits_number) != 0) {
		    new_bits_array [new_slices_number] &= ~((~((SXBA_ELT)0)) << MOD (new_bits_number));
		}
	    }
	}
	else
	    bits_matrix [line] = new_bits_array;
    }

    if (has_changed)
	sxfree (old_ba);

    return bits_matrix;
}



SXVOID	sxbm_free (bits_matrix)
    SXBA	*bits_matrix;
/*
 * "sxbm_free" frees the two memory zone in which the bits array matrix is allocated
 */
{
    sxfree (bits_matrix [0]);
    sxfree (bits_matrix);
}


