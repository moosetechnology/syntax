/******************************************************************************
 *                                S Y N T A X
 *-----------------------------------------------------------------------------
 *   Copyright (C) 1972-2018 INRIA (Institut National de Recherche en
 *   Informatique et Automatique)
 *-----------------------------------------------------------------------------
 *   URL: http://syntax.gforge.inria.fr
 *-----------------------------------------------------------------------------
 *   The source code of SYNTAX is distributed with two different licenses,
 *   depending on the files:
 *   - The recursive content of src/ and incl/ and the non-recursive content
 *     of SYNTAX's root directory are distributed under the CeCILL-C license
 *   - The recursive content of all other repertories is distributed under
 *     the CeCILL license
 *   All code produced by SYNTAX must be considered as being under the
 *   CeCILL-C license. Information about the CeCILL and CeCILL-C licenses
 *   can be found at, e.g., http://www.cecill.fr
 *****************************************************************************/
/* Name (for a symbol-table) manager */

/* Handle names (integers) which uniquely identify couples of integers
   (n, s) which can be viewed as :
   - n string-table-entry
   - s scope number
*/

#include "XxY.h"


struct sxsymbol_table_hd {
    XxY_header	scoped_access;
    SXINT	*nested_access;
    SXINT	*indexes;    
    SXINT	nested_access_size;
    void	(*oflw) (SXINT, SXINT);
};

extern void	sxsymbol_table_alloc	(struct sxsymbol_table_hd *header,
					 SXINT nested_size,
					 SXINT scoped_size,
					 void (*user_oflw) (SXINT, SXINT));
/* If estimated_string_table_entry_nb == 0, sxsymbol_table_get_nested is NOT allowed. */

extern void	sxsymbol_table_free	(struct sxsymbol_table_hd *header);

extern SXBOOLEAN	sxsymbol_table_put	(struct sxsymbol_table_hd *header,
					 SXINT name,
					 SXINT scope_nb,
					 SXINT *sxsymbol_table_put_index);
/* SXFALSE => already exists at the same scope_nb. */

extern void	sxsymbol_table_erase	(struct sxsymbol_table_hd *header,
					 SXINT sxsymbol_table_erase_index,
					 SXBOOLEAN all);
/* Erase "name" for "nested" entries. */
/* all == SXTRUE => also erases the "scoped" entries */

extern void	sxsymbol_table_close	(struct sxsymbol_table_hd *header,
					 SXINT scope_nb,
					 SXBOOLEAN all);
/* Erase "names" with "scope_nb" for "nested" entries. */
/* all == SXTRUE => also erases the "scoped" entries */


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
