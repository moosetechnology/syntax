/* ------------------------- Include File XxY.h ------------------------- */

#ifndef XxY_inter
#define XxY_inter

/* Interface of the Abstract Data Type XxY */

#include	"XxY_rep.h"

/* This package can manage a binary relation in NxN. */


/* PROCEDURES & FUNCTIONS */

extern void			XxY_alloc (/* XxY_header *header_ptr;
					      char *name;
					      int init_elem_nb;
					      int average_list_nb_per_bucket;
					      int average_X_assoc;
					      int average_Y_assoc;
					      int (*oflw)();
					      FILE *stat_file; */);
/* Allocates an extensible structure such that :
   - "header_ptr" is a pointer to its header (defined by the caller).
   - its name is "name".
   - "init_elem_nb" is the expected number of couples.
   - "average_list_nb_per_bucket" is the expected mean length of a collision list.
   - if "average_X_assoc" is positive it allows the usage of "XxY_Xforeach" else,
     if nul this usage is disabled.
   - if "average_Y_assoc" is positive it allows the usage of "XxY_Yforeach" else,
     if nul this usage is disabled.
   - "oflw" is a user's fonction called each time an overflow occurs in the structure.
      It allows to keep a concurrently growing structure.
   - "stat_file", if not NULL, is an output file on which statistics about the usage
     of the current structure will be printed.
*/

extern void			XxY_clear (/* XxY_header *header_ptr; */);
/* Clears the structure without changing its size. */

extern void			XxY_stat (/* FILE *stat_file;
					     XxY_header *header_ptr; */);
/* Prints on "stat_file" some statistics. */

extern void			XxY_free (/* XxY_header *header_ptr; */);
/* Frees the structure. */

extern void			XxY_gc (/* XxY_header *header_ptr; */);
/* Allows the "XxY_erased" elements to be reused. */

extern void			XxY_lock (/* XxY_header *header_ptr; */);
/* Frozes the structure to a minimum size. */

extern void			XxY_unlock (/* XxY_header *header_ptr; */);
/* Afterwards, the structure can grow... */

extern int			XxY_is_set (/* XxY_header *header_ptr;
					       int X ;
					       int Y; */);
/* If the couple ("X", "Y") is already stored it returns its index (a strictly
   positive integer) else it returns 0. */

extern BOOLEAN			XxY_set (/* XxY_header *header_ptr;
					    int X;
					    int Y;
					    int *result_ptr; */);
/* Put the couple ("X", "Y") in the structure and returns its index (a strictly
   positive integer). Moreover it returns TRUE if the couple ("X", "Y") is
   already stored else it returns FALSE. */

extern BOOLEAN			XxY_set_and_close (/* XxY_header *header_ptr;
						      int X;
						      int Y;
						      int *result_ptr; */);
/* Same as "XxY_set" but it also performs a transitive closure. */

extern void			XxY_to_SXBM (/* XxY_header *header_ptr;
						void (*get_couple) ();
						int couple_nb;
						X_header *X_header_ptr;
						SXBA **bits_matrix; */);
/* If "header_ptr" is not "NULL" it contains a binary relation else the
   elements of this relation can be get by the "get_couple" procedure.
   "couple_nb" is the number of couples.
   This procedure stores in the square bits matrix (see sxbitsarray)
   "bits_matrix" this binary relation. (This allow the usage of the
   transitive closure procedure "fermer").
   "bits_matrix" is minimum in size due to the usage of an "X_header" mapping. */


/* MACROS */

#define XxY_erase(h,x)		X_root_erase(h,x)
/* Erases the couple in structure "h" whose index is "x". */
/* WARNING: "h" is used several times in the macro expansion. */

#define XxY_is_erased(h,x)	X_root_is_erased(h,x)
/* Is TRUE if the couple whose index is "x" has been "XxY_erased". */

#define XxY_size(h)		X_root_size(h)
/* Is the current size of the structure (last index available). */

#define XxY_top(h)		X_root_top(h)
/* Is the higher index ever returned.
   CONDITION: XxY_top(h) <= XxY_size(h). */

#define XxY_X(h,x)		X_root_X(h,x)
/* Is the X_element of the couple whose index is "x". */

#define XxY_Y(h,x)		X_root_Y(h,x)
/* Is the Y_element of the couple whose index is "x". */



/* LOOP STATEMENTS */
/* Same usage as a for loop, "x" must be a variable, it takes successively all
   the "good" indexes. */
/* Break and continue C statements can be used inside foreach loops. */	

#define XxY_foreach(h,x)	X_root_foreach(h,x)
/* Walks upon all the valid elements (not "XxY_erased") of the structure. */

#define XxY_Xforeach(h,i,x)	X_root_Xforeach(h,i,x)
/* Walks upon all valid couples whose first element is "i". */

#define XxY_Yforeach(h,i,x)	X_root_Yforeach(h,i,x)
/* Walks upon all valid couples whose second element is "i". */


#endif
