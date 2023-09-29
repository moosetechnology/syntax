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
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* Jeu 19 Avr 2001 13:34 (pb):	sxbm3_to_c ; NULL si ensemble vide	*/
/************************************************************************/
/* 23-09-96 15:56 (phd&pb):	Bug detecte sur l'alpha pour les 64 bits*/
/*				les "constantes" doivent etre typees	*/
/************************************************************************/
/* Jul 25 1996 14:41 (pb):	Ajout de sxbm2_to_c			*/
/************************************************************************/
/* 15-02-94 18:17 (pb):		Ne depend plus de "sxunix.h"		*/
/************************************************************************/
/* 29-03-90 15:45 (phd&pb):	Remplacement de (-1) par (~0)		*/
/************************************************************************/
/* 09-05-88 11:54 (pb):		Creation				*/
/************************************************************************/


#include "sxcommon.h"
#include "sxba.h"
#include <stdio.h>
#include <string.h>

char WHAT_SXBITSMATRIX[] = "@(#)SYNTAX - $Id: sxbitsmatrix.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

SXBA
*sxbm_calloc (int lines_number, int bits_number)
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

    bits_matrix = (SXBA*) sxcalloc ((unsigned long int)lines_number, sizeof (SXBA));
    bits_array = (SXBA) sxcalloc ((unsigned long int) (elem_index = (lines_number * (line_size = NBLONGS (bits_number) + 1))), sizeof (SXBA_ELT));

    while (--lines_number >= 0) {
	BASIZE (bits_matrix [lines_number] = &bits_array [elem_index -= line_size]) = bits_number;
    }

    return bits_matrix;
}



SXBA
*sxbm_resize (SXBA *bits_matrix, int old_lines_number, int new_lines_number, int new_bits_number)
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

    sxinitialise(old_ba); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(min_slice_number); /* pour faire taire "gcc -Wuninitialized" */
    if ((has_changed = new_lines_number != old_lines_number)) {
	bits_matrix = (SXBA*) sxrealloc (bits_matrix, new_lines_number, sizeof (SXBA));
    }

    if ((has_changed = has_changed || new_slices_number != old_slices_number)) {
	old_ba = bits_matrix [0];
	bits_array = (SXBA) sxcalloc ((unsigned long int)(new_lines_number * (new_slices_number + 1)), sizeof (SXBA_ELT));
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



void
sxbm_free (SXBA *bits_matrix)
/*
 * "sxbm_free" frees the two memory zone in which the bits array matrix is allocated
 */
{
    sxfree (bits_matrix [0]);
    sxfree (bits_matrix);
}


BOOLEAN
sxbm_write (int file, SXBA *bits_matrix, int line_nb)
{
    int i;
    BOOLEAN result = TRUE;

    for (i = 0; result && (i < line_nb); i++)
	result = sxba_write (file, bits_matrix [i]);

    return result;
}

BOOLEAN
sxbm_read (int file, SXBA *bits_matrix, int line_nb)
{
    int i;
    BOOLEAN result = TRUE;

    for (i = 0; result && (i < line_nb); i++)
	result = sxba_read (file, bits_matrix [i]);

    return result;
}


void
sxbm_to_c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static)
{
    char	*prefix, str [10];
    int		i, l1, l2;

    l2 = (l1 = strlen (name1)) + strlen (name2);
    prefix = (char *) sxalloc (l2 + 6 /* strlen ("_line_") */ + 1 /* \nul */, sizeof (char));
    strcpy (prefix, name1);
    strcpy (prefix + l1, name2);
    strcpy (prefix + l2, "_line_");
		      
    for (i = 0; i < line_nb; i++)
    {
	sprintf (str, "%d", i);
	sxba_to_c (bits_matrix [i], file_descr, prefix, str, TRUE);
    }

    fprintf (file_descr, "\n\n%s SXBA %s%s [%i] = {\n", is_static ? "static " : "",
	     name1, name2, line_nb);

    for (i = 0; i < line_nb; i++)
    {
	sprintf (str, "%d", i);
	fprintf (file_descr, "%s%s,\n", prefix, str);
    }

    fprintf (file_descr, "} /* End %s%s */;\n", name1, name2);

    sxfree (prefix);
}

