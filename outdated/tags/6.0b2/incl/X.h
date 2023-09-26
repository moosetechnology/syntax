/* ------------------------------ Include File X.h ------------------------------ */

#ifndef X_inter
#define X_inter

/* Interface of the Abstract Data Type X */

#include	"X_rep.h"
#include	<unistd.h>
#include        <string.h>

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

extern BOOLEAN			X_set (/* X_header *header_ptr;
					  int X;
					  int *result_ptr; */);
/* Put the element "X" in the structure and returns its index (a strictly positive
   integer). Moreover it returns TRUE if the element "X" is already stored else
   it returns FALSE. */

extern BOOLEAN			X_write (/* X_header *header_ptr;
					    int file; */);
/* Write on "file" the structure "*header_ptr" */

extern BOOLEAN			X_read (/* X_header *header_ptr;
					   int file; 
					   char *name;
					   int (*user_oflw) ();
					   FILE *stat_file; */);
/* Read from "file" the structure "*header_ptr". "name" and "stat_file" may
   be reset. */

extern void			X_to_c (/* X_header *header_ptr;
					   FILE *F_X;
					   char *name;
					   BOOLEAN is_static; */);

/* Print on "F_X" a the C text for the structure "header_ptr" */

extern void			X_array_to_c (/* X_header *header_ptr;
						 FILE *F_X;
						 char *name; */);

/* Print on "F_X" the C text for the "arrays" of "header_ptr" */

extern void			X_header_to_c (/* X_header *header_ptr;
						  FILE *F_X;
						  char *name; */);

/* Print on "F_X" the C text for the "header" of "header_ptr" */

extern void			X_reuse (/* X_header *header_ptr;
					    char *name;
					    int (*user_oflw)();
					    FILE *stat_file; */);

/* Allow for the (re)use of the structure "header_ptr" based upon a C text */


/* MACROS */

#define X_erase(h,x)		X_root_erase(h,x)
/* Erases the element in structure "h" whose index is "x". */
/* WARNING: "h" is used several times in the macro expansion. */

#define X_is_erased(h,x)	X_root_is_erased(h,x)
/* Is TRUE if the element whose index is "x" has been "X_erased". */

#define X_is_static(h)		X_root_is_static(h)
/* Is TRUE iff the whole structure is defined statically. */

#define X_is_allocated(h)	X_root_is_allocated(h)
/* Is TRUE iff the structure is allocated. */

#define X_size(h)		X_root_size(h)
/* Is the current size of the structure (last index available). */

#define X_top(h)		X_root_top(h)
/* Is the higher index ever returned.
   CONDITION: X_top(h) <= X_size(h). */

#define X_X(h,x)		X_root_X(h,x)
/* Is the element whose index is "x". */

#define X_unlock(h)		X_root_unlock(h)
/* Afterwards, the structure can grow... */

#define X_total_sizeof(h)		X_root_Xtotal_sizeof(h)
/* Size in bytes of the whole structures (without sizeof (h)) */

#define X_used_sizeof(h)		X_root_Xused_sizeof(h)
/* Size in bytes of the whole structures (without sizeof (h) actually used) */




/* LOOP STATEMENT */
/* Same usage as a for loop, "x" must be a variable, it takes successively all
   the "good" indexes. */
/* Break and continue C statements can be used inside foreach loops. */	

#define X_foreach(h,x)		X_root_foreach(h,x)
/* Walks upon all the valid elements (not "X_erased") of the structure. */

#endif

