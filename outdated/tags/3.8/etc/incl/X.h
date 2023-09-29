/* ------------------------------ Include File X.h ------------------------------ */

#ifndef X_inter
#define X_inter

/* Interface of the Abstract Data Type X */

#include	"X_rep.h"

/* This package can manage any number of "int". */


/* PROCEDURES & FUNCTIONS */

extern void			X_alloc (/* X_header *header_ptr;
					    char *name;
					    int init_elem_nb;
					    int average_list_nb_per_bucket;
					    int (*oflw)();
					    FILE *stat_file; */);
/* Allocates an extensible structure such that :
   - "header_ptr" is a pointer to its header (defined by the caller).
   - its name is "name".
   - "init_elem_nb" is the expected number of elements.
   - "average_list_nb_per_bucket" is the expected mean length of a collision list.
   - "oflw" is a user's fonction called each time an overflow occurs in the structure.
      It allows to keep a concurrently growing structure.
   - "stat_file", if not NULL, is an output file on which statistics about the usage
     of the current structure will be printed.
*/
   

extern void			X_clear (/* X_header *header_ptr; */);
/* Clears the structure without changing its size. */

extern void			X_stat (/* FILE *stat_file; X_header *header_ptr; */);
/* Prints on "stat_file" some statistics. */

extern void			X_free (/* X_header *header_ptr; */);
/* Frees the structure. */

extern void			X_gc (/* X_header *header_ptr; */);
/* Allows the "X_erased" elements to be reused. */

extern void			X_lock (/* X_header *header_ptr; */);
/* Frozes the structure to a minimum size. */

extern int			X_is_set (/* X_header *header_ptr; int X; */);
/* If the element "X" is already stored it returns its index (a strictly positive
   integer) else it returns 0. */

extern BOOLEAN			X_set (/* X_header *header_ptr; int X; int *result_ptr; */);
/* Put the element "X" in the structure and returns its index (a strictly positive
   integer). Moreover it returns TRUE if the element "X" is already stored else
   it returns FALSE. */

/* MACROS */

#define X_erase(h,x)		X_root_erase(h,x)
/* Erases the element in structure "h" whose index is "x". */
/* WARNING: "h" is used several times in the macro expansion. */

#define X_is_erased(h,x)	X_root_is_erased(h,x)
/* Is TRUE if the element whose index is "x" has been "X_erased". */

#define X_size(h)		X_root_size(h)
/* Is the current size of the structure (last index available). */

#define X_top(h)		X_root_top(h)
/* Is the higher index ever returned.
   CONDITION: X_top(h) <= X_size(h). */

#define X_X(h,x)		X_root_X(h,x)
/* Is the element whose index is "x". */

#define X_unlock(h)		X_root_unlock(h)
/* Afterwards, the structure can grow... */

/* LOOP STATEMENT */
/* Same usage as a for loop, "x" must be a variable, it takes successively all
   the "good" indexes. */
/* Break and continue C statements can be used inside foreach loops. */	

#define X_foreach(h,x)		X_root_foreach(h,x)
/* Walks upon all the valid elements (not "X_erased") of the structure. */

#endif

