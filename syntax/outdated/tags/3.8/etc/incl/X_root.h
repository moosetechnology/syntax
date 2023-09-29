/* ------------------------- Include File X_root.h ------------------------- */

#ifndef X_root_inter
#define X_root_inter

/* Interface of the Abstract Data Type X_root */

#include	"X_root_rep.h"

/* This package is the basis for the X[xY[xZ]] managers. */


/* PROCEDURES & FUNCTIONS */

extern void			X_root_alloc (/* X_root_header *header_ptr;
						 char *name;
					    	 int init_elem_nb;
					    	 int average_list_nb_per_bucket;
						 int (*system_oflw)();
						 int (*user_oflw)();
						 int (*cmp)();
						 unsigned int (*scrmbl)();
						 int (*suppress)();
						 int (*assign)();
					    	 FILE *stat_file; */);
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
   

extern void			X_root_clear (/* X_root_header *header_ptr; */);
/* Clears the structure without changing its size. */

extern void			X_root_stat (/* FILE *stat_file;
						X_root_header *header_ptr; */);
/* Prints on "stat_file" some statistics. */

extern void			X_root_free (/* X_root_header *header_ptr; */);
/* Frees the structure. */

extern int			X_root_gc (/* X_root_header *header_ptr; */);
/* Allows the "X_root_erased" elements to be reused. */

extern void			X_root_lock (/* X_root_header *header_ptr; */);
/* Frozes the structure to a minimum size. */

extern int			X_root_is_set (/* X_root_header *header_ptr; int X; */);
/* If the element "X" is already stored it returns its index (a strictly positive
   integer) else it returns 0. */

extern BOOLEAN			X_root_set (/* X_root_header *header_ptr;
					       int X;
					       int *result_ptr; */);
/* Put the element "X" in the structure and returns its index (a strictly positive
   integer). Moreover it returns TRUE if the element "X" is already stored else
   it returns FALSE. */

#endif