void
sxbm2_to_c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static)
{
    /* Genere un SXBA_ELT [] [] et non un sxbm */
    int	i, lgth, nbmot;

#if SXBITS_PER_LONG!=32 && SXBITS_PER_LONG!=64
    sxtrap ("sxbitsmatrix", "sxbm2_to_c");
#endif

    lgth = BASIZE (bits_matrix [0]);
    nbmot = NBLONGS (lgth);
    
    fprintf (file_descr, "\n\
%s SXBA_ELT %s%s [%i]\n\
", is_static ? "static " : "",
	     name1, name2, line_nb);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%i] = {\n\
",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 nbmot
#else /* On est sur une machine 64 bits */
	 2*nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	fprintf (file_descr, "/* %i */ ", i);
	sxba32_to_c (file_descr, bits_matrix [i]);
	fprintf (file_descr, ",\n\
");
    }

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "};\n\
#else\n\
 [%i] = {\n\
",
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	 nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	fprintf (file_descr, "/* %i */ ", i);
	sxba64_to_c (file_descr, bits_matrix [i]);
	fprintf (file_descr, ",\n\
");
    }

    fprintf (file_descr, "};\n\
#endif\n\
/* End %s%s */\n\
", name1, name2);
}

void
sxbm3_to_c (FILE *file_descr, SXBA *bits_matrix, int line_nb, char *name1, char *name2, BOOLEAN is_static)
{
    /* genere un SXBM dont les SXBA egaux sont partages */
    int		i, j, lgth, nbmot;
    int		*repr;
    SXBA	linei;

#if SXBITS_PER_LONG!=32 && SXBITS_PER_LONG!=64
    sxtrap ("sxbitsmatrix", "sxbm3_to_c");
#endif

    repr = (int*) sxalloc (line_nb, sizeof (int));

    for (i = 0; i < line_nb; i++) {
	if ((linei = bits_matrix [i])) {
	    for (j = 0; j < i; j++) {
		if (j == repr [j] && sxba_first_difference (linei, bits_matrix [j]) == -1)
		    break;
	    }
	}
	else
	    j = -1;

	repr [i] = j < i ? j : i;
    }

    lgth = BASIZE (bits_matrix [0]);
    nbmot = NBLONGS (lgth);
    
    /* On genere pour une machine 32 bits */
    fputs ("#if SXBITS_PER_LONG==32 \n", file_descr);

    for (i = 0; i < line_nb; i++) {
	if (repr [i] == i) {
	    fprintf (file_descr, "%s SXBA_ELT %s%s_%i [%i] = ",
		     is_static ? "static " : "",
		     name1,
		     name2,
		     i,
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
		     nbmot+1
#else /* On est sur une machine 64 bits */
		     2*nbmot+1
#endif
		     );
	    sxba32_to_c (file_descr, bits_matrix [i]);
	    fputs (";\n", file_descr);
	}
    }
	
    fputs ("#else\n", file_descr);
    /* On genere pour une machine 64 bits */

    for (i = 0; i < line_nb; i++) {
	if (repr [i] == i) {
	    fprintf (file_descr, "%s SXBA_ELT %s%s_%i [%i] = ",
		     is_static ? "static " : "",
		     name1,
		     name2,
		     i,
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
		     (nbmot+1)/2+1
#else /* On est sur une machine 64 bits */
		     nbmot+1
#endif
		     );
	    sxba64_to_c (file_descr, bits_matrix [i]);
	    fputs (";\n", file_descr);
	}
    }
	
    fputs ("#endif\n", file_descr);

    fprintf (file_descr, "\n\n%s SXBA %s%s [%i] = {", is_static ? "static " : "",
	     name1, name2, line_nb);

    for (i = 0; i < line_nb; i++)
    {
	if (i%16 == 0)
	    fprintf (file_descr, "\n\
/* %i */ ", i);
		
	if (repr [i] < 0)
	    fputs ("NULL, ", file_descr);
	else
	    fprintf (file_descr, "%s%s_%i, ", name1, name2, repr [i]);
    }

    fprintf (file_descr, "} /* End %s%s */;\n", name1, name2);

    sxfree (repr);
}

