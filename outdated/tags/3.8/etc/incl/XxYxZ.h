/* ------------------------- Include File XxYxZ.h ------------------------- */

#ifndef XxYxZ_inter
#define XxYxZ_inter

/* Interface of the Abstract Data Type XxYxZ */

#include	"XxYxZ_rep.h"

/* This package can manage a ternary relation in NxNxN. */


/* PROCEDURES & FUNCTIONS */



extern void			XxYxZ_alloc (/* XxY_header *header_ptr;
						char *name;
						int init_elem_nb;
						int average_list_nb_per_bucket;
						int *average_XYZ_assoc;
						int (*oflw)();
						FILE *stat_file; */);
/* Allocates an extensible structure such that :
   - "header_ptr" is a pointer to its header (defined by the caller).
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

extern void			XxYxZ_clear (/* XxYxZ_header *header_ptr; */);
/* Clears the structure without changing its size. */

extern void			XxYxZ_stat (/* FILE *stat_file;
					       XxYxZ_header *header_ptr; */);
/* Prints on "stat_file" some statistics. */

extern void			XxYxZ_free (/* XxYxZ_header *header_ptr; */);
/* Frees the structure. */

extern void			XxYxZ_gc (/* XxYxZ_header *header_ptr; */);
/* Allows the "XxYxZ_erased" elements to be reused. */

extern void			XxYxZ_lock (/* XxYxZ_header *header_ptr; */);
/* Frozes the structure to a minimum size. */

extern void			XxYxZ_unlock (/* XxYxZ_header *header_ptr; */);
/* Afterwards, the structure can grow... */

extern int			XxYxZ_is_set (/* XxYxZ_header *header_ptr;
						 int X;
						 int Y;
						 int Z; */);
/* If the triple ("X", "Y", "Z") is already stored it returns its index
   (a strictly positive integer) else it returns 0. */

extern BOOLEAN			XxYxZ_set (/* XxYxZ_header *header_ptr;
					      int X;
					      int Y;
					      int Z;
					      int *result_ptr; */);
/* Put the triple ("X", "Y", "Z") in the structure and returns its index
   (a strictly positive integer). Moreover it returns TRUE if the triple
   ("X", "Y", "Z") is already stored else it returns FALSE. */



/* MACROS */

#define XxYxZ_erase(h,x)	X_root_erase(h,x)
/* Erases the couple in structure "h" whose index is "x". */
/* WARNING: "h" is used several times in the macro expansion. */

#define XxYxZ_is_erased(h,x)	X_root_is_erased(h,x)
/* Is TRUE if the triple whose index is "x" has been "XxYxZ_erased". */

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


