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

/* Manipulation de matrices de bits pour SYNTAX */

#include "sxversion.h"
#include "sxcommon.h"
#include "sxba.h"
#include <stdio.h>
#include <string.h>

char WHAT_SXBITSMATRIX[] = "@(#)SYNTAX - $Id: sxbitsmatrix.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

SXBA
*sxbm_calloc (SXBA_INDEX lines_number, SXBA_INDEX bits_number)
/*
 * "sxbm_calloc" allocates two memory zone. 
 * The first holding "lines_number" pointers to bitsarrays of size
 * "bits_number" bits, which are all initialized to zero and contained in the second zone.
 *  It returns a pointer to the first zone.
 */
{
    SXBA	*bits_matrix;
    SXBA	bits_array;
    SXINT	elem_index, line_size;

    bits_matrix = (SXBA*) sxcalloc ((SXUINT)lines_number, sizeof (SXBA));
    bits_array = (SXBA) sxcalloc ((SXUINT) (elem_index = (lines_number * (line_size = SXNBLONGS (bits_number) + 1))), sizeof (SXBA_ELT));

    while (--lines_number >= 0) {
	SXBASIZE (bits_matrix [lines_number] = &bits_array [elem_index -= line_size]) = bits_number;
    }

    return bits_matrix;
}



