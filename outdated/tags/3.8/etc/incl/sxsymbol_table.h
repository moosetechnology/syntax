/* Name (for a symbol-table) manager */

/* Handle names (integers) which uniquely identify couples of integers
   (n, s) which can be viewed as :
   - n string-table-entry
   - s scope number
*/

#include "XxY.h"


struct sxsymbol_table_hd {
    XxY_header	scoped_access;
    int		*nested_access;
    int		*indexes;    
    int		nested_access_size;
    void	(*oflw) ();
};

extern void	sxsymbol_table_alloc	(/* struct sxsymbol_table_hd *symbol_table_ptr,
					    int estimated_string_table_entry_nb,
					    int estimated_symbol_table_entry_nb,
					    void (*user_oflw) () */);
/* If estimated_string_table_entry_nb == 0, sxsymbol_table_get_nested is NOT allowed. */

extern void	sxsymbol_table_free	(/* struct sxsymbol_table_hd *symbol_table_ptr */);

extern BOOLEAN	sxsymbol_table_put	(/* struct sxsymbol_table_hd *symbol_table_ptr,
					    int string_table_entry,
					    int scope_nb,
					    int *name */);
/* FALSE => already exists at the same scope_nb. */

extern void	sxsymbol_table_erase	(/* struct sxsymbol_table_hd *symbol_table_ptr,
					    int name,
					    BOOLEAN all */);
/* Erase "name" for "nested" entries. */
/* all == TRUE => also erases the "scoped" entries */

extern void	sxsymbol_table_close	(/* struct sxsymbol_table_hd *symbol_table_ptr,
					    int scope_nb,
					    BOOLEAN all */);
/* Erase "names" with "scope_nb" for "nested" entries. */
/* all == TRUE => also erases the "scoped" entries */


#define sxsymbol_table_get_nested(h,n)		\
    (((n) > (h).nested_access_size) ? 0 : (h).nested_access [n])
/* Return the most recently opened name whose string_table_entry is "n". */

#define sxsymbol_table_get_scoped(h,n,s)	\
    XxY_is_set (&((h).scoped_access), n, s)	
/* Return the name (n, s) or 0 if unknown. */

#define sxsymbol_table_open(h,n)	/* nothing */

#define sxsymbol_table_size(h)		XxY_size ((h).scoped_access)
/* Allow handling of a concurrent data structure. */

#define sxsymbol_table_name(h,x)	XxY_X ((h).scoped_access, x)
/* name to string_table_entry */

#define sxsymbol_table_level(h,x)	XxY_Y ((h).scoped_access, x)
/* name to scope_number */
