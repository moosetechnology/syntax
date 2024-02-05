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
/* ------------------------- Include File XxYxZ.h ------------------------- */

#ifndef XxYxZ_inter
#define XxYxZ_inter

/* Interface of the Abstract Data Type XxYxZ */

#include	"XxYxZ_rep.h"

/* This package can manage a ternary relation in NxNxN. */


/* PROCEDURES & FUNCTIONS */



extern void			XxYxZ_alloc (XxYxZ_header *header, 
					     char *name, 
					     SXINT init_elem_nb, 
					     SXINT average_list_nb_per_bucket, 
					     SXINT *average_XYZ_assoc, 
					     void (*user_oflw) (SXINT, SXINT), 
					     FILE *stat_file);
/* Allocates an extensible structure such that :
   - "header" is a pointer to its header (defined by the caller).
   - its name is "name".
   - "init_elem_nb" is the expected number of triples.
   - "average_list_nb_per_bucket" is the expected mean length of a collision list.
   - "average_XYZ_assoc" is a pointeur to an array of 6 integers:
              index          value           meaning
	      XYZ_X	       v	     if v is 0 XxYxZ_Xforeach is disabled
	      XYZ_Y	       v	     if v is 0 XxYxZ_Yforeach is disabled
	      XYZ_Z	       v	     if v is 0 XxYxZ_Zforeach is disabled
	      XYZ_XY	       v	     if v is 0 XxYxZ_XYforeach is disabled
	      XYZ_XZ	       v	     if v is 0 XxYxZ_XZforeach is disabled
	      XYZ_YZ	       v	     if v is 0 XxYxZ_YZforeach is disabled
   - "oflw" is a user's fonction called each time an overflow occurs in the structure.
      It allows to keep a concurrently growing structure.
   - "stat_file", if not NULL, is an output file on which statistics about the usage
     of the current structure will be printed.
*/

extern void			XxYxZ_clear (XxYxZ_header *header);
/* Clears the structure without changing its size. */

extern void			XxYxZ_stat (FILE *stat_file, XxYxZ_header *header);
/* Prints on "stat_file" some statistics. */

extern void			XxYxZ_free (XxYxZ_header *header);
/* Frees the structure. */

extern void			XxYxZ_gc (XxYxZ_header *header);
/* Allows the "XxYxZ_erased" elements to be reused. */

extern void			XxYxZ_lock (XxYxZ_header *header);
/* Frozes the structure to a minimum size. */

extern void			XxYxZ_unlock (XxYxZ_header *header);
/* Afterwards, the structure can grow... */

extern SXINT			XxYxZ_is_set (XxYxZ_header *header, 
					      SXINT X, 
					      SXINT Y, 
					      SXINT Z);
/* If the triple ("X", "Y", "Z") is already stored it returns its index
   (a strictly positive integer) else it returns 0. */

extern bool			XxYxZ_set (XxYxZ_header *header, 
					   SXINT X, 
					   SXINT Y, 
					   SXINT Z, 
					   SXINT *ref);
/* Put the triple ("X", "Y", "Z") in the structure and returns its index
   (a strictly positive integer). Moreover it returns true if the triple
   ("X", "Y", "Z") is already stored else it returns false. */


extern bool			XxYxZ_write (XxYxZ_header *header, 
					     sxfiledesc_t F_XxYxZ /* file descriptor */);
/* Write on "file" the structure "*header" */

extern bool			XxYxZ_read (XxYxZ_header *header, 
					    sxfiledesc_t F_XxYxZ /* file descriptor */, 
					    char *name, 
					    void (*user_oflw) (SXINT, SXINT), 
					    FILE *stat_file);
/* Read from "file" the structure "*header". "name" and "stat_file" may
   be reset. */



extern void XxYxZ_to_c (
			XxYxZ_header *header, 
			FILE *F_XxYxZ, 
			char *name, 
			bool local_is_static);

/* Print on "F_X" the C text for the structure "header" */

extern void			XxYxZ_array_to_c (XxYxZ_header *header, 
						  FILE *F_XxYxZ /* named output stream */, 
						  char *name);

/* Print on "F_X" the C text for the "arrays" of "header" */

extern void			XxYxZ_header_to_c (XxYxZ_header *header, 
						   FILE *F_XxYxZ /* named output stream */, 
						   char *name);

/* Print on "F_X" the C text for the "header" of "header" */

extern void			XxYxZ_reuse (XxYxZ_header *header, 
					     char *name, 
					     void (*user_oflw) (SXINT, SXINT), 
					     FILE *stat_file);

/* Allow for the (re)use of the structure "header" based upon a C text */

/* MACROS */

#define XxYxZ_erase(h,x)	X_root_erase(h,x)
/* Erases the couple in structure "h" whose index is "x". */
/* WARNING: "h" is used several times in the macro expansion. */

#define XxYxZ_is_erased(h,x)	X_root_is_erased(h,x)
/* Is true if the triple whose index is "x" has been "XxYxZ_erased". */

#define XxYxZ_is_static(h)	X_root_is_static(h)
/* Is true iff the whole structure is defined statically. */

#define XxYxZ_is_allocated(h)	X_root_is_allocated(h)
/* Is true iff the structure is allocated. */

#define XxYxZ_size(h)		X_root_size(h)
/* Is the current size of the structure (last index available). */

#define XxYxZ_top(h)		X_root_top(h)
/* Is the higher index ever returned.
   CONDITION: XxYxZ_top(h) <= XxYxZ_size(h). */

#define XxYxZ_X(h,x)		X_root_X(h,x)
/* Is the X_element of the triple whose index is "x". */

#define XxYxZ_Y(h,x)		X_root_Y(h,x)
/* Is the Y_element of the triple whose index is "x". */

#define XxYxZ_Z(h,x)		X_root_Z(h,x)
/* Is the Z_element of the triple whose index is "x". */

#define XxYxZ_total_sizeof(h)		X_root_XYZtotal_sizeof(h)
/* Size in bytes of the whole structures (without sizeof (h)) */

#define XxYxZ_used_sizeof(h)		X_root_XYZused_sizeof(h)
/* Size in bytes of the whole structures (without sizeof (h) actually used) */

/* LOOP STATEMENTS */
/* Same usage as a for loop, "x" must be a variable, it takes successively all
   the "good" indexes. */
/* Break and continue C statements can be used inside foreach loops. */	

#define XxYxZ_foreach(h,x)	X_root_foreach(h,x)
/* Walks upon all the valid elements (not "XxY_erased") of the structure. */

#define XxYxZ_Xforeach(h,X,x)	X_root_Xforeach(h,X,x)
/* Walks upon all valid triples whose first element is "X". */

#define XxYxZ_Yforeach(h,Y,x)	X_root_Yforeach(h,Y,x)
/* Walks upon all valid triples whose second element is "Y". */

#define XxYxZ_Zforeach(h,Z,x)	X_root_Zforeach(h,Z,x)
/* Walks upon all valid triples whose second element is "Z". */

#define XxYxZ_XYforeach(h,X,Y,x)	X_root_XYforeach(h,X,Y,x)
/* Walks upon all valid triples whose first element is "X"
   and second element is "Y". */

#define XxYxZ_XZforeach(h,X,Z,x)	X_root_XZforeach(h,X,Z,x)
/* Walks upon all valid triples whose first element is "X"
   and second element is "Z". */


#define XxYxZ_YZforeach(h,Y,Z,x)	X_root_YZforeach(h,Y,Z,x)
/* Walks upon all valid triples whose first element is "Y"
   and second element is "Z". */

#endif


