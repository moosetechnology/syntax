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
/* ------------------------- Include File X_root.h ------------------------- */

#ifndef X_root_inter
#define X_root_inter

/* Interface of the Abstract Data Type X_root */

#include	"X_root_rep.h"

/* This package is the basis for the X[xY[xZ]] managers. */


/* PROCEDURES & FUNCTIONS */

extern void X_root_alloc (X_root_header *header_ptr, 
		      char *name, 
		      SXINT init_elem_nb, 
		      SXINT average_list_nb_per_bucket, 
		      void (*system_oflw) (SXINT),
		      void (*user_oflw) (SXINT, SXINT), /* i.e., sxoflw0_t */
		      SXINT (*cmp) (SXINT), 
		      SXUINT (*scrmbl) (SXINT), 
		      SXINT (*suppress) (SXINT), 
		      SXINT (*assign) (SXINT), 
		      FILE *stat_file);
/* Allocates an extensible structure such that :
   - "header_ptr" is a pointer to its header (defined by the caller).
   - its name is "name".
   - "init_elem_nb" is the expected number of elements.
   - "average_list_nb_per_bucket" is the expected mean length of a collision list.
   - "system_oflw" is a user's fonction called each time an overflow occurs in
     the structure.
     It allows to manage the structure where the "X[xY[xZ]]" are kept.
   - "user_oflw" is a user's fonction called each time an overflow occurs in
     the structure.
      It allows to keep a concurrently growing structure.
   - "cmp" is the fonction used to compare "X[xY[xZ]]" elements.
   - "scrmbl" is used to [re]compute the initial hash-code value of a "X[xY[xZ]]"
     element when an overflow occurs.
   - "suppress" is used to manage the "foreach" lists when the buckets used to store
     elements which have been erased are put into the free list.
   - "assign" is the fonction used to assign "X[xY[xZ]]" elements.
   - "stat_file", if not NULL, is an output file on which statistics about the usage
     of the current structure will be printed.
*/
   

extern void			X_root_clear (X_root_header *header_ptr);
/* Clears the structure without changing its size. */

extern void			X_root_stat (FILE *stat_file,
					     X_root_header *header_ptr);
/* Prints on "stat_file" some statistics. */

extern void			X_root_free (X_root_header *header_ptr);
/* Frees the structure. */

extern SXINT			X_root_gc (X_root_header *header_ptr);
/* Allows the "X_root_erased" elements to be reused. */

extern void			X_root_lock (X_root_header *header_ptr);
/* Frozes the structure to a minimum size. */

extern SXINT			X_root_is_set (X_root_header *header_ptr, SXUINT X);
/* If the element "X" is already stored it returns its index (a strictly positive
   integer) else it returns 0. */

extern SXBOOLEAN			X_root_set (X_root_header *header, 
					    SXUINT scramble, 
					    SXINT *ref);
/* Put the element "X" in the structure and returns its index (a strictly positive
   integer). Moreover it returns SXTRUE if the element "X" is already stored else
   it returns SXFALSE. */

extern SXBOOLEAN			X_root_write (X_root_header *header, sxfiledesc_t F_X_root /* file descriptor */);
/* Write on "file" the structure "*header_ptr" */

extern SXBOOLEAN			X_root_read (X_root_header *header, sxfiledesc_t F_X_root /* file descriptor */);
/* Read from "file" the structure "*header_ptr" */

extern void			X_root_to_c (X_root_header *header_ptr,
					     FILE *F_X,
					     char *name,
					     SXBOOLEAN is_static);

/* Print on "F_X" the C text for the structure "header_ptr" */

extern void			X_root_array_to_c (X_root_header *header, 
						   FILE *F_X /* named output stream */, 
						   char *name);

/* Print on "F_X" the C text for the "arrays" of "header_ptr" */

extern void			X_root_header_to_c (X_root_header *header, 
						    FILE *F_X /* named output stream */, 
						    char *name);

/* Print on "F_X" the C text for the "header" of "header_ptr" */

extern void			X_root_reuse (X_root_header *header, 
					      char *name, 
					      void (*system_oflw) (SXINT), 
					      void (*user_oflw) (SXINT, SXINT), 
					      SXINT (*cmp) (SXINT), 
					      SXUINT (*scrmbl) (SXINT), 
					      SXINT (*suppress) (SXINT), 
					      SXINT (*assign) (SXINT), 
					      FILE *stat_file);

/* Allow for the (re)use of the structure "header_ptr" based upon a C text */

extern void			X_root_int_cpy (SXINT *old, SXINT *new, SXINT old_size);

/* Perform  new_ptr [0:old_size] = old_ptr [0:old_size] */
#endif