SXBA
*sxbm_resize (SXBA *bits_matrix, SXBA_INDEX old_lines_number, SXBA_INDEX new_lines_number, SXBA_INDEX new_bits_number)
/*
 * "sxbm_resize" reallocates the existing "bits_matrix", so that it may
 * afterwards hold "new_lines_number" of "new_bits_number" bits each.
 *  If the new number of bits or the new number of lines
 * are greater than the old one, the bits that become allocated are
 * reset.  The bits belonging to both the old and the new arrays are
 * not changed.
 */
{
    SXBA_INDEX	old_bits_number = sxba_cast (SXBASIZE (bits_matrix [0]));
    SXBA_INDEX	new_slices_number = SXNBLONGS (new_bits_number);
    SXBA_INDEX	line, slice;
    SXBA_INDEX	old_slices_number = SXNBLONGS (old_bits_number);
    bool	has_changed;
    SXBA_INDEX	min_slice_number;
    SXBA	old_bits_array, new_bits_array, bits_array, old_ba;

    sxinitialise(old_ba); /* pour faire taire "gcc -Wuninitialized" */
    sxinitialise(min_slice_number); /* pour faire taire "gcc -Wuninitialized" */
    if ((has_changed = new_lines_number != old_lines_number)) {
	bits_matrix = (SXBA*) sxrealloc (bits_matrix, new_lines_number, sizeof (SXBA));
    }

    if ((has_changed = has_changed || new_slices_number != old_slices_number)) {
	old_ba = bits_matrix [0];
	bits_array = (SXBA) sxcalloc ((SXUINT)(new_lines_number * (new_slices_number + 1)), sizeof (SXBA_ELT));
	min_slice_number = (new_slices_number < old_slices_number) ? new_slices_number : old_slices_number;
    }
    else
	bits_array = bits_matrix [0];

    for (line = 0, new_bits_array = bits_array; line < new_lines_number; line++, new_bits_array += new_slices_number + 1)
	 {
	SXBASIZE (new_bits_array) = new_bits_number;

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


bool
sxbm_write (sxfiledesc_t file, SXBA *bits_matrix, SXBA_INDEX line_nb)
{
    SXBA_INDEX i;
    bool result = true;

    for (i = 0; result && (i < line_nb); i++)
	result = sxba_write (file, bits_matrix [i]);

    return result;
}

bool
sxbm_read (sxfiledesc_t file, SXBA *bits_matrix, SXBA_INDEX line_nb)
{
    SXBA_INDEX i;
    bool result = true;

    for (i = 0; result && (i < line_nb); i++)
	result = sxba_read (file, bits_matrix [i]);

    return result;
}


void
sxbm_to_c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, bool is_static)
{
    char	*prefix, str [20];
    SXBA_INDEX		i;
    size_t              l1, l2;

    l1 = strlen (name1);
    l2 = l1 + strlen (name2);
    prefix = (char *) sxalloc (l2 + 6 /* strlen ("_line_") */ + 1 /* \nul */, sizeof (char));
    strcpy (prefix, name1);
    strcpy (prefix + l1, name2);
    strcpy (prefix + l2, "_line_");
		      
    for (i = 0; i < line_nb; i++)
    {
	sprintf (str, "%ld", (SXINT) i);
	sxba_to_c (bits_matrix [i], file_descr, prefix, str, true);
    }

    fprintf (file_descr, "\n\n%s SXBA %s%s [%li] = {\n", is_static ? "static " : "",
	     name1, name2, (SXINT) line_nb);

    for (i = 0; i < line_nb; i++)
    {
	sprintf (str, "%ld", (SXINT) i);
	fprintf (file_descr, "%s%s,\n", prefix, str);
    }

    fprintf (file_descr, "} /* End %s%s */;\n", name1, name2);

    sxfree (prefix);
}

void
sxbm2_to_c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, bool is_static)
{
    /* Genere un SXBA_ELT [] [] et non un sxbm */
    SXBA_INDEX	i, lgth, nbmot;

#if SXBITS_PER_LONG!=32 && SXBITS_PER_LONG!=64
    sxtrap ("sxbitsmatrix", "sxbm2_to_c");
#endif

    lgth = sxba_cast (SXBASIZE (bits_matrix [0]));
    nbmot = SXNBLONGS (lgth);
    
    fprintf (file_descr, "\n\
%s SXBA_ELT %s%s [%li]\n\
", is_static ? "static " : "",
	     name1, name2, (SXINT) line_nb);

    /* On genere pour une machine 32 bits */
    fprintf (file_descr, "#if SXBITS_PER_LONG==32\n\
 [%li] = {\n\
",  (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 nbmot
#else /* On est sur une machine 64 bits */
	 2*nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	fprintf (file_descr, "/* %li */ ", (SXINT) i);
	sxba32_to_c (file_descr, bits_matrix [i]);
	fprintf (file_descr, ",\n\
");
    }

    /* On genere pour une machine 64 bits */
    fprintf (file_descr, "};\n\
#else\n\
 [%li] = {\n\
",  (SXINT)
#if SXBITS_PER_LONG==32 /* On est sur une machine 32 bits */
	 (nbmot+1)/2
#else /* On est sur une machine 64 bits */
	 nbmot
#endif
+1);

    for (i = 0; i < line_nb; i++) {
	fprintf (file_descr, "/* %li */ ", (SXINT) i);
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
sxbm3header_to_c (SXBA_INDEX *repr, FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2)
{
  SXBA_INDEX	i, j, lgth, nbmot;
  SXBA	        linei;

#if SXBITS_PER_LONG!=32 && SXBITS_PER_LONG!=64
  sxtrap ("sxbitsmatrix", "sxbm3header_to_c");
#endif

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

  lgth = sxba_cast (SXBASIZE (bits_matrix [0]));
  nbmot = SXNBLONGS (lgth);
    
  /* On genere pour une machine 32 bits */
  fputs ("#if SXBITS_PER_LONG==32 \n", file_descr);

  for (i = 0; i < line_nb; i++) {
    if (repr [i] == i) {
      fprintf (file_descr, "static SXBA_ELT %s%s_%li [%li] = ",
	       name1,
	       name2,
	       (SXINT) i, (SXINT)
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
      fprintf (file_descr, "static SXBA_ELT %s%s_%li [%li] = ",
	       name1,
	       name2,
	       (SXINT) i,  (SXINT)
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
}



void
sxbm3core_to_c (SXBA_INDEX *repr, FILE *file_descr, SXBA_INDEX line_nb, char *name1, char *name2, bool is_static)
{
  SXBA_INDEX		i;

  fprintf (file_descr, "\n\n%s SXBA %s%s [%li] = {", is_static ? "static " : "",
	   name1, name2, (SXINT) line_nb);

  for (i = 0; i < line_nb; i++) {
    if (i%16 == 0)
      fprintf (file_descr, "\n\
/* %li */ ", (SXINT) i);
		
    if (repr [i] < 0)
      fputs ("NULL, ", file_descr);
    else
      fprintf (file_descr, "%s%s_%li, ", name1, name2, (SXINT) repr [i]);
  }

  fprintf (file_descr, "} /* End %s%s */;\n", name1, name2);
}

void
sxbm3_to_c (FILE *file_descr, SXBA *bits_matrix, SXBA_INDEX line_nb, char *name1, char *name2, bool is_static)
{
  /* genere un SXBM dont les SXBA egaux sont partages */
  SXBA_INDEX		*repr;

  repr = (SXBA_INDEX*) sxalloc (line_nb, sizeof (SXBA_INDEX));

  sxbm3header_to_c (repr, file_descr, bits_matrix, line_nb, name1, name2);

  sxbm3core_to_c (repr, file_descr, line_nb, name1, name2, is_static);

  sxfree (repr);
}

void
sxbm_empty (SXBA *bits_matrix, SXBA_INDEX line_nb)
{
  SXBA_INDEX bits_number, i;
  size_t sxbm_global_size;

  bits_number = SXBASIZE (bits_matrix [0]);

  sxbm_global_size = (/* line_size */ SXNBLONGS (bits_number) + 1) * line_nb * sizeof (SXBA_ELT); /* en octets */

  memset (bits_matrix[0], 0, sxbm_global_size);

  for (i = 0; i < line_nb; i++)
    bits_matrix [i][0] = bits_number;
}
